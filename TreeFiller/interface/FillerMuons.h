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
  class FillerMuons : public BaseFiller
  {  
    public:
      FillerMuons(const edm::ParameterSet &cfg, edm::ConsumesCollector&, ObjectService*, const char *name, bool active=1);
      ~FillerMuons();

      void                        BookDataBlock(TreeWriter &tws);
      void                        FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
      int                         NumberOfSegments(reco::Muon const& inMuon,
						   int station, int muonSubdetId,
						   reco::Muon::ArbitrationType arbitrationType =
						   reco::Muon::SegmentAndTrackArbitration);

      typedef edm::View<reco::Muon> MuonView;

    private:
      edm::EDGetTokenT<MuonView> edmToken_;               //edm name of muons collection
      edm::EDGetTokenT<reco::VertexCollection> pvEdmToken_;             //name of primary vertex collection
      edm::EDGetTokenT<reco::VertexCollection> pvBSEdmToken_;           //name of bs-constrained pv collection
      edm::EDGetTokenT<reco::BeamSpot> beamSpotToken_;     //It is very likely unnecessary to prepare three
      edm::EDGetTokenT<reco::BeamSpot> pvBeamSpotToken_;   //different BeamSpot tokens since there is basically
      edm::EDGetTokenT<reco::BeamSpot> pvbsBeamSpotToken_; //always only one. Just following the 53X implementation.
      
      std::string                 mitName_;               //mit name of Muons
      std::string                 globalTrackMapName_;    //name of imported map wrt global muons
      std::string                 staTrackMapName_;       //name of imported map wrt sta muons
      std::string                 staVtxTrackMapName_;    //name of imported map wrt sta vtx muons
      std::string                 trackerTrackMapName_;   //name of imported map wrt tracker muons
      std::string                 muonMapName_;           //name of exported muon map
      bool                        fitUnbiasedVertex_;     //recompute vertex position without muon track
      const mithep::TrackMap     *globalTrackMap_;        //map wrt global muons
      const mithep::TrackMap     *standaloneTrackMap_;    //map wrt standalone muons
      const mithep::TrackMap     *standaloneVtxTrackMap_; //map wrt standalone vertex muons
      const mithep::TrackMap     *trackerTrackMap_;       //map wrt tracker track muons
      mithep::MuonMap            *muonMap_;               //exported muon map
      mithep::MuonArr            *muons_;                 //array of Muons
  };
}
#endif
