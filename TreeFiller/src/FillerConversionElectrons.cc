// $Id: FillerConversionElectrons.cc,v 1.2 2008/07/03 07:56:14 loizides Exp $

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
FillerConversionElectrons::FillerConversionElectrons(const edm::ParameterSet &cfg, bool active, 
                                                     const TrackCol *convInOutTracks, 
                                                     const TrackCol *convOutInTracks, 
                                                     const TrackMap *convInOutTrackMap, 
                                                     const TrackMap *convOutInTrackMap) : 
  BaseFiller(cfg, "ConversionElectrons", active),
  mitName_(Conf().getUntrackedParameter<string>("mitName","")),
  conversionElectrons_(new mithep::ElectronArr),
  convElectronMap_(new mithep::ConversionElectronMap),
  conversionInOutTracks_(convInOutTracks),
  conversionOutInTracks_(convOutInTracks),
  conversionInOutTrackMap_(convInOutTrackMap),
  conversionOutInTrackMap_(convOutInTrackMap)
{
  // Constructor.
}

//--------------------------------------------------------------------------------------------------
FillerConversionElectrons::~FillerConversionElectrons()
{
  // Destructor.

  delete conversionElectrons_;
  delete convElectronMap_;
}

//--------------------------------------------------------------------------------------------------
void FillerConversionElectrons::BookDataBlock(TreeWriter &tws)
{
  // Add converted electron branch to tree.

  tws.AddBranch(mitName_.c_str(),&conversionElectrons_);
}

//--------------------------------------------------------------------------------------------------
void FillerConversionElectrons::FillDataBlock(const edm::Event      &event, 
                                              const edm::EventSetup &setup)
{
  // Fill converted electron array.

  conversionElectrons_->Reset();
  convElectronMap_->Reset();
  FillFromTracks(conversionInOutTracks_, conversionInOutTrackMap_);
  FillFromTracks(conversionOutInTracks_, conversionOutInTrackMap_);
  conversionElectrons_->Trim();
}

//--------------------------------------------------------------------------------------------------
void FillerConversionElectrons::FillFromTracks(const TrackCol *tracks, 
                                               const TrackMap *trackMap) 
{
  // Obtain information from tracks.

  for (UInt_t i=0; i<tracks->GetEntries(); ++i) {
    mithep::Track *track = const_cast<Track*>(tracks->At(i));
    mithep::Electron* electron = conversionElectrons_->AddNew();
    electron->SetTrackerTrack(track);
    convElectronMap_->Add(trackMap->GetEdmRef(track), electron);
  }
}
