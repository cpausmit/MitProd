//--------------------------------------------------------------------------------------------------
// $Id: FillerTracks.h,v 1.23 2009/09/25 08:42:50 loizides Exp $
//
// FillerTracks
//
// Implementation of a filler to fill EDM tracks into our mithep::Track data structure.
//
// Authors: J.Bendavid, C.Loizides
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERTRACKS_H
#define MITPROD_TREEFILLER_FILLERTRACKS_H

#include "TrackingTools/TrackAssociator/interface/TrackAssociatorParameters.h"
#include "MitAna/DataTree/interface/TrackFwd.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"
#include "MitProd/TreeFiller/interface/HitPatternReader.h"

#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/Common/interface/View.h"
#include "DataFormats/RecoCandidate/interface/TrackAssociation.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"

namespace mithep 
{
  class FillerTracks : public BaseFiller
  {  
    public:
      FillerTracks(const edm::ParameterSet &cfg, edm::ConsumesCollector&, ObjectService*, const char *name, bool active=1);
      ~FillerTracks();

      void                                BookDataBlock(TreeWriter &tws);
      void                                FillDataBlock(const edm::Event &e, 
                                                        const edm::EventSetup &es);

    protected:
      static void                         InitLayerMap(std::map<uint32_t,
                                                       mithep::Track::EHitLayer> &layerMap);

      bool                                fromPATElectron_;             //when filling from embedded tracks in PAT electron
      bool                                ecalAssocActive_;             //do track-ecal associations
      edm::EDGetTokenT<edm::View<reco::Track> > edmToken_;                     //edm name of tracks coll
      edm::EDGetTokenT<edm::View<reco::GsfElectron> > edmElectronToken_; //when filling from embedded tracks in PAT electron
      edm::EDGetTokenT<reco::RecoToSimCollection> edmSimAssocToken_;             //edm name of sim assoc map
      std::string                         mitName_;                     //mit name of Tracks
      std::string                         trackingMapName_;             //name of imp. map wrt sim
      std::string                         barrelSuperClusterIdMapName_; //name of barrel sc id map
      std::string                         endcapSuperClusterIdMapName_; //name of endcap sc id map
      std::string                         trackMapName_;                //name of export map
      const mithep::TrackingParticleMap  *trackingMap_;                 //map wrt sim. particles
      const mithep::SuperClusterIdMap    *barrelSuperClusterIdMap_;     //barrel sc id map
      const mithep::SuperClusterIdMap    *endcapSuperClusterIdMap_;     //endcap sc id map
      mithep::TrackArr                   *tracks_;                      //array of tracks
      mithep::HitPatternReader            hitReader_;                   //hit pattern reader
      TrackAssociatorParameters           assocParams_;                 //track associator params

    private:
      mithep::TrackMap                   *trackMap_;                    //map wrt tracks
      mithep::ElectronTrackMap           *eleTrackMap_;                 //map GsfElectron -> GsfTrack (when filling from PAT)
  };
}
#endif
