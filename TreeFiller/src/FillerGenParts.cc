// $Id: FillerGenParts.cc,v 1.5 2008/07/02 13:30:09 bendavid Exp $

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
  genParticles_(new mithep::Array<mithep::GenParticle>)
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
  genParticles_->Reset();

  Handle<HepMCProduct> theMCProduct;
  try {
    event.getByLabel(edm::InputTag(edmName_), theMCProduct);
  } catch (cms::Exception& ex) {
    edm::LogError("FillGenParts") << "Error! Can not get collection with label " 
                                  << edmName_ << endl;
    throw edm::Exception(edm::errors::Configuration, "FillGenParts::analyze()\n")
      << "Error! Can not get collection with label " << edmName_ << endl;
  }

  const HepMC::GenEvent GenEvent = theMCProduct->getHepMCData();  

  for (HepMC::GenEvent::particle_const_iterator pgen = GenEvent.particles_begin();
       pgen != GenEvent.particles_end(); ++pgen) {

    HepMC::GenParticle* mcPart = (*pgen);
    if(!mcPart) continue;

    Short_t moind = -1;
    HepMC::GenVertex * momVert = mcPart->production_vertex();
    if (momVert) {
      if (momVert->particles_in_size() == 1) {
	HepMC::GenVertex::particles_in_const_iterator mom = 
          momVert->particles_in_const_begin();
        moind = (*mom)->barcode() - 1;
      } 
    }

    mithep::GenParticle *genParticle = genParticles_->Allocate();
    new (genParticle) mithep::GenParticle(mcPart->momentum().x(),mcPart->momentum().y(),
                                          mcPart->momentum().z(),mcPart->momentum().e(),
                                          mcPart->pdg_id(),
                                          mcPart->status()); 
  }

  genParticles_->Trim();
}

//--------------------------------------------------------------------------------------------------
void FillerGenParts::ResolveLinks(const edm::Event      &event, 
                                  const edm::EventSetup &setup)
{
}
