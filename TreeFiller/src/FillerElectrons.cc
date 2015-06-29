#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "MitProd/TreeFiller/interface/FillerElectrons.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"
#include "DataFormats/EgammaReco/interface/ClusterShape.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/Common/interface/RefToPtr.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/TransientTrack/plugins/TransientTrackBuilderESProducer.h"
#include "RecoVertex/GaussianSumVertexFit/interface/GsfVertexTrackCompatibilityEstimator.h"
#include "TrackingTools/IPTools/interface/IPTools.h"
#include "RecoEgamma/EgammaTools/interface/ConversionFinder.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "MagneticField/Engine/interface/MagneticField.h"

#include "MitAna/DataTree/interface/ElectronCol.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataTree/interface/Track.h"
#include "MitEdm/DataFormats/interface/RefToBaseToPtr.h"
#include "MitProd/ObjectService/interface/ObjectService.h"
#include "MitEdm/DataFormats/interface/DecayPart.h"
#include "MitEdm/ConversionRejection/interface/ConversionMatcher.h"
#include "MitEdm/Tools/interface/VertexReProducer.h"
#include "FWCore/Framework/interface/ESHandle.h"

mithep::FillerElectrons::FillerElectrons(const edm::ParameterSet &cfg, edm::ConsumesCollector& collector, ObjectService* os, const char *name, bool active/* = true*/) :
  BaseFiller(cfg,os,name,active),
  recomputeConversionInfo_  (cfg.getUntrackedParameter<bool>("recomputeConversionInfo", false)),
  fitUnbiasedVertex_        (cfg.getUntrackedParameter<bool>("fitUnbiasedVertex", true)),
  fillFromPAT_              (cfg.getUntrackedParameter<bool>("fillFromPAT", false)),
  edmToken_                 (GetToken<GsfElectronView>(collector, cfg, "edmName")), //pixelMatchGsfElectrons
  pvEdmToken_               (GetToken<reco::VertexCollection>(collector, cfg, "pvEdmName", false)), //offlinePrimaryVertices
  pvBSEdmToken_             (GetToken<reco::VertexCollection>(collector, cfg, "pvBSEdmName", false)), //offlinePrimaryVerticesWithBS
  generalTracksToken_       (GetToken<reco::TrackCollection>(collector, cfg, "generalTracksName", recomputeConversionInfo_)), //generalTracks
  gsfTracksToken_           (GetToken<reco::GsfTrackCollection>(collector, cfg, "gsfTracksName", recomputeConversionInfo_)), //electronGsfTracks
  conversionsToken_         (GetToken<mitedm::DecayPartCol>(collector, cfg, "conversionsName", false)), //
  pvBeamSpotToken_          (GetToken<reco::BeamSpot>(collector, cfg, "pvBeamSpotName", !pvEdmToken_.isUninitialized() && fitUnbiasedVertex_)), //offlineBeamSpot
  pvbsBeamSpotToken_        (GetToken<reco::BeamSpot>(collector, cfg, "pvbsBeamSpotName", !pvBSEdmToken_.isUninitialized() && fitUnbiasedVertex_)), //offlineBeamSpot
  footprintToken_           (GetToken<edm::ValueMap<PFCandRefV> >(collector, cfg, "footprintName", !fillFromPAT_)),
  eIDCutBasedTightToken_    (GetToken<edm::ValueMap<float> >(collector, cfg, "eIDCutBasedTightName", !fillFromPAT_)), //eidTight
  eIDCutBasedLooseToken_    (GetToken<edm::ValueMap<float> >(collector, cfg, "eIDCutBasedLooseName", !fillFromPAT_)), //eidLoose
  eIDLikelihoodToken_       (GetToken<edm::ValueMap<float> >(collector, cfg, "eIDLikelihoodName", false)),
  eIDCutBasedTightName_     (cfg.getUntrackedParameter<string>("eIDCutBasedTightName", "eidTight")),
  eIDCutBasedLooseName_     (cfg.getUntrackedParameter<string>("eIDCutBasedLooseName", "eidLoose")),
  mitName_                  (cfg.getUntrackedParameter<string>("mitName", Names::gkElectronBrn)),
  electronMapName_          (cfg.getUntrackedParameter<string>("electronMapName", "")),
  electronPFMapName_        (cfg.getUntrackedParameter<string>("electronPFMapName", "")),
  gsfTrackMapName_          (cfg.getUntrackedParameter<string>("gsfTrackMapName", "")),
  trackerTrackMapName_      (cfg.getUntrackedParameter<string>("trackerTrackMapName", "")),
  barrelSuperClusterMapName_(cfg.getUntrackedParameter<string>("barrelSuperClusterMapName", "")),
  endcapSuperClusterMapName_(cfg.getUntrackedParameter<string>("endcapSuperClusterMapName", "")),
  checkClusterActive_       (cfg.getUntrackedParameter<bool>("requireClusterAndGsfMap", true)),
  pfEcalBarrelSuperClusterMapName_(cfg.getUntrackedParameter<string>("pfEcalBarrelSuperClusterMapName", "")),
  pfEcalEndcapSuperClusterMapName_(cfg.getUntrackedParameter<string>("pfEcalEndcapSuperClusterMapName", "")),
  pfCandidateMapName_       (cfg.getUntrackedParameter<std::string>("pfCandidateMapName", "")),
  electronMap_              (new mithep::ElectronMap),
  electronPFMap_            (0),
  electrons_                (new mithep::ElectronArr(16)),
  gsfTrackMap_              (0),
  trackerTrackMap_          (0),
  eleGsfTrackMap_           (0),
  eleTrackerTrackMap_       (0),
  barrelSuperClusterMap_    (0),
  endcapSuperClusterMap_    (0),
  pfEcalBarrelSuperClusterMap_(0),
  pfEcalEndcapSuperClusterMap_(0),
  pfCandidateMap_(0)
{
}

mithep::FillerElectrons::~FillerElectrons()
{
  delete electrons_;
  delete electronMap_;
  delete electronPFMap_;
}

void
mithep::FillerElectrons::BookDataBlock(TreeWriter &tws)
{
  // Add electron branch to our tree and get our maps.

  tws.AddBranch(mitName_, &electrons_);
  OS()->add(electrons_, mitName_);

  if (!electronMapName_.empty()) {
    electronMap_->SetBrName(mitName_);
    OS()->add(electronMap_, electronMapName_);
  }
  if (fillFromPAT_ && !electronPFMapName_.empty()) {
    electronPFMap_ = new mithep::CandidateMap;
    electronPFMap_->SetBrName(mitName_);
    OS()->add(electronPFMap_, electronPFMapName_);
  }
}

void
mithep::FillerElectrons::PrepareLinks()
{
  if (fillFromPAT_) {
    if (!gsfTrackMapName_.empty()) {
      eleGsfTrackMap_ = OS()->get<ElectronTrackMap>(gsfTrackMapName_);
      if (eleGsfTrackMap_)
        AddBranchDep(mitName_, eleGsfTrackMap_->GetBrName());
    }
    if (!trackerTrackMapName_.empty()) {
      eleTrackerTrackMap_ = OS()->get<ElectronTrackMap>(trackerTrackMapName_);
      if (eleTrackerTrackMap_)
        AddBranchDep(mitName_, eleTrackerTrackMap_->GetBrName());
    }
  }
  else {
    if (!gsfTrackMapName_.empty()) {
      gsfTrackMap_ = OS()->get<TrackMap>(gsfTrackMapName_);
      if (gsfTrackMap_)
        AddBranchDep(mitName_, gsfTrackMap_->GetBrName());
    }
    if (!trackerTrackMapName_.empty()) {
      trackerTrackMap_ = OS()->get<TrackMap>(trackerTrackMapName_);
      if (trackerTrackMap_)
        AddBranchDep(mitName_, trackerTrackMap_->GetBrName());
    }
  }
  if (!barrelSuperClusterMapName_.empty()) {
    barrelSuperClusterMap_ = OS()->get<SuperClusterMap>(barrelSuperClusterMapName_);
    if (barrelSuperClusterMap_)
      AddBranchDep(mitName_, barrelSuperClusterMap_->GetBrName());
  }
  if (!endcapSuperClusterMapName_.empty()) {
    endcapSuperClusterMap_ = OS()->get<SuperClusterMap>(endcapSuperClusterMapName_);
    if (endcapSuperClusterMap_)
      AddBranchDep(mitName_, endcapSuperClusterMap_->GetBrName());
  }
  if (!pfEcalBarrelSuperClusterMapName_.empty()) {
    pfEcalBarrelSuperClusterMap_ = OS()->get<SuperClusterMap>(pfEcalBarrelSuperClusterMapName_);
    if (pfEcalBarrelSuperClusterMap_)
      AddBranchDep(mitName_, pfEcalBarrelSuperClusterMap_->GetBrName());
  }
  if (!pfEcalEndcapSuperClusterMapName_.empty()) {
    pfEcalEndcapSuperClusterMap_ = OS()->get<SuperClusterMap>(pfEcalEndcapSuperClusterMapName_);
    if (pfEcalEndcapSuperClusterMap_)
      AddBranchDep(mitName_, pfEcalEndcapSuperClusterMap_->GetBrName());
  }
  if (!pfCandidateMapName_.empty()) {
    pfCandidateMap_ = OS()->get<PFCandidateMap>(pfCandidateMapName_);
    if (pfCandidateMap_)
      AddBranchDep(mitName_, pfCandidateMap_->GetBrName());
  }
}

void
mithep::FillerElectrons::FillDataBlock(const edm::Event &event, const edm::EventSetup &setup)	 
{
  // Fill electrons from edm collection into our collection.

  electrons_  ->Delete();
  electronMap_->Reset();
  if (electronPFMap_)
    electronPFMap_->Reset();

  conversionInfoStore_.clear();

  edm::Handle<GsfElectronView> hElectronProduct;
  GetProduct(edmToken_, hElectronProduct, event);
  auto& inElectrons = *hElectronProduct;

  if (fillFromPAT_ && inElectrons.size() != 0 &&
      !dynamic_cast<pat::Electron const*>(&inElectrons.at(0)))
    throw edm::Exception(edm::errors::Configuration, "FillerElectrons:FillDataBlock()\n")
      << "Error! fillFromPAT set on non-PAT input";

  // handles to get the electron ID information
  edm::ValueMap<float> const* eidLooseMap = 0;
  edm::ValueMap<float> const* eidTightMap = 0;
  if (!fillFromPAT_) {
    edm::Handle<edm::ValueMap<float> > hEidLooseMap;
    GetProduct(eIDCutBasedLooseToken_, hEidLooseMap, event);
    eidLooseMap = hEidLooseMap.product();
    edm::Handle<edm::ValueMap<float> > hEidTightMap;
    GetProduct(eIDCutBasedTightToken_, hEidTightMap, event);
    eidTightMap = hEidTightMap.product();
  }

  edm::ValueMap<float> const* eidLikelihoodMap = 0;
  if (!eIDLikelihoodToken_.isUninitialized()) {
    edm::Handle<edm::ValueMap<float> > eidLikelihoodMapH;
    GetProduct(eIDLikelihoodToken_, eidLikelihoodMapH, event);
    eidLikelihoodMap = eidLikelihoodMapH.product();
  }

  edm::Handle<reco::TrackCollection> hGeneralTracks;
  edm::Handle<reco::GsfTrackCollection> hGsfTracks;
  if (recomputeConversionInfo_) {
    GetProduct(generalTracksToken_, hGeneralTracks, event);
    GetProduct(gsfTracksToken_, hGsfTracks, event);
  }

  edm::Handle<mitedm::DecayPartCol> hConversions;
  if (!conversionsToken_.isUninitialized())
    GetProduct(conversionsToken_, hConversions, event);
  
  mitedm::ConversionMatcher convMatcher;
     
  edm::ESHandle<TransientTrackBuilder> hTransientTrackBuilder;
  setup.get<TransientTrackRecord>().get("TransientTrackBuilder",hTransientTrackBuilder);
  auto transientTrackBuilder = hTransientTrackBuilder.product();
  
  //Get Magnetic Field from event setup, taking value at (0,0,0)
  double bfield = 0.;
  if (recomputeConversionInfo_) {
    edm::ESHandle<MagneticField> magneticField;
    setup.get<IdealMagneticFieldRecord>().get(magneticField);
    bfield = magneticField->inTesla(GlobalPoint(0.,0.,0.)).z();
  }

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

  typedef std::function<void(mithep::Electron&, double)> Setter;

  Setter d0pvSetters[] = {
    &mithep::Electron::SetD0PV, &mithep::Electron::SetD0PVBS,
    &mithep::Electron::SetD0PVUB, &mithep::Electron::SetD0PVUBBS
  };
  Setter d0pverrSetters[] = {
    &mithep::Electron::SetD0PVErr, &mithep::Electron::SetD0PVBSErr,
    &mithep::Electron::SetD0PVUBErr, &mithep::Electron::SetD0PVUBBSErr
  };
  Setter ip3dpvSetters[] = {
    &mithep::Electron::SetIp3dPV, &mithep::Electron::SetIp3dPVBS,
    &mithep::Electron::SetIp3dPVUB, &mithep::Electron::SetIp3dPVUBBS
  };
  Setter ip3dpverrSetters[] = {
    &mithep::Electron::SetIp3dPVErr, &mithep::Electron::SetIp3dPVBSErr,
    &mithep::Electron::SetIp3dPVUBErr, &mithep::Electron::SetIp3dPVUBBSErr
  };
  Setter d0pvckfSetters[] = {
    &mithep::Electron::SetD0PVCkf, &mithep::Electron::SetD0PVBSCkf,
    &mithep::Electron::SetD0PVUBCkf, &mithep::Electron::SetD0PVUBBSCkf
  };
  Setter d0pvckferrSetters[] = {
    &mithep::Electron::SetD0PVCkfErr, &mithep::Electron::SetD0PVBSCkfErr,
    &mithep::Electron::SetD0PVUBCkfErr, &mithep::Electron::SetD0PVUBBSCkfErr
  };
  Setter ip3dpvckfSetters[] = {
    &mithep::Electron::SetIp3dPVCkf, &mithep::Electron::SetIp3dPVBSCkf,
    &mithep::Electron::SetIp3dPVUBCkf, &mithep::Electron::SetIp3dPVUBBSCkf
  };
  Setter ip3dpvckferrSetters[] = {
    &mithep::Electron::SetIp3dPVCkfErr, &mithep::Electron::SetIp3dPVBSCkfErr,
    &mithep::Electron::SetIp3dPVUBCkfErr, &mithep::Electron::SetIp3dPVUBBSCkfErr
  };

  // loop over electrons
  unsigned iElectron = 0;
  for (auto&& inElectron : inElectrons) {
    // the index and Ref are needed for the eID association Map
    edm::Ref<GsfElectronView> eRef(hElectronProduct, iElectron);
    edm::Ptr<reco::GsfElectron> ePtr(hElectronProduct, iElectron);
    ++iElectron;

    auto outElectron = electrons_->AddNew();

    outElectron->SetPtEtaPhi(inElectron.pt(),inElectron.eta(),inElectron.phi());
         
    outElectron->SetCharge(inElectron.charge());
    outElectron->SetScPixCharge(inElectron.scPixCharge());
    outElectron->SetESuperClusterOverP(inElectron.eSuperClusterOverP());
    outElectron->SetESeedClusterOverPout(inElectron.eSeedClusterOverPout());
    outElectron->SetEEleClusterOverPout(inElectron.eEleClusterOverPout());
    outElectron->SetPIn(inElectron.trackMomentumAtVtx().R());
    outElectron->SetPOut(inElectron.trackMomentumOut().R());
    outElectron->SetDeltaEtaSuperClusterTrackAtVtx(inElectron.deltaEtaSuperClusterTrackAtVtx());
    outElectron->SetDeltaEtaSeedClusterTrackAtCalo(inElectron.deltaEtaSeedClusterTrackAtCalo());
    outElectron->SetDeltaPhiSuperClusterTrackAtVtx(inElectron.deltaPhiSuperClusterTrackAtVtx());
    outElectron->SetDeltaPhiSeedClusterTrackAtCalo(inElectron.deltaPhiSeedClusterTrackAtCalo());
    outElectron->SetIsEnergyScaleCorrected(inElectron.isEnergyScaleCorrected());
    //outElectron->SetIsMomentumCorrected(inElectron.isMomentumCorrected());
    outElectron->SetNumberOfClusters(inElectron.basicClustersSize());
    outElectron->SetClassification(inElectron.classification());
    outElectron->SetFBrem(inElectron.fbrem());
    outElectron->SetEcalEnergy(inElectron.correctedEcalEnergy());
    outElectron->SetEcalEnergyError(inElectron.correctedEcalEnergyError());
    outElectron->SetTrackMomentumError(inElectron.trackMomentumError());
    
    // pflow electron stuff
    outElectron->SetIsEcalDriven(inElectron.ecalDrivenSeed());
    outElectron->SetIsTrackerDriven(inElectron.trackerDrivenSeed());
    outElectron->SetMva(inElectron.mva_Isolated());
    
    // shower shape variables   
    outElectron->SetE15(inElectron.e1x5());
    outElectron->SetE25Max(inElectron.e2x5Max());
    outElectron->SetE55(inElectron.e5x5());
    outElectron->SetCovEtaEta(inElectron.sigmaEtaEta());
    outElectron->SetCoviEtaiEta(inElectron.sigmaIetaIeta());
    outElectron->SetCoviEtaiEta5x5(inElectron.full5x5_sigmaIetaIeta());
    outElectron->SetHadronicOverEm(inElectron.hcalOverEcal());
    outElectron->SetHcalDepth1OverEcal(inElectron.hcalDepth1OverEcal());
    outElectron->SetHcalDepth2OverEcal(inElectron.hcalDepth2OverEcal());
    outElectron->SetHadOverEmTow(inElectron.hcalOverEcalBc()); 

    // fill isolation variables for both cone sizes
    outElectron->SetEcalRecHitIsoDr04(inElectron.dr04EcalRecHitSumEt());
    outElectron->SetHcalDepth1TowerSumEtDr04(inElectron.dr04HcalDepth1TowerSumEt());
    outElectron->SetHcalDepth2TowerSumEtDr04(inElectron.dr04HcalDepth2TowerSumEt());
    outElectron->SetTrackIsolationDr04(inElectron.dr04TkSumPt());
    outElectron->SetHCalIsoTowDr04(inElectron.dr04HcalTowerSumEtBc());
    outElectron->SetEcalRecHitIsoDr03(inElectron.dr03EcalRecHitSumEt());
    outElectron->SetHcalTowerSumEtDr03(inElectron.dr03HcalTowerSumEt());
    outElectron->SetHcalDepth1TowerSumEtDr03(inElectron.dr03HcalDepth1TowerSumEt());
    outElectron->SetHcalDepth2TowerSumEtDr03(inElectron.dr03HcalDepth2TowerSumEt());
    outElectron->SetTrackIsolationDr03(inElectron.dr03TkSumPt());
    outElectron->SetHCalIsoTowDr03(inElectron.dr03HcalTowerSumEtBc());    
    
    //pflow isolation
    outElectron->SetPFChargedHadronIso(inElectron.pfIsolationVariables().sumChargedHadronPt);
    outElectron->SetPFNeutralHadronIso(inElectron.pfIsolationVariables().sumNeutralHadronEt);
    outElectron->SetPFPhotonIso       (inElectron.pfIsolationVariables().sumPhotonEt);
    
    // fiducial flags
    outElectron->SetIsEB(inElectron.isEB());
    outElectron->SetIsEE(inElectron.isEE());
    outElectron->SetIsEBEEGap(inElectron.isEBEEGap());
    outElectron->SetIsEBEtaGap(inElectron.isEBEtaGap());
    outElectron->SetIsEBPhiGap(inElectron.isEBPhiGap());
    outElectron->SetIsEEDeeGap(inElectron.isEEDeeGap());
    outElectron->SetIsEERingGap(inElectron.isEERingGap());
    
    // gsf-tracker match quality
    outElectron->SetFracSharedHits(inElectron.shFracInnerHits());

    auto&& gsfTrackRef = inElectron.gsfTrack();
    auto&& ctfTrackRef = inElectron.closestCtfTrackRef();
    bool gsfAvailable = gsfTrackRef.isAvailable();
    bool ctfAvailable = ctfTrackRef.isAvailable();

    //compute NLayersWithMeasurement for associated ctf track
    if (ctfAvailable)
      outElectron->SetCTFTrkNLayersWithMeasurement(ctfTrackRef->hitPattern().trackerLayersWithMeasurement());
    else
      outElectron->SetCTFTrkNLayersWithMeasurement(-1);

    //compute impact parameter with respect to PV
    if (gsfAvailable) {
      auto tt = transientTrackBuilder->build(gsfTrackRef);
      auto& gsfTrack = *gsfTrackRef;

      if (verbose_ > 1) {
        printf("gsf track      pt = %5f\n", gsfTrack.pt());
        printf("gsf track mode pt = %5f\n", gsfTrack.ptMode());
        printf("ttrack         pt = %5f\n", tt.initialFreeState().momentum().perp());
      }

      reco::TransientTrack ttckf = ctfAvailable ? transientTrackBuilder->build(ctfTrackRef) : reco::TransientTrack();

      for (unsigned iPVType : {0, 1}) {
        if (!vertexCols[iPVType])
          continue;

        reco::Vertex const& pv = vertexCols[iPVType]->at(0);
        reco::Vertex pvub = pv;

        if (fitUnbiasedVertex_ && ctfTrackRef.isNonnull() && inElectron.shFracInnerHits() > 0.5) {
          if (!beamspots[iPVType])
            throw edm::Exception(edm::errors::Configuration, "FillerElectrons:FillDataBlock()\n")
              << "Null Beamspot pointer";

          reco::TrackCollection newTkCollection;
          bool foundMatch = false;
          for (auto itk = pv.tracks_begin(); itk != pv.tracks_end(); ++itk) {
            if (itk->get() == ctfTrackRef.get()) {
              foundMatch = true;
              continue;
            }
            newTkCollection.push_back(*itk->get());
          }

          if (foundMatch) {
            auto&& pvs = vtxReProducers[iPVType]->makeVertices(newTkCollection, *beamspots[iPVType], setup);
            if (pvs.size() > 0)
              pvub = pvs.front();      // take the first in the list
          }
        }
        
        // preserve sign of transverse impact parameter (cross-product definition from track, not
        // lifetime-signing)
        const double ipsign = ((-gsfTrack.dxy(pv.position())) >= 0) ? 1. : -1.;

        reco::Vertex const* verts[] = {&pv, &pvub};
        for (unsigned iBias : {0, 1}) {
          unsigned iSetter = iBias * 2 + iPVType;

          auto d0pv = IPTools::absoluteTransverseImpactParameter(tt, *verts[iBias]);
          if (d0pv.first) {
            d0pvSetters[iSetter](*outElectron, ipsign * d0pv.second.value());
            d0pverrSetters[iSetter](*outElectron, d0pv.second.error());
          }
          else
            d0pvSetters[iSetter](*outElectron, -99.);

          auto ip3dpv = IPTools::absoluteImpactParameter3D(tt, *verts[iBias]);
          if (ip3dpv.first) {
            ip3dpvSetters[iSetter](*outElectron, ipsign * ip3dpv.second.value());
            ip3dpverrSetters[iSetter](*outElectron, ip3dpv.second.error());
          }
          else
            ip3dpvSetters[iSetter](*outElectron, -99.);
        }

        if (ctfAvailable) {
          const double ckfipsign = ((-ctfTrackRef->dxy(pv.position())) >= 0) ? 1. : -1.;

          for (unsigned iBias : {0, 1}) {
            unsigned iSetter = iBias * 2 + iPVType;

            auto d0pvckf = IPTools::absoluteTransverseImpactParameter(ttckf, *verts[iBias]);
            if (d0pvckf.first) {
              d0pvckfSetters[iSetter](*outElectron, ckfipsign * d0pvckf.second.value());
              d0pvckferrSetters[iSetter](*outElectron, d0pvckf.second.error());
            }
            else
              d0pvckfSetters[iSetter](*outElectron, -999.0);
  
            auto ip3dpvckf = IPTools::absoluteImpactParameter3D(ttckf, *verts[iBias]);
            if (ip3dpvckf.first) {
              ip3dpvckfSetters[iSetter](*outElectron, ckfipsign * ip3dpvckf.second.value());
              ip3dpvckferrSetters[iSetter](*outElectron, ip3dpvckf.second.error());
            }
            else
              ip3dpvckfSetters[iSetter](*outElectron, -999.);
          }
        }
        else {
          for (unsigned iBias : {0, 1}) {
            unsigned iSetter = iBias * 2 + iPVType;

            d0pvckfSetters[iSetter](*outElectron, -999.0);
            ip3dpvckfSetters[iSetter](*outElectron, -999.);
          }
        }
      }
    }

    //fill conversion partner track info
    if (recomputeConversionInfo_) {
      ConversionFinder convFinder; // does not have member data; essentially a static class
      outElectron->SetConvPartnerDCotTheta(inElectron.convDcot());
      auto&& convInfo = convFinder.getConversionInfo(inElectron, hGeneralTracks, hGsfTracks, bfield);
      conversionInfoStore_.emplace(outElectron, convInfo);
  
      outElectron->SetConvFlag(convInfo.flag());
      outElectron->SetConvPartnerDCotTheta(convInfo.dcot());
      outElectron->SetConvPartnerDist(convInfo.dist());
      outElectron->SetConvPartnerRadius(convInfo.radiusOfConversion());
    }
    else {
      outElectron->SetConvFlag(inElectron.convFlags());
      outElectron->SetConvPartnerDCotTheta(inElectron.convDcot());
      outElectron->SetConvPartnerDist(inElectron.convDist());
      outElectron->SetConvPartnerRadius(inElectron.convRadius());
    }

    // fill Electron ID information
    if (fillFromPAT_) {
      auto& patElectron = static_cast<pat::Electron const&>(inElectron);
      outElectron->SetPassLooseID(patElectron.electronID(eIDCutBasedLooseName_));
      outElectron->SetPassTightID(patElectron.electronID(eIDCutBasedTightName_));
    }
    else {
      outElectron->SetPassLooseID((*eidLooseMap)[eRef]);
      outElectron->SetPassTightID((*eidTightMap)[eRef]);
    }

    if (eidLikelihoodMap)
      outElectron->SetIDLikelihood((*eidLikelihoodMap)[eRef]);

    // fill corrected expected inner hits
    if (gsfTrackRef.isNonnull()) {
      outElectron->
        SetCorrectedNExpectedHitsInner(gsfTrackRef->hitPattern().numberOfHits(reco::HitPattern::MISSING_INNER_HITS));
    }

    //fill additional conversion flag
    if (hConversions.isValid())
      outElectron->SetMatchesVertexConversion(convMatcher.matchesGoodConversion(inElectron, hConversions));
    
    // add electron to map
    electronMap_->Add(ePtr, outElectron);

    if (electronPFMap_) {
      auto& patElectron = dynamic_cast<pat::Electron const&>(inElectron);

      unsigned iS = 0;

      // first sourceCandidatePtr may point to pfCandidates instead of packedPFCandidates
      if (patElectron.pfCandidateRef().isNonnull())
        ++iS;

      unsigned nS = patElectron.numberOfSourceCandidatePtrs();
      for (; iS != nS; ++iS)
        electronPFMap_->Add(patElectron.sourceCandidatePtr(iS), outElectron);
    }

    if (verbose_ > 1) {
      double recomass = sqrt(inElectron.energy()*inElectron.energy() - inElectron.p()*inElectron.p());
      printf(" mithep::Electron,    pt=%5f, eta=%5f, phi=%5f, energy=%5f, p=%5f, mass=%5f\n",
             outElectron->Pt(), outElectron->Eta(), outElectron->Phi(), 
             outElectron->E(), outElectron->P(), outElectron->Mass());
      printf("reco::GsfElectron   , pt=%5f, eta=%5f, phi=%5f, energy=%5f, p=%5f, mass=%5f\n",
             inElectron.pt(), inElectron.eta(), inElectron.phi(), inElectron.energy(), inElectron.p(), recomass);  
    }
  }

  electrons_->Trim();

  delete vtxReProducers[0];
  delete vtxReProducers[1];
}

void
mithep::FillerElectrons::ResolveLinks(edm::Event const& event, edm::EventSetup const&)
{
  edm::ValueMap<PFCandRefV> const* footprintMap = 0;
  if (!fillFromPAT_ && pfCandidateMap_) {
    edm::Handle<edm::ValueMap<PFCandRefV> > hFootprintMap;
    GetProduct(footprintToken_, hFootprintMap, event);
    footprintMap = hFootprintMap.product();
  }

  // make proper links to Tracks and Super Clusters

  for (auto& mapElem : electronMap_->FwdMap()) {
    auto&& ePtr = mapElem.first;
    auto& inElectron = static_cast<reco::GsfElectron const&>(*ePtr);
    auto* outElectron = mapElem.second;

    if (fillFromPAT_) {
      if (eleGsfTrackMap_) {
        auto* trk = eleGsfTrackMap_->GetMit(ePtr, false);
        if (trk)
          outElectron->SetGsfTrk(trk);
        else if (checkClusterActive_)
          throw edm::Exception(edm::errors::Configuration, "FillerElectrons:FillDataBlock()\n")
            << "Error! GSF track unmapped collection";
      }

      if (eleTrackerTrackMap_) {
        reco::TrackRef trkRef;
        // bug in reco::GsfElectron: closestCtfTrackRef is not virtual!
        auto* patE = dynamic_cast<pat::Electron const*>(ePtr.get());
        if (patE)
          trkRef = patE->closestCtfTrackRef();
        else
          trkRef = ePtr->closestCtfTrackRef();

        if (trkRef.isNonnull()) {
          auto* trk = eleTrackerTrackMap_->GetMit(ePtr, false);
          if (trk)
            outElectron->SetTrackerTrk(trk);
          else if (checkClusterActive_)
            throw edm::Exception(edm::errors::Configuration, "FillerElectrons:FillDataBlock()\n")
              << "Error! Tracker track unmapped collection";
        }
      }
    }
    else {
      if (gsfTrackMap_) {
        auto&& gsfTrackRef = inElectron.gsfTrack();
        if (gsfTrackRef.isNonnull()) {
          auto* trk = gsfTrackMap_->GetMit(edm::refToPtr(gsfTrackRef), false);
          if (trk)
            outElectron->SetGsfTrk(trk);
          else if (checkClusterActive_)
            throw edm::Exception(edm::errors::Configuration, "FillerElectrons:FillDataBlock()\n")
              << "Error! GSF track unmapped collection";
        }
        // make links to ambigous gsf tracks
        for (auto&& agsfi = inElectron.ambiguousGsfTracksBegin(); agsfi != inElectron.ambiguousGsfTracksEnd(); ++agsfi) {
          mithep::Track const* trk = gsfTrackMap_->GetMit(edm::refToPtr(*agsfi), false);

          if (trk)
            outElectron->AddAmbiguousGsfTrack(trk);
          else if (checkClusterActive_)
            throw edm::Exception(edm::errors::Configuration, "FillerElectrons:FillDataBlock()\n")
              << "Error! Ambiguous GSF track unmapped collection";
        }
      }
      // make tracker track links
      if (trackerTrackMap_) {
        auto&& ctfTrackRef = inElectron.closestCtfTrackRef();
        if (ctfTrackRef.isNonnull()) {
          mithep::Track const* trk = trackerTrackMap_->GetMit(edm::refToPtr(ctfTrackRef), false);
          if (trk)
            outElectron->SetTrackerTrk(trk);
          else if (checkClusterActive_)
            throw edm::Exception(edm::errors::Configuration, "FillerElectrons:FillDataBlock()\n")
              << "Error! Tracker track unmapped collection";
        }
      }
    }

    // make supercluster links
    if (barrelSuperClusterMap_ && endcapSuperClusterMap_) {
      auto&& scRef = inElectron.superCluster();

      if(scRef.isNonnull()) {
        mithep::SuperCluster const* sc = barrelSuperClusterMap_->GetMit(scRef, false);
        if (!sc)
          sc = endcapSuperClusterMap_->GetMit(scRef, false);

        if (sc)
          outElectron->SetSuperCluster(sc);
        else if (checkClusterActive_)
          throw edm::Exception(edm::errors::Configuration, "FillerElectrons:FillDataBlock()\n")
            << "Error! Refined SuperCluster reference in unmapped collection";
      }
    }

    if (pfEcalBarrelSuperClusterMap_ && pfEcalEndcapSuperClusterMap_) {
      auto&& scRef = inElectron.parentSuperCluster();

      if(scRef.isNonnull()) {
        mithep::SuperCluster const* sc = pfEcalBarrelSuperClusterMap_->GetMit(scRef, false);
        if (!sc)
          sc = pfEcalEndcapSuperClusterMap_->GetMit(scRef, false);

        if (sc)
          outElectron->SetECALOnlySuperCluster(sc);
        else if (checkClusterActive_)
          throw edm::Exception(edm::errors::Configuration, "FillerElectrons:FillDataBlock()\n")
            << "Error! PFEcal SuperCluster reference in unmapped collection";
      }
    }

    //fill conversion partner track info
    if (recomputeConversionInfo_) {
      auto& convInfo = conversionInfoStore_[outElectron];
  
      auto&& ckfconvTrackRef = convInfo.conversionPartnerCtfTk();
      auto&& gsfconvTrackRef = convInfo.conversionPartnerGsfTk();
  
      if (gsfconvTrackRef.isNonnull() && gsfTrackMap_) {
        auto* trk = gsfTrackMap_->GetMit(edm::refToPtr(gsfconvTrackRef), false);

        if (trk)
          outElectron->SetConvPartnerTrk(trk);
        else if (checkClusterActive_)
          throw edm::Exception(edm::errors::Configuration, "FillerElectrons:FillDataBlock()\n")
            << "Error! GSF track unmapped collection";
      }
      else if (ckfconvTrackRef.isNonnull() && trackerTrackMap_) {
        auto* trk = trackerTrackMap_->GetMit(edm::refToPtr(ckfconvTrackRef), false);

        if (trk)
          outElectron->SetConvPartnerTrk(trk);
        else if (checkClusterActive_)
          throw edm::Exception(edm::errors::Configuration, "FillerElectrons:FillDataBlock()\n")
            << "Error! Conversion Tracker track unmapped collection";
      }
    }
    else {
      auto&& convTrackRef = inElectron.convPartner();
      if (convTrackRef.isAvailable()) {
        if (dynamic_cast<const reco::GsfTrack*>(convTrackRef.get()) && gsfTrackMap_) {
          auto* trk = gsfTrackMap_->GetMit(mitedm::refToBaseToPtr(convTrackRef), false);

          if (trk)
            outElectron->SetConvPartnerTrk(trk);
          else if (checkClusterActive_)
            throw edm::Exception(edm::errors::Configuration, "FillerElectrons:FillDataBlock()\n")
              << "Error! GSF track unmapped collection";
        }
        else if (trackerTrackMap_) {
          auto* trk = trackerTrackMap_->GetMit(mitedm::refToBaseToPtr(convTrackRef), false);

          if (trk)
            outElectron->SetConvPartnerTrk(trk);
          else if (checkClusterActive_)
            throw edm::Exception(edm::errors::Configuration, "FillerElectrons:FillDataBlock()\n")
              << "Error! Conversion track unmapped collection";
        }
      }
    }

    // make link to footprint PFCandidates
    if (pfCandidateMap_) {
      if (fillFromPAT_) {
        auto& patElectron = static_cast<pat::Electron const&>(inElectron);
        auto&& footprint = patElectron.associatedPackedPFCandidates();
        for (auto& candRef : footprint) {
          auto* mitCand = pfCandidateMap_->GetMit(reco::CandidatePtr(edm::refToPtr(candRef)));
          outElectron->AddFootprintCandidate(mitCand);
        }
      }
      else { // footprintMap must exist
        auto& footprint = (*footprintMap)[ePtr];
        for (auto& candRef : footprint) {
          auto* mitCand = pfCandidateMap_->GetMit(reco::CandidatePtr(edm::refToPtr(candRef)));
          outElectron->AddFootprintCandidate(mitCand);
        }
      }
    }
  }
}

DEFINE_MITHEP_TREEFILLER(FillerElectrons);
