#include "MitProd/TreeFiller/interface/FillerPATMuons.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/Common/interface/RefToPtr.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Tau.h"
#include "DataFormats/PatCandidates/interface/Photon.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "MitAna/DataTree/interface/MuonCol.h"
#include "MitAna/DataTree/interface/Track.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitProd/ObjectService/interface/ObjectService.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerPATMuons::FillerPATMuons(const edm::ParameterSet &cfg, edm::ConsumesCollector& collector, ObjectService* os, const char *name, bool active) :
  BaseFiller(cfg,os,"PATMuons",active),
  edmToken_(GetToken<edm::View<pat::Muon> >(collector, "edmName","selectedLayer1Muons")),
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
}

//--------------------------------------------------------------------------------------------------
void FillerPATMuons::FillDataBlock(const edm::Event      &event, 
                                const edm::EventSetup &setup)
{
  // Fill muon information. 

  muons_->Delete();
  
  edm::Handle<edm::View<pat::Muon> > muonHandle;
  GetProduct(edmToken_, muonHandle, event);
  edm::View<pat::Muon> const& muons = *muonHandle;

  for(edm::View<pat::Muon>::const_iterator iM = muons.begin(); iM!=muons.end(); ++iM){

    if(iM->pt()>0) {
      mithep::Muon* outMuon = muons_->AddNew();
     
      if (globalTrackMap_ && iM->combinedMuon().isNonnull()) 
        outMuon->SetGlobalTrk(globalTrackMap_->GetMit(refToPtr(iM->combinedMuon())));
       
      if (standaloneTrackMap_ && standaloneVtxTrackMap_ && iM->standAloneMuon().isNonnull()) { 
        Int_t refProductId = iM->standAloneMuon().id().id();
        if ( refProductId == standaloneVtxTrackMap_->GetEdmProductId())
          outMuon->SetStandaloneTrk(standaloneVtxTrackMap_->GetMit(refToPtr(iM->standAloneMuon())));
        else if ( refProductId == standaloneTrackMap_->GetEdmProductId())
          outMuon->SetStandaloneTrk(standaloneTrackMap_->GetMit(refToPtr(iM->standAloneMuon())));
        else throw edm::Exception(edm::errors::Configuration, "FillerPATMuons:FillDataBlock()\n")
               << "Error! Track reference in unmapped collection";
      }

      if (trackerTrackMap_ && iM->track().isNonnull()) 
        outMuon->SetTrackerTrk(trackerTrackMap_->GetMit(refToPtr(iM->track())));          
           
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

DEFINE_MITHEP_TREEFILLER(FillerPATMuons);
