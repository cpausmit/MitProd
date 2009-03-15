// $Id: FillerPATElectrons.cc,v 1.4 2009/02/26 17:04:03 bendavid Exp $

#include "MitProd/TreeFiller/interface/FillerPATElectrons.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/Common/interface/RefToPtr.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h"
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
FillerPATElectrons::FillerPATElectrons(const edm::ParameterSet &cfg, 
                                       const char *name, bool active) :
  BaseFiller(cfg,"PATElectrons",active),
  edmName_(Conf().getUntrackedParameter<string>("edmName","selectedLayer1Electrons")),
  mitName_(Conf().getUntrackedParameter<string>("mitName",Names::gkElectronBrn)),
  gsfTrackMapName_(Conf().getUntrackedParameter<string>("gsfTrackMapName","")),
  trackerTrackMapName_(Conf().getUntrackedParameter<string>("trackerTrackMapName","")),
  electrons_(new mithep::ElectronArr(16)),
  gsfTrackMap_(0),
  trackerTrackMap_(0)
{
  // Constructor.
}

//--------------------------------------------------------------------------------------------------
FillerPATElectrons::~FillerPATElectrons()
{
  // Destructor.

  delete electrons_;
}

//--------------------------------------------------------------------------------------------------
void FillerPATElectrons::BookDataBlock(TreeWriter &tws)
{
  // Add electron branch to our tree and get our maps.

  tws.AddBranch(mitName_,&electrons_);
  OS()->add<mithep::ElectronArr>(electrons_,mitName_);

  if (!gsfTrackMapName_.empty()) {
    gsfTrackMap_ = OS()->get<TrackMap>(gsfTrackMapName_);
    if (gsfTrackMap_)
      AddBranchDep(mitName_,gsfTrackMap_->GetBrName());
  }
  if (!trackerTrackMapName_.empty()) {
    trackerTrackMap_ = OS()->get<TrackMap>(trackerTrackMapName_);
    if (trackerTrackMap_)
      AddBranchDep(mitName_,trackerTrackMap_->GetBrName());
  }
}

//--------------------------------------------------------------------------------------------------
void FillerPATElectrons::FillDataBlock(const edm::Event      &event, 
                                       const edm::EventSetup &setup)
{
  // Fill electrons from edm collection into our collection.  

  electrons_->Delete();
  
  edm::Handle<edm::View<pat::Electron> > electronHandle;
  event.getByLabel(edm::InputTag(edmName_),electronHandle);

  edm::View<pat::Electron> electrons = *electronHandle;

  for(edm::View<pat::Electron>::const_iterator iM = electrons.begin(); 
      iM!=electrons.end(); ++iM){
    
    if(iM->pt()>0) {
      
      mithep::Electron *outElectron = electrons_->AddNew();
      
      if (gsfTrackMap_ && iM->gsfTrack().isNonnull()) 
	outElectron->SetGsfTrk(gsfTrackMap_->GetMit(refToPtr(iM->gsfTrack())));
      if (trackerTrackMap_ && iM->track().isNonnull()) 
	outElectron->SetTrackerTrk(trackerTrackMap_->GetMit(refToPtr(iM->track())));
      
      outElectron->SetESuperClusterOverP(iM->eSuperClusterOverP());
      outElectron->SetESeedClusterOverPout(iM->eSeedClusterOverPout());
      outElectron->SetDeltaEtaSuperClusterTrackAtVtx(iM->deltaEtaSuperClusterTrackAtVtx());
      outElectron->SetDeltaEtaSeedClusterTrackAtCalo(iM->deltaEtaSeedClusterTrackAtCalo());
      outElectron->SetDeltaPhiSuperClusterTrackAtVtx(iM->deltaPhiSuperClusterTrackAtVtx());
      outElectron->SetDeltaPhiSeedClusterTrackAtCalo(iM->deltaPhiSeedClusterTrackAtCalo());
      outElectron->SetHadronicOverEm(iM->hadronicOverEm());
      outElectron->SetIsEnergyScaleCorrected(iM->isEnergyScaleCorrected());
      outElectron->SetIsMomentumCorrected(iM->isMomentumCorrected());
      outElectron->SetNumberOfClusters(iM->numberOfClusters());
      outElectron->SetClassification(iM->classification());           
      outElectron->SetCaloIsolation(iM->caloIso());
      outElectron->SetTrackIsolation(iM->trackIso());
    }
  }
  electrons_->Trim();
}
