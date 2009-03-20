// $Id: FillerMuons.cc,v 1.20 2009/03/15 11:20:41 loizides Exp $

#include "MitProd/TreeFiller/interface/FillerMuons.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/Common/interface/RefToPtr.h"
#include "MitAna/DataTree/interface/Muon.h"
#include "MitAna/DataTree/interface/Track.h"
#include "MitAna/DataTree/interface/Names.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerMuons::FillerMuons(const edm::ParameterSet &cfg, const char *name, bool active) :
  BaseFiller(cfg,name,active),
  edmName_(Conf().getUntrackedParameter<string>("edmName","muons")),
  mitName_(Conf().getUntrackedParameter<string>("mitName",Names::gkMuonBrn)),
  globalTrackMapName_(Conf().getUntrackedParameter<string>("globalTrackMapName","")),
  staTrackMapName_(Conf().getUntrackedParameter<string>("staTrackMapName","")),
  staVtxTrackMapName_(Conf().getUntrackedParameter<string>("staVtxTrackMapName","")),
  trackerTrackMapName_(Conf().getUntrackedParameter<string>("trackerTrackMapName","")),
  muonMapName_(Conf().getUntrackedParameter<string>("muonMapName","")),
  globalTrackMap_(0),
  standaloneTrackMap_(0),
  standaloneVtxTrackMap_(0),
  trackerTrackMap_(0),
  muonMap_(new mithep::MuonMap),
  muons_(new mithep::MuonArr(16))
{
  // Constructor.
}

//--------------------------------------------------------------------------------------------------
FillerMuons::~FillerMuons() 
{
  // Destructor.

  delete muons_;
  delete muonMap_;
}

//--------------------------------------------------------------------------------------------------
void FillerMuons::BookDataBlock(TreeWriter &tws) 
{ 
  // Add muons branch to tree and get pointers to maps.

  tws.AddBranch(mitName_,&muons_);
  OS()->add<mithep::MuonArr>(muons_,mitName_);

  if (!globalTrackMapName_.empty()) {
    globalTrackMap_ = OS()->get<TrackMap>(globalTrackMapName_);
    if (globalTrackMap_)
      AddBranchDep(mitName_,globalTrackMap_->GetBrName());
  }
  if (!staTrackMapName_.empty()) {
    standaloneTrackMap_ = OS()->get<TrackMap>(staTrackMapName_);
    if (standaloneTrackMap_)
      AddBranchDep(mitName_,standaloneTrackMap_->GetBrName());
  }
  if (!staVtxTrackMapName_.empty()) {
    standaloneVtxTrackMap_ = OS()->get<TrackMap>(staVtxTrackMapName_);
    if (standaloneVtxTrackMap_)
      AddBranchDep(mitName_,standaloneVtxTrackMap_->GetBrName());
  }
  if (!trackerTrackMapName_.empty()) {
    trackerTrackMap_ = OS()->get<TrackMap>(trackerTrackMapName_);
    if (trackerTrackMap_)
      AddBranchDep(mitName_,trackerTrackMap_->GetBrName());
  }
  if (!muonMapName_.empty()) {
    muonMap_->SetBrName(mitName_);
    OS()->add<MuonMap>(muonMap_,muonMapName_);
  }
}

//--------------------------------------------------------------------------------------------------
void FillerMuons::FillDataBlock(const edm::Event      &event, 
                                const edm::EventSetup &setup)
{
  // Fill muon information. 

  muons_->Delete();
  muonMap_->Reset();
  
  Handle<reco::MuonCollection> hMuonProduct;
  GetProduct(edmName_, hMuonProduct, event);  
  const reco::MuonCollection inMuons = *(hMuonProduct.product());  

  for (reco::MuonCollection::const_iterator iM = inMuons.begin(); iM != inMuons.end(); ++iM) {  
    mithep::Muon* outMuon = muons_->AddNew();      
    outMuon->SetIsoR03SumPt(iM->isolationR03().sumPt);
    outMuon->SetIsoR03EmEt(iM->isolationR03().emEt);
    outMuon->SetIsoR03HadEt(iM->isolationR03().hadEt);
    outMuon->SetIsoR03HoEt(iM->isolationR03().hoEt);
    outMuon->SetIsoR03NTracks(iM->isolationR03().nTracks);
    outMuon->SetIsoR03NJets(iM->isolationR03().nJets);
    outMuon->SetIsoR05SumPt(iM->isolationR05().sumPt);
    outMuon->SetIsoR05EmEt(iM->isolationR05().emEt);
    outMuon->SetIsoR05HadEt(iM->isolationR05().hadEt);
    outMuon->SetIsoR05HoEt(iM->isolationR05().hoEt);
    outMuon->SetIsoR05NTracks(iM->isolationR05().nTracks);
    outMuon->SetIsoR05NJets(iM->isolationR05().nJets);
    outMuon->SetEmEnergy(iM->calEnergy().em);
    outMuon->SetHadEnergy(iM->calEnergy().had);
    outMuon->SetHoEnergy(iM->calEnergy().ho);
    outMuon->SetEmS9Energy(iM->calEnergy().emS9);
    outMuon->SetHadS9Energy(iM->calEnergy().hadS9);
    outMuon->SetHoS9Energy(iM->calEnergy().hoS9);
    outMuon->SetIsGlobalMuon(iM->isGlobalMuon());
    outMuon->SetIsTrackerMuon(iM->isTrackerMuon());
    outMuon->SetIsStandaloneMuon(iM->isStandAloneMuon());
    outMuon->SetIsCaloMuon(iM->isCaloMuon());

    if (globalTrackMap_ && iM->combinedMuon().isNonnull()) 
      outMuon->SetGlobalTrk(globalTrackMap_->GetMit(refToPtr(iM->combinedMuon())));    
    if (standaloneTrackMap_ && standaloneVtxTrackMap_ && iM->standAloneMuon().isNonnull()) { 
      Int_t refProductId = iM->standAloneMuon().id().id();
      if ( refProductId == standaloneVtxTrackMap_->GetEdmProductId())
	outMuon->SetStandaloneTrk(standaloneVtxTrackMap_->GetMit(refToPtr(iM->standAloneMuon())));
      else if ( refProductId == standaloneTrackMap_->GetEdmProductId())
	outMuon->SetStandaloneTrk(standaloneTrackMap_->GetMit(refToPtr(iM->standAloneMuon())));
      else throw edm::Exception(edm::errors::Configuration, "FillerMuons:FillDataBlock()\n")
             << "Error! Track reference in unmapped collection " << edmName_ << endl;
    }
    if (trackerTrackMap_ && iM->track().isNonnull()) 
      outMuon->SetTrackerTrk(trackerTrackMap_->GetMit(refToPtr(iM->track())));

    outMuon->SetNChambers  (iM->numberOfChambers());
    outMuon->SetStationMask(iM->stationMask(reco::Muon::SegmentAndTrackArbitration));
    for(int i0 = 0; i0 < 4; i0++) {
      // DTs
      outMuon->SetDX(i0,            iM->dX(i0+1,1));
      outMuon->SetDY(i0,            iM->dY(i0+1,1));
      outMuon->SetPullX(i0,         iM->pullX(i0+1,1));
      outMuon->SetPullY(i0,         iM->pullY(i0+1,1));
      outMuon->SetTrackDist(i0,     iM->trackDist(i0+1,1));
      outMuon->SetTrackDistErr(i0,  iM->trackDistErr(i0+1,1));
      outMuon->SetNSegments(i0,     iM->numberOfSegments(i0+1,1));
      // CSCs
      outMuon->SetDX(4+i0,          iM->dX       (i0+1,2));
      outMuon->SetDY(4+i0,          iM->dY       (i0+1,2));
      outMuon->SetPullX(4+i0,       iM->pullX    (i0+1,2));
      outMuon->SetPullY(4+i0,       iM->pullY    (i0+1,2));
      outMuon->SetTrackDist(4+i0,   iM->trackDist(i0+1,2));
      outMuon->SetTrackDistErr(4+i0,iM->trackDistErr(i0+1,2));
      outMuon->SetNSegments(4+i0,   iM->numberOfSegments(i0+1,2));
    }

    // add muon to map
    edm::Ptr<reco::Muon> thePtr(hMuonProduct, iM - inMuons.begin());
    muonMap_->Add(thePtr, outMuon);
    
    if (verbose_>1) {
      if (!outMuon->HasGlobalTrk() && !outMuon->HasStandaloneTrk()) {
        printf("mithep::Muon, pt=%5f, eta=%5f, phi=%5f, mass=%5f\n",outMuon->Pt(),outMuon->Eta(),outMuon->Phi(), outMuon->Mass());
        printf("  reco::Muon, pt=%5f, eta=%5f, phi=%5f, mass=%5f\n",iM->pt(),iM->eta(),iM->phi(),iM->mass());  
      }
    }

  }
  muons_->Trim();
}
