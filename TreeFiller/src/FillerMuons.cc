#include "MitProd/TreeFiller/interface/FillerMuons.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonQuality.h"
#include "DataFormats/MuonReco/interface/MuonSelectors.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/Common/interface/RefToPtr.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/TransientTrack/plugins/TransientTrackBuilderESProducer.h"
#include "TrackingTools/IPTools/interface/IPTools.h"
#include "RecoVertex/KalmanVertexFit/interface/KalmanVertexTrackCompatibilityEstimator.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataTree/interface/MuonCol.h"
#include "MitAna/DataTree/interface/Track.h"
#include "MitProd/ObjectService/interface/ObjectService.h"
#include "MitEdm/Tools/interface/VertexReProducer.h"

#include <functional>

mithep::FillerMuons::FillerMuons(edm::ParameterSet const& cfg, edm::ConsumesCollector& collector, mithep::ObjectService* os, char const* name, bool active/* = true*/) :
  BaseFiller(cfg, os, name, active),
  fitUnbiasedVertex_        (cfg.getUntrackedParameter<bool>("fitUnbiasedVertex", true)),
  fillFromPAT_              (cfg.getUntrackedParameter<bool>("fillFromPAT", false)),
  edmToken_                 (GetToken<MuonView>(collector, cfg, "edmName")), //muons
  pvEdmToken_               (GetToken<reco::VertexCollection>(collector, cfg, "pvEdmName", false)), //offlinePrimaryVertices
  pvBSEdmToken_             (GetToken<reco::VertexCollection>(collector, cfg, "pvBSEdmName", false)), //offlinePrimaryVerticesWithBS
  pvBeamSpotToken_          (GetToken<reco::BeamSpot>(collector, cfg, "pvBeamSpotName", !pvEdmToken_.isUninitialized() && fitUnbiasedVertex_)), //offlineBeamSpot
  pvbsBeamSpotToken_        (GetToken<reco::BeamSpot>(collector, cfg, "pvbsBeamSpotName", !pvBSEdmToken_.isUninitialized() && fitUnbiasedVertex_)), //offlineBeamSpot
  mitName_                  (cfg.getUntrackedParameter<std::string>("mitName", Names::gkMuonBrn)),
  trackMapName_             {},
  staVtxTrackMapName_       (cfg.getUntrackedParameter<std::string>("staVtxTrackMapName", "")),
  muonMapName_              (cfg.getUntrackedParameter<std::string>("muonMapName", "")),
  muonPFMapName_            (cfg.getUntrackedParameter<std::string>("muonPFMapName", "")),
  trackMap_                 {},
  staVtxTrackMap_           (0),
  muonTrackMap_             {},
  muonMap_                  (new mithep::MuonMap),
  muonPFMap_                (0),
  muons_                    (new mithep::MuonArr(16))
{
  trackMapName_[kInnerTrack] = cfg.getUntrackedParameter<std::string>("trackerTrackMapName", "");
  trackMapName_[kOuterTrack] = cfg.getUntrackedParameter<std::string>("staTrackMapName", "");
  trackMapName_[kCombinedTrack] = cfg.getUntrackedParameter<std::string>("globalTrackMapName", "");
  trackMapName_[kTPFMS] = cfg.getUntrackedParameter<std::string>("firstHitTrackMapName", "");
  trackMapName_[kPicky] = cfg.getUntrackedParameter<std::string>("pickyTrackMapName", "");
  trackMapName_[kDYT] = cfg.getUntrackedParameter<std::string>("dytTrackMapName", "");
}

mithep::FillerMuons::~FillerMuons()
{
  delete muons_;
  delete muonMap_;
  delete muonPFMap_;
}

void
mithep::FillerMuons::BookDataBlock(TreeWriter& tws)
{
  // Add muons branch to tree and get pointers to maps.

  tws.AddBranch(mitName_, &muons_);
  OS()->add(muons_, mitName_);

  if (!muonMapName_.empty()) {
    muonMap_->SetBrName(mitName_);
    OS()->add(muonMap_, muonMapName_);
  }
  if (fillFromPAT_ && !muonPFMapName_.empty()) {
    muonPFMap_ = new mithep::CandidateMap;
    muonPFMap_->SetBrName(mitName_);
    OS()->add(muonPFMap_, muonPFMapName_);
  }
}

void
mithep::FillerMuons::PrepareLinks()
{
  if (fillFromPAT_) {
    for (unsigned iT = 0; iT != nMuonTrackTypes; ++iT) {
      if (!trackMapName_[iT].empty()) {
        muonTrackMap_[iT] = OS()->get<MuonTrackMap>(trackMapName_[iT]);
        if (muonTrackMap_[iT])
          AddBranchDep(mitName_, muonTrackMap_[iT]->GetBrName());
      }
    }
  }
  else {
    for (unsigned iT = 0; iT != nMuonTrackTypes; ++iT) {
      if (!trackMapName_[iT].empty()) {
        trackMap_[iT] = OS()->get<TrackMap>(trackMapName_[iT]);
        if (trackMap_[iT])
          AddBranchDep(mitName_, trackMap_[iT]->GetBrName());
      }
    }
    if (!staVtxTrackMapName_.empty()) {
      staVtxTrackMap_ = OS()->get<TrackMap>(staVtxTrackMapName_);
      if (staVtxTrackMap_)
        AddBranchDep(mitName_, staVtxTrackMap_->GetBrName());
    }
  }
}

void
mithep::FillerMuons::FillDataBlock(edm::Event const& event,
                                   edm::EventSetup const& setup)
{
  // Fill muon information.
  muons_  ->Delete();
  muonMap_->Reset();
  if (muonPFMap_)
    muonPFMap_->Reset();

  edm::Handle<MuonView> hMuonProduct;
  GetProduct(edmToken_, hMuonProduct, event);
  auto& inMuons = *hMuonProduct;

  if (fillFromPAT_ && inMuons.size() != 0 &&
      !dynamic_cast<pat::Muon const*>(&inMuons.at(0)))
    throw edm::Exception(edm::errors::Configuration, "FillerMuons:FillDataBlock()")
      << "Error! Input muon is not PAT";

  edm::ESHandle<TransientTrackBuilder> hTransientTrackBuilder;
  setup.get<TransientTrackRecord>().get("TransientTrackBuilder", hTransientTrackBuilder);
  auto transientTrackBuilder = hTransientTrackBuilder.product();

  KalmanVertexTrackCompatibilityEstimator<5> kalmanEstimator;

  reco::VertexCollection const* vertexCols[2] = {};
  reco::BeamSpot const* beamspots[2] = {};
  VertexReProducer* vtxReProducers[2] = {};

  if (!pvEdmToken_.isUninitialized()) {
    edm::Handle<reco::VertexCollection> hVertex;
    GetProduct(pvEdmToken_, hVertex, event);
    vertexCols[0] = hVertex.product();
    if (fitUnbiasedVertex_) {
      edm::Handle<reco::BeamSpot> pvbeamspotH;
      GetProduct(pvBeamSpotToken_, pvbeamspotH, event);
      beamspots[0] = pvbeamspotH.product();
      vtxReProducers[0] = new VertexReProducer(hVertex, event);
    }
  }

  if (!pvBSEdmToken_.isUninitialized()) {
    edm::Handle<reco::VertexCollection> hVertexBS;
    GetProduct(pvBSEdmToken_, hVertexBS, event);
    vertexCols[1] = hVertexBS.product();
    if (fitUnbiasedVertex_) {
      edm::Handle<reco::BeamSpot> pvbsbeamspotH;
      GetProduct(pvbsBeamSpotToken_, pvbsbeamspotH, event);
      beamspots[1] = pvbsbeamspotH.product();
      vtxReProducers[1] = new VertexReProducer(hVertexBS, event);
    }
  }

  typedef std::function<void(mithep::Muon&, double)> Setter;

  Setter d0pvSetters[] = {
    &mithep::Muon::SetD0PV, &mithep::Muon::SetD0PVBS,
    &mithep::Muon::SetD0PVUB, &mithep::Muon::SetD0PVUBBS
  };
  Setter d0pverrSetters[] = {
    &mithep::Muon::SetD0PVErr, &mithep::Muon::SetD0PVBSErr,
    &mithep::Muon::SetD0PVUBErr, &mithep::Muon::SetD0PVUBBSErr
  };
  Setter ip3dpvSetters[] = {
    &mithep::Muon::SetIp3dPV, &mithep::Muon::SetIp3dPVBS,
    &mithep::Muon::SetIp3dPVUB, &mithep::Muon::SetIp3dPVUBBS
  };
  Setter ip3dpverrSetters[] = {
    &mithep::Muon::SetIp3dPVErr, &mithep::Muon::SetIp3dPVBSErr,
    &mithep::Muon::SetIp3dPVUBErr, &mithep::Muon::SetIp3dPVUBBSErr
  };
  Setter pvcompatSetters[] = {&mithep::Muon::SetPVCompatibility, &mithep::Muon::SetPVBSCompatibility};

  for (auto&& mPtr : inMuons.ptrs()) {
    auto& inMuon = *mPtr;
    auto* outMuon = muons_->AddNew();

    outMuon->SetBestTrkType     (RecoToMithep(inMuon.muonBestTrackType()));
    outMuon->SetTunePBestTrkType(RecoToMithep(inMuon.tunePMuonBestTrackType()));

    outMuon->SetPtEtaPhi        (inMuon.pt(),inMuon.eta(),inMuon.phi());
    outMuon->SetCharge          (inMuon.charge());
    outMuon->SetIsoR03SumPt     (inMuon.isolationR03().sumPt);
    outMuon->SetIsoR03EmEt      (inMuon.isolationR03().emEt);
    outMuon->SetIsoR03HadEt     (inMuon.isolationR03().hadEt);
    outMuon->SetIsoR03HoEt      (inMuon.isolationR03().hoEt);
    outMuon->SetIsoR03NTracks   (inMuon.isolationR03().nTracks);
    outMuon->SetIsoR03NJets     (inMuon.isolationR03().nJets);
    outMuon->SetIsoR05SumPt     (inMuon.isolationR05().sumPt);
    outMuon->SetIsoR05EmEt      (inMuon.isolationR05().emEt);
    outMuon->SetIsoR05HadEt     (inMuon.isolationR05().hadEt);
    outMuon->SetIsoR05HoEt      (inMuon.isolationR05().hoEt);
    outMuon->SetIsoR05NTracks   (inMuon.isolationR05().nTracks);
    outMuon->SetIsoR05NJets     (inMuon.isolationR05().nJets);
    outMuon->SetEmEnergy        (inMuon.calEnergy().em);
    outMuon->SetHadEnergy       (inMuon.calEnergy().had);
    outMuon->SetHoEnergy        (inMuon.calEnergy().ho);
    outMuon->SetEmS9Energy      (inMuon.calEnergy().emS9);
    outMuon->SetHadS9Energy     (inMuon.calEnergy().hadS9);
    outMuon->SetHoS9Energy      (inMuon.calEnergy().hoS9);
    outMuon->SetIsGlobalMuon    (inMuon.isGlobalMuon());
    outMuon->SetIsTrackerMuon   (inMuon.isTrackerMuon());
    outMuon->SetIsStandaloneMuon(inMuon.isStandAloneMuon());
    outMuon->SetIsPFMuon        (inMuon.isPFMuon());
    outMuon->SetIsCaloMuon      (inMuon.isCaloMuon());
    //kink algorithm
    outMuon->SetTrkKink         (inMuon.combinedQuality().trkKink);
    outMuon->SetGlbKink         (inMuon.combinedQuality().glbKink);
    //save results from the muon selector in the MuonQuality bitmask
    outMuon->Quality().SetQuality(MuonQuality::All);
    if (muon::isGoodMuon(inMuon,muon::AllGlobalMuons))
      outMuon->Quality().SetQuality(MuonQuality::AllGlobalMuons);
    if (muon::isGoodMuon(inMuon,muon::AllStandAloneMuons))
      outMuon->Quality().SetQuality(MuonQuality::AllStandAloneMuons);
    if (muon::isGoodMuon(inMuon,muon::AllTrackerMuons))
      outMuon->Quality().SetQuality(MuonQuality::AllTrackerMuons);
    if (muon::isGoodMuon(inMuon,muon::TrackerMuonArbitrated))
      outMuon->Quality().SetQuality(MuonQuality::TrackerMuonArbitrated);
    if (muon::isGoodMuon(inMuon,muon::AllArbitrated))
      outMuon->Quality().SetQuality(MuonQuality::AllArbitrated);
    if (muon::isGoodMuon(inMuon,muon::GlobalMuonPromptTight))
      outMuon->Quality().SetQuality(MuonQuality::GlobalMuonPromptTight);
    if (muon::isGoodMuon(inMuon,muon::TMLastStationLoose))
      outMuon->Quality().SetQuality(MuonQuality::TMLastStationLoose);
    if (muon::isGoodMuon(inMuon,muon::TMLastStationTight))
      outMuon->Quality().SetQuality(MuonQuality::TMLastStationTight);
    if (muon::isGoodMuon(inMuon,muon::TM2DCompatibilityLoose))
      outMuon->Quality().SetQuality(MuonQuality::TM2DCompatibilityLoose);
    if (muon::isGoodMuon(inMuon,muon::TM2DCompatibilityTight))
      outMuon->Quality().SetQuality(MuonQuality::TM2DCompatibilityTight);
    if (muon::isGoodMuon(inMuon,muon::TMOneStationLoose))
      outMuon->Quality().SetQuality(MuonQuality::TMOneStationLoose);
    if (muon::isGoodMuon(inMuon,muon::TMOneStationTight))
      outMuon->Quality().SetQuality(MuonQuality::TMOneStationTight);
    if (muon::isGoodMuon(inMuon,muon::TMLastStationOptimizedLowPtLoose))
      outMuon->Quality().SetQuality(MuonQuality::TMLastStationOptimizedLowPtLoose);
    if (muon::isGoodMuon(inMuon,muon::TMLastStationOptimizedLowPtTight))
      outMuon->Quality().SetQuality(MuonQuality::TMLastStationOptimizedLowPtTight);
    if (muon::isGoodMuon(inMuon,muon::GMTkChiCompatibility))
      outMuon->Quality().SetQuality(MuonQuality::GMTkChiCompatibility);
    if (muon::isGoodMuon(inMuon,muon::GMStaChiCompatibility))
      outMuon->Quality().SetQuality(MuonQuality::GMStaChiCompatibility);
    if (muon::isGoodMuon(inMuon,muon::GMTkKinkTight))
      outMuon->Quality().SetQuality(MuonQuality::GMTkKinkTight);
    if (muon::isGoodMuon(inMuon,muon::TMLastStationAngLoose))
      outMuon->Quality().SetQuality(MuonQuality::TMLastStationAngLoose);
    if (muon::isGoodMuon(inMuon,muon::TMLastStationAngTight))
      outMuon->Quality().SetQuality(MuonQuality::TMLastStationAngTight);
    if (muon::isGoodMuon(inMuon,muon::TMOneStationAngLoose))
      outMuon->Quality().SetQuality(MuonQuality::TMOneStationAngLoose);
    if (muon::isGoodMuon(inMuon,muon::TMOneStationAngTight))
      outMuon->Quality().SetQuality(MuonQuality::TMOneStationAngTight);
    if (muon::isGoodMuon(inMuon,muon::TMLastStationOptimizedBarrelLowPtLoose))
      outMuon->Quality().SetQuality(MuonQuality::TMLastStationOptimizedBarrelLowPtLoose);
    if (muon::isGoodMuon(inMuon,muon::TMLastStationOptimizedBarrelLowPtTight))
      outMuon->Quality().SetQuality(MuonQuality::TMLastStationOptimizedBarrelLowPtTight);

    auto&& combinedMuonRef = inMuon.combinedMuon();

    if (combinedMuonRef.isNonnull())
      outMuon->SetNValidHits(combinedMuonRef->hitPattern().numberOfValidMuonHits());

    auto&& trackRef = inMuon.track();

    // compute impact parameter with respect to PV
    if (trackRef.isNonnull()) {
      auto tt = transientTrackBuilder->build(trackRef);
      auto& track = *trackRef.get();

      for (unsigned iPVType : {0, 1}) {
        if (!vertexCols[iPVType])
          continue;

        reco::Vertex const& pv = vertexCols[iPVType]->at(0);
        reco::Vertex pvub = pv;

        if (fitUnbiasedVertex_) {
          if (!beamspots[iPVType])
            throw edm::Exception(edm::errors::Configuration, "FillerElectrons:FillDataBlock()\n")
              << "Null Beamspot pointer";

          reco::TrackCollection newTkCollection;
          bool foundMatch = false;
          for (auto itk = pv.tracks_begin(); itk != pv.tracks_end(); ++itk) {
            if (itk->get() == trackRef.get()) {
              foundMatch = true;
              continue;
            }
            newTkCollection.push_back(*itk->get());
          }

          if (foundMatch) {
            auto pvs = vtxReProducers[iPVType]->makeVertices(newTkCollection, *beamspots[iPVType], setup);
            if (pvs.size() > 0)
              pvub = pvs.front();      // take the first in the list
          }
        }

        // preserve sign of transverse impact parameter (cross-product definition from track, not
        // lifetime-signing)
        const double ipsign = ((-track.dxy(pv.position())) >= 0) ? 1. : -1.;

        reco::Vertex const* verts[] = {&pv, &pvub};
        for (unsigned iBias : {0, 1}) {
          unsigned iSetter = iBias * 2 + iPVType;

          auto d0pv = IPTools::absoluteTransverseImpactParameter(tt, *verts[iBias]);
          if (d0pv.first) {
            d0pvSetters[iSetter](*outMuon, ipsign * d0pv.second.value());
            d0pverrSetters[iSetter](*outMuon, d0pv.second.error());
          }
          else
            d0pvSetters[iSetter](*outMuon, -99.);

          auto ip3dpv = IPTools::absoluteImpactParameter3D(tt, *verts[iBias]);
          if (ip3dpv.first) {
            ip3dpvSetters[iSetter](*outMuon, ipsign * ip3dpv.second.value());
            ip3dpverrSetters[iSetter](*outMuon, ip3dpv.second.error());
          }
          else
            ip3dpvSetters[iSetter](*outMuon, -99.);
        }

        // compute compatibility with PV using taking into account also the case where muon track was
        // included in the vertex fit
        if (track.extra().isAvailable()) {
          auto pvCompat = kalmanEstimator.estimate(pv, tt);
          if (pvCompat.first)
            pvcompatSetters[iPVType](*outMuon, pvCompat.second);
          else
            pvcompatSetters[iPVType](*outMuon, -99.);
        }
      }
    }

    outMuon->SetNChambers  (inMuon.numberOfChambers());
    outMuon->SetStationMask(inMuon.stationMask(reco::Muon::SegmentAndTrackArbitration));
    outMuon->SetNMatches   (inMuon.numberOfMatches());
    for (int i0 = 0; i0 < 4; i0++) {
      // DTs
      outMuon->SetDX(i0,            inMuon.dX(i0+1,1));
      outMuon->SetDY(i0,            inMuon.dY(i0+1,1));
      outMuon->SetPullX(i0,         inMuon.pullX(i0+1,1));
      outMuon->SetPullY(i0,         inMuon.pullY(i0+1,1));
      outMuon->SetTrackDist(i0,     inMuon.trackDist(i0+1,1));
      outMuon->SetTrackDistErr(i0,  inMuon.trackDistErr(i0+1,1));
      outMuon->SetNSegments(i0,     NumberOfSegments(inMuon,i0+1,1));
      // CSCs
      outMuon->SetDX(4+i0,          inMuon.dX       (i0+1,2));
      outMuon->SetDY(4+i0,          inMuon.dY       (i0+1,2));
      outMuon->SetPullX(4+i0,       inMuon.pullX    (i0+1,2));
      outMuon->SetPullY(4+i0,       inMuon.pullY    (i0+1,2));
      outMuon->SetTrackDist(4+i0,   inMuon.trackDist(i0+1,2));
      outMuon->SetTrackDistErr(4+i0,inMuon.trackDistErr(i0+1,2));
      outMuon->SetNSegments(4+i0,   NumberOfSegments(inMuon,i0+1,2));
    }

    // fill corrected expected inner hits
    if (trackRef.isNonnull()) {
      auto& track = *trackRef;
      outMuon->SetCorrectedNExpectedHitsInner (track.hitPattern().numberOfHits(reco::HitPattern::MISSING_INNER_HITS));
      outMuon->SetValidFraction               (track.validFraction());
      outMuon->SetNTrkLayersHit               (track.hitPattern().trackerLayersWithMeasurement());
      outMuon->SetNTrkLayersNoHit             (track.hitPattern().trackerLayersWithoutMeasurement(reco::HitPattern::TRACK_HITS));
      outMuon->SetNPxlLayersHit               (track.hitPattern().pixelLayersWithMeasurement());
      outMuon->SetNTrkLostHitsIn              (track.hitPattern().numberOfLostTrackerHits(reco::HitPattern::MISSING_INNER_HITS));
      outMuon->SetNTrkLostHitsOut             (track.hitPattern().numberOfLostTrackerHits(reco::HitPattern::MISSING_OUTER_HITS));
    }

    // add muon to map
    muonMap_->Add(mPtr, outMuon);

    if (muonPFMap_) {
      auto& patMuon = static_cast<pat::Muon const&>(inMuon);

      unsigned iS = 0;

      // first sourceCandidatePtr may point to pfCandidates instead of packedPFCandidates
      if (patMuon.pfCandidateRef().isNonnull())
        ++iS;

      auto pfPtr = patMuon.sourceCandidatePtr(iS);
      if (pfPtr.isNonnull())
        muonPFMap_->Add(pfPtr, outMuon);
    }        

    if (verbose_>1) {
      if (!outMuon->HasGlobalTrk() && !outMuon->HasStandaloneTrk()) {
        printf("mithep::Muon, pt=%5f, eta=%5f, phi=%5f, mass=%5f\n",outMuon->Pt(),outMuon->Eta(),outMuon->Phi(), outMuon->Mass());
        printf("  reco::Muon, pt=%5f, eta=%5f, phi=%5f, mass=%5f\n",inMuon.pt(),inMuon.eta(),inMuon.phi(),inMuon.mass());
      }
    }

  }
  muons_->Trim();

  delete vtxReProducers[0];
  delete vtxReProducers[1];
}

void
mithep::FillerMuons::ResolveLinks(edm::Event const& event, edm::EventSetup const&)
{
  // Supported patterns
  // Fill from reco::MuonCollection:
  //  - xyzTrackMap_->GetMit(xyzRef)
  // Fill from pat::Muon:
  //  - xyzMuTrackMap_->GetMit(mPtr)

  std::function<void(mithep::Muon*, mithep::Track const*)> setters[nMuonTrackTypes];
  setters[kInnerTrack] = &mithep::Muon::SetTrackerTrk;
  setters[kOuterTrack] = &mithep::Muon::SetStandaloneTrk;
  setters[kCombinedTrack] = &mithep::Muon::SetGlobalTrk;
  setters[kTPFMS] = &mithep::Muon::SetTrackerPlusFirstStationTrk;
  setters[kPicky] = &mithep::Muon::SetPickyTrk;
  setters[kDYT] = &mithep::Muon::SetDYTTrk;

  for (auto& mapElem : muonMap_->FwdMap()) {
    auto&& mPtr = mapElem.first;
    auto& inMuon = static_cast<reco::Muon const&>(*mPtr);
    bool inHasBest = inMuon.muonBestTrack().isAvailable();
    bool inHasTunePBest = inMuon.tunePMuonBestTrack().isAvailable();
    auto* outMuon = mapElem.second;

    if (fillFromPAT_) {
      for (unsigned iT = 0; iT != nMuonTrackTypes; ++iT) {
        // MuonTrackType in reco::Muon is shifted by 1
        auto recoType = reco::Muon::MuonTrackType(iT + 1);
        if (muonTrackMap_[iT] &&
            (inMuon.muonTrack(recoType).isAvailable() ||
             (inMuon.muonBestTrackType() == recoType && inHasBest) ||
             (inMuon.tunePMuonBestTrackType() == recoType && inHasTunePBest)))
          setters[iT](outMuon, muonTrackMap_[iT]->GetMit(mPtr));
      }
    }
    else {
      for (unsigned iT = 0; iT != nMuonTrackTypes; ++iT) {
        auto recoType = reco::Muon::MuonTrackType(iT + 1);
        auto&& ref = inMuon.muonTrack(recoType);

        if (iT == kOuterTrack) {
          if (ref.isNonnull()) {
            // outer track can come from either vertex-constrained or unconstrained tracks
            if (staVtxTrackMap_ && ref.id().id() == staVtxTrackMap_->GetEdmProductId())
              outMuon->SetStandaloneTrk(staVtxTrackMap_->GetMit(edm::refToPtr(ref)));
            else if (trackMap_[iT])
              outMuon->SetStandaloneTrk(trackMap_[iT]->GetMit(edm::refToPtr(ref)));
          }
        }
        else {
          if (trackMap_[iT] && ref.isNonnull())
            setters[iT](outMuon, trackMap_[iT]->GetMit(edm::refToPtr(ref)));
        }
      }
    }
  }
}

unsigned char
mithep::FillerMuons::RecoToMithep(unsigned char recoType) const
{
  switch (recoType) {
  case reco::Muon::InnerTrack:
    return mithep::Muon::kTracker;
  case reco::Muon::OuterTrack:
    return mithep::Muon::kSta;
  case reco::Muon::CombinedTrack:
    return mithep::Muon::kGlobal;
  case reco::Muon::TPFMS:
    return mithep::Muon::kTrackerPlusFirstStation;
  case reco::Muon::Picky:
    return mithep::Muon::kPicky;
  case reco::Muon::DYT:
    return mithep::Muon::kDYT;
  default:
    return mithep::Muon::kNone;
  }
}

unsigned char
mithep::FillerMuons::MithepToReco(unsigned char mitType) const
{
  switch (mitType) {
  case mithep::Muon::kTracker:
    return reco::Muon::InnerTrack;
  case mithep::Muon::kSta:
    return reco::Muon::OuterTrack;
  case mithep::Muon::kGlobal:
    return reco::Muon::CombinedTrack;
  case mithep::Muon::kTrackerPlusFirstStation:
    return reco::Muon::TPFMS;
  case mithep::Muon::kPicky:
    return reco::Muon::Picky;
  case mithep::Muon::kDYT:
    return reco::Muon::DYT;
  default:
    return reco::Muon::None;
  }
}

int
mithep::FillerMuons::NumberOfSegments(reco::Muon const& inMuon, int station, int muonSubdetId, reco::Muon::ArbitrationType type)
{
  if (!inMuon.isMatchesValid())
    return 0;

  int segments(0);

  std::vector<reco::MuonChamberMatch> const& matches = inMuon.matches();
  for (auto&& chamberMatch : matches) {
    if (chamberMatch.segmentMatches.empty())
      continue;

    if (!(chamberMatch.station()==station && chamberMatch.detector()==muonSubdetId))
      continue;

    if (type == reco::Muon::NoArbitration) {
      segments += chamberMatch.segmentMatches.size();
      continue;
    }

    for (auto&& segmentMatch : chamberMatch.segmentMatches) {
      if (type == reco::Muon::SegmentArbitration) {
        if (segmentMatch.isMask(reco::MuonSegmentMatch::BestInChamberByDR)) {
          ++segments;
          break;
        }
      }
      if (type == reco::Muon::SegmentAndTrackArbitration) {
        if (segmentMatch.isMask(reco::MuonSegmentMatch::BestInChamberByDR) &&
            segmentMatch.isMask(reco::MuonSegmentMatch::BelongsToTrackByDR)) {
          ++segments;
          break;
        }
      }
      if (type == reco::Muon::SegmentAndTrackArbitrationCleaned) {
        if (segmentMatch.isMask(reco::MuonSegmentMatch::BestInChamberByDR) &&
            segmentMatch.isMask(reco::MuonSegmentMatch::BelongsToTrackByDR) &&
            segmentMatch.isMask(reco::MuonSegmentMatch::BelongsToTrackByCleaning)) {
          ++segments;
          break;
        }
      }
      if (type > 1<<7) {
        if (segmentMatch.isMask(type)) {
          ++segments;
          break;
        }
      }
    }
  }
  return segments;
}

DEFINE_MITHEP_TREEFILLER(FillerMuons);
