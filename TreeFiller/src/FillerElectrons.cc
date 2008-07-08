// $Id: FillerElectrons.cc,v 1.6 2008/07/07 16:14:01 loizides Exp $

#include "MitProd/TreeFiller/interface/FillerElectrons.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/EgammaCandidates/interface/PixelMatchGsfElectron.h"
#include "DataFormats/EgammaCandidates/interface/PixelMatchGsfElectronFwd.h"

#include "MitAna/DataTree/interface/Track.h"
#include "MitAna/DataTree/interface/Names.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerElectrons::FillerElectrons(const edm::ParameterSet &cfg, bool active,
                                 const GsfTrackMap* gsfTrackMap, const TrackMap* trackerTrackMap) : 
  BaseFiller(cfg, "Electrons", active),
  edmName_(Conf().getUntrackedParameter<string>("edmName","pixelMatchGsfElectrons")),
  mitName_(Conf().getUntrackedParameter<string>("mitName",Names::gkElectronBrn)),
  electrons_(new mithep::ElectronArr),
  gsfTrackMap_(gsfTrackMap),
  trackerTrackMap_(trackerTrackMap)
{
  // Constructor.
}

//--------------------------------------------------------------------------------------------------
FillerElectrons::~FillerElectrons()
{
  // Destructor.

  delete electrons_;
}

//--------------------------------------------------------------------------------------------------
void FillerElectrons::BookDataBlock(TreeWriter &tws)
{
  // Add electron branch to our tree.

  tws.AddBranch(mitName_.c_str(),&electrons_);
}

//--------------------------------------------------------------------------------------------------
void FillerElectrons::FillDataBlock(const edm::Event      &event, 
				    const edm::EventSetup &setup)
{
  // Fill electrons from edm collection into our collection.

  electrons_->Reset();
  
  Handle<reco::PixelMatchGsfElectronCollection> hElectronProduct;
  GetProduct(edmName_, hElectronProduct, event);
  
  const reco::PixelMatchGsfElectronCollection inElectrons = *(hElectronProduct.product());
  
  for (reco::PixelMatchGsfElectronCollection::const_iterator iM = inElectrons.begin(); 
       iM != inElectrons.end(); ++iM) {

    mithep::Electron* outElectron = electrons_->AddNew();
    if (gsfTrackMap_ && iM->gsfTrack().isNonnull()) 
      outElectron->SetGsfTrack(gsfTrackMap_->GetMit(iM->gsfTrack()));
    if (trackerTrackMap_ && iM->track().isNonnull()) 
      outElectron->SetTrackerTrack(trackerTrackMap_->GetMit(iM->track()));
  }

  electrons_->Trim();
}
