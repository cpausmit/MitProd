// $Id: FillMuons.cc,v 1.2 2008/06/11 12:50:17 loizides Exp $

#include "MitProd/TreeFiller/interface/FillMuons.h"
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

using namespace std;
using namespace edm;
using namespace mithep;

//-------------------------------------------------------------------------------------------------
FillMuons::FillMuons(const edm::ParameterSet &iConfig) : 
  muons_(new mithep::Array<mithep::Muon>()),
  muonSource_(iConfig.getUntrackedParameter<string>("muonSource" , "muons")),
  muonBranch_(iConfig.getUntrackedParameter<string>("muonBrname", Names::gkMuonBrn))
{
}

//-------------------------------------------------------------------------------------------------
FillMuons::~FillMuons()
{
}

//-------------------------------------------------------------------------------------------------
void FillMuons::analyze(const edm::Event &theEvent, 
                           const edm::EventSetup &iSetup)
{
  muons_->Reset();

  Handle<reco::MuonCollection> theMuonProduct;
  try {
    theEvent.getByLabel(muonSource_, theMuonProduct);
  } catch (cms::Exception& ex) {
    edm::LogError("FillMuons") << "Error! Can not get collection with label " 
                                  << muonSource_ << endl;
    throw edm::Exception(edm::errors::Configuration, "FillMuons:analyze()\n")
      << "Error! Can not get collection with label " << muonSource_ << endl;
  }

  const reco::MuonCollection Muons = *(theMuonProduct.product());  

  int nMuons = 0;
  for (reco::MuonCollection::const_iterator inMuon = Muons.begin();
       inMuon != Muons.end(); ++inMuon) {
    
    mithep::Muon* outMuon = new mithep::Muon(inMuon->px(),inMuon->py(),inMuon->pz(),inMuon->energy());
    
    //Fill muon track info using global (tracker+muon chambers) track fit if available, 
    //or standalone tracker or muon tracks otherwise
    const reco::Track* inMuonTrack = &*inMuon->combinedMuon().get();
    if (!inMuonTrack)
    	inMuonTrack = &*inMuon->standAloneMuon().get();
    if (!inMuonTrack)
    	inMuonTrack = &*inMuon->track().get();
    if (inMuonTrack) {
	outMuon->GetTrack()->SetHelix(inMuonTrack->phi(),
                                      inMuonTrack->d0(),
                                      inMuonTrack->pt(),
                                      inMuonTrack->dz(),
                                      inMuonTrack->theta());
	outMuon->GetTrack()->SetErrors(inMuonTrack->phiError(),
                                       inMuonTrack->d0Error(),
                                       inMuonTrack->ptError(),
                                       inMuonTrack->dzError(),
                                       inMuonTrack->thetaError());
	outMuon->GetTrack()->SetCharge(inMuonTrack->charge());
    }
    
    muons_->AddCopy(outMuon);
    nMuons++;
  }

  muons_->Trim();
}

//-------------------------------------------------------------------------------------------------
void FillMuons::beginJob(edm::EventSetup const &iEvent)
{
  Service<TreeService> ts;
  TreeWriter *tws = ts->get();
  if(!tws) {
    throw edm::Exception(edm::errors::Configuration, "FillMuons::beginJob()\n")
      << "Could not get pointer to Tree with name " << tws->GetName() << "\n";
    return;
  }

  tws->AddBranch(muonBranch_.c_str(),&muons_);
}

//-------------------------------------------------------------------------------------------------
void FillMuons::endJob()
{
  edm::LogInfo("FillMuons::endJob") << "Ending Job" << endl;
}
