//--------------------------------------------------------------------------------------------------
// $Id: FillerGsfTracks.h,v 1.1 2008/07/01 14:38:33 loizides Exp $
//
// FillerGsfTracks
//
// Imlementation of a filler to fill EDM gsf tracks into our mithep::Track data structure.
//
// Authors: J.Bendavid
//--------------------------------------------------------------------------------------------------

#ifndef TREEFILLER_FILLERGSFTRACKS_H
#define TREEFILLER_FILLERGSFTRACKS_H

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "MitAna/DataUtil/interface/TreeWriter.h"
#include "MitAna/DataTree/interface/Track.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrackFwd.h"
#include "MitAna/DataTree/interface/Collections.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"

namespace mithep 
{
  class FillerGsfTracks : public BaseFiller
  {  
    public:
      FillerGsfTracks(const edm::ParameterSet &cfg, const char *name, 
                      bool active=1, const SimParticleMap *sm=0);
      ~FillerGsfTracks();

      void 		 BookDataBlock(TreeWriter &tws);
      void 		 FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
      const GsfTrackMap *GetTrackMap() { return trackMap_; }
      const TrackCol    *GetTrackCol() { return tracks_; }
  
    private:
      std::string                            edmName_;
      std::string                            edmDataName_;
      std::string                            mitName_;
      std::string			     edmSimAssociationName_;
      const mithep::SimParticleMap          *simMap_;
      mithep::Array<mithep::Track>          *tracks_;
      mithep::GsfTrackMap                   *trackMap_;
      edm::Handle<reco::GsfTrackCollection>  trackProduct_;
  };
}
#endif
