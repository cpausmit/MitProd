//--------------------------------------------------------------------------------------------------
// FillerTracks
//
// Implementation of a filler to fill EDM tracks into our mithep::Track data structure.
//
// Authors: J.Bendavid, C.Loizides, Y.Iiyama
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERTRACKS_H
#define MITPROD_TREEFILLER_FILLERTRACKS_H

#include "MitProd/TreeFiller/interface/BaseFiller.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"
#include "MitProd/TreeFiller/interface/HitPatternReader.h"
#include "MitAna/DataTree/interface/TrackFwd.h"

#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/RecoCandidate/interface/TrackAssociation.h"
#include "DataFormats/Common/interface/View.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/MuonReco/interface/Muon.h"

class TrackAssociatorParameters;
class TrackDetectorAssociator;
class MagneticField;

namespace mithep 
{
  class FillerTracks : public BaseFiller {  
    public:
      FillerTracks(const edm::ParameterSet &cfg, edm::ConsumesCollector&, ObjectService*, const char *name, bool active=1);
      ~FillerTracks();

      void BookDataBlock(TreeWriter&) override;
      void PrepareLinks() override;
      void FillDataBlock(const edm::Event&, const edm::EventSetup&) override;
      void ResolveLinks(const edm::Event&, const edm::EventSetup&) override;

    protected:
      static void InitLayerMap(std::map<uint32_t, mithep::Track::EHitLayer>&);
      mithep::Track* ProduceTrack(reco::Track const&);
      void EcalAssoc(mithep::Track*, reco::Track const&, edm::Event const&, edm::EventSetup const&, TrackDetectorAssociator&, MagneticField const&);

      typedef edm::View<reco::Track> TrackView;
      typedef edm::View<reco::GsfElectron> ElectronView;
      typedef edm::View<reco::Muon> MuonView;

      enum SourceType {
        kTracks,        // 0
        kElectronGsf,   // 1
        kElectronCtf,   // 2
        kMuonInner,     // 3
        kMuonStandalone,// 4
        kMuonCombined,  // 5
        kMuonTPFMS,     // 6
        kMuonPicky,     // 7
        kMuonDYT,       // 8
        nSourceTypes
      };        

      SourceType                          sourceType_;                  //what we are filling tracks from
      bool                                ecalAssocActive_;             //do track-ecal associations
      edm::EDGetTokenT<TrackView> edmToken_;              //edm name of tracks coll
      edm::EDGetTokenT<ElectronView> edmElectronToken_;//when filling from embedded tracks in PAT electron
      edm::EDGetTokenT<MuonView> edmMuonToken_;           //when filling from embedded tracks in PAT muon
      edm::EDGetTokenT<reco::RecoToSimCollection> edmSimAssocToken_;    //edm name of sim assoc map
      std::string                         mitName_;                     //mit name of Tracks
      std::string                         trackingMapName_;             //name of imp. map wrt sim
      std::string                         barrelSuperClusterIdMapName_; //name of barrel sc id map
      std::string                         endcapSuperClusterIdMapName_; //name of endcap sc id map
      std::string                         trackMapName_;                //name of export map
      const mithep::TrackingParticleMap*  trackingMap_;                 //map wrt sim. particles
      const mithep::SuperClusterIdMap*    barrelSuperClusterIdMap_;     //barrel sc id map
      const mithep::SuperClusterIdMap*    endcapSuperClusterIdMap_;     //endcap sc id map
      mithep::TrackArr*                   tracks_;                      //array of tracks
      mithep::HitPatternReader            hitReader_;                   //hit pattern reader
      TrackAssociatorParameters*          assocParams_;                 //track associator params

    private:
      mithep::TrackMap*                   trackMap_;                    //map wrt tracks
      mithep::ElectronTrackMap*           eleTrackMap_;                 //map GsfElectron -> GsfTrack (when filling from PAT)
      mithep::MuonTrackMap*               muTrackMap_;                  //map Muon -> Track (when filling from PAT)
  };
}
#endif
