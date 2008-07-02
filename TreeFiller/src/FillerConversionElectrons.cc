// $Id: FillerGlobalMuons.cc,v 1.3 2008/06/18 19:17:21 loizides Exp $

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "MitAna/DataTree/interface/Electron.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"


#include "MitAna/DataTree/interface/Track.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitProd/TreeFiller/interface/FillerConversionElectrons.h"

using namespace std;
using namespace edm;
using namespace mithep;

//-------------------------------------------------------------------------------------------------
FillerConversionElectrons::FillerConversionElectrons(const edm::ParameterSet& cfg, bool active, const mithep::TrackCol* conversionInOutTracks, const mithep::TrackCol* conversionOutInTracks, const mithep::TrackMap* conversionInOutTrackMap, const mithep::TrackMap* conversionOutInTrackMap) : 
  BaseFiller(cfg, "ConversionElectrons", active),
  mitName_(Conf().getUntrackedParameter<string>("mitName","")),
  conversionElectrons_(new mithep::ElectronArr),
  convElectronMap_(new mithep::ConversionElectronMap),
  conversionInOutTracks_(conversionInOutTracks),
  conversionOutInTracks_(conversionOutInTracks),
  conversionInOutTrackMap_(conversionInOutTrackMap),
  conversionOutInTrackMap_(conversionOutInTrackMap)
{
}

//-------------------------------------------------------------------------------------------------
FillerConversionElectrons::~FillerConversionElectrons()
{
  delete convElectronMap_;
}

//-------------------------------------------------------------------------------------------------
void FillerConversionElectrons::BookDataBlock(TreeWriter &tws)
{
  tws.AddBranch(mitName_.c_str(),&conversionElectrons_);
}

//-------------------------------------------------------------------------------------------------
void FillerConversionElectrons::FillDataBlock(const edm::Event      &event, 
                                      const edm::EventSetup &setup)
{
  // Fill global muon information. Links to the various standalone components are created in the
  // link resolution.

  conversionElectrons_->Reset();
  convElectronMap_->Reset();
  FillFromTracks(conversionInOutTracks_, conversionInOutTrackMap_);
  FillFromTracks(conversionOutInTracks_, conversionOutInTrackMap_);
  conversionElectrons_->Trim();
  
  
}

void FillerConversionElectrons::FillFromTracks(const mithep::TrackCol* tracks, const TrackMap* trackMap) {
        for (UInt_t i=0; i<tracks->GetEntries(); i++) {
                mithep::Track* track = (Track*)tracks->At(i);
                mithep::Electron* electron = conversionElectrons_->AddNew();
                electron->SetTrackerTrack(track);
                convElectronMap_->Add(trackMap->GetEdmRef(track), electron);
        }
}


