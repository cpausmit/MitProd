// $Id: FillerMCParticles.cc,v 1.13 2008/07/14 21:01:00 loizides Exp $

#include "MitProd/TreeFiller/interface/FillerMCParticles.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/Common/interface/Handle.h"
#include "SimDataFormats/HepMCProduct/interface/HepMCProduct.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticleFwd.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingVertex.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingVertexContainer.h"
#include "MitAna/DataTree/interface/Names.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerMCParticles::FillerMCParticles(const ParameterSet &cfg, const char *name, bool active) : 
  BaseFiller(cfg, name, active),
  genActive_(Conf().getUntrackedParameter<bool>("genActive",true)),
  simActive_(Conf().getUntrackedParameter<bool>("simActive",true)),
  genEdmName_(Conf().getUntrackedParameter<string>("genEdmName","source")),
  simEdmName_(Conf().getUntrackedParameter<string>("simEdmName","mergedtruth:MergedTrackTruth")),
  mitName_(Conf().getUntrackedParameter<string>("mitName",Names::gkMCPartBrn)),
  mcParticles_(new mithep::MCParticleArr(250)),
  genMap_(new mithep::GenParticleMap),
  simMap_(new mithep::SimParticleMap)
{
  // Constructor.
}

//--------------------------------------------------------------------------------------------------
FillerMCParticles::~FillerMCParticles()
{
  // Destructor.

  delete mcParticles_;
  delete genMap_;
  delete simMap_;
}

//--------------------------------------------------------------------------------------------------
void FillerMCParticles::BookDataBlock(TreeWriter &tws)
{
  // Add branch to tree.

  tws.AddBranch(mitName_.c_str(),&mcParticles_);
  
}

//--------------------------------------------------------------------------------------------------
void FillerMCParticles::FillDataBlock(const edm::Event      &event, 
                                   const edm::EventSetup &setup)
{
  // Loop over HepMC particle and fill their information.

  genMap_->Reset();
  simMap_->Reset();
  mcParticles_->Reset();

  if (genActive_) {
  
    Handle<edm::HepMCProduct> hHepMCProduct;
    GetProduct(genEdmName_, hHepMCProduct, event);  
  
    const HepMC::GenEvent &GenEvent = hHepMCProduct->getHepMCData();  
  
    //loop over all hepmc particles and copy their information
    for (HepMC::GenEvent::particle_const_iterator pgen = GenEvent.particles_begin();
        pgen != GenEvent.particles_end(); ++pgen) {
  
      HepMC::GenParticle *mcPart = (*pgen);
      if(!mcPart) continue;
  
      mithep::MCParticle *genParticle = mcParticles_->Allocate();
      new (genParticle) mithep::MCParticle(mcPart->momentum().x(),mcPart->momentum().y(),
                                            mcPart->momentum().z(),mcPart->momentum().e(),
                                            mcPart->pdg_id(),
                                            mcPart->status());
                                            
      genParticle->SetIsGenerated();                                          
      
      genMap_->Add(mcPart->barcode(), genParticle);
    }
    
  }
  
  if (simActive_) {
  
    Handle<TrackingParticleCollection> hTrackingParticleProduct;
    GetProduct(simEdmName_, hTrackingParticleProduct, event);
    
    const TrackingParticleCollection trackingParticles = *(hTrackingParticleProduct.product());  
    
    // loop through all simParticles
    for (TrackingParticleCollection::const_iterator iM = trackingParticles.begin(); 
        iM != trackingParticles.end(); ++iM) {
  
      TrackingParticleRef theRef(hTrackingParticleProduct, iM-trackingParticles.begin());
      mithep::MCParticle *outSimParticle;
      
      if (genActive_ && iM->genParticle().size()) {
        const HepMC::GenParticle *mcPart = iM->genParticle_begin()->get();
        outSimParticle=genMap_->GetMit(mcPart->barcode());
        outSimParticle->SetStatus(iM->status());
      }
      else {
        outSimParticle = mcParticles_->Allocate();
        new (outSimParticle) mithep::MCParticle(iM->px(),iM->py(),iM->pz(),iM->energy(),iM->pdgId(), iM->status());
      }
      
      outSimParticle->SetIsSimulated();
      simMap_->Add(theRef, outSimParticle);
      
    }
  
  }
  

  mcParticles_->Trim();
}

//--------------------------------------------------------------------------------------------------
void FillerMCParticles::ResolveLinks(const edm::Event      &event, 
                                  const edm::EventSetup &setup)
{
  // Loop over HepMC particle and resolve their links.

  if (genActive_) {
  
    Handle<edm::HepMCProduct> hHepMCProduct;
    GetProduct(genEdmName_, hHepMCProduct, event);
    
    const HepMC::GenEvent &GenEvent = hHepMCProduct->getHepMCData();  
  
    for (HepMC::GenEvent::particle_const_iterator pgen = GenEvent.particles_begin();
        pgen != GenEvent.particles_end(); ++pgen) {
  
      HepMC::GenParticle *mcPart = (*pgen);
      if(!mcPart) continue;
      
      //check if genpart has a decay vertex
      HepMC::GenVertex *dVertex = mcPart->end_vertex();
      if (!dVertex) continue;
  
      //find corresponding mithep genparticle parent in association table
      mithep::MCParticle *genParent = genMap_->GetMit(mcPart->barcode());
      
      if (genParent->IsSimulated()) continue;
  
      //set decay vertex
      //division by 10.0 is needed due to HepMC use of mm instead of cm for distance units
      genParent->SetVertex(dVertex->point3d().x()/10.0,
                          dVertex->point3d().y()/10.0,
                          dVertex->point3d().z()/10.0);
  
      //loop through daugthers
      for (HepMC::GenVertex::particles_out_const_iterator pgenD = dVertex->particles_out_const_begin();
          pgenD != dVertex->particles_out_const_end(); ++pgenD) {
        HepMC::GenParticle *mcDaughter = (*pgenD);
        mithep::MCParticle *genDaughter = genMap_->GetMit(mcDaughter->barcode());
        genParent->AddDaughter(genDaughter);
        if (!(genDaughter->HasMother()))
          genDaughter->SetMother(genParent);
      }
    }
    
  }
  
  if (simActive_) {
  
    Handle<TrackingParticleCollection> hTrackingParticleProduct;
    GetProduct(simEdmName_, hTrackingParticleProduct, event);  
    
    const TrackingParticleCollection trackingParticles = *(hTrackingParticleProduct.product());  
  
    // loop through all simParticles
    for (TrackingParticleCollection::const_iterator iM = trackingParticles.begin(); 
        iM != trackingParticles.end(); ++iM) {
    
      if (iM->decayVertices().size() <= 0) 
        continue;
  
      TrackingParticleRef theRef(hTrackingParticleProduct, iM-trackingParticles.begin());
      mithep::MCParticle *simParent = simMap_->GetMit(theRef);
      for (TrackingVertexRefVector::iterator v= iM->decayVertices().begin(); 
          v != iM->decayVertices().end(); ++v) {
        for (TrackingParticleRefVector::iterator pd = v->get()->daughterTracks().begin(); 
            pd != v->get()->daughterTracks().end(); ++pd) {
          mithep::MCParticle *simDaughter = simMap_->GetMit(*pd);
          simParent->AddDaughter(simDaughter);
          simDaughter->SetMother(simParent);
        }
        if (v == iM->decayVertices().end()-1) {
          simParent->SetVertex(v->get()->position().x(),
                              v->get()->position().y(),
                              v->get()->position().z());
        }
      }
    }
  
  }
  
}
