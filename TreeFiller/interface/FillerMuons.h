//--------------------------------------------------------------------------------------------------
// FillerMuons
//
// Implementation of a filler to fill EDM muons into our mithep::Muon data structure.
//
// Authors: J.Bendavid
//--------------------------------------------------------------------------------------------------
#ifndef MITPROD_TREEFILLER_FILLERMUONS_H
#define MITPROD_TREEFILLER_FILLERMUONS_H

#include "MitAna/DataTree/interface/MuonFwd.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"

#include "DataFormats/Common/interface/View.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/BeamSpot/interface/BeamSpot.h"

namespace mithep 
{
  class FillerMuons : public BaseFiller {  
  public:
    FillerMuons(edm::ParameterSet const&, edm::ConsumesCollector&, mithep::ObjectService*, char const*, bool = true);
    ~FillerMuons();

    void BookDataBlock(TreeWriter &) override;
    void PrepareLinks() override;
    void FillDataBlock(edm::Event const&, edm::EventSetup const&) override;
    void ResolveLinks(edm::Event const&, edm::EventSetup const&) override;

    typedef edm::View<reco::Muon> MuonView;

  private:
    // because someone decided that it's a good idea to have track types ordered differently
    unsigned char RecoToMithep(unsigned char) const;
    unsigned char MithepToReco(unsigned char) const;

    int  NumberOfSegments(reco::Muon const&, int, int,
                          reco::Muon::ArbitrationType = reco::Muon::SegmentAndTrackArbitration);

    // taken from reco::Muon::MuonTrackType
    enum MuonTrackType {
      kInnerTrack,
      kOuterTrack,
      kCombinedTrack,
      kTPFMS,
      kPicky,
      kDYT,
      nMuonTrackTypes
    };

    bool                        fitUnbiasedVertex_;     //recompute vertex position without muon track
    bool                        fillFromPAT_;           //true when filling from PAT (e.g. MiniAOD)

    edm::EDGetTokenT<MuonView> edmToken_;               //edm name of muons collection
    edm::EDGetTokenT<reco::VertexCollection> pvEdmToken_;             //name of primary vertex collection
    edm::EDGetTokenT<reco::VertexCollection> pvBSEdmToken_;           //name of bs-constrained pv collection
    edm::EDGetTokenT<reco::BeamSpot> pvBeamSpotToken_;   //different BeamSpot tokens since there is basically
    edm::EDGetTokenT<reco::BeamSpot> pvbsBeamSpotToken_; //always only one. Just following the 53X implementation.
      
    std::string                 mitName_;               //mit name of Muons
    std::string                 trackMapName_[nMuonTrackTypes];
    std::string                 staVtxTrackMapName_;
    std::string                 muonMapName_;           //name of exported muon map
    std::string                 muonPFMapName_;         //name of exported PF candidate -> muon map (PAT)
    mithep::TrackMap const*     trackMap_[nMuonTrackTypes];
    mithep::TrackMap const*     staVtxTrackMap_;
    mithep::MuonTrackMap const* muonTrackMap_[nMuonTrackTypes];
    mithep::MuonMap*            muonMap_;               //exported muon map
    mithep::CandidateMap*       muonPFMap_;             //exported PF -> muon map (PAT)
    mithep::MuonArr*            muons_;                 //array of Muons
  };
}
#endif
