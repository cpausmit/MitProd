// $Id: FillerMuons.cc,v 1.10 2008/08/08 11:21:08 sixie Exp $

#include "MitProd/TreeFiller/interface/FillerMuons.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "RecoMuon/MuonIdentification/interface/IdGlobalFunctions.h"
#include "DataFormats/MuonReco/interface/MuonSelectors.h"
#include "MitAna/DataTree/interface/Muon.h"
#include "MitAna/DataTree/interface/Track.h"
#include "MitAna/DataTree/interface/Names.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerMuons::FillerMuons(const edm::ParameterSet &cfg, bool active) :
  BaseFiller(cfg,"Muons",active),
  edmName_(Conf().getUntrackedParameter<string>("edmName","muons")),
  mitName_(Conf().getUntrackedParameter<string>("mitName",Names::gkMuonBrn)),
  globalTrackMapName_(Conf().getUntrackedParameter<string>("globalTrackMapName","")),
  staTrackMapName_(Conf().getUntrackedParameter<string>("staTrackMapName","")),
  staVtxTrackMapName_(Conf().getUntrackedParameter<string>("staVtxTrackMapName","")),
  trackerTrackMapName_(Conf().getUntrackedParameter<string>("trackerTrackMapName","")),
  globalTrackMap_(0), 
  standaloneTrackMap_(0), 
  standaloneVtxTrackMap_(0), 
  trackerTrackMap_(0),
  muons_(new mithep::MuonArr(16)),
  fGMAllArbitrated(0),            // all muons with the tracker muon arbitrated
  fGMGlobalMuonPromptTight(0),    // global muons with tighter fit requirements
  fGMTMLastStationLoose(0),       // penetration depth loose selector
  fGMTMLastStationTight(0),       // penetration depth tight selector
  fGMTM2DCompatibilityLoose(0),   // likelihood based loose selector
  fGMTM2DCompatibilityTight(0)    // likelihood based tight selector
{
  // Constructor.
}

//--------------------------------------------------------------------------------------------------
FillerMuons::~FillerMuons()
{
  // Destructor.
  cout << "ALL Muons        : " << fAll                      << endl;
  cout << "ALL Arbitrated   : " << fGMAllArbitrated          << endl;
  cout << "ALL Prompt Tight : " << fGMGlobalMuonPromptTight  << endl;
  cout << "ALL LastLoose    : " << fGMTMLastStationLoose     << endl;
  cout << "ALL LastTight    : " << fGMTMLastStationTight     << endl;
  cout << "ALL TMCOMPatLoose: " << fGMTM2DCompatibilityLoose << endl;
  cout << "ALL TMCOMPatTIght: " << fGMTM2DCompatibilityTight << endl;

  delete muons_;
}

//--------------------------------------------------------------------------------------------------
void FillerMuons::BookDataBlock(TreeWriter &tws)
{ vector<string> lCuts; lCuts.push_back("test");
 fTest = new AnaMuon("M",lCuts); 
  // Add muons branch to tree and get pointers to maps.

  tws.AddBranch(mitName_.c_str(),&muons_);

  if (!globalTrackMapName_.empty()) 
    globalTrackMap_ = OS()->get<TrackMap>(globalTrackMapName_.c_str());
  if (!staTrackMapName_.empty()) 
    standaloneTrackMap_ = OS()->get<TrackMap>(staTrackMapName_.c_str());
  if (!staVtxTrackMapName_.empty()) 
    standaloneVtxTrackMap_ = OS()->get<TrackMap>(staVtxTrackMapName_.c_str());
  if (!trackerTrackMapName_.empty()) 
    trackerTrackMap_ = OS()->get<TrackMap>(trackerTrackMapName_.c_str());
}



//--------------------------------------------------------------------------------------------------
void FillerMuons::FillDataBlock(const edm::Event      &event, 
                                const edm::EventSetup &setup)
{
  // Fill muon information. 

  muons_->Reset();
  
  Handle<reco::MuonCollection> hMuonProduct;
  GetProduct(edmName_, hMuonProduct, event);  
  const reco::MuonCollection inMuons = *(hMuonProduct.product());  

  for (reco::MuonCollection::const_iterator iM = inMuons.begin(); iM != inMuons.end(); ++iM) {  
    fAll++;
    if(iM->isGood(reco::Muon::AllArbitrated))          fGMAllArbitrated++;
    if(iM->isGood(reco::Muon::GlobalMuonPromptTight))  fGMGlobalMuonPromptTight++;    
    if(iM->isGood(reco::Muon::TMLastStationLoose))     fGMTMLastStationLoose++;
    if(iM->isGood(reco::Muon::TMLastStationTight))     fGMTMLastStationTight++;
    if(iM->isGood(reco::Muon::TM2DCompatibilityLoose)) fGMTM2DCompatibilityLoose++;
    if(iM->isGood(reco::Muon::TM2DCompatibilityTight)) fGMTM2DCompatibilityTight++;
    
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

    if (globalTrackMap_ && iM->combinedMuon().isNonnull()) 
      outMuon->SetGlobalTrk(globalTrackMap_->GetMit(iM->combinedMuon()));    
    if (standaloneTrackMap_ && standaloneVtxTrackMap_ && iM->standAloneMuon().isNonnull()) { 
      Int_t refProductId = iM->standAloneMuon().id().id();
      if ( refProductId == standaloneVtxTrackMap_->GetEdmProductId())
	outMuon->SetStandaloneTrk(standaloneVtxTrackMap_->GetMit(iM->standAloneMuon()));
      else if ( refProductId == standaloneTrackMap_->GetEdmProductId())
	outMuon->SetStandaloneTrk(standaloneTrackMap_->GetMit(iM->standAloneMuon()));
      else throw edm::Exception(edm::errors::Configuration, "FillerMuons:FillDataBlock()\n")
	<< "Error! Track reference in unmapped collection " << edmName_ << endl;
    }
    if (trackerTrackMap_ && iM->track().isNonnull()) 
      outMuon->SetTrackerTrk(trackerTrackMap_->GetMit(iM->track()));
    //PCH 
    //outMuon->SetNHits      (iM->numberOfValidHits());
    outMuon->SetNChambers  (iM->numberOfChambers());
    outMuon->SetStationMask(iM->stationMask(reco::Muon::SegmentAndTrackArbitration));
    //outMuon->SetTrackStationMask(iM->stationMask(reco::Muon::TrackandSegment));
    for(int i0 = 0; i0 < 4; i0++) {
      //DTs
      outMuon->SetDX(i0,            iM->dX(i0+1,1));
      outMuon->SetDY(i0,            iM->dY(i0+1,1));
      outMuon->SetPullX(i0,         iM->pullX(i0+1,1));
      outMuon->SetPullY(i0,         iM->pullY(i0+1,1));
      outMuon->SetTrackDist(i0,     iM->trackDist(i0+1,1));
      outMuon->SetTrackDistErr(i0,  iM->trackDistErr(i0+1,1));
      outMuon->SetNSegments(i0,     iM->numberOfSegments(i0+1, 1));
      //CSCs
      outMuon->SetDX(4+i0,          iM->dX       (i0+1,2));
      outMuon->SetDY(4+i0,          iM->dY       (i0+1,2));
      outMuon->SetPullX(4+i0,       iM->pullX    (i0+1,2));
      outMuon->SetPullY(4+i0,       iM->pullY    (i0+1,2));
      outMuon->SetTrackDist(4+i0,   iM->trackDist(i0+1,2));
      outMuon->SetTrackDistErr(4+i0,iM->trackDistErr(i0+1,2));
      outMuon->SetNSegments(4+i0,   iM->numberOfSegments(i0+1,2));
      //cout << "Dist 1 " << i0 << " -- "<< iM->trackDist(i0+1,1) << " -- " << endl;
    }
    //   if (iM->isMatchesValid()) {
    //        std::vector<reco::MuonChamberMatch> lMatches = iM->matches();
    //        for(  unsigned  int i0 = 0; i0 < lSegments.size(); i0++) {
    //        std::vector<reco::MuonSegmentMatch> lSegements = lMatches[i0]->segmentMatches();
    // 	 for( unsigned int i1 = 0; i1 < lMatches.size(); i1++ ) {
    // 	   outMuon->SetSegmentMask(i1, lMatches[i1].mask);
    // 	 }
    //        }
//     if(double(iM->caloCompatibility()) != double(fTest->getCaloCompatability(outMuon,true,true))) {
//       cout << " Calo Compat ---> " << iM->caloCompatibility()             << " -- " << fTest->getCaloCompatability   (outMuon,true,true) << endl;
//       cout << "----------> " << outMuon->Eta() << " -- " <<   outMuon->P() << " -- " << outMuon->EmEnergy() << " -- " << outMuon->HadEnergy() << " -- " << outMuon->HoEnergy() << endl;
//       cout << "----------> " << iM->eta() << " -- " <<   iM->p() << " -- " << iM->calEnergy().em << " -- " << iM->calEnergy().had << " -- " << iM->calEnergy().ho << endl;
//     }
      
    if(muon::isGoodMuon(*iM,reco::Muon::TMLastStationTight)     != fTest->isGoodMuon(outMuon,AnaMuon::TMLastStationTight)     || 
       muon::isGoodMuon(*iM,reco::Muon::TM2DCompatibilityLoose) != fTest->isGoodMuon(outMuon,AnaMuon::TM2DCompatibilityLoose) || 
       muon::isGoodMuon(*iM,reco::Muon::TM2DCompatibilityTight) != fTest->isGoodMuon(outMuon,AnaMuon::TM2DCompatibilityTight)) {
       
      if(muon::isGoodMuon(*iM,reco::Muon::TMLastStationTight))   cout << "Last---> Reco is True " << endl;
      if(fTest->isGoodMuon(outMuon,AnaMuon::TMLastStationTight)) cout << "Last---> Ana  is True " << endl;

       
      if(muon::isGoodMuon(*iM,reco::Muon::TM2DCompatibilityLoose))   
	cout << "TMLoose---> Reco is True " << fTest->getCaloCompatability(outMuon,true,true) << " : " << iM->caloCompatibility()  << " : " 
	     << fTest->getSegmentCompatability(outMuon) << " : " << muonid::getSegmentCompatibility(*iM) << endl;
      if(fTest->isGoodMuon(outMuon,AnaMuon::TM2DCompatibilityLoose)) 
	cout << "TMLoose---> Ana  is True " << fTest->getCaloCompatability(outMuon,true,true) << " : " << iM->caloCompatibility()  << " : " 
	     << fTest->getSegmentCompatability(outMuon) << " : " << muonid::getSegmentCompatibility(*iM) << endl;
      if(muon::isGoodMuon(*iM,reco::Muon::TM2DCompatibilityTight))   
	cout << "TMTight---> Reco is True  " << fTest->getCaloCompatability(outMuon,true,true) << " : " << iM->caloCompatibility()  << " : " 
	     << fTest->getSegmentCompatability(outMuon) << " : " << muonid::getSegmentCompatibility(*iM) << endl;
      if(fTest->isGoodMuon(outMuon,AnaMuon::TM2DCompatibilityTight)) 
	cout << "TMTight---> Ana  is True "  << fTest->getCaloCompatability(outMuon,true,true) << " : " << iM->caloCompatibility()  << " : " 
	     << fTest->getSegmentCompatability(outMuon) << " : " << muonid::getSegmentCompatibility(*iM) << endl;
      
      cout << "Size --- > " << outMuon->NSegments() <<  " - " << iM->numberOfMatches(reco::Muon::SegmentArbitration) << endl;
      
      for(int i0 = 0; i0 < 4; i0++) {
 	cout << "A---------> " << outMuon->GetDX(i0) << " -- " <<   outMuon->GetDY(i0) << " -- " << outMuon->GetPullX(i0) << " -- " << outMuon->GetPullY(i0) << " -- " << outMuon->GetTrackDist(i0) << " -- " << outMuon->GetTrackDistErr(i0) << " -- " << outMuon->GetNSegments(i0) <<  endl;
 	cout << "A1--------> " << iM->dX(i0+1,1) << " -- " <<   iM->dY(i0+1,1) << " -- " << iM->pullX(i0+1,1) << " -- " << iM->pullY(i0+1,1) << " -- " << iM->trackDist(i0+1,1) << " -- " << iM->trackDistErr(i0+1,1) << " -- " << iM->segmentX(i0+1,1) << " -- " << endl;
 	cout << "B---------> " << outMuon->GetDX(i0+4) << " -- " <<   outMuon->GetDY(i0+4) << " -- " << outMuon->GetPullX(i0+4) << " -- " << outMuon->GetPullY(i0+4) << " -- " << outMuon->GetTrackDist(i0+4) << " -- " << outMuon->GetTrackDistErr(i0+4) << " -- " << outMuon->GetNSegments(i0) << endl;
 	cout << "B1--------> " << iM->dX(i0+1,2) << " -- " <<   iM->dY(i0+1,2) << " -- " << iM->pullX(i0+1,2) << " -- " << iM->pullY(i0+1,2) << " -- " << iM->trackDist(i0+1,2) << " -- " << iM->trackDistErr(i0+1,2) << " -- " << iM->segmentX(i0+1,2) <<  endl;
       }
     }
    
    if(fTest->isGoodMuon(outMuon,AnaMuon::AllArbitrated) != muon::isGoodMuon(*iM,reco::Muon::AllArbitrated)) {
      if(muon::isGoodMuon(*iM,reco::Muon::AllArbitrated))     cout << "---> Reco is True " << endl;
      if(fTest->isGoodMuon(outMuon,AnaMuon::AllArbitrated))   cout << "---> Ana  is True " << endl;
       cout << "All Arbitrated --- " << outMuon->NSegments() << " - " << iM->numberOfMatches(reco::Muon::SegmentAndTrackArbitration) << endl;
       cout << "SA : " << outMuon->StandaloneTrk() << " GM: " << outMuon->GlobalTrk() << " : " << outMuon->TrackerTrk() << endl;
       //if(outMuon->StandaloneTrk() != 0) cout << "S Eta : " << -log(tan(outMuon->StandaloneTrk()->Theta()/2.)) << endl;
       //if(outMuon->TrackerTrk()    != 0) cout << "T Eta : " << -log(tan(outMuon->TrackerTrk()->Theta()/2.)) << endl;
       if(iM->isTrackerMuon())    cout << "Not Tracker Muon : ";
       if(iM->isStandAloneMuon()) cout << "StandAlone Muon  : ";
       if(iM->isGlobalMuon())     cout << "Global Muon      : ";
       cout << endl;
    }
  }
  muons_->Trim();
}
