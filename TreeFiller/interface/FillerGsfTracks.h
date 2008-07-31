//--------------------------------------------------------------------------------------------------
// $Id: FillerGsfTracks.h,v 1.5 2008/07/08 12:38:20 loizides Exp $
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
      FillerGsfTracks(const edm::ParameterSet &cfg, const char *name, bool active=1, const SimParticleMap *sm=0);
      ~FillerGsfTracks();

      void 		 BookDataBlock(TreeWriter &tws);
      void 		 FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
  
    private:
      std::string                      edmName_;               //edm name of gsftracks collection
      std::string                      mitName_;               //name of Tracks in OAK
      std::string                      edmSimAssociationName_; //edm name of sim association map
      std::string                      simMapName_;            //name of inported map wrt simparts
      std::string                      trackMapName_;          //name of export map
      const mithep::SimParticleMap    *simMap_;                //map wrt simulated particles
      mithep::Array<mithep::Track>    *tracks_;                //array of Tracks
      mithep::GsfTrackMap             *trackMap_;              //map wrt tracks
  };
}
#endif
