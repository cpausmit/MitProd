// $Id: FillerGlobalMuons.cc,v 1.4 2008/06/19 16:53:43 loizides Exp $

#include "MitProd/TreeFiller/interface/FillerGlobalMuons.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "MitAna/DataTree/interface/Names.h"

using namespace std;
using namespace edm;
using namespace mithep;

//-------------------------------------------------------------------------------------------------
FillerGlobalMuons::FillerGlobalMuons(const ParameterSet &cfg) : 
  BaseFiller(cfg,"GlobalMuons"),
  edmName_(Conf().getUntrackedParameter<string>("edmName","muons")),
  mitName_(Conf().getUntrackedParameter<string>("mitName",Names::gkGlobalMuonBrn)),
  muons_(new mithep::Array<mithep::GlobalMuon>)
{
}

//-------------------------------------------------------------------------------------------------
FillerGlobalMuons::~FillerGlobalMuons()
{
}

//-------------------------------------------------------------------------------------------------
void FillerGlobalMuons::BookDataBlock(TreeWriter &tws)
{
  tws.AddBranch(mitName_.c_str(),&muons_);
}

//-------------------------------------------------------------------------------------------------
void FillerGlobalMuons::FillDataBlock(const edm::Event      &event, 
				      const edm::EventSetup &setup)
{
  // Fill global muon information. Links to the various standalone components are created in the
  // link resolution.

  muons_->Reset();
  
  Handle<reco::MuonCollection> muonProduct;
  try {
    event.getByLabel(edmName_, muonProduct);
  } catch (cms::Exception& ex) {
    edm::LogError("FillerGlobalMuons") << "Error! Cannot get collection with label " 
				       << edmName_ << endl;
    throw edm::Exception(edm::errors::Configuration, "FillerGlobalMuons:FillDataBlock()\n")
      << "Error! Cannot get collection with label " << edmName_ << endl;
  }
  
  const reco::MuonCollection Muons = *(muonProduct.product());  
  
  // Loop through all muons
  for (reco::MuonCollection::const_iterator iM = Muons.begin(); iM != Muons.end(); ++iM) {

    // Here we are interested in the global (= combined) muon information only
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
      muons_->AddCopy(outMuon);      
    }
  }
  muons_->Trim();
}

//-------------------------------------------------------------------------------------------------
void FillerGlobalMuons::ResolveLinks(const edm::Event      &event, 
				     const edm::EventSetup &setup)
{
  // Complete global muon information: links to the various standalone components are created here.

}
