// $Id: FillerMuons.cc,v 1.2 2008/07/01 21:11:47 loizides Exp $

#include "MitProd/TreeFiller/interface/FillerMuons.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "MitAna/DataTree/interface/Muon.h"
#include "MitAna/DataTree/interface/Track.h"
#include "MitAna/DataTree/interface/Names.h"

using namespace std;
using namespace edm;
using namespace mithep;

//-------------------------------------------------------------------------------------------------
FillerMuons::FillerMuons(const edm::ParameterSet &cfg, bool active,
                         const TrackMap *globalMap, const TrackMap *stdMap, 
                         const TrackMap *stdVtxMap, const TrackMap* trackerMap) : 
  BaseFiller(cfg, "Muons", active),
  edmName_(Conf().getUntrackedParameter<string>("edmName","muons")),
  mitName_(Conf().getUntrackedParameter<string>("mitName",Names::gkMuonBrn)),
  muons_(new mithep::MuonArr), 
  globalTrackMap_(globalMap), 
  standaloneTrackMap_(stdMap), 
  standaloneVtxTrackMap_(stdVtxMap), 
  trackerTrackMap_(trackerMap)
{
  // Constructor.
}

//-------------------------------------------------------------------------------------------------
FillerMuons::~FillerMuons()
{
  // Destructor.
}

//-------------------------------------------------------------------------------------------------
void FillerMuons::BookDataBlock(TreeWriter &tws)
{
  // Add muons branch to tree.

  tws.AddBranch(mitName_.c_str(),&muons_);
}

//-------------------------------------------------------------------------------------------------
void FillerMuons::FillDataBlock(const edm::Event      &event, 
				      const edm::EventSetup &setup)
{
  // Fill muon information. 

  muons_->Reset();
  
  // get the muons collection
  try {
    event.getByLabel(edm::InputTag(edmName_),muonProduct_);
  } catch (cms::Exception& ex) {
    edm::LogError("FillerMuons") << "Error! Cannot get collection with label " 
				       << edmName_ << endl;
    throw edm::Exception(edm::errors::Configuration, "FillerMuons:FillDataBlock()\n")
      << "Error! Cannot get collection with label " << edmName_ << endl;
  }
  
  const reco::MuonCollection inMuons = *(muonProduct_.product());  
  
  for (reco::MuonCollection::const_iterator iM = inMuons.begin(); iM != inMuons.end(); ++iM) {

    mithep::Muon* outMuon = muons_->AddNew();

    if (globalTrackMap_ && iM->combinedMuon().isNonnull()) 
      outMuon->SetGlobalTrack(globalTrackMap_->GetMit(iM->combinedMuon()));

    if (standaloneTrackMap_ && standaloneVtxTrackMap_ && iM->standAloneMuon().isNonnull()) { 
      Int_t refProductId = iM->standAloneMuon().id().id();
      if ( refProductId == standaloneVtxTrackMap_->GetEdmProductId())
        outMuon->SetStandaloneTrack(standaloneVtxTrackMap_->GetMit(iM->standAloneMuon()));
      else if ( refProductId == standaloneTrackMap_->GetEdmProductId())
        outMuon->SetStandaloneTrack(standaloneTrackMap_->GetMit(iM->standAloneMuon()));
      else throw edm::Exception(edm::errors::Configuration, "FillerMuons:FillDataBlock()\n")
        << "Error! Track reference in unmapped collection " << edmName_ << endl;
    }
    if (trackerTrackMap_ && iM->track().isNonnull()) 
      outMuon->SetTrackerTrack(trackerTrackMap_->GetMit(iM->track()));
  }

  muons_->Trim();
}
