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
  edmToken_                 (GetToken<GsfElectronView>(collector, "edmName",
                                                       "pixelMatchGsfElectrons")),
  pvEdmToken_               (GetToken<reco::VertexCollection>(collector, "pvEdmName",
                                                              "offlinePrimaryVertices")),
  pvBSEdmToken_             (GetToken<reco::VertexCollection>(collector, "pvBSEdmName",
                                                              "offlinePrimaryVerticesWithBS")),  
  eIDCutBasedTightToken_    (GetToken<edm::ValueMap<float> >(collector, "eIDCutBasedTightName", "eidTight")),
  eIDCutBasedLooseToken_    (GetToken<edm::ValueMap<float> >(collector, "eIDCutBasedLooseName", "eidLoose")),
  eIDLikelihoodToken_       (GetToken<edm::ValueMap<float> >(collector, "eIDLikelihoodName", "")),
  generalTracksToken_       (GetToken<reco::TrackCollection>(collector, "generalTracksName", "generalTracks")),
  gsfTracksToken_           (GetToken<reco::GsfTrackCollection>(collector, "gsfTracksName", "electronGsfTracks")),
  conversionsToken_         (GetToken<mitedm::DecayPartCol>(collector, "conversionsName")),
  pvBeamSpotToken_          (GetToken<reco::BeamSpot>(collector, "pvBeamSpotName", "offlineBeamSpot")),
  pvbsBeamSpotToken_        (GetToken<reco::BeamSpot>(collector, "pvbsBeamSpotName", "offlineBeamSpot")),
  eIDCutBasedTightName_     (Conf().getUntrackedParameter<string>("eIDCutBasedTightName", "eidTight")),
  eIDCutBasedLooseName_     (Conf().getUntrackedParameter<string>("eIDCutBasedLooseName", "eidLoose")),
  mitName_                  (Conf().getUntrackedParameter<string>("mitName", Names::gkElectronBrn)),
  electronMapName_          (Conf().getUntrackedParameter<string>("electronMapName", "")),
  electronPFMapName_        (Conf().getUntrackedParameter<string>("electronPFMapName", "")),
  gsfTrackMapName_          (Conf().getUntrackedParameter<string>("gsfTrackMapName", "")),
  trackerTrackMapName_      (Conf().getUntrackedParameter<string>("trackerTrackMapName", "")),
  barrelSuperClusterMapName_(Conf().getUntrackedParameter<string>("barrelSuperClusterMapName", "")),
  endcapSuperClusterMapName_(Conf().getUntrackedParameter<string>("endcapSuperClusterMapName", "")),
  checkClusterActive_       (Conf().getUntrackedParameter<bool>("requireClusterAndGsfMap", true)),
  pfEcalBarrelSuperClusterMapName_(Conf().getUntrackedParameter<string>("pfEcalBarrelSuperClusterMapName", "")),
  pfEcalEndcapSuperClusterMapName_(Conf().getUntrackedParameter<string>("pfEcalEndcapSuperClusterMapName", "")),
  recomputeConversionInfo_  (Conf().getUntrackedParameter<bool>("recomputeConversionInfo", false)),  
  fitUnbiasedVertex_        (Conf().getUntrackedParameter<bool>("fitUnbiasedVertex", true)),
  fillFromPAT_              (Conf().getUntrackedParameter<bool>("fillFromPAT", false)),
  electronMap_              (new mithep::ElectronMap),
  electronPFMap_            (0),
  electrons_                (new mithep::ElectronArr(16)),
  gsfTrackMap_              (0),
  trackerTrackMap_          (0),
  barrelSuperClusterMap_    (0),
  endcapSuperClusterMap_    (0),
  pfEcalBarrelSuperClusterMap_(0),
  pfEcalEndcapSuperClusterMap_(0)
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

  tws.AddBranch(mitName_,&electrons_);
  OS()->add(electrons_,mitName_);

  if (!electronMapName_.empty()) {
    electronMap_->SetBrName(mitName_);
    OS()->add(electronMap_, electronMapName_);
  }
  if (fillFromPAT_ && !electronPFMapName_.empty()) {
    electronPFMap_ = new mithep::CandidateMap;
    electronPFMap_->SetBrName(mitName_);
    OS()->add(electronPFMap_, electronPFMapName_);
  }

  if (!gsfTrackMapName_.empty()) {
    gsfTrackMap_ = OS()->get<TrackMap>(gsfTrackMapName_);
    if (gsfTrackMap_)
      AddBranchDep(mitName_,gsfTrackMap_->GetBrName());
  }
  if (!trackerTrackMapName_.empty()) {
    trackerTrackMap_ = OS()->get<TrackMap>(trackerTrackMapName_);
    if (trackerTrackMap_)
      AddBranchDep(mitName_,trackerTrackMap_->GetBrName());
  }
  if (!barrelSuperClusterMapName_.empty()) {
    barrelSuperClusterMap_ = OS()->get<SuperClusterMap>(barrelSuperClusterMapName_);
    if (barrelSuperClusterMap_)
      AddBranchDep(mitName_,barrelSuperClusterMap_->GetBrName());
  }
  if (!endcapSuperClusterMapName_.empty()) {
    endcapSuperClusterMap_ = OS()->get<SuperClusterMap>(endcapSuperClusterMapName_);
    if (endcapSuperClusterMap_)
      AddBranchDep(mitName_,endcapSuperClusterMap_->GetBrName());
  }
  if (!pfEcalBarrelSuperClusterMapName_.empty()) {
    pfEcalBarrelSuperClusterMap_ = OS()->get<SuperClusterMap>(pfEcalBarrelSuperClusterMapName_);
    if (pfEcalBarrelSuperClusterMap_)
      AddBranchDep(mitName_,pfEcalBarrelSuperClusterMap_->GetBrName());
  }
  if (!pfEcalEndcapSuperClusterMapName_.empty()) {
    pfEcalEndcapSuperClusterMap_ = OS()->get<SuperClusterMap>(pfEcalEndcapSuperClusterMapName_);
    if (pfEcalEndcapSuperClusterMap_)
      AddBranchDep(mitName_,pfEcalEndcapSuperClusterMap_->GetBrName());
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

  edm::Handle<GsfElectronView> hElectronProduct;
  GetProduct(edmToken_, hElectronProduct, event);

  auto& inElectrons = *hElectronProduct;

  if (fillFromPAT_ && inElectrons.size() != 0 &&
      !dynamic_cast<pat::Electron const*>(&inElectrons.at(0)))
    throw edm::Exception(edm::errors::Configuration, "FillerElectrons:FillDataBlock()\n")
      << "Error! fillFromPAT set on non-PAT input";

  // handles to get the electron ID information
  edm::Handle<edm::ValueMap<float> > eidLooseMap;
  edm::Handle<edm::ValueMap<float> > eidTightMap;
  if (!fillFromPAT_) {
    GetProduct(eIDCutBasedLooseToken_, eidLooseMap, event);
    GetProduct(eIDCutBasedTightToken_, eidTightMap, event);
  }

  edm::Handle<edm::ValueMap<float> > eidLikelihoodMap;
  if (!eIDLikelihoodToken_.isUninitialized())
    GetProduct(eIDLikelihoodToken_, eidLikelihoodMap, event);

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

  VertexReProducer* vtxReProducers[] = {0, 0};
  if (fitUnbiasedVertex_) {
    if (hVertex.isValid())
      vtxReProducers[0] = new VertexReProducer(hVertex, event);
    if (hVertexBS.isValid())
      vtxReProducers[1] = new VertexReProducer(hVertexBS, event);
  }

  typedef std::function<void(mithep::Electron&, double)> Setter;

  edm::Handle<reco::VertexCollection>* pvcHandles[] = {&hVertex, &hVertexBS};
  edm::Handle<reco::BeamSpot>* bsHandles[] = {&pvbeamspot, &pvbsbeamspot};
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

    auto gsfTrackRef = inElectron.gsfTrack();
    auto ctfTrackRef = inElectron.closestCtfTrackRef();
    bool gsfAvailable = gsfTrackRef.isAvailable();
    bool ctfAvailable = ctfTrackRef.isAvailable();

    // make proper links to Tracks and Super Clusters
    if (gsfTrackMap_) {
      if (gsfTrackRef.isNonnull()) {
        mithep::Track const* trk = 0;
        try {
          trk = gsfTrackMap_->GetMit(edm::refToPtr(gsfTrackRef));
        }
        catch (edm::Exception& ex) {
          if (checkClusterActive_)
            throw edm::Exception(edm::errors::Configuration, "FillerElectrons:FillDataBlock()\n")
              << "Error! GSF track unmapped collection";
        }
        if (trk)
          outElectron->SetGsfTrk(trk);
      }

      // make links to ambigous gsf tracks

      for (auto agsfi = inElectron.ambiguousGsfTracksBegin(); agsfi != inElectron.ambiguousGsfTracksEnd(); ++agsfi) {
        mithep::Track const* trk = 0;
        try {
          trk = gsfTrackMap_->GetMit(edm::refToPtr(*agsfi));
        }
        catch (edm::Exception& ex) {
          if (checkClusterActive_)
            throw edm::Exception(edm::errors::Configuration, "FillerElectrons:FillDataBlock()\n")
              << "Error! Ambiguous GSF track unmapped collection";
        }
        if (trk)
          outElectron->AddAmbiguousGsfTrack(trk);
      }
    }

    // make tracker track links,
    if (trackerTrackMap_) {
      if (ctfTrackRef.isNonnull()) {
        mithep::Track const* trk = 0;
        try {
          trk = trackerTrackMap_->GetMit(edm::refToPtr(ctfTrackRef));
        }
        catch (edm::Exception& ex) {
          if (checkClusterActive_)
            throw edm::Exception(edm::errors::Configuration, "FillerElectrons:FillDataBlock()\n")
              << "Error! Tracker track unmapped collection";
        }
        if (trk)
          outElectron->SetTrackerTrk(trk);
      }
    }

    if (barrelSuperClusterMap_ && endcapSuperClusterMap_) {
      auto ref = inElectron.superCluster();
      if(ref.isNonnull()) {
        mithep::SuperCluster const* sc = 0;
        try {
          sc = barrelSuperClusterMap_->GetMit(ref, false); // no-throw version
          if (!sc)
            sc = endcapSuperClusterMap_->GetMit(ref, true);
        }
        catch (edm::Exception& ex) {
          if (checkClusterActive_)
            throw edm::Exception(edm::errors::Configuration, "FillerElectrons:FillDataBlock()\n")
              << "Error! Refined SuperCluster reference in unmapped collection";
        }
        if (sc)
          outElectron->SetSuperCluster(sc);
      }
    }

    if (pfEcalBarrelSuperClusterMap_ && pfEcalEndcapSuperClusterMap_) {
      auto ref = inElectron.parentSuperCluster();
      if(ref.isNonnull()) {
        mithep::SuperCluster const* sc = 0;
        try {
          sc = pfEcalBarrelSuperClusterMap_->GetMit(ref, false); //no-throw version
          if (!sc)
            sc = pfEcalEndcapSuperClusterMap_->GetMit(ref, true);
        }
        catch (edm::Exception& ex) {
          if (checkClusterActive_)
            throw edm::Exception(edm::errors::Configuration, "FillerElectrons:FillDataBlock()\n")
              << "Error! PFEcal SuperCluster reference in unmapped collection";
        }
        if (sc)
          outElectron->SetECALOnlySuperCluster(sc);
      }
    }

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
        if (!pvcHandles[iPVType]->isValid())
          continue;

        auto& pvcHandle = *pvcHandles[iPVType];
        auto& bs = **bsHandles[iPVType];

        reco::Vertex const& pv = pvcHandle->at(0);
        reco::Vertex pvub = pvcHandle->at(0);

        if (fitUnbiasedVertex_ && ctfTrackRef.isNonnull() && inElectron.shFracInnerHits() > 0.5) {
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
            auto pvs = vtxReProducers[iPVType]->makeVertices(newTkCollection, bs, setup);
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
      auto convInfo = convFinder.getConversionInfo(inElectron, hGeneralTracks, hGsfTracks, bfield);
  
      outElectron->SetConvFlag(convInfo.flag());
      outElectron->SetConvPartnerDCotTheta(convInfo.dcot());
      outElectron->SetConvPartnerDist(convInfo.dist());
      outElectron->SetConvPartnerRadius(convInfo.radiusOfConversion());
      auto ckfconvTrackRef = convInfo.conversionPartnerCtfTk();
      auto gsfconvTrackRef = convInfo.conversionPartnerGsfTk();
  
      if (gsfconvTrackRef.isNonnull() && gsfTrackMap_) {
        try {
          outElectron->SetConvPartnerTrk(gsfTrackMap_->GetMit(edm::refToPtr(gsfconvTrackRef)));
        }
        catch(...) { 
          if (checkClusterActive_)
            throw edm::Exception(edm::errors::Configuration, "FillerElectrons:FillDataBlock()\n")
              << "Error! GSF track unmapped collection";
        }
      }
      else if (ckfconvTrackRef.isNonnull() && trackerTrackMap_) {
        try {
          outElectron->SetConvPartnerTrk(trackerTrackMap_->GetMit(edm::refToPtr(ckfconvTrackRef)));
        }
        catch(...) {
          if (checkClusterActive_)
            throw edm::Exception(edm::errors::Configuration, "FillerElectrons:FillDataBlock()\n")
              << "Error! Conversion Tracker track unmapped collection";
        }
      }
    }
    else {
      outElectron->SetConvFlag(inElectron.convFlags());
      outElectron->SetConvPartnerDCotTheta(inElectron.convDcot());
      outElectron->SetConvPartnerDist(inElectron.convDist());
      outElectron->SetConvPartnerRadius(inElectron.convRadius());
      auto convTrackRef = inElectron.convPartner();
      if (convTrackRef.isAvailable()) {
        if (dynamic_cast<const reco::GsfTrack*>(convTrackRef.get()) && gsfTrackMap_) {
          try{
            outElectron->
              SetConvPartnerTrk(gsfTrackMap_->GetMit(mitedm::refToBaseToPtr(convTrackRef)));
          }
          catch(...) { 
            if (checkClusterActive_)
              throw edm::Exception(edm::errors::Configuration, "FillerElectrons:FillDataBlock()\n")
                << "Error! GSF track unmapped collection";
          }
        }
        else if (trackerTrackMap_) {
          try{
            outElectron->
              SetConvPartnerTrk(trackerTrackMap_->GetMit(mitedm::refToBaseToPtr(convTrackRef)));
          }
          catch(...) {
            if (checkClusterActive_)
              throw edm::Exception(edm::errors::Configuration, "FillerElectrons:FillDataBlock()\n")
                << "Error! Conversion track unmapped collection";
          }
        }
      }
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

    if (eidLikelihoodMap.isValid())
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

DEFINE_MITHEP_TREEFILLER(FillerElectrons);
