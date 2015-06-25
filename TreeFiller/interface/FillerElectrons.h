//--------------------------------------------------------------------------------------------------
// FillerElectrons
//
// Implementation of a filler to fill EDM electrons into our mithep::Electron data structure.
//
// Authors: C.Paus, P.Harris, J.Bendavid, S.Xie, Y.Iiyama
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERELECTRONS_H
#define MITPROD_TREEFILLER_FILLERELECTRONS_H

#include "MitAna/DataTree/interface/ElectronFwd.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"
#include "MitEdm/DataFormats/interface/Collections.h"

#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/Common/interface/View.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrackFwd.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "DataFormats/BeamSpot/interface/BeamSpot.h"
#include "RecoEgamma/EgammaTools/interface/ConversionInfo.h"

namespace mithep
{
  class FillerElectrons : public BaseFiller
  {
  public:
    FillerElectrons(edm::ParameterSet const&, edm::ConsumesCollector&, ObjectService*, char const*, bool = true);
    ~FillerElectrons();

    void BookDataBlock(mithep::TreeWriter &) override;
    void PrepareLinks() override;
    void FillDataBlock(edm::Event const&, edm::EventSetup const&) override;
    void ResolveLinks(edm::Event const&, edm::EventSetup const&) override;

    typedef edm::View<reco::GsfElectron> GsfElectronView;                 //using View to deal with PAT electrons
    typedef std::vector<reco::PFCandidateRef> PFCandRefV;

  private:
    edm::EDGetTokenT<GsfElectronView> edmToken_;              //edm name of electrons collection
    edm::EDGetTokenT<reco::VertexCollection> pvEdmToken_;                 //name of primary vertex collection
    edm::EDGetTokenT<reco::VertexCollection> pvBSEdmToken_;               //name of bs-constrained pv collection
    edm::EDGetTokenT<edm::ValueMap<float> > eIDCutBasedTightToken_;      //name of tight cut eID algo
    edm::EDGetTokenT<edm::ValueMap<float> > eIDCutBasedLooseToken_;      //name of loose cut eID algo
    edm::EDGetTokenT<edm::ValueMap<float> > eIDLikelihoodToken_;         //name of likelihood cut eID algo
    edm::EDGetTokenT<reco::TrackCollection> generalTracksToken_;
    edm::EDGetTokenT<reco::GsfTrackCollection> gsfTracksToken_;
    edm::EDGetTokenT<mitedm::DecayPartCol> conversionsToken_;
    edm::EDGetTokenT<reco::BeamSpot> pvBeamSpotToken_;   //different BeamSpot tokens since there is basically
    edm::EDGetTokenT<reco::BeamSpot> pvbsBeamSpotToken_; //always only one. Just following the 53X implementation.
    std::string eIDCutBasedTightName_;
    std::string eIDCutBasedLooseName_;
    edm::EDGetTokenT<edm::ValueMap<PFCandRefV> > footprintToken_;

    std::string                    mitName_;                   //mit name of Electrons collection
    std::string                    electronMapName_;           //name of exported electron map
    std::string                    electronPFMapName_;         //name of exported PF->electron map (PAT)
    std::string                    gsfTrackMapName_;           //name of imported map wrt gsf trks
    std::string                    trackerTrackMapName_;       //name of imported map wrt trk trks
    std::string                    barrelSuperClusterMapName_; //name of imp. map wrt barrel sclus
    std::string                    endcapSuperClusterMapName_; //name of imp. map wrt endcap sclus
    bool                           checkClusterActive_;
    std::string                    pfEcalBarrelSuperClusterMapName_; //name of imp. map wrt ecal-only pflow sclus
    std::string                    pfEcalEndcapSuperClusterMapName_; //name of imp. map wrt ecal-only pflow sclus
    std::string                    pfCandidateMapName_;
    bool                           recomputeConversionInfo_;   //recompute conversion info
    bool                           fitUnbiasedVertex_;         //recompute vertex position without electron
    bool                           fillFromPAT_;               //true when filling from PAT (e.g. MiniAOD)
    mithep::ElectronMap*           electronMap_;               //exported electron map
    mithep::CandidateMap*          electronPFMap_;             //exported PF->electron map
    mithep::ElectronArr*           electrons_;                 //array of Electrons
    mithep::TrackMap const*        gsfTrackMap_;               //map wrt gsf tracks
    mithep::TrackMap const*        trackerTrackMap_;           //map wrt tracker tracks
    mithep::SuperClusterMap const* barrelSuperClusterMap_;     //map wrt barrel super clusters
    mithep::SuperClusterMap const* endcapSuperClusterMap_;     //map wrt endcap super clusters
    mithep::SuperClusterMap const* pfEcalBarrelSuperClusterMap_;     //map wrt pflow super clusters
    mithep::SuperClusterMap const* pfEcalEndcapSuperClusterMap_;     //map wrt pflow super clusters
    mithep::PFCandidateMap const*  pfCandidateMap_;
    std::map<mithep::Electron*, ConversionInfo> conversionInfoStore_; //temporary storage to avoid recomputing conversionInfo in FillDataBlock and ResolveRefs
  };
}
#endif
