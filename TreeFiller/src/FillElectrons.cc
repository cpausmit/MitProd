// $Id: FillElectrons.cc,v 1.2 2008/06/11 12:50:17 loizides Exp $

#include "MitProd/TreeFiller/interface/FillElectrons.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/EgammaCandidates/interface/PixelMatchGsfElectron.h"
#include "DataFormats/EgammaCandidates/interface/PixelMatchGsfElectronFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"
#include "MitAna/DataTree/interface/Particle.h"
#include "MitAna/DataTree/interface/Electron.h"
#include "MitAna/DataTree/interface/Names.h"

using namespace std;
using namespace edm;
using namespace mithep;

FillElectrons::FillElectrons(const edm::ParameterSet &iConfig) :
  electrons_(new mithep::Vector<mithep::Electron>()),
  electronSource_(iConfig.getUntrackedParameter<string>("electronSource","pixelMatchGsfElectrons")),
  electronBranch_(iConfig.getUntrackedParameter<string>("electronBrname", Names::gkElectronBrn))
{
}

//--------------------------------------------------------------------------------------------------
FillElectrons::~FillElectrons()
{
}

//-------------------------------------------------------------------------------------------------
void FillElectrons::analyze(const edm::Event &theEvent, 
                            const edm::EventSetup &iSetup)
{
  electrons_->Reset();

  Handle<reco::PixelMatchGsfElectronCollection> theElectronProduct;
  try {
    theEvent.getByLabel(electronSource_, theElectronProduct);
  } catch (cms::Exception& ex) {
    edm::LogError("FillElectrons") << "Error! Can not get collection with label " 
                                   << electronSource_ << endl;
    throw edm::Exception(edm::errors::Configuration, "FillElectrons:analyze()\n")
      << "Error! Can not get collection with label " << electronSource_ << endl;
  }

  const reco::PixelMatchGsfElectronCollection Electrons = *(theElectronProduct.product());  

  int nElectrons = 0;
  for (reco::PixelMatchGsfElectronCollection::const_iterator inElectron = Electrons.begin(); 
       inElectron != Electrons.end(); ++inElectron) {
    
    mithep::Electron* outElectron = 
      new mithep::Electron(inElectron->px(),inElectron->py(),inElectron->pz(),inElectron->energy());

    // Fill electron track info using global (tracker+electron chambers) track fit if available, or
    // standalone tracker or electron tracks otherwise

    const reco::GsfTrack* inElectronTrack = &*inElectron->gsfTrack().get();
    
    if (inElectronTrack) {
	outElectron->GetTrack()->SetHelix(inElectronTrack->phi(),
                                          inElectronTrack->d0(),
                                          inElectronTrack->pt(),
                                          inElectronTrack->dz(),
                                          inElectronTrack->theta());

	outElectron->GetTrack()->SetErrors(inElectronTrack->phiError(),
                                           inElectronTrack->d0Error(),
                                           inElectronTrack->ptError(),
                                           inElectronTrack->dzError(),
                                           inElectronTrack->thetaError());
	outElectron->GetTrack()->SetCharge(inElectronTrack->charge());
    }
    
    electrons_->Add(outElectron);
    nElectrons++;
  }

  electrons_->Trim();
}

//-------------------------------------------------------------------------------------------------
void FillElectrons::beginJob(edm::EventSetup const &iEvent)
{
  Service<TreeService> ts;
  TreeWriter *tws = ts->get();
  if(!tws) {
    throw edm::Exception(edm::errors::Configuration, "FillElectrons::beginJob()\n")
      << "Could not get pointer to Tree with name " << tws->GetName() << "\n";
    return;
  }

  tws->AddBranch(electronBranch_.c_str(),&electrons_);
}

//-------------------------------------------------------------------------------------------------
void FillElectrons::endJob()
{
  edm::LogInfo("FillElectrons::endJob") << "Ending Job" << endl;
}
