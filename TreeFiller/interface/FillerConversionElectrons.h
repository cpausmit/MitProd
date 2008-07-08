//--------------------------------------------------------------------------------------------------
// $Id: FillerConversionElectrons.h,v 1.3 2008/07/07 16:14:01 loizides Exp $
//
// FillerConversionElectrons
//
// Implementation of a filler creating mithep::Electron objects from the conversion finder tracks.
// This filler differs significantly from the others to deal with the special way in which
// conversions are stored in the edm.
//
// Authors: J.Bendavid
//--------------------------------------------------------------------------------------------------

#ifndef TREEFILLER_FILLERCONVERSIONELECTRONS_H
#define TREEFILLER_FILLERCONVERSIONELECTRONS_H

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "MitAna/DataUtil/interface/TreeWriter.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "MitAna/DataTree/interface/Collections.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"

namespace mithep 
{
  class FillerConversionElectrons : public BaseFiller
  {  
    public:
      FillerConversionElectrons(const edm::ParameterSet &cfg, bool active=1, 
                                const mithep::TrackCol *convInOutTracks=0, 
                                const mithep::TrackCol *convOutInTracks=0, 
                                const mithep::TrackMap *convInOutTrackMap=0, 
                                const mithep::TrackMap *convOutInTrackMap=0);
      ~FillerConversionElectrons();

      void                         BookDataBlock(TreeWriter &tws);
      void                         FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
      void                         FillFromTracks(const mithep::TrackCol *tracks, 
                                                  const TrackMap *trackMap);
      
      const ConversionElectronMap *GetConversionElectronMap() const { return convElectronMap_; }
  
    private:
      std::string                               mitName_;
      mithep::ElectronArr                      *conversionElectrons_;
      mithep::ConversionElectronMap            *convElectronMap_;
      const mithep::TrackCol                   *conversionInOutTracks_;
      const mithep::TrackCol                   *conversionOutInTracks_;
      const mithep::TrackMap                   *conversionInOutTrackMap_;
      const mithep::TrackMap                   *conversionOutInTrackMap_;
  };
}
#endif
