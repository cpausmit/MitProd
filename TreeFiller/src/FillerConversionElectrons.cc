// $Id: FillerConversionElectrons.cc,v 1.8 2009/02/26 17:04:03 bendavid Exp $

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

  tws.AddBranch(mitName_.c_str(),&convElectrons_);

  OS()->add<ElectronArr>(convElectrons_,mitName_.c_str());
  OS()->add<ConversionElectronMap>(convElectronMap_,convElectronMapName_.c_str());
  convInOutTracks_ = OS()->get<TrackArr>(convInOutTracksName_.c_str());
  convOutInTracks_ = OS()->get<TrackArr>(convOutInTracksName_.c_str());
  convInOutTrackMap_ = OS()->get<TrackMap>(convInOutTrackMapName_.c_str());
  convOutInTrackMap_ = OS()->get<TrackMap>(convOutInTrackMapName_.c_str());
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

  for (UInt_t i=0; i<tracks->GetEntries(); ++i) {
    mithep::Track *track = const_cast<Track*>(tracks->At(i));
    mithep::Electron *electron = convElectrons_->AddNew();
    electron->SetTrackerTrk(track);
    convElectronMap_->Add(trackMap->GetEdm(track), electron);
  }
}
