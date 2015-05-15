#include "MitProd/TreeFiller/interface/FillerMuons.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonQuality.h"
#include "DataFormats/MuonReco/interface/MuonSelectors.h"
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

//--------------------------------------------------------------------------------------------------
mithep::FillerMuons::FillerMuons(const edm::ParameterSet &cfg, edm::ConsumesCollector& collector, ObjectService* os, const char *name, bool active) :
  BaseFiller            (cfg,os,name,active),
  edmToken_(GetToken<MuonView>(collector, "edmName","muons")),
  pvEdmToken_(GetToken<reco::VertexCollection>(collector, "pvEdmName",
                                                          "offlinePrimaryVertices")),
  pvBSEdmToken_(GetToken<reco::VertexCollection>(collector, "pvBSEdmName",
                                                            "offlinePrimaryVerticesWithBS")),
  beamSpotToken_(GetToken<reco::BeamSpot>(collector, "beamSpotName", "offlineBeamSpot")),
  pvBeamSpotToken_(GetToken<reco::BeamSpot>(collector, "pvBeamSpotName", "offlineBeamSpot")),
  pvbsBeamSpotToken_(GetToken<reco::BeamSpot>(collector, "pvbsBeamSpotName", "offlineBeamSpot")),
  mitName_              (Conf().getUntrackedParameter<string>("mitName",Names::gkMuonBrn)),
  globalTrackMapName_   (Conf().getUntrackedParameter<string>("globalTrackMapName","")),
  staTrackMapName_      (Conf().getUntrackedParameter<string>("staTrackMapName","")),
  staVtxTrackMapName_   (Conf().getUntrackedParameter<string>("staVtxTrackMapName","")),
  trackerTrackMapName_  (Conf().getUntrackedParameter<string>("trackerTrackMapName","")),
  muonMapName_          (Conf().getUntrackedParameter<string>("muonMapName","")),
  fitUnbiasedVertex_    (Conf().getUntrackedParameter<bool>("fitUnbiasedVertex",true)),
  globalTrackMap_       (0),
  standaloneTrackMap_   (0),
  standaloneVtxTrackMap_(0),
  trackerTrackMap_      (0),
  muonMap_              (new mithep::MuonMap),
  muons_                (new mithep::MuonArr(16))
{
  // Constructor.
}

//--------------------------------------------------------------------------------------------------
mithep::FillerMuons::~FillerMuons()
{
  // Destructor.

  delete muons_;
  delete muonMap_;
}

//--------------------------------------------------------------------------------------------------
void mithep::FillerMuons::BookDataBlock(TreeWriter &tws)
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

//--------------------------------------------------------------------------------------------------
void mithep::FillerMuons::FillDataBlock(const edm::Event      &event,
                                const edm::EventSetup &setup)
{
  // Fill muon information.

  muons_  ->Delete();
  muonMap_->Reset();

  edm::Handle<MuonView> hMuonProduct;
  GetProduct(edmToken_, hMuonProduct, event);
  MuonView const& inMuons = *hMuonProduct;

  edm::Handle<reco::VertexCollection> hVertex;
  GetProduct(pvEdmToken_, hVertex, event);
  const reco::VertexCollection *pvCol = hVertex.product();

  edm::Handle<reco::VertexCollection> hVertexBS;
  GetProduct(pvBSEdmToken_, hVertexBS, event);
  const reco::VertexCollection *pvBSCol = hVertexBS.product();

  edm::ESHandle<TransientTrackBuilder> hTransientTrackBuilder;
  setup.get<TransientTrackRecord>().get("TransientTrackBuilder", hTransientTrackBuilder);
  const TransientTrackBuilder *transientTrackBuilder = hTransientTrackBuilder.product();

  KalmanVertexTrackCompatibilityEstimator<5> kalmanEstimator;

  unsigned iMuon = 0;
  for (auto&& inMuon : inMuons) {
    edm::Ptr<reco::Muon> mPtr(hMuonProduct, iMuon);
    ++iMuon;

    mithep::Muon* outMuon = muons_->AddNew();

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

    reco::TrackRef combinedMuonRef = inMuon.combinedMuon();
    reco::TrackRef standaloneMuonRef = inMuon.standAloneMuon();
    reco::TrackRef trackRef = inMuon.track();

    if (globalTrackMap_ && combinedMuonRef.isNonnull()) {
      outMuon->SetGlobalTrk (globalTrackMap_->GetMit(edm::refToPtr(combinedMuonRef)));
      outMuon->SetNValidHits(combinedMuonRef->hitPattern().numberOfValidMuonHits());
    }
    if (standaloneTrackMap_ && standaloneVtxTrackMap_ && standaloneMuonRef.isNonnull()) {
      mitedm::TrackPtr ptr = edm::refToPtr(standaloneMuonRef);

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
      const reco::TransientTrack &tt = transientTrackBuilder->build(trackRef);
      reco::Track const& track = *trackRef.get();

      reco::Vertex const& thevtx     = pvCol  ->at(0);
      reco::Vertex const& thevtxbs   = pvBSCol->at(0);

      reco::Vertex thevtxub   = pvCol  ->at(0);
      reco::Vertex thevtxubbs = pvBSCol->at(0);

      reco::TrackCollection newTkCollection;
      bool foundMatch = false;
      for (reco::Vertex::trackRef_iterator itk = thevtx.tracks_begin(); itk!=thevtx.tracks_end();
	   itk++) {
        if (itk->get() == trackRef.get()) {
          foundMatch = true;
          continue;
        }
        newTkCollection.push_back(*itk->get());
      }

      if (foundMatch && fitUnbiasedVertex_) {
        edm::Handle<reco::BeamSpot> bs;
        GetProduct(beamSpotToken_, bs, event);

        VertexReProducer revertex(hVertex,event);
        edm::Handle<reco::BeamSpot> pvbeamspot;
        GetProduct(pvBeamSpotToken_, pvbeamspot, event);
        std::vector<TransientVertex> pvs = revertex.makeVertices(newTkCollection,*pvbeamspot,setup);
        if (pvs.size()>0) {
          thevtxub = pvs.front();      // take the first in the list
        }

        VertexReProducer revertexbs(hVertexBS,event);
        edm::Handle<reco::BeamSpot> pvbsbeamspot;
        GetProduct(pvbsBeamSpotToken_, pvbsbeamspot, event);
        std::vector<TransientVertex> pvbss = revertexbs.makeVertices(newTkCollection,*pvbsbeamspot,setup);
        if (pvbss.size()>0) {
          thevtxubbs = pvbss.front();  // take the first in the list
        }
      }

      // preserve sign of transverse impact parameter (cross-product definition from track, not
      // lifetime-signing)
      const double thesign   = ( (-track.dxy(thevtx.position()))   >=0 ) ? 1. : -1.;
      const double thesignbs = ( (-track.dxy(thevtxbs.position())) >=0 ) ? 1. : -1.;

      const std::pair<bool,Measurement1D> &d0pv =
	IPTools::absoluteTransverseImpactParameter(tt,thevtx);
      if (d0pv.first) {
        outMuon->SetD0PV(thesign*d0pv.second.value());
        outMuon->SetD0PVErr(d0pv.second.error());
      }
      else {
        outMuon->SetD0PV(-99.0);
      }

      const std::pair<bool,Measurement1D> &ip3dpv =  IPTools::absoluteImpactParameter3D(tt,thevtx);
      if (ip3dpv.first) {
        outMuon->SetIp3dPV(thesign*ip3dpv.second.value());
        outMuon->SetIp3dPVErr(ip3dpv.second.error());
      }
      else {
        outMuon->SetIp3dPV(-99.0);
      }

      const std::pair<bool,Measurement1D> &d0pvbs =
	IPTools::absoluteTransverseImpactParameter(tt,thevtxbs);
      if (d0pvbs.first) {
        outMuon->SetD0PVBS(thesignbs*d0pvbs.second.value());
        outMuon->SetD0PVBSErr(d0pvbs.second.error());
      }
      else {
        outMuon->SetD0PVBS(-99.0);
      }

      const std::pair<bool,Measurement1D> &ip3dpvbs =
	IPTools::absoluteImpactParameter3D(tt,thevtxbs);
      if (ip3dpvbs.first) {
        outMuon->SetIp3dPVBS(thesignbs*ip3dpvbs.second.value());
        outMuon->SetIp3dPVBSErr(ip3dpvbs.second.error());
      }
      else {
        outMuon->SetIp3dPVBS(-99.0);
      }

      const std::pair<bool,Measurement1D> &d0pvub =
	IPTools::absoluteTransverseImpactParameter(tt,thevtxub);
      if (d0pvub.first) {
        outMuon->SetD0PVUB(thesign*d0pvub.second.value());
        outMuon->SetD0PVUBErr(d0pvub.second.error());
      }
      else {
        outMuon->SetD0PVUB(-99.0);
      }

      const std::pair<bool,Measurement1D> &ip3dpvub =
	IPTools::absoluteImpactParameter3D(tt,thevtxub);
      if (ip3dpvub.first) {
        outMuon->SetIp3dPVUB(thesign*ip3dpvub.second.value());
        outMuon->SetIp3dPVUBErr(ip3dpvub.second.error());
      }
      else {
        outMuon->SetIp3dPVUB(-99.0);
      }

      const std::pair<bool,Measurement1D> &d0pvubbs =
	IPTools::absoluteTransverseImpactParameter(tt,thevtxubbs);
      if (d0pvubbs.first) {
        outMuon->SetD0PVUBBS(thesignbs*d0pvubbs.second.value());
        outMuon->SetD0PVUBBSErr(d0pvubbs.second.error());
      }
      else {
        outMuon->SetD0PVUBBS(-99.0);
      }

      const std::pair<bool,Measurement1D> &ip3dpvubbs =
	IPTools::absoluteImpactParameter3D(tt,thevtxubbs);
      if (ip3dpvubbs.first) {
        outMuon->SetIp3dPVUBBS(thesignbs*ip3dpvubbs.second.value());
        outMuon->SetIp3dPVUBBSErr(ip3dpvubbs.second.error());
      }
      else {
        outMuon->SetIp3dPVUBBS(-99.0);
      }

      // compute compatibility with PV using taking into account also the case where muon track was
      // included in the vertex fit
      if (track.extra().isAvailable()) {

        const std::pair<bool,double> &pvCompat = kalmanEstimator.estimate(pvCol->at(0),tt);
        if (pvCompat.first) {
          outMuon->SetPVCompatibility(pvCompat.second);
        }
        else {
          outMuon->SetPVCompatibility(-99.0);
        }

        const std::pair<bool,double> &pvbsCompat = kalmanEstimator.estimate(pvBSCol->at(0),tt);
        if (pvbsCompat.first) {
          outMuon->SetPVBSCompatibility(pvbsCompat.second);
        }
        else {
          outMuon->SetPVBSCompatibility(-99.0);
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
      reco::Track const& track = *trackRef.get();
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
