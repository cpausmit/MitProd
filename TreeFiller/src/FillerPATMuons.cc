// $Id: FillerPATMuons.cc,v 1.1 2008/08/12 10:13:46 sixie Exp $

#include "MitProd/TreeFiller/interface/FillerPATMuons.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "MitAna/DataTree/interface/Muon.h"
#include "MitAna/DataTree/interface/Track.h"
#include "MitAna/DataTree/interface/Names.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Tau.h"
#include "DataFormats/PatCandidates/interface/Photon.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/Common/interface/View.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerPATMuons::FillerPATMuons(const edm::ParameterSet &cfg, bool active,
                               const TrackMap *globalMap, const TrackMap *stdMap, 
                               const TrackMap *stdVtxMap, const TrackMap *trackerMap) : 
  BaseFiller(cfg,"PATMuons",active),
  edmName_(Conf().getUntrackedParameter<string>("edmName","selectedLayer1Muons")),
  mitName_(Conf().getUntrackedParameter<string>("mitName",Names::gkMuonBrn)),
  globalTrackMapName_(Conf().getUntrackedParameter<string>("globalTrackMapName","")),
  staTrackMapName_(Conf().getUntrackedParameter<string>("staTrackMapName","")),
  staVtxTrackMapName_(Conf().getUntrackedParameter<string>("staVtxTrackMapName","")),
  trackerTrackMapName_(Conf().getUntrackedParameter<string>("trackerTrackMapName","")),
  globalTrackMap_(0), 
  standaloneTrackMap_(0), 
  standaloneVtxTrackMap_(0), 
  trackerTrackMap_(0),
  muons_(new mithep::MuonArr(16))
{
  // Constructor.
}

//--------------------------------------------------------------------------------------------------
FillerPATMuons::~FillerPATMuons()
{
  // Destructor.

  delete muons_;
}

//--------------------------------------------------------------------------------------------------
void FillerPATMuons::BookDataBlock(TreeWriter &tws)
{
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
void FillerPATMuons::FillDataBlock(const edm::Event      &event, 
                                const edm::EventSetup &setup)
{
  // Fill muon information. 

  muons_->Reset();
  
  edm::Handle<edm::View<pat::Muon> > muonHandle;
  event.getByLabel(edm::InputTag(edmName_),muonHandle);
  edm::View<pat::Muon> muons = *muonHandle;

   for(edm::View<pat::Muon>::const_iterator iM = muons.begin(); iM!=muons.end(); ++iM){
              
     if(iM->pt()>0) {
       mithep::Muon* outMuon = muons_->AddNew();
     
       if (globalTrackMap_ && iM->combinedMuon().isNonnull()) 
	 outMuon->SetGlobalTrk(globalTrackMap_->GetMit(iM->combinedMuon()));
       
       if (standaloneTrackMap_ && standaloneVtxTrackMap_ && iM->standAloneMuon().isNonnull()) { 
	 Int_t refProductId = iM->standAloneMuon().id().id();
	 if ( refProductId == standaloneVtxTrackMap_->GetEdmProductId())
	   outMuon->SetStandaloneTrk(standaloneVtxTrackMap_->GetMit(iM->standAloneMuon()));
	 else if ( refProductId == standaloneTrackMap_->GetEdmProductId())
	   outMuon->SetStandaloneTrk(standaloneTrackMap_->GetMit(iM->standAloneMuon()));
	 else throw edm::Exception(edm::errors::Configuration, "FillerPATMuons:FillDataBlock()\n")
	   << "Error! Track reference in unmapped collection " << edmName_ << endl;
       }

       if (trackerTrackMap_ && iM->track().isNonnull()) 
	 outMuon->SetTrackerTrk(trackerTrackMap_->GetMit(iM->track()));          
           
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
     }
   }
   muons_->Trim();
}
