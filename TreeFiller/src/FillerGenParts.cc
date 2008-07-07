// $Id: FillerGenParts.cc,v 1.6 2008/07/03 07:56:14 loizides Exp $

#include "MitProd/TreeFiller/interface/FillerGenParts.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/Common/interface/Handle.h"
#include "SimDataFormats/HepMCProduct/interface/HepMCProduct.h"
#include "MitAna/DataTree/interface/GenParticle.h"
#include "MitAna/DataTree/interface/Names.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerGenParts::FillerGenParts(const ParameterSet &cfg, bool active) : 
  BaseFiller(cfg, "GenParts", active),
  edmName_(Conf().getUntrackedParameter<string>("edmName","source")),
  mitName_(Conf().getUntrackedParameter<string>("mitName",Names::gkGenPartBrn)),
  genParticles_(new mithep::Array<mithep::GenParticle>),
  genMap_(new mithep::GenParticleMap)
{
}

//--------------------------------------------------------------------------------------------------
FillerGenParts::~FillerGenParts()
{
}

//--------------------------------------------------------------------------------------------------
void FillerGenParts::BookDataBlock(TreeWriter &tws)
{
  tws.AddBranch(mitName_.c_str(),&genParticles_);
}

//--------------------------------------------------------------------------------------------------
void FillerGenParts::FillDataBlock(const edm::Event      &event, 
                                   const edm::EventSetup &setup)
{
  genMap_->Reset();
  genParticles_->Reset();


  try {
    event.getByLabel(edm::InputTag(edmName_), hepMCProduct_);
  } catch (cms::Exception& ex) {
    edm::LogError("FillGenParts") << "Error! Can not get collection with label " 
                                  << edmName_ << endl;
    throw edm::Exception(edm::errors::Configuration, "FillGenParts::analyze()\n")
      << "Error! Can not get collection with label " << edmName_ << endl;
  }

  const HepMC::GenEvent GenEvent = hepMCProduct_->getHepMCData();  

  for (HepMC::GenEvent::particle_const_iterator pgen = GenEvent.particles_begin();
       pgen != GenEvent.particles_end(); ++pgen) {

    HepMC::GenParticle* mcPart = (*pgen);
    if(!mcPart) continue;

//     Short_t moind = -1;
//     HepMC::GenVertex * momVert = mcPart->production_vertex();
//     if (momVert) {
//       if (momVert->particles_in_size() == 1) {
// 	HepMC::GenVertex::particles_in_const_iterator mom = 
//           momVert->particles_in_const_begin();
//         moind = (*mom)->barcode() - 1;
//       } 
//     }
    
    mithep::GenParticle *genParticle = genParticles_->Allocate();
    new (genParticle) mithep::GenParticle(mcPart->momentum().x(),mcPart->momentum().y(),
                                          mcPart->momentum().z(),mcPart->momentum().e(),
                                          mcPart->pdg_id(),
                                          mcPart->status());
                                          
    genMap_->Add(mcPart->barcode(), genParticle);
  }

  genParticles_->Trim();
}

//--------------------------------------------------------------------------------------------------
void FillerGenParts::ResolveLinks(const edm::Event      &event, 
                                  const edm::EventSetup &setup)
{

  const HepMC::GenEvent GenEvent = hepMCProduct_->getHepMCData();  

  for (HepMC::GenEvent::particle_const_iterator pgen = GenEvent.particles_begin();
       pgen != GenEvent.particles_end(); ++pgen) {

    HepMC::GenParticle* mcPart = (*pgen);
    if(!mcPart) continue;

    //check if genpart has a decay vertex
    HepMC::GenVertex* decayVertex = mcPart->end_vertex();
    if (!decayVertex) continue;

    //find corresponding mithep genparticle parent in association table
    mithep::GenParticle* genParent = genMap_->GetMit(mcPart->barcode());

    //set decay vertex
    genParent->SetVertex(decayVertex->point3d().x(),decayVertex->point3d().y(),decayVertex->point3d().z());

    //loop through daugthers
    for (HepMC::GenVertex::particles_out_const_iterator pgenD = decayVertex->particles_in_const_begin();
       pgenD != decayVertex->particles_in_const_end(); ++pgenD) {
                HepMC::GenParticle* mcDaughter = (*pgenD);
                mithep::GenParticle* genDaughter = genMap_->GetMit(mcDaughter->barcode());
                genParent->AddDaughter(genDaughter);
                genDaughter->SetMother(genParent);
    }

  }

}
