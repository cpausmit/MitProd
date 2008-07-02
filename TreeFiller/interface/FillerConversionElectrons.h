//--------------------------------------------------------------------------------------------------
// $Id: FillerGlobalMuons.h,v 1.3 2008/06/18 19:17:21 loizides Exp $
//
// FillerConversionElectrons
//
// Implementation of a filler create mithep::Electron objects from the conversion finder tracks.
// This filler differs significantly from the others to deal with the special way in which
// conversions are stored in the edm.
//
// Authors: J. Bendavid
//--------------------------------------------------------------------------------------------------

#ifndef TREEFILLER_FILLERCONVERSIONELECTRONS_H
#define TREEFILLER_FILLERCONVERSIONELECTRONS_H

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "MitAna/DataUtil/interface/TreeWriter.h"
#include "DataFormats/Common/interface/Handle.h"
#include "MitAna/DataTree/interface/Track.h"
#include "MitAna/DataTree/interface/Electron.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "MitAna/DataTree/interface/Array.h"
#include "MitAna/DataTree/interface/Collections.h"
#include "MitProd/TreeService/interface/TreeService.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"
#include "MitProd/TreeFiller/interface/AssociationMap.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"

using namespace std;
using namespace mithep;



namespace mithep 
{
  class FillerConversionElectrons : public BaseFiller
  {  
    public:
      FillerConversionElectrons(const edm::ParameterSet&, bool active, const mithep::TrackCol* conversionInOutTracks, const mithep::TrackCol* conversionOutInTracks, const mithep::TrackMap* conversionInOutTrackMap, const mithep::TrackMap* conversionOutInTrackMap);
      ~FillerConversionElectrons();

      void BookDataBlock(TreeWriter &tws);
      void FillDataBlock(const edm::Event&, const edm::EventSetup&);
      void FillFromTracks(const mithep::TrackCol* tracks, const TrackMap* trackMap);
      
      const ConversionElectronMap*              GetConversionElectronMap() { return convElectronMap_; }
  
    private:
      std::string                               mitName_;
      mithep::ElectronArr                      *conversionElectrons_;
      mithep::ConversionElectronMap*            convElectronMap_;
      const mithep::TrackCol*                   conversionInOutTracks_;
      const mithep::TrackCol*                   conversionOutInTracks_;
      const mithep::TrackMap*                   conversionInOutTrackMap_;
      const mithep::TrackMap*                   conversionOutInTrackMap_;

  };
}
#endif
