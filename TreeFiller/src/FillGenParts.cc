// $Id:$

#include "MitProd/TreeFiller/interface/FillGenParts.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "SimDataFormats/HepMCProduct/interface/HepMCProduct.h"
#include "MitAna/DataTree/interface/Particle.h"

#include "TLorentzVector.h"

using namespace std;
using namespace edm;
using namespace mithep;

//-------------------------------------------------------------------------------------------------
FillGenParts::FillGenParts(const edm::ParameterSet &iConfig)
   : anaMCSource_(iConfig.getUntrackedParameter<string>("anaMCSource" , "source"))
{
   genParticles_ = new mithep::Vector<mithep::Particle>();
}

//-------------------------------------------------------------------------------------------------
FillGenParts::~FillGenParts()
{
  cout << " Fillgenpars done " <<endl;
}

//-------------------------------------------------------------------------------------------------
void FillGenParts::analyze(const edm::Event &theEvent, 
                           const edm::EventSetup &iSetup)
{
  genParticles_->Reset();

  Handle<HepMCProduct> theMCProduct;
  try {
    theEvent.getByLabel(anaMCSource_, theMCProduct);
  } catch (cms::Exception& ex) {
    printf("Error! can't get collection with label %s\n",anaMCSource_.c_str());
  }

  const HepMC::GenEvent GenEvent = theMCProduct->getHepMCData();  

  int nGen = 0;
  for (HepMC::GenEvent::particle_const_iterator pgen = 
         GenEvent.particles_begin();
       pgen != GenEvent.particles_end(); ++pgen) {

    HepMC::GenParticle* mcPart = (*pgen);
    if(!mcPart) continue;

    mithep::Particle genParticle;
    genParticle.SetPxPyPzE(mcPart->momentum().x(),mcPart->momentum().y(),
                           mcPart->momentum().z(),mcPart->momentum().e());

    //printf("ngen %d\n",nGen);
#if 0
    genParticle.setPdgID(MCPart->pdg_id());
    genParticle.setBarCode(MCPart->barcode()-1);
    genParticle.setStatus(MCPart->status());
   
    HepMC::GenVertex * momVert = MCPart->production_vertex();
    //genParticle.setMother(-1);
    if(momVert) {
      if(momVert->particles_in_size() == 1) {
	HepMC::GenVertex::particles_in_const_iterator mom = momVert->particles_in_const_begin();
	genParticle.setMother((*mom)->barcode() - 1);
      } else {genParticle.setMother(-1);}
    }  else {genParticle.setMother(-1);}
    //if(MCPart->mother()) genParticle.setMother(MCPart->mother()->barcode()-1);
    //else                 genParticle.setMother(-1);
    genParticle.setPtr(nGen);
#endif
    genParticles_->Add(genParticle);
    nGen++;
  }

  //genParticles_->resize(nGen);
}

//-------------------------------------------------------------------------------------------------
void FillGenParts::beginJob(edm::EventSetup const &iEvent)
{
  Service<TreeService> ts;
  TreeWriter *tws = ts->get();
  if(!tws) {
    //todo error
    return;
  }

  tws->AddBranch("Particles","mithep::Vector<mithep::Particle>",&genParticles_,32000,99);
}

//-------------------------------------------------------------------------------------------------
void FillGenParts::endJob()
{
  edm::LogInfo("FillGenParts::endJob") << "Ending Job" << endl;
}
