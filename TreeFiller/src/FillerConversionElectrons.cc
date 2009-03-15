// $Id: FillerConversionElectrons.cc,v 1.9 2009/03/10 15:56:01 loizides Exp $

#include "MitProd/TreeFiller/interface/FillerConversionElectrons.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "MitAna/DataTree/interface/Names.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerConversionElectrons::FillerConversionElectrons(const edm::ParameterSet &cfg, 
                                                     const char *name, bool active) :
  BaseFiller(cfg,name,active),
  mitName_(Conf().getUntrackedParameter<string>("mitName","ConversionElectrons")),
  convInOutTracksName_(Conf().getUntrackedParameter<string>("convInOutTracksName","")),
  convOutInTracksName_(Conf().getUntrackedParameter<string>("convOutInTracksName","")),
  convInOutTrackMapName_(Conf().getUntrackedParameter<string>("convInOutTrackMapName","")),
  convOutInTrackMapName_(Conf().getUntrackedParameter<string>("convOutInTrackMapName","")),
  convElectronMapName_(Conf().getUntrackedParameter<string>("convElectronMapName",
                                                            Form("%sMapName",mitName_.c_str()))),
  convInOutTracks_(0),
  convOutInTracks_(0),
  convInOutTrackMap_(0),
  convOutInTrackMap_(0),
  convElectrons_(new mithep::ElectronArr(16)),
  convElectronMap_(new mithep::ConversionElectronMap)
{
  // Constructor.
}

//--------------------------------------------------------------------------------------------------
FillerConversionElectrons::~FillerConversionElectrons()
{
  // Destructor.

  delete convElectrons_;
  delete convElectronMap_;
}

//--------------------------------------------------------------------------------------------------
void FillerConversionElectrons::BookDataBlock(TreeWriter &tws)
{
  // Add converted electron branch to tree. Publish and get our objects.

  tws.AddBranch(mitName_,&convElectrons_);
  OS()->add<ElectronArr>(convElectrons_,mitName_);

  if (!convElectronMapName_.empty()) {
    convElectronMap_->SetBrName(mitName_);
    OS()->add<ConversionElectronMap>(convElectronMap_,convElectronMapName_);
  }
  if (!convInOutTracksName_.empty()) 
    convInOutTracks_ = OS()->get<TrackArr>(convInOutTracksName_);
  if (!convOutInTracksName_.empty()) 
    convOutInTracks_ = OS()->get<TrackArr>(convOutInTracksName_);
  if (!convInOutTrackMapName_.empty()) {
    convInOutTrackMap_ = OS()->get<TrackMap>(convInOutTrackMapName_);
    if (convInOutTrackMap_)
      AddBranchDep(mitName_,convInOutTrackMap_->GetBrName());
  }
  if (!convOutInTrackMapName_.empty()) {
    convOutInTrackMap_ = OS()->get<TrackMap>(convOutInTrackMapName_);
    if (convOutInTrackMap_)
      AddBranchDep(mitName_,convOutInTrackMap_->GetBrName());
  }
}

//--------------------------------------------------------------------------------------------------
void FillerConversionElectrons::FillDataBlock(const edm::Event      &event, 
                                              const edm::EventSetup &setup)
{
  // Fill converted electron array.

  convElectrons_->Delete();
  convElectronMap_->Reset();
  FillFromTracks(convInOutTracks_, convInOutTrackMap_);
  FillFromTracks(convOutInTracks_, convOutInTrackMap_);
  convElectrons_->Trim();
}

//--------------------------------------------------------------------------------------------------
void FillerConversionElectrons::FillFromTracks(const TrackCol *tracks, 
                                               const TrackMap *trackMap) 
{
  // Obtain information from tracks.

  if (!tracks || !trackMap)
    return;

  for (UInt_t i=0; i<tracks->GetEntries(); ++i) {
    mithep::Track *track = const_cast<Track*>(tracks->At(i));
    mithep::Electron *electron = convElectrons_->AddNew();
    electron->SetTrackerTrk(track);
    convElectronMap_->Add(trackMap->GetEdm(track), electron);
  }
}
