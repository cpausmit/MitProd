// $Id: FillerAODMCParticles.cc,v 1.1 2009/02/23 20:53:35 bendavid Exp $

#include "MitProd/TreeFiller/interface/FillerAODMCParticles.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Common/interface/RefToPtr.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "MitAna/DataTree/interface/Names.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerAODMCParticles::FillerAODMCParticles(const ParameterSet &cfg, const char *name, bool active) : 
  BaseFiller(cfg, name, active),
  genEdmName_(Conf().getUntrackedParameter<string>("genEdmName","genParticles")),
  genMapName_(Conf().getUntrackedParameter<string>("genMapName","AODGenMap")),
  mitName_(Conf().getUntrackedParameter<string>("mitName",Names::gkMCPartBrn)),
  mcParticles_(new mithep::MCParticleArr(250)),
  genMap_(new mithep::AODGenParticleMap)
{
  // Constructor.
}

//--------------------------------------------------------------------------------------------------
FillerAODMCParticles::~FillerAODMCParticles()
{
  // Destructor.

  delete mcParticles_;
  delete genMap_;
}

//--------------------------------------------------------------------------------------------------
void FillerAODMCParticles::BookDataBlock(TreeWriter &tws)
{
  // Add branch to tree and publish our maps.

  tws.AddBranch(mitName_.c_str(),&mcParticles_);

  OS()->add(genMap_,genMapName_.c_str());

}

//--------------------------------------------------------------------------------------------------
void FillerAODMCParticles::FillDataBlock(const edm::Event      &event, 
                                      const edm::EventSetup &setup)
{
  // Loop over HepMC particle and fill their information.

  mcParticles_->Delete();
  genMap_->Reset();

  Handle<reco::GenParticleCollection> hGenPProduct;
  GetProduct(genEdmName_, hGenPProduct, event);  

  const reco::GenParticleCollection genParticles = *(hGenPProduct.product());  

  //loop over all genparticles and copy their information
  for (reco::GenParticleCollection::const_iterator pgen = genParticles.begin();
      pgen != genParticles.end(); ++pgen) {

    mithep::MCParticle *mcPart = mcParticles_->Allocate();
    new (mcPart) mithep::MCParticle(pgen->px(),pgen->py(),
                                          pgen->pz(),pgen->energy(),
                                          pgen->pdgId(),
                                          pgen->status());
                                          
    mcPart->SetIsGenerated();                                          
    
    edm::Ptr<reco::GenParticle> thePtr(hGenPProduct, pgen - genParticles.begin());
    genMap_->Add(thePtr, mcPart);
    
  }
  
  mcParticles_->Trim();
}

//--------------------------------------------------------------------------------------------------
void FillerAODMCParticles::ResolveLinks(const edm::Event      &event, 
                                     const edm::EventSetup &setup)
{
  // Loop over HepMC particle and resolve their links.

  
  
  Handle<reco::GenParticleCollection> hGenPProduct;
  GetProduct(genEdmName_, hGenPProduct, event);  

  const reco::GenParticleCollection genParticles = *(hGenPProduct.product());  

  //loop over all genparticles and copy their information
  for (reco::GenParticleCollection::const_iterator pgen = genParticles.begin();
      pgen != genParticles.end(); ++pgen) {

    int nDaughters = pgen->numberOfDaughters();
    
    if (nDaughters>0) {
    
      edm::Ptr<reco::GenParticle> thePtr(hGenPProduct, pgen - genParticles.begin());
      MCParticle *mcMother = genMap_->GetMit(thePtr);
      for (int i=0; i<nDaughters; ++i) {
        const reco::Candidate *genDaughter = pgen->daughter(i);
        MCParticle *mcDaughter = genMap_->GetMit(refToPtr(pgen->daughterRef(i)));
        //set mother decay vertex
        if (i==0)
          mcMother->SetVertex(genDaughter->vx(),genDaughter->vy(),genDaughter->vz());
        
        //set mother-daughter links
        mcMother->AddDaughter(mcDaughter);
        if (!mcDaughter->HasMother())
          mcDaughter->SetMother(mcMother);
        
      }
    }
  }  
}
