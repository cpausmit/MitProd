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
#include "MitAna/DataTree/interface/PFCandidate.h"
#include "MitProd/ObjectService/interface/ObjectService.h"
#include "MitEdm/Tools/interface/VertexReProducer.h"

#include <functional>

mithep::FillerMuons::FillerMuons(edm::ParameterSet const& cfg, edm::ConsumesCollector& collector, mithep::ObjectService* os, char const* name, bool active/* = true*/) :
  BaseFiller(cfg, os, name, active),
  edmToken_             (GetToken<MuonView>(collector, "edmName","muons")),
  pvEdmToken_           (GetToken<reco::VertexCollection>(collector, "pvEdmName",
                                                          "offlinePrimaryVertices")),
  pvBSEdmToken_         (GetToken<reco::VertexCollection>(collector, "pvBSEdmName",
                                                          "offlinePrimaryVerticesWithBS")),
  pvBeamSpotToken_      (GetToken<reco::BeamSpot>(collector, "pvBeamSpotName", "offlineBeamSpot")),
  pvbsBeamSpotToken_    (GetToken<reco::BeamSpot>(collector, "pvbsBeamSpotName", "offlineBeamSpot")),
  mitName_              (Conf().getUntrackedParameter<string>("mitName", Names::gkMuonBrn)),
  globalTrackMapName_   (Conf().getUntrackedParameter<string>("globalTrackMapName", "")),
  staTrackMapName_      (Conf().getUntrackedParameter<string>("staTrackMapName", "")),
  staVtxTrackMapName_   (Conf().getUntrackedParameter<string>("staVtxTrackMapName", "")),
  trackerTrackMapName_  (Conf().getUntrackedParameter<string>("trackerTrackMapName", "")),
  muonMapName_          (Conf().getUntrackedParameter<string>("muonMapName", "")),
  pfCandMapName_        (Conf().getUntrackedParameter<string>("pfCandMapName", "")),
  fitUnbiasedVertex_    (Conf().getUntrackedParameter<bool>("fitUnbiasedVertex", true)),
  fillFromPAT_          (Conf().getUntrackedParameter<bool>("fillFromPAT", false)),
  globalTrackMap_       (0),
  standaloneTrackMap_   (0),
  standaloneVtxTrackMap_(0),
  trackerTrackMap_      (0),
  muonMap_              (new mithep::MuonMap),
  muons_                (new mithep::MuonArr(16))
{
  // Constructor.
}

mithep::FillerMuons::~FillerMuons()
{
  // Destructor.

  delete muons_;
  delete muonMap_;
}

void
mithep::FillerMuons::BookDataBlock(TreeWriter& tws)
{
  // Add muons branch to tree and get pointers to maps.

  tws.AddBranch(mitName_,&muons_);
  OS()->add<mithep::MuonArr>(muons_,mitName_);

  if (!globalTrackMapName_.empty()) {
    globalTrackMap_ = OS()->get<TrackMap>(globalTrackMapName_);
    if (globalTrackMap_)
      AddBranchDep(mitName_,globalTrackMap_->GetBrName());
  }
  if (!staTrackMapName_.empty()) {
    standaloneTrackMap_ = OS()->get<TrackMap>(staTrackMapName_);
    if (standaloneTrackMap_)
      AddBranchDep(mitName_,standaloneTrackMap_->GetBrName());
  }
  if (!staVtxTrackMapName_.empty()) {
    standaloneVtxTrackMap_ = OS()->get<TrackMap>(staVtxTrackMapName_);
    if (standaloneVtxTrackMap_)
      AddBranchDep(mitName_,standaloneVtxTrackMap_->GetBrName());
  }
  if (!trackerTrackMapName_.empty()) {
    trackerTrackMap_ = OS()->get<TrackMap>(trackerTrackMapName_);
    if (trackerTrackMap_)
      AddBranchDep(mitName_,trackerTrackMap_->GetBrName());
  }
  if (!muonMapName_.empty()) {
    muonMap_->SetBrName(mitName_);
    OS()->add<MuonMap>(muonMap_,muonMapName_);
  }
}

void
mithep::FillerMuons::FillDataBlock(edm::Event const& event,
                                   edm::EventSetup const& setup)
{
  // Fill muon information.

  muons_  ->Delete();
  muonMap_->Reset();

  edm::Handle<MuonView> hMuonProduct;
  GetProduct(edmToken_, hMuonProduct, event);
  auto& inMuons = *hMuonProduct;

  edm::Handle<reco::VertexCollection> hVertex;
  if (!pvEdmToken_.isUninitialized())
    GetProduct(pvEdmToken_, hVertex, event);

  edm::Handle<reco::VertexCollection> hVertexBS;
  if (!pvBSEdmToken_.isUninitialized())
    GetProduct(pvBSEdmToken_, hVertexBS, event);

  edm::Handle<reco::BeamSpot> pvbeamspot;
  if (fitUnbiasedVertex_ && !pvBeamSpotToken_.isUninitialized())
    GetProduct(pvBeamSpotToken_, pvbeamspot, event);

  edm::Handle<reco::BeamSpot> pvbsbeamspot;
  if (fitUnbiasedVertex_ && !pvbsBeamSpotToken_.isUninitialized())
    GetProduct(pvbsBeamSpotToken_, pvbsbeamspot, event);

  edm::ESHandle<TransientTrackBuilder> hTransientTrackBuilder;
  setup.get<TransientTrackRecord>().get("TransientTrackBuilder", hTransientTrackBuilder);
  auto transientTrackBuilder = hTransientTrackBuilder.product();

  KalmanVertexTrackCompatibilityEstimator<5> kalmanEstimator;

  VertexReProducer* vtxReProducers[] = {0, 0};
  if (fitUnbiasedVertex_) {
    if (hVertex.isValid())
      vtxReProducers[0] = new VertexReProducer(hVertex, event);
    if (hVertexBS.isValid())
      vtxReProducers[1] = new VertexReProducer(hVertexBS, event);
  }

  typedef std::function<void(mithep::Muon&, double)> Setter;

  edm::Handle<reco::VertexCollection>* pvcHandles[] = {&hVertex, &hVertexBS};
  edm::Handle<reco::BeamSpot>* bsHandles[] = {&pvbeamspot, &pvbsbeamspot};
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

  unsigned iMuon = 0;
  for (auto&& inMuon : inMuons) {
    edm::Ptr<reco::Muon> mPtr(hMuonProduct, iMuon);
    ++iMuon;

    auto outMuon = muons_->AddNew();

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

    auto combinedMuonRef = inMuon.combinedMuon();
    auto standaloneMuonRef = inMuon.standAloneMuon();
    auto trackRef = inMuon.track();

    if (globalTrackMap_ && combinedMuonRef.isNonnull()) {
      outMuon->SetGlobalTrk (globalTrackMap_->GetMit(edm::refToPtr(combinedMuonRef)));
      outMuon->SetNValidHits(combinedMuonRef->hitPattern().numberOfValidMuonHits());
    }
    if (standaloneTrackMap_ && standaloneVtxTrackMap_ && standaloneMuonRef.isNonnull()) {
      auto ptr = edm::refToPtr(standaloneMuonRef);

      Int_t refProductId = standaloneMuonRef.id().id();
      if (refProductId == standaloneVtxTrackMap_->GetEdmProductId())
        outMuon->SetStandaloneTrk(standaloneVtxTrackMap_->GetMit(ptr));
      else if (refProductId == standaloneTrackMap_->GetEdmProductId())
        outMuon->SetStandaloneTrk(standaloneTrackMap_->GetMit(ptr));
      else
        throw edm::Exception(edm::errors::Configuration, "FillerMuons:FillDataBlock()")
          << "Error! Track reference in unmapped collection";
    }
    if (trackerTrackMap_ && trackRef.isNonnull()) {
      outMuon->SetTrackerTrk(trackerTrackMap_->GetMit(edm::refToPtr(trackRef)));
    }

    // compute impact parameter with respect to PV
    if (trackRef.isNonnull()) {
      auto tt = transientTrackBuilder->build(trackRef);
      auto& track = *trackRef.get();

      for (unsigned iPVType : {0, 1}) {
        if (!pvcHandles[iPVType]->isValid())
          continue;

        auto& pvcHandle = *pvcHandles[iPVType];
        auto& bs = **bsHandles[iPVType];

        reco::Vertex const& pv = pvcHandle->at(0);
        reco::Vertex pvub = pvcHandle->at(0);

        if (fitUnbiasedVertex_) {
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
            auto pvs = vtxReProducers[iPVType]->makeVertices(newTkCollection, bs, setup);
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
  if (!fillFromPAT_ || pfCandMapName_.empty())
    return;

  auto pfCandMap = OS()->get<mithep::PFCandidateMap>(pfCandMapName_);
  if (!pfCandMap)
    throw edm::Exception(edm::errors::Configuration, "FillerMuons:ResolveLinks()\n")
      << "Error! fillFromPAT set but PF Candidate map not found";

  for (unsigned iE = 0; iE != muons_->GetEntries(); ++iE) {
    auto mu = muons_->At(iE);

    auto mPtr = muonMap_->GetEdm(mu);
    auto patMuon = dynamic_cast<pat::Muon const*>(mPtr.get());
    if (!patMuon)
      throw edm::Exception(edm::errors::Configuration, "FillerMuons:ResolveLinks()\n")
        << "Error! fillFromPAT set on non-PAT input";

    unsigned iS = 0;
    if (patMuon->pfCandidateRef().isNonnull())
      ++iS;

    auto pfCand = pfCandMap->GetMit(patMuon->sourceCandidatePtr(iS));
    pfCand->SetMuon(mu);
  }
}

//--------------------------------------------------------------------------------------------------
int mithep::FillerMuons::NumberOfSegments(reco::Muon const& inMuon, int station, int muonSubdetId, reco::Muon::ArbitrationType type)
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
      if (type == reco::Muon::SegmentArbitration)
	if (segmentMatch.isMask(reco::MuonSegmentMatch::BestInChamberByDR)) {
	  segments++;
	  break;
	}
      if (type == reco::Muon::SegmentAndTrackArbitration)
	if (segmentMatch.isMask(reco::MuonSegmentMatch::BestInChamberByDR) &&
	    segmentMatch.isMask(reco::MuonSegmentMatch::BelongsToTrackByDR)) {
	  segments++;
	  break;
	}

      if (type == reco::Muon::SegmentAndTrackArbitrationCleaned)
	if (segmentMatch.isMask(reco::MuonSegmentMatch::BestInChamberByDR) &&
	    segmentMatch.isMask(reco::MuonSegmentMatch::BelongsToTrackByDR) &&
	    segmentMatch.isMask(reco::MuonSegmentMatch::BelongsToTrackByCleaning)) {
	  segments++;
	  break;
	}

      if (type > 1<<7)
	if (segmentMatch.isMask(type)) {
	  segments++;
	  break;
	}
    }
  }
  return segments;
}
