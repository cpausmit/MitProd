// $Id: FillGlobalMuons.cc,v 1.2 2008/06/11 12:50:17 loizides Exp $

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "SimDataFormats/HepMCProduct/interface/HepMCProduct.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/TrackReco/interface/Track.h"

#include "MitAna/DataTree/interface/Particle.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitProd/TreeFiller/interface/FillGlobalMuons.h"

using namespace std;
using namespace edm;
using namespace mithep;

//-------------------------------------------------------------------------------------------------
FillGlobalMuons::FillGlobalMuons(const edm::ParameterSet &iConfig) : 
  muons_(new mithep::Vector<mithep::GlobalMuon>()),
  muonSource_(iConfig.getUntrackedParameter<string>("muonSource", "muons")),
  muonBranch_(iConfig.getUntrackedParameter<string>("globalMuonBrname", Names::gkGlobalMuonBrn))
{
}

//-------------------------------------------------------------------------------------------------
FillGlobalMuons::~FillGlobalMuons()
{
}

//-------------------------------------------------------------------------------------------------
void FillGlobalMuons::analyze(const edm::Event      &event, 
			      const edm::EventSetup &iSetup)
{
  // Fill muon track info using global (tracker+muon chambers) track fit if available, or standalone
  // muon or tracker tracks otherwise
  muons_->Reset();
  
  Handle<reco::MuonCollection> muonProduct;
  try {
    event.getByLabel(muonSource_, muonProduct);
  } catch (cms::Exception& ex) {
    edm::LogError("FillGlobalMuons") << "Error! Can not get collection with label " 
				     << muonSource_ << endl;
    throw edm::Exception(edm::errors::Configuration, "FillGlobalMuons:analyze()\n")
      << "Error! Can not get collection with label " << muonSource_ << endl;
  }
  
  const reco::MuonCollection Muons = *(muonProduct.product());  
  
  // Loop through all muons
  for (reco::MuonCollection::const_iterator iM = Muons.begin(); iM != Muons.end(); ++iM) {
    // Here we are interested in the standalone muon information only
    const reco::Track* muonTrk = &*iM->combinedMuon().get();
    if (! muonTrk)
      cout << "Error - no combined muon track in the global muon. Filling nothing!\n";
    else {
      // Initialize a new MitDataObject
      mithep::GlobalMuon* outMuon = new mithep::GlobalMuon(iM->px(),iM->py(),iM->pz(),iM->energy());
      // Fill the information
      outMuon->GetTrack()->SetHelix (muonTrk->phi(),muonTrk->d0(),muonTrk->pt(),
				     muonTrk->dz(), muonTrk->theta());
      outMuon->GetTrack()->SetErrors(muonTrk->phiError(),muonTrk->d0Error(),muonTrk->ptError(),
				     muonTrk->dzError(), muonTrk->thetaError());
      outMuon->GetTrack()->SetCharge(muonTrk->charge());
      // Add the muon to the collection
      muons_->Add(outMuon);      
    }
  }
  muons_->Trim();
}

//-------------------------------------------------------------------------------------------------
void FillGlobalMuons::beginJob(edm::EventSetup const &iEvent)
{
  Service<TreeService> ts;
  TreeWriter *tws = ts->get();
  if (!tws) {
    throw edm::Exception(edm::errors::Configuration, "FillGlobalMuons::beginJob()\n")
      << "Could not get pointer to Tree with name " << tws->GetName() << "\n";
    return;
  }

  tws->AddBranch(muonBranch_.c_str(),&muons_);
}

//-------------------------------------------------------------------------------------------------
void FillGlobalMuons::endJob()
{
  edm::LogInfo("FillGlobalMuons::endJob") << "Ending Job" << endl;
}
