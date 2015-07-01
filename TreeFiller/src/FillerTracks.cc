#include "MitProd/TreeFiller/interface/FillerTracks.h"
#include "DataFormats/Common/interface/Ptr.h"
#include "DataFormats/Common/interface/RefToBase.h"
#include "DataFormats/TrackReco/interface/HitPattern.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticleFwd.h"
#include "MagneticField/Engine/interface/MagneticField.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "TrackingTools/TrackAssociator/interface/TrackDetectorAssociator.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateTransform.h"
#include "TrackingTools/TrackAssociator/interface/TrackAssociatorParameters.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataTree/interface/TrackCol.h"
#include "MitProd/ObjectService/interface/ObjectService.h"
#include "MitEdm/DataFormats/interface/Types.h"

mithep::FillerTracks::FillerTracks(edm::ParameterSet const& cfg, edm::ConsumesCollector& collector, mithep::ObjectService* os, const char *name, bool active) :
  BaseFiller(cfg,os,name,active),
  sourceType_(nSourceTypes),
  ecalAssocActive_(cfg.getUntrackedParameter<bool>("ecalAssocActive", false)),
  edmSimAssocToken_(GetToken<reco::RecoToSimCollection>(collector, cfg, "edmSimAssociationName", false)),
  mitName_(cfg.getUntrackedParameter<std::string>("mitName", Names::gkTrackBrn)),
  trackingMapName_(cfg.getUntrackedParameter<std::string>("trackingMapName", "")),
  barrelSuperClusterIdMapName_(cfg.getUntrackedParameter<std::string>("superClusterIdMapName", "")),
  endcapSuperClusterIdMapName_(cfg.getUntrackedParameter<std::string>("endcapClusterIdMapName", "")),
  trackMapName_(cfg.getUntrackedParameter<std::string>("trackMapName", mitName_ + "MapName")),
  trackingMap_(0),
  tracks_(new mithep::TrackArr(250)),
  hitReader_(),
  assocParams_(0),
  trackMap_(0),
  eleTrackMap_(0),
  muTrackMap_(0)
{
  std::string sourceType(cfg.getUntrackedParameter<std::string>("sourceType", "Tracks"));
  if (sourceType == "Tracks")
    sourceType_ = kTracks;
  else if (sourceType == "ElectronGsf")
    sourceType_ = kElectronGsf;
  else if (sourceType == "ElectronCtf")
    sourceType_ = kElectronCtf;
  else if (sourceType == "MuonInner")
    sourceType_ = kMuonInner;
  else if (sourceType == "MuonStandalone")
    sourceType_ = kMuonStandalone;
  else if (sourceType == "MuonCombined")
    sourceType_ = kMuonCombined;
  else if (sourceType == "MuonTPFMS")
    sourceType_ = kMuonTPFMS;
  else if (sourceType == "MuonPicky")
    sourceType_ = kMuonPicky;
  else if (sourceType == "MuonDYT")
    sourceType_ = kMuonDYT;
  else
    throw edm::Exception(edm::errors::Configuration, "FillerTracks::Ctor")
      << "Invalid source type " << sourceType;

  if (trackMapName_.empty()) {
    throw edm::Exception(edm::errors::Configuration, "FillerTracks::Ctor")
      << "trackMapName cannot be empty";
  }

  if (!trackingMapName_.empty() && sourceType_ != kTracks) {
    throw edm::Exception(edm::errors::Configuration, "FillerTracks::Ctor")
      << "Track-to-sim association only available for TrackCollection";
  }

  switch (sourceType_) {
  case kTracks:
    edmToken_ = GetToken<TrackView>(collector, cfg, "edmName"); //generalTracks
    break;
  case kElectronGsf:
  case kElectronCtf:
    edmElectronToken_ = GetToken<ElectronView>(collector, cfg, "edmName"); //slimmedElectrons
    break;
  case kMuonInner:
  case kMuonStandalone:
  case kMuonCombined:
  case kMuonTPFMS:
  case kMuonPicky:
  case kMuonDYT:
    edmMuonToken_ = GetToken<MuonView>(collector, cfg, "edmName"); //slimmedMuons
    break;
  default:
    break;
  }

  if (ecalAssocActive_) {
    // initialize track associator configuration if needed
    assocParams_ = new TrackAssociatorParameters;
    assocParams_->loadParameters(cfg.getUntrackedParameterSet("TrackAssociatorParameters"), collector);
  }
}

//--------------------------------------------------------------------------------------------------
mithep::FillerTracks::~FillerTracks()
{
  delete tracks_;
  delete assocParams_;
  delete trackMap_;
  delete eleTrackMap_;
  delete muTrackMap_;
}

//--------------------------------------------------------------------------------------------------
void
mithep::FillerTracks::BookDataBlock(TreeWriter& tws)
{
  // Add tracks branch to tree, publish and get our objects.

  tws.AddBranch(mitName_,&tracks_);
  OS()->add(tracks_, mitName_);

  switch (sourceType_) {
  case kTracks:
    trackMap_ = new mithep::TrackMap;
    trackMap_->SetBrName(mitName_);
    OS()->add(trackMap_, trackMapName_);
    break;
  case kElectronGsf:
  case kElectronCtf:
    eleTrackMap_ = new mithep::ElectronTrackMap;
    eleTrackMap_->SetBrName(mitName_);
    OS()->add(eleTrackMap_, trackMapName_);
    break;
  case kMuonInner:
  case kMuonStandalone:
  case kMuonCombined:
  case kMuonTPFMS:
  case kMuonPicky:
  case kMuonDYT:
    muTrackMap_ = new mithep::MuonTrackMap;
    muTrackMap_->SetBrName(mitName_);
    OS()->add(muTrackMap_, trackMapName_);
    break;
  default:
    break;
  }
}

void
mithep::FillerTracks::PrepareLinks()
{
  if (!trackingMapName_.empty()) {
    trackingMap_ = OS()->get<TrackingParticleMap>(trackingMapName_);
    if (trackingMap_)
      AddBranchDep(mitName_,trackingMap_->GetBrName());
  }
  if (ecalAssocActive_ && !barrelSuperClusterIdMapName_.empty()) {
    barrelSuperClusterIdMap_ = OS()->get<SuperClusterIdMap>(barrelSuperClusterIdMapName_);
    if (barrelSuperClusterIdMap_)
      AddBranchDep(mitName_,barrelSuperClusterIdMap_->GetBrName());
  }
  if (ecalAssocActive_ && !endcapSuperClusterIdMapName_.empty()) {
    endcapSuperClusterIdMap_ = OS()->get<SuperClusterIdMap>(endcapSuperClusterIdMapName_);
    if (endcapSuperClusterIdMap_)
      AddBranchDep(mitName_,endcapSuperClusterIdMap_->GetBrName());
  }
}

//--------------------------------------------------------------------------------------------------
void
mithep::FillerTracks::FillDataBlock(edm::Event const& event, edm::EventSetup const& setup)
{
  // Fill tracks from edm collection into our collection.
  tracks_->Delete();
  if (trackMap_)
    trackMap_->Reset();
  if (eleTrackMap_)
    eleTrackMap_->Reset();
  if (muTrackMap_)
    muTrackMap_->Reset();

  TrackView const* tracksView = 0;
  ElectronView const* electronsView = 0;
  MuonView const* muonsView = 0;

  switch (sourceType_) {
  case kTracks:
    {
      edm::Handle<TrackView> hTrackProduct;
      GetProduct(edmToken_, hTrackProduct, event);
      tracksView = hTrackProduct.product();
      trackMap_->SetEdmProductId(hTrackProduct.id().id());
    }
    break;
  case kElectronGsf:
  case kElectronCtf:
    {
      edm::Handle<ElectronView> hElectrons;
      GetProduct(edmElectronToken_, hElectrons, event);
      electronsView = hElectrons.product();
    }
    break;
  case kMuonInner:
  case kMuonStandalone:
  case kMuonCombined:
  case kMuonTPFMS:
  case kMuonPicky:
  case kMuonDYT:
    {
      edm::Handle<MuonView> hMuons;
      GetProduct(edmMuonToken_, hMuons, event);
      muonsView = hMuons.product();
    }
    break;
  default:
    break;
  }

  typedef std::pair<reco::Track const*, mithep::Track*> InOutPair;
  std::vector<InOutPair> inOutPairs;

  // lambda to not have to write this for all muon track types
  auto fillMuonMap = 
    [this, muonsView, &inOutPairs](reco::Muon::MuonTrackType type) {
    for (auto&& mPtr : muonsView->ptrs()) {
      auto& mu = *mPtr;
      reco::Track const* inTrack = 0;
      auto&& ref = mu.muonTrack(type);
      if (ref.isAvailable())
        inTrack = ref.get();
      else if (mu.muonBestTrackType() == type) {
        auto&& bestRef = mu.muonBestTrack();
        if (bestRef.isAvailable())
          inTrack = bestRef.get();
        else if(mu.tunePMuonBestTrackType() == type) {
          auto&& tunePBestRef = mu.tunePMuonBestTrack();
          if (tunePBestRef.isAvailable())
            inTrack = tunePBestRef.get();
        }
      }
      if (inTrack) {
        auto* outTrack = this->ProduceTrack(*inTrack);
        this->muTrackMap_->Add(mPtr, outTrack);
        inOutPairs.push_back(InOutPair(inTrack, outTrack));
      }
    }
  };

  switch (sourceType_) {
  case kTracks:
    for (auto&& tPtr : tracksView->ptrs()) {
      auto* outTrack = ProduceTrack(*tPtr);
      trackMap_->Add(tPtr, outTrack);
      inOutPairs.push_back(InOutPair(tPtr.get(), outTrack));
    }
    break;

  case kElectronGsf:
    for (auto&& ePtr : electronsView->ptrs()) {
      auto* outTrack = ProduceTrack(*ePtr->gsfTrack());
      eleTrackMap_->Add(ePtr, outTrack);
      inOutPairs.push_back(InOutPair(ePtr->gsfTrack().get(), outTrack));
    }
    break;

  case kElectronCtf:
    for (auto&& ePtr : electronsView->ptrs()) {
      reco::TrackRef trkRef;
      // bug in reco::GsfElectron: closestCtfTrackRef is not virtual!
      auto* patE = dynamic_cast<pat::Electron const*>(ePtr.get());
      if (patE)
        trkRef = patE->closestCtfTrackRef();
      else
        trkRef = ePtr->closestCtfTrackRef();

      if (trkRef.isNonnull()) {
        auto* outTrack = ProduceTrack(*trkRef);
        eleTrackMap_->Add(ePtr, outTrack);
        inOutPairs.push_back(InOutPair(trkRef.get(), outTrack));
      }
    }
    break;

  case kMuonInner:
    fillMuonMap(reco::Muon::InnerTrack);
    break;

  case kMuonStandalone:
    fillMuonMap(reco::Muon::OuterTrack);
    break;

  case kMuonCombined:
    fillMuonMap(reco::Muon::CombinedTrack);
    break;

  case kMuonTPFMS:
    fillMuonMap(reco::Muon::TPFMS);
    break;

  case kMuonPicky:
    fillMuonMap(reco::Muon::Picky);
    break;

  case kMuonDYT:
    fillMuonMap(reco::Muon::DYT);
    break;

  default:
    break;
  }

  if (ecalAssocActive_) {
    // set up associator for Track-Ecal associations
    TrackDetectorAssociator trackAssociator;
    trackAssociator.useDefaultPropagator();

    edm::ESHandle<MagneticField> bFieldH;
    setup.get<IdealMagneticFieldRecord>().get(bFieldH);
    auto* bField = bFieldH.product();

    for (auto& inOut : inOutPairs)
      EcalAssoc(inOut.second, *inOut.first, event, setup, trackAssociator, *bField);
  }

  tracks_->Trim();
}

void
mithep::FillerTracks::ResolveLinks(edm::Event const& event, edm::EventSetup const&)
{
  if (sourceType_ == kTracks && trackingMap_ && !edmSimAssocToken_.isUninitialized()) {
    // for MC SimParticle association (reco->sim mappings)

    edm::Handle<TrackView> hTrackProduct;
    GetProduct(edmToken_, hTrackProduct, event);
    auto& trackView = *hTrackProduct;

    edm::Handle<reco::RecoToSimCollection> simAssociationProduct;
    GetProduct(edmSimAssocToken_, simAssociationProduct, event);  
    auto& simAssociation = *simAssociationProduct;
    if (verbose_>1)
      std::cout << "SimAssociation Map Size = " << simAssociation.size() << std::endl;

    unsigned iT = 0;
    for (auto& tPtr : trackView.ptrs()) {
      auto* outTrack = trackMap_->GetMit(tPtr);
      auto&& refToBase = trackView.refAt(iT);
      ++iT;

      // Conversion from Ptr to RefToBase - possible in >= 7_5_0
      // revisit when we have a version based on 7_5_X - can loop over trackMap and not have to get TrackProduct
      // RefToBase<reco::Track> refToBase(std::unique_ptr<reftobase::BaseHolder<reco::Track>>{
      //     new reftobase::IndirectHolder<reco::Track>{
      //       std::unique_ptr<reftobase::RefHolder<Ptr<reco::Track>>>{
      //         new reftobase::RefHolder<Ptr<reco::Track>>{tPtr}
      //       }
      //     }
      //   });

      try {
        auto&& simRefs = simAssociation[refToBase]; //try to get the sim references if existing

        for (auto&& assoc : simRefs) {
          if (assoc.second > 0.5) // require more than 50% shared hits between reco and sim
            outTrack->SetMCPart(trackingMap_->GetMit(assoc.first)); //add reco->sim reference
        }
      }
      catch (edm::Exception&) {
      }
    }
  }
}

mithep::Track*
mithep::FillerTracks::ProduceTrack(reco::Track const& inTrack)
{
  mithep::Track *outTrack = tracks_->Allocate();
  // create track and set the core parameters
  new (outTrack) mithep::Track(inTrack.qoverp(),
                               inTrack.lambda(),
                               inTrack.phi(),
                               inTrack.dxy(),
                               inTrack.dsz());

  outTrack->SetErrors(inTrack.qoverpError(),inTrack.lambdaError(),
                      inTrack.phiError(),inTrack.dxyError(),inTrack.dszError());
                        
  outTrack->SetPtErr(inTrack.ptError());
	
  // fill track quality information
  outTrack->SetChi2(inTrack.chi2());
  outTrack->SetNdof(static_cast<Int_t>(inTrack.ndof()));

  //fill algo enum
  outTrack->SetAlgo(static_cast<mithep::Track::ETrackAlgorithm>(inTrack.algo()));

  //fill quality bitmask
  outTrack->Quality().SetQualityMask(BitMask8(UChar_t(inTrack.qualityMask())));
    
  if (verbose_>2) {
    std::cout << "reco::Track   high purity = " << inTrack.quality(reco::TrackBase::highPurity) << ",  ";
    std::cout << "mithep::Track high purity = " << outTrack->Quality().Quality(mithep::TrackQuality::highPurity) << std::endl;
  }
    
  //fill gsf flag, some type gymastics needed...
  if (dynamic_cast<reco::GsfTrack const*>(&inTrack))
    outTrack->SetIsGsf(kTRUE);
  else
    outTrack->SetIsGsf(kFALSE);
    
  if (verbose_>2) {
    std::cout << "Track pt = " << inTrack.pt();
    std::cout << ", eta = " << inTrack.eta();
    std::cout << ", phi = " << inTrack.phi();
    std::cout << ", nhits = " << inTrack.hitPattern().numberOfHits(reco::HitPattern::TRACK_HITS);
    std::cout << ", numberofvalidhits = " << inTrack.numberOfValidHits();
    std::cout << ", numberofmissinghits = " << inTrack.hitPattern().numberOfLostHits(reco::HitPattern::TRACK_HITS);
    std::cout << ", expectedhitsinner = " << inTrack.hitPattern().numberOfHits(reco::HitPattern::MISSING_INNER_HITS);
    std::cout << ", expectedhitsouter = " << inTrack.hitPattern().numberOfHits(reco::HitPattern::MISSING_OUTER_HITS);
    std::cout << std::endl;
  }

  //fill hit layer map and missing hits
  const reco::HitPattern &hits = inTrack.hitPattern();
  BitMask48 hitLayers;
  BitMask48 missingHitLayers;
  for (Int_t i=0; i<hits.numberOfHits(reco::HitPattern::TRACK_HITS); i++) {
    uint32_t hit = hits.getHitPattern(reco::HitPattern::TRACK_HITS, i);
    if (reco::HitPattern::validHitFilter(hit)) {
      if (reco::HitPattern::trackerHitFilter(hit)) {
        hitLayers.SetBit(hitReader_.Layer(hit));
        if (verbose_>2)
          std::cout << "Found valid hit with layer " << hitReader_.Layer(hit) << std::endl;
      }
    }

    if (reco::HitPattern::getHitType(hit)==1) {
      if (reco::HitPattern::trackerHitFilter(hit)) {
        missingHitLayers.SetBit(hitReader_.Layer(hit));
      }
    }
       
    if (verbose_>2) {
      if (reco::HitPattern::muonDTHitFilter(hit))
        std::cout << "Muon DT Layer = " << reco::HitPattern::getLayer(hit) << std::endl;
      if (reco::HitPattern::muonCSCHitFilter(hit))
        std::cout << "Muon CSC Layer = " << reco::HitPattern::getLayer(hit) << std::endl;
      if (reco::HitPattern::muonRPCHitFilter(hit))
        std::cout << "Muon RPC Layer = " << reco::HitPattern::getLayer(hit) << std::endl;
    }
  }

  outTrack->SetHits(hitLayers);
  outTrack->SetMissingHits(missingHitLayers);

  //set expected inner hits
  BitMask48 expectedHitsInner;
  // search for all good crossed layers (with or without hit)
  for (Int_t hi=0; hi<hits.numberOfHits(reco::HitPattern::MISSING_INNER_HITS); ++hi) {
    uint32_t hit = hits.getHitPattern(reco::HitPattern::MISSING_INNER_HITS, hi);
    if (reco::HitPattern::getHitType(hit) < 2) {
      if (reco::HitPattern::trackerHitFilter(hit)) {
        expectedHitsInner.SetBit(hitReader_.Layer(hit));
      }
    }
  }

  outTrack->SetExpectedHitsInner(expectedHitsInner);

  //set expected outer hits
  BitMask48 expectedHitsOuter;
  // search for all good crossed layers (with or without hit)
  for (Int_t hi=0; hi < hits.numberOfHits(reco::HitPattern::MISSING_OUTER_HITS); ++hi) {
    uint32_t hit = hits.getHitPattern(reco::HitPattern::MISSING_OUTER_HITS, hi);
    if (reco::HitPattern::getHitType(hit) < 2) {
      if (reco::HitPattern::trackerHitFilter(hit)) {
        expectedHitsOuter.SetBit(hitReader_.Layer(hit));
      }
    }
  }

  outTrack->SetExpectedHitsOuter(expectedHitsOuter);

  //fill actual hit counts
  outTrack->SetNHits(inTrack.numberOfValidHits());
  outTrack->SetNPixelHits(inTrack.hitPattern().numberOfValidPixelHits());
  outTrack->SetNMissingHits(inTrack.hitPattern().numberOfLostHits(reco::HitPattern::TRACK_HITS));
  outTrack->SetNExpectedHitsInner(inTrack.hitPattern().numberOfHits(reco::HitPattern::MISSING_INNER_HITS));
  outTrack->SetNExpectedHitsOuter(inTrack.hitPattern().numberOfHits(reco::HitPattern::MISSING_OUTER_HITS));

  if (verbose_>2) {
    std::cout << "OutTrack NHits = " << outTrack->NHits();
    std::cout << ", NMissingHits = " << outTrack->NMissingHits();
    std::cout << ", NExpectedHitsInner = " << outTrack->NExpectedHitsInner();
    std::cout << ", NExpectedHitsOuter = " << outTrack->NExpectedHitsOuter();
    std::cout << std::endl;
  }

  return outTrack;
}

void
mithep::FillerTracks::EcalAssoc(mithep::Track* outTrack, reco::Track const& inTrack,
                                edm::Event const& event, edm::EventSetup const& setup,
                                TrackDetectorAssociator& trackAssociator,
                                MagneticField const& bField)
{
  TrackDetMatchInfo matchInfo;
  //Extra check to allow propagation to work in AOD where no TrackExtra is available
  if (inTrack.extra().isAvailable())
    matchInfo = trackAssociator.associate(event, setup, inTrack, *assocParams_);
  else {
    auto&& initialState = trajectoryStateTransform::initialFreeState(inTrack, &bField);
    matchInfo = trackAssociator.associate(event, setup, *assocParams_, &initialState);
  }
       
  if (matchInfo.isGoodEcal) {
    outTrack->SetEtaEcal(matchInfo.trkGlobPosAtEcal.eta());
    outTrack->SetPhiEcal(matchInfo.trkGlobPosAtEcal.phi());
  }

  //fill supercluster link
  if (barrelSuperClusterIdMap_ || endcapSuperClusterIdMap_) {
    mithep::SuperCluster* cluster = 0;
    for (auto&& tower : matchInfo.crossedTowers) {
      for (uint ihit=0; ihit < tower->constituentsSize() && !cluster; ihit++) {
        DetId hit = tower->constituent(ihit);
        if (barrelSuperClusterIdMap_) {
          cluster = barrelSuperClusterIdMap_->GetMit(hit, false);
          if (!cluster && endcapSuperClusterIdMap_)
            cluster = endcapSuperClusterIdMap_->GetMit(hit, false);
        }
      }
    }
    if (cluster)
      outTrack->SetSCluster(cluster);
  }
}


DEFINE_MITHEP_TREEFILLER(FillerTracks);
