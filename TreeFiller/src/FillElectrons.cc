// $Id: FillElectrons.cc,v 1.6 2008/06/19 16:53:43 loizides Exp $

#include "MitProd/TreeFiller/interface/FillElectrons.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/EgammaCandidates/interface/PixelMatchGsfElectron.h"
#include "DataFormats/EgammaCandidates/interface/PixelMatchGsfElectronFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"
#include "MitAna/DataTree/interface/Electron.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitProd/TreeService/interface/TreeService.h"
#include "MitAna/DataUtil/interface/TreeWriter.h"

using namespace std;
using namespace edm;
using namespace mithep;

FillElectrons::FillElectrons(const edm::ParameterSet &cfg) :
  electrons_(new mithep::Array<mithep::Electron>()),
  electronSource_(cfg.getUntrackedParameter<string>("electronSource","pixelMatchGsfElectrons")),
  electronBranch_(cfg.getUntrackedParameter<string>("electronBrname", Names::gkElectronBrn))
{
}

//--------------------------------------------------------------------------------------------------
FillElectrons::~FillElectrons()
{
}

//-------------------------------------------------------------------------------------------------
void FillElectrons::analyze(const edm::Event &event, 
                            const edm::EventSetup &setup)
{
  electrons_->Reset();

  Handle<reco::PixelMatchGsfElectronCollection> theElectronProduct;
  try {
    event.getByLabel(electronSource_, theElectronProduct);
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
    
    electrons_->AddCopy(outElectron);
    nElectrons++;
  }

  electrons_->Trim();
}

//-------------------------------------------------------------------------------------------------
void FillElectrons::beginJob(const edm::EventSetup &setup)
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
