// $Id: FillerGenParts.cc,v 1.11 2008/07/10 14:22:54 bendavid Exp $

#include "MitProd/TreeFiller/interface/FillerGenParts.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/Common/interface/Handle.h"
#include "SimDataFormats/HepMCProduct/interface/HepMCProduct.h"
#include "MitAna/DataTree/interface/Names.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerGenParts::FillerGenParts(const ParameterSet &cfg, bool active) : 
  BaseFiller(cfg, "GenParts", active),
  edmName_(Conf().getUntrackedParameter<string>("edmName","source")),
  mitName_(Conf().getUntrackedParameter<string>("mitName",Names::gkGenPartBrn)),
  genParticles_(new mithep::GenParticleArr(250)),
  genMap_(new mithep::GenParticleMap)
{
  // Constructor.
}

//--------------------------------------------------------------------------------------------------
FillerGenParts::~FillerGenParts()
{
  // Destructor.

  delete genParticles_;
  delete genMap_;
}

//--------------------------------------------------------------------------------------------------
void FillerGenParts::BookDataBlock(TreeWriter &tws)
{
  // Add branch to tree.

  tws.AddBranch(mitName_.c_str(),&genParticles_);
}

//--------------------------------------------------------------------------------------------------
void FillerGenParts::FillDataBlock(const edm::Event      &event, 
                                   const edm::EventSetup &setup)
{
  // Loop over HepMC particle and fill their information.

  genMap_->Reset();
  genParticles_->Reset();

  Handle<edm::HepMCProduct> hHepMCProduct;
  GetProduct(edmName_, hHepMCProduct, event);

  const HepMC::GenEvent &GenEvent = hHepMCProduct->getHepMCData();  

  //loop over all hepmc particles and copy their information
  for (HepMC::GenEvent::particle_const_iterator pgen = GenEvent.particles_begin();
       pgen != GenEvent.particles_end(); ++pgen) {

    HepMC::GenParticle *mcPart = (*pgen);
    if(!mcPart) continue;

    mithep::GenParticle *genParticle = genParticles_->Allocate();
    new (genParticle) mithep::GenParticle(mcPart->momentum().x(),mcPart->momentum().y(),
                                          mcPart->momentum().z(),mcPart->momentum().e(),
                                          mcPart->pdg_id(),
                                          mcPart->status());
    
    genMap_->Add(mcPart->barcode(), genParticle);
  }
}

//--------------------------------------------------------------------------------------------------
void FillerGenParts::ResolveLinks(const edm::Event      &event, 
                                  const edm::EventSetup &setup)
{
  // Loop over HepMC particle and resolve their links.

  Handle<edm::HepMCProduct> hHepMCProduct;
  GetProduct(edmName_, hHepMCProduct, event);

  const HepMC::GenEvent &GenEvent = hHepMCProduct->getHepMCData();  

  for (HepMC::GenEvent::particle_const_iterator pgen = GenEvent.particles_begin();
       pgen != GenEvent.particles_end(); ++pgen) {

    HepMC::GenParticle *mcPart = (*pgen);
    if(!mcPart) continue;

    //check if genpart has a decay vertex
    HepMC::GenVertex *dVertex = mcPart->end_vertex();
    if (!dVertex) continue;

    //find corresponding mithep genparticle parent in association table
    mithep::GenParticle *genParent = genMap_->GetMit(mcPart->barcode());

    //set decay vertex
    genParent->SetVertex(dVertex->point3d().x(),
                         dVertex->point3d().y(),
                         dVertex->point3d().z());

    //loop through daugthers
    for (HepMC::GenVertex::particles_out_const_iterator pgenD = dVertex->particles_out_const_begin();
         pgenD != dVertex->particles_out_const_end(); ++pgenD) {
      HepMC::GenParticle *mcDaughter = (*pgenD);
      mithep::GenParticle *genDaughter = genMap_->GetMit(mcDaughter->barcode());
      genParent->AddDaughter(genDaughter);
      if (!(genDaughter->HasMother()))
        genDaughter->SetMother(genParent);
    }
  }

  genParticles_->Trim();
  for(UInt_t i=0; i<genParticles_->GetEntries(); ++i) 
    genParticles_->At(i)->Trim();
}
