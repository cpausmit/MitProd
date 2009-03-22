//--------------------------------------------------------------------------------------------------
// $Id: FillerTracks.h,v 1.20 2009/03/18 14:58:18 loizides Exp $
//
// FillerTracks
//
// Implementation of a filler to fill EDM tracks into our mithep::Track data structure.
//
// Authors: J.Bendavid, C.Loizides
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERTRACKS_H
#define MITPROD_TREEFILLER_FILLERTRACKS_H

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "TrackingTools/TrackAssociator/interface/TrackAssociatorParameters.h"
#include "MitAna/DataUtil/interface/TreeWriter.h"
#include "MitAna/DataTree/interface/Collections.h"
#include "MitAna/DataTree/interface/Track.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"
#include "MitProd/TreeFiller/interface/HitPatternReader.h"
#include "MitAna/DataTree/interface/Names.h"

namespace mithep 
{
  class FillerTracks : public BaseFiller
  {  
    public:
      FillerTracks(const edm::ParameterSet &cfg, const char *name, bool active=1);
      virtual ~FillerTracks();

      void                                BookDataBlock(TreeWriter &tws);
      void                                FillDataBlock(const edm::Event &e, 
                                                        const edm::EventSetup &es);

    protected:
      static void                         InitLayerMap(std::map<uint32_t,
                                                       mithep::Track::EHitLayer> &layerMap);

      bool                                ecalAssocActive_;             //do track-ecal associations
      std::string                         edmName_;                     //edm name of tracks coll
      std::string                         mitName_;                     //mit name of Tracks
      std::string                         edmSimAssocName_;             //edm name of sim assoc map
      std::string                         trackingMapName_;             //name of imp. map wrt sim
      std::string                         barrelSuperClusterIdMapName_; //name of barrel sc id map
      std::string                         endcapSuperClusterIdMapName_; //name of endcap sc id map
      std::string                         trackMapName_;                //name of export map
      const mithep::TrackingParticleMap  *trackingMap_;                 //map wrt sim. particles
      const mithep::SuperClusterIdMap    *barrelSuperClusterIdMap_;     //barrel sc id map
      const mithep::SuperClusterIdMap    *endcapSuperClusterIdMap_;     //endcap sc id map
      mithep::TrackArr                   *tracks_;                      //array of tracks
      HitPatternReader                    hitReader_;                   //hit pattern reader
      TrackAssociatorParameters           assocParams_;                 //track associator params

    private:
      mithep::TrackMap                   *trackMap_;                    //map wrt tracks
  };
}
#endif
