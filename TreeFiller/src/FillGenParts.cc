// $Id: FillGenParts.cc,v 1.6 2008/06/17 13:31:38 loizides Exp $

#include "MitProd/TreeFiller/interface/FillGenParts.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "DataFormats/Common/interface/Handle.h"
#include "SimDataFormats/HepMCProduct/interface/HepMCProduct.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataTree/interface/GenParticle.h"

using namespace std;
using namespace edm;
using namespace mithep;

//-------------------------------------------------------------------------------------------------
FillGenParts::FillGenParts(const edm::ParameterSet &cfg) : 
  genParticles_(new mithep::Array<mithep::GenParticle>()),
  mcSource_(cfg.getUntrackedParameter<string>("source", "source")),
  genPartBrn_(cfg.getUntrackedParameter<string>("brname", Names::gkGenPartBrn))
{
}

//-------------------------------------------------------------------------------------------------
FillGenParts::~FillGenParts()
{
}

//-------------------------------------------------------------------------------------------------
void FillGenParts::analyze(const edm::Event &event, const edm::EventSetup &setup)
{
  genParticles_->Reset();

  Handle<HepMCProduct> theMCProduct;
  try {
    event.getByLabel(mcSource_, theMCProduct);
  } catch (cms::Exception& ex) {
    edm::LogError("FillGenParts") << "Error! Can not get collection with label " 
                                  << mcSource_ << endl;
    throw edm::Exception(edm::errors::Configuration, "FillGenParts::analyze()\n")
      << "Error! Can not get collection with label " << mcSource_ << endl;
  }

  const HepMC::GenEvent GenEvent = theMCProduct->getHepMCData();  

  int nGen = 0;
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

    mithep::GenParticle genParticle(mcPart->momentum().x(),mcPart->momentum().y(),
                                    mcPart->momentum().z(),mcPart->momentum().e(),
                                    mcPart->pdg_id(),
                                    mcPart->status(), 
                                    moind);          
    genParticles_->AddCopy(genParticle);
    nGen++;
  }

  genParticles_->Trim();
}

//-------------------------------------------------------------------------------------------------
void FillGenParts::beginJob(const edm::EventSetup &event)
{
  Service<TreeService> ts;
  TreeWriter *tws = ts->get();
  if(!tws) {
    throw edm::Exception(edm::errors::Configuration, "FillGenParts::beginJob()\n")
      << "Could not get pointer to Tree with name " << tws->GetName() << "\n";
    return;
  }

  tws->AddBranch(genPartBrn_.c_str(), &genParticles_);
}

//-------------------------------------------------------------------------------------------------
void FillGenParts::endJob()
{
  edm::LogInfo("FillGenParts::endJob") << "Ending Job" << endl;
}
