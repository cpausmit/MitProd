// $Id: FillGenParts.cc,v 1.3 2008/06/03 07:21:45 paus Exp $

#include "MitProd/TreeFiller/interface/FillGenParts.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "SimDataFormats/HepMCProduct/interface/HepMCProduct.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataTree/interface/GenParticle.h"

using namespace std;
using namespace edm;
using namespace mithep;

//-------------------------------------------------------------------------------------------------
FillGenParts::FillGenParts(const edm::ParameterSet &iConfig)
  : genParticles_(new mithep::Vector<mithep::GenParticle>()),
    mcSource_(iConfig.getUntrackedParameter<string>("source", "source")),
    genPartBrn_(iConfig.getUntrackedParameter<string>("brname", Names::gkGenPartBrn))
{
}

//-------------------------------------------------------------------------------------------------
FillGenParts::~FillGenParts()
{
}

//-------------------------------------------------------------------------------------------------
void FillGenParts::analyze(const edm::Event &theEvent, const edm::EventSetup &iSetup)
{
  // Analyzer called on every entry.

  genParticles_->Reset();

  Handle<HepMCProduct> theMCProduct;
  try {
    theEvent.getByLabel(mcSource_, theMCProduct);
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
    genParticles_->Add(genParticle);
    nGen++;
  }

  genParticles_->Trim();
}

//-------------------------------------------------------------------------------------------------
void FillGenParts::beginJob(edm::EventSetup const &iEvent)
{
  Service<TreeService> ts;
  TreeWriter *tws = ts->get();
  if(!tws) {
    throw edm::Exception(edm::errors::Configuration, "FillGenParts::beginJob()\n")
      << "Could not get pointer to Tree with name " << tws->GetName() << "\n";
    return;
  }

  //tws->AddBranch(genPartBrn_.c_str(), genParticles_->ClassName(), &genParticles_, 32*1024, 2);
  tws->AddBranch(genPartBrn_.c_str(), &genParticles_, 32*1024, 99);
}

//-------------------------------------------------------------------------------------------------
void FillGenParts::endJob()
{
  edm::LogInfo("FillGenParts::endJob") << "Ending Job" << endl;
}
