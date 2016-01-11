#include "MitProd/TreeFiller/interface/FillerPhotons.h"

#include "FWCore/Framework/interface/ESHandle.h"
#include "DataFormats/EgammaCandidates/interface/Photon.h"
#include "DataFormats/EgammaCandidates/interface/Conversion.h"
#include "DataFormats/EgammaCandidates/interface/ConversionFwd.h"
#include "DataFormats/PatCandidates/interface/Photon.h"
#include "DataFormats/Common/interface/RefToPtr.h"
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/CaloTopology/interface/CaloSubdetectorTopology.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"

#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataTree/interface/PhotonCol.h"
#include "MitProd/ObjectService/interface/ObjectService.h"

#include <iostream>

mithep::FillerPhotons::FillerPhotons(edm::ParameterSet const& cfg, edm::ConsumesCollector& collector, mithep::ObjectService* os, char const* name, bool active/* = true*/) :
  BaseFiller(cfg, os, name, active),
  fillFromPAT_              (cfg.getUntrackedParameter<bool>("fillFromPAT", false)),
  edmToken_                 (GetToken<PhotonView>(collector, cfg, "edmName")), //photons
  HBHERecHitsEdmToken_      (GetToken<HBHERecHitCollection>(collector, cfg, "HBHERecHitsEdmName", false)), //reducedHcalRecHits:hbhereco
  phIDCutBasedTightToken_   (GetToken<edm::ValueMap<bool> >(collector, cfg, "phIDCutBasedTightName", false)), //PhotonIDProd:PhotonCutBasedIDTight
  phIDCutBasedLooseToken_   (GetToken<edm::ValueMap<bool> >(collector, cfg, "phIDCutBasedLooseName", false)), //PhotonIDProd:PhotonCutBasedIDLoose
  phIDCutBasedTightName_    (cfg.getUntrackedParameter<std::string>("phIDCutBasedTightName", "PhotonIDProd:PhotonCutBasedIDTight")),
  phIDCutBasedLooseName_    (cfg.getUntrackedParameter<std::string>("phIDCutBasedLooseName", "PhotonIDProd:PhotonCutBasedIDLoose")),
  footprintToken_           (GetToken<edm::ValueMap<PFCandRefV> >(collector, cfg, "footprintName", !fillFromPAT_)),
  mitName_                  (cfg.getUntrackedParameter<std::string>("mitName", Names::gkPhotonBrn)),
  conversionMapName_        (cfg.getUntrackedParameter<std::string>("conversionMapName", "")),
  oneLegConversionMapName_  (cfg.getUntrackedParameter<std::string>("oneLegConversionMapName", "")),
  barrelSuperClusterMapName_(cfg.getUntrackedParameter<std::string>("barrelSuperClusterMapName", "")),
  endcapSuperClusterMapName_(cfg.getUntrackedParameter<std::string>("endcapSuperClusterMapName", "")),
  checkClusterActive_       (cfg.getUntrackedParameter<bool>("requireClusterMap", true)),
  pfEcalBarrelSuperClusterMapName_(cfg.getUntrackedParameter<std::string>("pfEcalBarrelSuperClusterMapName", "")),
  pfEcalEndcapSuperClusterMapName_(cfg.getUntrackedParameter<std::string>("pfEcalEndcapSuperClusterMapName", "")),
  pfCandidateMapName_       (cfg.getUntrackedParameter<std::string>("pfCandidateMapName", "")),
  photonMapName_            (cfg.getUntrackedParameter<std::string>("photonMapName", "")),
  photonPFMapName_          (cfg.getUntrackedParameter<std::string>("photonPFMapName", "")),
  photonMap_                (new mithep::PhotonMap),
  photonPFMap_              (0),
  photons_                  (new mithep::PhotonArr(16)),
  conversionMap_            (0),
  oneLegConversionMap_      (0),
  barrelSuperClusterMap_    (0),
  endcapSuperClusterMap_    (0),
  pfEcalBarrelSuperClusterMap_(0),
  pfEcalEndcapSuperClusterMap_(0),
  pfCandidateMap_(0)
{
}

mithep::FillerPhotons::~FillerPhotons()
{
  delete photons_;
  delete photonMap_;
  delete photonPFMap_;
}

void
mithep::FillerPhotons::BookDataBlock(TreeWriter& tws)
{
  // Add photon branch to tree

  tws.AddBranch(mitName_, &photons_);
  OS()->add(photons_, mitName_);

  if (!photonMapName_.empty()) {
    photonMap_->SetBrName(mitName_);
    OS()->add(photonMap_, photonMapName_);
  }

  if (!photonPFMapName_.empty()) {
    photonPFMap_ = new mithep::CandidateMap;
    photonPFMap_->SetBrName(mitName_);
    OS()->add(photonPFMap_, photonPFMapName_);
  }
}

void
mithep::FillerPhotons::PrepareLinks()
{
  if (!conversionMapName_.empty()) {
    conversionMap_ = OS()->get<ConversionDecayMap>(conversionMapName_);
    AddBranchDep(mitName_, conversionMap_->GetBrName());
  }
  if (!oneLegConversionMapName_.empty()) {
    oneLegConversionMap_ = OS()->get<ConversionDecayMap>(oneLegConversionMapName_);
    AddBranchDep(mitName_, oneLegConversionMap_->GetBrName());
  }
  if (!barrelSuperClusterMapName_.empty()) {
    barrelSuperClusterMap_ = OS()->get<SuperClusterMap>(barrelSuperClusterMapName_);
    AddBranchDep(mitName_, barrelSuperClusterMap_->GetBrName());
  }
  if (!endcapSuperClusterMapName_.empty()) {
    endcapSuperClusterMap_ = OS()->get<SuperClusterMap>(endcapSuperClusterMapName_);
    AddBranchDep(mitName_, endcapSuperClusterMap_->GetBrName());
  }
  if (!pfEcalBarrelSuperClusterMapName_.empty()) {
    pfEcalBarrelSuperClusterMap_ = OS()->get<SuperClusterMap>(pfEcalBarrelSuperClusterMapName_);
    AddBranchDep(mitName_, pfEcalBarrelSuperClusterMap_->GetBrName());
  }
  if (!pfEcalEndcapSuperClusterMapName_.empty()) {
    pfEcalEndcapSuperClusterMap_ = OS()->get<SuperClusterMap>(pfEcalEndcapSuperClusterMapName_);
    AddBranchDep(mitName_, pfEcalEndcapSuperClusterMap_->GetBrName());
  }
  if (!pfCandidateMapName_.empty()) {
    pfCandidateMap_ = OS()->get<PFCandidateMap>(pfCandidateMapName_);
    AddBranchDep(mitName_, pfCandidateMap_->GetBrName());
  }
}

void
mithep::FillerPhotons::FillDataBlock(edm::Event const& event, 
                                     edm::EventSetup const& setup)
{
  // Fill photon array.
  
  photons_->Delete();
  photonMap_->Reset();
  if (photonPFMap_)
    photonPFMap_->Reset();

  // get photon collection
  edm::Handle<PhotonView> hPhotonProduct;
  GetProduct(edmToken_, hPhotonProduct, event);
  auto& inPhotons = *hPhotonProduct;

  if (fillFromPAT_ && inPhotons.size() != 0 &&
      !dynamic_cast<pat::Photon const*>(&inPhotons.at(0)))
    throw edm::Exception(edm::errors::Configuration, "FillerPhotons:FillDataBlock()\n")
      << "Error! fillFromPAT set on non-PAT input";
 
  edm::ESHandle<CaloGeometry> pGeometry;
  setup.get<CaloGeometryRecord>().get(pGeometry);
  auto& caloGeom = *pGeometry;

  // handles to the the pho-HErecHit matching 
  HBHERecHitCollection const* hbheRecHitCol = 0;
  if (!HBHERecHitsEdmToken_.isUninitialized()) {
    edm::Handle<HBHERecHitCollection> hcalRecHitHandle;
    GetProduct(HBHERecHitsEdmToken_, hcalRecHitHandle, event);
    hbheRecHitCol =  hcalRecHitHandle.product();
  }
   
  // handles to get the photon ID information
  edm::ValueMap<bool> const* phidLooseMap = 0;
  edm::ValueMap<bool> const* phidTightMap = 0;
  if (!fillFromPAT_) {
    edm::Handle<edm::ValueMap<bool> > hPhidLooseMap;
    if (GetProductSafe(phIDCutBasedLooseToken_, hPhidLooseMap, event))
      phidLooseMap = hPhidLooseMap.product();
    edm::Handle<edm::ValueMap<bool> > hPhidTightMap;
    if (GetProductSafe(phIDCutBasedTightToken_, hPhidTightMap, event))
      phidTightMap = hPhidTightMap.product();
  }

  edm::ValueMap<PFCandRefV> const* footprintMap = 0;
  if (photonPFMap_ && !fillFromPAT_) {
    edm::Handle<edm::ValueMap<PFCandRefV> > hFootprintMap;
    GetProduct(footprintToken_, hFootprintMap, event);
    footprintMap = hFootprintMap.product();
  }

  unsigned iPhoton = 0;
  for (auto&& inPhoton : inPhotons) {
    // get photon reference
    edm::Ref<PhotonView> phRef(hPhotonProduct, iPhoton);
    edm::Ptr<reco::Photon> phPtr(hPhotonProduct, iPhoton);
    ++iPhoton;

    auto&& scRef = inPhoton.superCluster();
    reco::SuperCluster const* sc = 0;
    if (scRef.isAvailable())
      sc = scRef.get();

    // book the new bambu photon
    mithep::Photon *outPhoton = photons_->AddNew();
    
    outPhoton->SetPtEtaPhi(inPhoton.pt(), inPhoton.eta(), inPhoton.phi());

    // set standard variables
    outPhoton->SetIsConverted(inPhoton.hasConversionTracks());
    outPhoton->SetR9(inPhoton.r9());
    outPhoton->SetHadOverEm(inPhoton.hadronicOverEm());
    outPhoton->SetHasPixelSeed(inPhoton.hasPixelSeed());
    
    // shower shape variables   
    outPhoton->SetHcalDepth1OverEcal(inPhoton.hadronicDepth1OverEm());
    outPhoton->SetHcalDepth2OverEcal(inPhoton.hadronicDepth2OverEm());        
    outPhoton->SetMaxEnergyXtal(inPhoton.maxEnergyXtal());   
    outPhoton->SetE15(inPhoton.e1x5());
    outPhoton->SetE25(inPhoton.e2x5());
    outPhoton->SetE33(inPhoton.e3x3());    
    outPhoton->SetE55(inPhoton.e5x5());
    outPhoton->SetHadOverEmTow(inPhoton.hadTowOverEm());

    // isolation variables for dR=0.3
    outPhoton->SetEcalRecHitIsoDr03(inPhoton.ecalRecHitSumEtConeDR03());
    outPhoton->SetHcalTowerSumEtDr03(inPhoton.hcalTowerSumEtConeDR03());
    outPhoton->SetHcalDepth1TowerSumEtDr03(inPhoton.hcalDepth1TowerSumEtConeDR03());
    outPhoton->SetHcalDepth2TowerSumEtDr03(inPhoton.hcalDepth2TowerSumEtConeDR03());
    outPhoton->SetSolidConeTrkIsoDr03(inPhoton.trkSumPtSolidConeDR03());
    outPhoton->SetHollowConeTrkIsoDr03(inPhoton.trkSumPtHollowConeDR03());
    outPhoton->SetSolidConeNTrkDr03(inPhoton.nTrkSolidConeDR03());
    outPhoton->SetHollowConeNTrkDr03(inPhoton.nTrkHollowConeDR03());
    if (sc)
      outPhoton->SetHCalIsoTowDr03(inPhoton.hcalTowerSumEtConeDR03() +
                                   (inPhoton.hadronicOverEm() - inPhoton.hadTowOverEm())
                                   * sc->energy() /cosh(sc->eta()));

    // isolation variables for dR=0.4
    outPhoton->SetEcalRecHitIsoDr04(inPhoton.ecalRecHitSumEtConeDR04());
    outPhoton->SetHcalTowerSumEtDr04(inPhoton.hcalTowerSumEtConeDR04());
    outPhoton->SetHcalDepth1TowerSumEtDr04(inPhoton.hcalDepth1TowerSumEtConeDR04());
    outPhoton->SetHcalDepth2TowerSumEtDr04(inPhoton.hcalDepth2TowerSumEtConeDR04());
    outPhoton->SetSolidConeTrkIsoDr04(inPhoton.trkSumPtSolidConeDR04());
    outPhoton->SetHollowConeTrkIsoDr04(inPhoton.trkSumPtHollowConeDR04());
    outPhoton->SetSolidConeNTrkDr04(inPhoton.nTrkSolidConeDR04());
    outPhoton->SetHollowConeNTrkDr04(inPhoton.nTrkHollowConeDR04());
    if (sc)
      outPhoton->SetHCalIsoTowDr04(inPhoton.hcalTowerSumEtConeDR04() +
                                   (inPhoton.hadronicOverEm() - inPhoton.hadTowOverEm())
                                   * sc->energy() / cosh(sc->eta()));

    // pflow isolation
    outPhoton->SetPFChargedHadronIso(inPhoton.chargedHadronIso());
    outPhoton->SetPFChargedHadronIso(inPhoton.neutralHadronIso());
    outPhoton->SetPFChargedHadronIso(inPhoton.photonIso());    
    
    // fiducial and quality flags
    outPhoton->SetIsEB(inPhoton.isEB());
    outPhoton->SetIsEE(inPhoton.isEE());
    outPhoton->SetIsEBGap(inPhoton.isEBGap());
    outPhoton->SetIsEEGap(inPhoton.isEEGap());
    outPhoton->SetIsEBEEGap(inPhoton.isEBEEGap());

    if (fillFromPAT_) {
      auto& patPhoton = static_cast<pat::Photon const&>(inPhoton);
      outPhoton->SetIsLoosePhoton(patPhoton.photonID(phIDCutBasedLooseName_));
      outPhoton->SetIsTightPhoton(patPhoton.photonID(phIDCutBasedTightName_));
    }
    else {
      if (phidLooseMap)
        outPhoton->SetIsLoosePhoton((*phidLooseMap)[phRef]);
      if (phidTightMap)
        outPhoton->SetIsTightPhoton((*phidTightMap)[phRef]);
    }

    // calo position
    outPhoton->SetCaloPosXYZ(inPhoton.caloPosition().x(),inPhoton.caloPosition().y(),inPhoton.caloPosition().z());

    // MIP tagger information
    outPhoton->SetMipChi2(inPhoton.mipChi2());
    outPhoton->SetMipTotEnergy(inPhoton.mipTotEnergy());                    
    outPhoton->SetMipSlope(inPhoton.mipSlope());                        
    outPhoton->SetMipIntercept(inPhoton.mipIntercept());                    
    outPhoton->SetMipNhitCone(inPhoton.mipNhitCone());                     
    outPhoton->SetMipIsHalo(inPhoton.mipIsHalo());                      

    if (hbheRecHitCol) {
      // make the pho-HErecHit matching
      ThreeVector matchRhPos(0,0,0);
      double deltaPhiMin = 0.2;
      double rhoMin = 110.;
      double rhoMax = 140.;
      double rhEnMin = 1.;
      double matchedRhEnergy = -1.;
      double matchedRhTime = -1000.;
      // first do the std 2012 matching (wide DR window) - Plus side
      HERecHitMatcher(inPhoton, +1,
                      deltaPhiMin, rhoMin, rhoMax, rhEnMin,
                      matchRhPos, matchedRhEnergy, matchedRhTime,
                      *hbheRecHitCol, caloGeom);
      outPhoton->SetMatchHePlusPos(matchRhPos.X(),matchRhPos.Y(),matchRhPos.Z());
      outPhoton->SetMatchHePlusEn(matchedRhEnergy);
      outPhoton->SetMatchHePlusTime(matchedRhTime);
      // then do the std 2012 matching (wide DR window) - Minus side
      matchRhPos.SetXYZ(0,0,0);
      matchedRhEnergy = -1.;
      matchedRhTime = -1000.;
      HERecHitMatcher(inPhoton, -1,
                      deltaPhiMin, rhoMin, rhoMax, rhEnMin,
                      matchRhPos, matchedRhEnergy, matchedRhTime,
                      *hbheRecHitCol, caloGeom);
      outPhoton->SetMatchHeMinusPos(matchRhPos.X(),matchRhPos.Y(),matchRhPos.Z());
      outPhoton->SetMatchHeMinusEn(matchedRhEnergy);
      outPhoton->SetMatchHeMinusTime(matchedRhTime);
      //thirdly do the std 2011 matching (narrow DR window) - Plus side
      matchRhPos.SetXYZ(0,0,0);
      deltaPhiMin = 0.2;
      rhoMin = 115.;
      rhoMax = 130.;
      rhEnMin = 1.;
      matchedRhEnergy = -1.;
      matchedRhTime = -1000.;
      HERecHitMatcher(inPhoton, +1,
                      deltaPhiMin, rhoMin, rhoMax, rhEnMin,
                      matchRhPos, matchedRhEnergy, matchedRhTime,
                      *hbheRecHitCol, caloGeom);
      outPhoton->SetMatchHePlusPosDR15(matchRhPos.X(),matchRhPos.Y(),matchRhPos.Z());
      outPhoton->SetMatchHePlusEnDR15(matchedRhEnergy);
      outPhoton->SetMatchHePlusTimeDR15(matchedRhTime);
      // finally do the std 2011 matching (narrow DR window) - Minus side
      matchRhPos.SetXYZ(0,0,0);
      matchedRhEnergy = -1.;
      matchedRhTime = -1000.;
      HERecHitMatcher(inPhoton, -1,
                      deltaPhiMin, rhoMin, rhoMax, rhEnMin,
                      matchRhPos, matchedRhEnergy, matchedRhTime,
                      *hbheRecHitCol, caloGeom);
      outPhoton->SetMatchHeMinusPosDR15(matchRhPos.X(),matchRhPos.Y(),matchRhPos.Z());
      outPhoton->SetMatchHeMinusEnDR15(matchedRhEnergy);
      outPhoton->SetMatchHeMinusTimeDR15(matchedRhTime);
    }

    // add photon to map
    photonMap_->Add(phPtr, outPhoton);

    if (photonPFMap_) {
      // associate the photon to its footprint candidates
      if (fillFromPAT_) {
        auto& patPhoton = static_cast<pat::Photon const&>(inPhoton);
        auto&& footprint = patPhoton.associatedPackedPFCandidates();
        for (auto& candRef : footprint)
          photonPFMap_->Add(reco::CandidatePtr(edm::refToPtr(candRef)), outPhoton);
      }
      else {
        auto& footprint = (*footprintMap)[phPtr];
        for (auto& candRef : footprint) {
          photonPFMap_->Add(reco::CandidatePtr(edm::refToPtr(candRef)), outPhoton);
        }
      }
    }
  }

  photons_->Trim();
}

void
mithep::FillerPhotons::ResolveLinks(edm::Event const& event, edm::EventSetup const&)
{
  edm::ValueMap<PFCandRefV> const* footprintMap = 0;

  if (!fillFromPAT_ && pfCandidateMap_) {
    edm::Handle<edm::ValueMap<PFCandRefV> > hFootprintMap;
    GetProduct(footprintToken_, hFootprintMap, event);
    footprintMap = hFootprintMap.product();
  }

  for (auto& mapElem : photonMap_->FwdMap()) {
    auto& phPtr = mapElem.first;
    auto& inPhoton = static_cast<reco::Photon const&>(*phPtr);
    auto* outPhoton = mapElem.second;

    // make links to conversions
    if (conversionMap_) {
      for (auto&& convRef : inPhoton.conversions()) {
        auto* mitConv = conversionMap_->GetMit(convRef, false);
        if (mitConv) // miniAOD photon might not have a valid conversion ref depending on quality
          outPhoton->AddConversionD(mitConv);
      }
    }

    // make links to conversions (single leg)
    if (oneLegConversionMap_) {
      for (auto&& convRef : inPhoton.conversionsOneLeg()) {
        auto* mitConv = oneLegConversionMap_->GetMit(convRef, false);
        if (mitConv)
          outPhoton->AddConversionS(mitConv);
      }
    }

    // make link to supercluster
    auto&& scRef = inPhoton.superCluster();
    if (barrelSuperClusterMap_ && endcapSuperClusterMap_ && scRef.isAvailable()) {
      auto* mitSC = barrelSuperClusterMap_->GetMit(scRef, false);
      if (!mitSC)
        mitSC = endcapSuperClusterMap_->GetMit(scRef, false);

      if (mitSC)
        outPhoton->SetSuperCluster(mitSC);
      else if(checkClusterActive_)
        throw edm::Exception(edm::errors::Configuration, "FillerPhotons:FillDataBlock()\n")
          << "Error! Refined SuperCluster reference in unmapped collection";
    }

    // make link to pf supercluster
    if (pfEcalBarrelSuperClusterMap_ && pfEcalEndcapSuperClusterMap_) {
      auto&& parentSCRef = inPhoton.parentSuperCluster();
      if (parentSCRef.isAvailable()) {
        auto* mitSC = pfEcalBarrelSuperClusterMap_->GetMit(parentSCRef, false);
        if (!mitSC)
          mitSC = pfEcalEndcapSuperClusterMap_->GetMit(parentSCRef, false);

        if (mitSC)
          outPhoton->SetECALOnlySuperCluster(mitSC);
        else if (checkClusterActive_)
          throw edm::Exception(edm::errors::Configuration, "FillerPhotons:FillDataBlock()\n")
            << "Error! PFEcal SuperCluster reference in unmapped collection";
      }
    }

    // make link to footprint PFCandidates
    if (pfCandidateMap_) {
      if (fillFromPAT_) {
        auto& patPhoton = static_cast<pat::Photon const&>(inPhoton);
        auto&& footprint = patPhoton.associatedPackedPFCandidates();
        for (auto& candRef : footprint) {
          auto* mitCand = pfCandidateMap_->GetMit(reco::CandidatePtr(edm::refToPtr(candRef)));
          outPhoton->AddFootprintCandidate(mitCand);
        }
      }
      else { // footprintMap must exist
        auto& footprint = (*footprintMap)[phPtr];
        for (auto& candRef : footprint) {
          auto* mitCand = pfCandidateMap_->GetMit(reco::CandidatePtr(edm::refToPtr(candRef)));
          outPhoton->AddFootprintCandidate(mitCand);
        }
      }
    }
  }
}

void
mithep::FillerPhotons::HERecHitMatcher(reco::Photon const& pho, int zSide,
                                       double deltaPhiMin, double rhoMin, double rhoMax,
                                       double rhEnMin, ThreeVector& matchRhPos,
                                       double& matchedRhEnergy, double& matchedRhTime,
                                       HBHERecHitCollection const& hbheRecHitCol,
                                       CaloGeometry const& caloGeom)
{
  // This function provides information on the HBHE recHits aligned to the photon ECAL cluster.  A
  // phi window around the photon is scanned, within the rho strip covering the ECAL EB, and the
  // location, energy, timing of the most energetic recHits. One in HE+ and one in HE- are kept as
  // photon attributes. The goal is to find beam halo candidates.

  // re-initialize the input variables to be safe
  matchRhPos.SetXYZ(0,0,0);
  matchedRhEnergy = -1.;
  matchedRhTime = -1000.;
  
  // check if the photon is in EB
  if (!pho.isEB()) 
    return;

  // loop on the HE rechits 
  for (auto&& hbheHit : hbheRecHitCol) {
    HcalDetId id(hbheHit.detid());
    
    // discard the rh if not in HBHE
    if (id.subdet() != HcalEndcap) 
      continue;

    // get global 3d point for rechit
    auto hbhe_cell = caloGeom.getGeometry(hbheHit.id());
    auto hbhe_position = hbhe_cell->getPosition();

    // discard the rh if not in right side
    if (hbhe_position.z() * zSide < 0) 
      continue;

    // discard the rh if not enough energetic
    if (hbheHit.energy() < rhEnMin)
      continue;

    // discard the rh if not in the rho window around ECAL EB
    double rho = sqrt(hbhe_position.x()*hbhe_position.x() + hbhe_position.y()*hbhe_position.y());

    if ((rho < rhoMin) || (rho > rhoMax))
      continue;
	  
    // discard the rh if not in the phi window around the photon
    double corrDeltaPhi = TMath::Abs(pho.phi()-hbhe_position.phi());
    if (corrDeltaPhi > TMath::Pi())
      corrDeltaPhi = TMath::TwoPi() - corrDeltaPhi;     
    if (corrDeltaPhi > deltaPhiMin) 
      continue;
    
    // update the selected rechit
    if (hbheHit.energy() > matchedRhEnergy) {
      matchedRhEnergy = hbheHit.energy();
      matchedRhTime = hbheHit.time();
      matchRhPos.SetXYZ(hbhe_position.x(),hbhe_position.y(),hbhe_position.z());
    } 
  }
}

DEFINE_MITHEP_TREEFILLER(FillerPhotons);
