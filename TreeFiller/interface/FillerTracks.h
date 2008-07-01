//--------------------------------------------------------------------------------------------------
// $Id: FillerTracks.h,v 1.1 2008/07/01 14:38:33 loizides Exp $
//
// FillerTracks
//
// Imlementation of a filler to fill EDM tracks into our mithep::Track data structure.
//
// Authors: J.Bendavid, C.Loizides
//--------------------------------------------------------------------------------------------------

#ifndef TREEFILLER_FILLERTRACKS_H
#define TREEFILLER_FILLERTRACKS_H

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "MitAna/DataUtil/interface/TreeWriter.h"
#include "MitAna/DataTree/interface/Collections.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"

namespace mithep 
{
  class FillerTracks : public BaseFiller
  {  
    public:
      FillerTracks(const edm::ParameterSet &cfg, const char *name, 
                   bool active=1, const SimParticleMap* sm=0);
      ~FillerTracks();

      void 				  BookDataBlock(TreeWriter &tws);
      void 				  FillDataBlock(const edm::Event &e, 
                                                        const edm::EventSetup &es);
      const TrackMap			 *GetTrackMap() { return trackMap_; }
      const TrackCol	  		 *GetTrackCol() { return tracks_; }
  
    private:
      std::string                         edmName_;
      std::string                         edmDataName_;
      std::string                         mitName_;
      std::string			  edmSimAssociationName_;
      const mithep::SimParticleMap       *simMap_;
      mithep::TrackArr                   *tracks_;
      mithep::TrackMap* 		  trackMap_;
      edm::Handle<reco::TrackCollection>  trackProduct_;
  };
}
#endif
