//--------------------------------------------------------------------------------------------------
// $Id: FillerElectrons.h,v 1.27 2012/03/11 23:11:55 pharris Exp $
//
// FillerElectrons
//
// Implementation of a filler to fill EDM electrons into our mithep::Electron data structure.
//
// Authors: C.Paus, P.Harris, J.Bendavid, S.Xie
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERELECTRONS_H
#define MITPROD_TREEFILLER_FILLERELECTRONS_H

#include "MitAna/DataTree/interface/ElectronFwd.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"
#include "MitEdm/DataFormats/interface/Collections.h"

#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrackFwd.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "DataFormats/BeamSpot/interface/BeamSpot.h"

namespace mithep
{
  class FillerElectrons : public BaseFiller
  {
    public:
      FillerElectrons(const edm::ParameterSet &cfg, edm::ConsumesCollector&, ObjectService*, const char *name, bool active=1);
      ~FillerElectrons();

      void                           BookDataBlock(TreeWriter &tws);
      void                           FillDataBlock(const edm::Event &e,
                                                   const edm::EventSetup &es);
    private:
      edm::EDGetTokenT<reco::GsfElectronCollection> edmToken_;                   //edm name of electrons collection
      edm::EDGetTokenT<reco::VertexCollection> pvEdmToken_;                 //name of primary vertex collection
      edm::EDGetTokenT<reco::VertexCollection> pvBSEdmToken_;               //name of bs-constrained pv collection
      edm::EDGetTokenT<edm::ValueMap<float> > eIDCutBasedTightToken_;      //name of tight cut eID algo
      edm::EDGetTokenT<edm::ValueMap<float> > eIDCutBasedLooseToken_;      //name of loose cut eID algo
      edm::EDGetTokenT<edm::ValueMap<float> > eIDLikelihoodToken_;         //name of likelihood cut eID algo
      edm::EDGetTokenT<reco::TrackCollection> generalTracksToken_;
      edm::EDGetTokenT<reco::GsfTrackCollection> gsfTracksToken_;
      edm::EDGetTokenT<mitedm::DecayPartCol> conversionsToken_;
      edm::EDGetTokenT<EcalRecHitCollection> ebRecHitsToken_;
      edm::EDGetTokenT<EcalRecHitCollection> eeRecHitsToken_;
      edm::EDGetTokenT<reco::BeamSpot> beamSpotToken_;     //It is very likely unnecessary to prepare three
      edm::EDGetTokenT<reco::BeamSpot> pvBeamSpotToken_;   //different BeamSpot tokens since there is basically
      edm::EDGetTokenT<reco::BeamSpot> pvbsBeamSpotToken_; //always only one. Just following the 53X implementation.

      std::string                    mitName_;                   //mit name of Electrons collection
      std::string                    gsfTrackMapName_;           //name of imported map wrt gsf trks
      std::string                    trackerTrackMapName_;       //name of imported map wrt trk trks
      std::string                    barrelSuperClusterMapName_; //name of imp. map wrt barrel sclus
      std::string                    endcapSuperClusterMapName_; //name of imp. map wrt endcap sclus
      bool                           checkClusterActive_;
      std::string                    pfEcalBarrelSuperClusterMapName_; //name of imp. map wrt ecal-only pflow sclus
      std::string                    pfEcalEndcapSuperClusterMapName_; //name of imp. map wrt ecal-only pflow sclus
      std::string                    electronMapName_;           //name of exported electron map
      bool                           recomputeConversionInfo_;   //recompute conversion info
      bool                           fitUnbiasedVertex_;         //recompute vertex position without electron
      mithep::ElectronMap           *electronMap_;               //exported electron map
      mithep::ElectronArr           *electrons_;                 //array of Electrons
      const mithep::TrackMap        *gsfTrackMap_;               //map wrt gsf tracks
      const mithep::TrackMap        *trackerTrackMap_;           //map wrt tracker tracks
      const mithep::SuperClusterMap *barrelSuperClusterMap_;     //map wrt barrel super clusters
      const mithep::SuperClusterMap *endcapSuperClusterMap_;     //map wrt endcap super clusters
      const mithep::SuperClusterMap *pfEcalBarrelSuperClusterMap_;     //map wrt pflow super clusters
      const mithep::SuperClusterMap *pfEcalEndcapSuperClusterMap_;     //map wrt pflow super clusters
  };
}
#endif
