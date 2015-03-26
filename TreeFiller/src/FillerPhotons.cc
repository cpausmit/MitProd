#include "MitProd/TreeFiller/interface/FillerPhotons.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/EgammaCandidates/interface/Photon.h"
#include "DataFormats/EgammaCandidates/interface/Conversion.h"
#include "DataFormats/EgammaCandidates/interface/ConversionFwd.h"
//#include "RecoEgamma/EgammaTools/interface/ggPFPhotons.h"
//#include "RecoEgamma/EgammaTools/interface/ggPFClusters.h"
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/CaloTopology/interface/CaloSubdetectorTopology.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHit.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "DataFormats/EcalDetId/interface/EcalSubdetector.h"

#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataTree/interface/PhotonCol.h"
#include "MitProd/ObjectService/interface/ObjectService.h"
#include "TSystem.h"

using namespace std;
using namespace edm;
using namespace mithep;

//---------------------------------------------------------------------------------------------------
FillerPhotons::FillerPhotons(const edm::ParameterSet &cfg, edm::ConsumesCollector& collector, ObjectService* os, const char *name, bool active) :
  BaseFiller                (cfg,os,name,active),
  edmToken_(GetToken<reco::PhotonCollection>(collector, "edmName","photons")),

  HBHERecHitsEdmToken_(GetToken<HBHERecHitCollection>(collector, "HBHERecHitsEdmName", "reducedHcalRecHits:hbhereco")),
  phIDCutBasedTightToken_(GetToken<edm::ValueMap<bool> >(collector, "phIDCutBasedTightName", "PhotonIDProd:PhotonCutBasedIDTight")),
  phIDCutBasedLooseToken_(GetToken<edm::ValueMap<bool> >(collector, "phIDCutBasedLooseName", "PhotonIDProd:PhotonCutBasedIDLoose")),
  // EBRecHitsEdmName_         (Conf().getUntrackedParameter<string>("EBRecHitsEdmName_", "reducedEcalRecHitsEB")),
  // EERecHitsEdmName_         (Conf().getUntrackedParameter<string>("EERecHitsEdmName_", "reducedEcalRecHitsEE")),  
  // PFCandsEdmName_           (Conf().getUntrackedParameter<string>("PFCandsEdmName","particleFlow")),  
  mitName_                  (Conf().getUntrackedParameter<string>("mitName",Names::gkPhotonBrn)),
  conversionMapName_        (Conf().getUntrackedParameter<string>("conversionMapName","")),
  oneLegConversionMapName_  (Conf().getUntrackedParameter<string>("oneLegConversionMapName","")),
  barrelSuperClusterMapName_(Conf().getUntrackedParameter<string>("barrelSuperClusterMapName","")),
  endcapSuperClusterMapName_(Conf().getUntrackedParameter<string>("endcapSuperClusterMapName","")),
  pfSuperClusterMapName_    (Conf().getUntrackedParameter<string>("pfSuperClusterMapName","")),
  pfClusterMapName_         (Conf().getUntrackedParameter<string>("pfClusterMapName","")),  
  pfCandMapName_            (Conf().getUntrackedParameter<string>("pfCandMapName","")),
  photonMapName_            (Conf().getUntrackedParameter<string>("photonMapName","")),  
  photonMap_                (new mithep::PhotonMap),
  photons_                  (new mithep::PhotonArr(16)),
  conversionMap_            (0),
  oneLegConversionMap_      (0),
  barrelSuperClusterMap_    (0),
  endcapSuperClusterMap_    (0),
  pfSuperClusterMap_        (0),
  pfClusterMap_             (0),
  pfCandMap_                (0)
{
  // Constructor.
}

//--------------------------------------------------------------------------------------------------
FillerPhotons::~FillerPhotons()
{
  // Destructor.

  delete photons_;
}

//--------------------------------------------------------------------------------------------------
void FillerPhotons::BookDataBlock(TreeWriter &tws)
{
  // Add photon branch to tree and get the map.

  tws.AddBranch(mitName_,&photons_);
  OS()->add<mithep::PhotonArr>(photons_,mitName_);

  if (!conversionMapName_.empty()) {
    conversionMap_ = OS()->get<ConversionDecayMap>(conversionMapName_);
    if (conversionMap_)
      AddBranchDep(mitName_,conversionMap_->GetBrName());
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
  if (!pfSuperClusterMapName_.empty()) {
    pfSuperClusterMap_ = OS()->get<SuperClusterMap>(pfSuperClusterMapName_);
    if (pfSuperClusterMap_)
      AddBranchDep(mitName_,pfSuperClusterMap_->GetBrName());
  }
  if (!pfClusterMapName_.empty()) {
    pfClusterMap_ = OS()->get<BasicClusterMap>(pfClusterMapName_);
    if (pfClusterMap_)
      AddBranchDep(mitName_,pfClusterMap_->GetBrName());
  }  
  if (!pfCandMapName_.empty()) {
    pfCandMap_ = OS()->get<PFCandViewMap>(pfCandMapName_);
    if (pfCandMap_)
      AddBranchDep(mitName_,pfCandMap_->GetBrName());
  }  
  if (!photonMapName_.empty()) {
    photonMap_->SetBrName(mitName_);
    OS()->add<PhotonMap>(photonMap_,photonMapName_);
  }  
}

//--------------------------------------------------------------------------------------------------
void FillerPhotons::FillDataBlock(const edm::Event      &event, 
                                  const edm::EventSetup &setup)
{
  // Fill photon array.
  
  photons_->Delete();
  photonMap_->Reset();

  // get photon collection
  Handle<reco::PhotonCollection> hPhotonProduct;
  GetProduct(edmToken_, hPhotonProduct, event);
  const reco::PhotonCollection inPhotons = *(hPhotonProduct.product());  
  
  // pf photon stuff 
  //YI edm::Handle<EcalRecHitCollection> pEBRecHits;
  //YI event.getByLabel(EBRecHitsEdmName_, pEBRecHits);
  //YI edm::Handle<EcalRecHitCollection> pEERecHits;
  //YI event.getByLabel(EERecHitsEdmName_, pEERecHits);  

  edm::ESHandle<CaloGeometry> pGeometry;
  setup.get<CaloGeometryRecord>().get(pGeometry);

  //CP const CaloSubdetectorGeometry *geometryEB =
  //CP   pGeometry->getSubdetectorGeometry(DetId::Ecal, EcalBarrel);
  //CP const CaloSubdetectorGeometry *geometryEE =
  //CP   pGeometry->getSubdetectorGeometry(DetId::Ecal, EcalEndcap);
  //CP ggPFClusters pfclusters(pEBRecHits, pEERecHits, geometryEB, geometryEE);
  

  //CP Handle<reco::PFCandidateCollection> pPFCands;
  //CP event.getByLabel(PFCandsEdmName_, pPFCands);

  //CP Handle<PFCollection> hPFCands;
  //CP event.getByLabel(PFCandsEdmName_, pPFCands);

  //GetProduct(edmName_, hPfCandProduct, event);  
  //const PFCollection &inPfCands = *(hPfCandProduct.product());

  // handles to the the pho-HErecHit matching 
  edm::Handle<HBHERecHitCollection> hcalRecHitHandle;
  GetProduct(HBHERecHitsEdmToken_, hcalRecHitHandle, event);
  const HBHERecHitCollection* hbheRecHitCol =  hcalRecHitHandle.product(); 
  const CaloGeometry* caloGeom = pGeometry.product(); 
   
  // handles to get the photon ID information
  Handle<edm::ValueMap<bool> > phidLooseMap;
  if (!phIDCutBasedLooseToken_.isUninitialized())
    GetProduct(phIDCutBasedLooseToken_, phidLooseMap, event);

  Handle<edm::ValueMap<bool> > phidTightMap;
  if (!phIDCutBasedTightToken_.isUninitialized())  
    GetProduct(phIDCutBasedTightToken_, phidTightMap, event);
  
  for (reco::PhotonCollection::const_iterator iP = inPhotons.begin(); 
       iP != inPhotons.end(); ++iP) {
    
    // get photon reference
    int photonIndex = iP-inPhotons.begin();
    reco::PhotonRef phRef(hPhotonProduct,photonIndex);

    // book the new bambu photon
    mithep::Photon *outPhoton = photons_->Allocate();
    new (outPhoton) mithep::Photon(iP->px(),iP->py(),iP->pz(),iP->energy());

    // set standard variables
    outPhoton->SetIsConverted(iP->hasConversionTracks());
    outPhoton->SetR9(iP->r9());
    outPhoton->SetHadOverEm(iP->hadronicOverEm());
    outPhoton->SetHasPixelSeed(iP->hasPixelSeed());
    
    // shower shape variables   
    outPhoton->SetHcalDepth1OverEcal(iP->hadronicDepth1OverEm());
    outPhoton->SetHcalDepth2OverEcal(iP->hadronicDepth2OverEm());        
    outPhoton->SetMaxEnergyXtal(iP->maxEnergyXtal());   
    outPhoton->SetE15(iP->e1x5());
    outPhoton->SetE25(iP->e2x5());
    outPhoton->SetE33(iP->e3x3());    
    outPhoton->SetE55(iP->e5x5());
    outPhoton->SetCovEtaEta(iP->sigmaEtaEta());
    outPhoton->SetCoviEtaiEta(iP->sigmaIetaIeta());
    outPhoton->SetHadOverEmTow(iP->hadTowOverEm());

    // isolation variables for dR=0.3
    outPhoton->SetEcalRecHitIsoDr03(iP->ecalRecHitSumEtConeDR03());
    outPhoton->SetHcalTowerSumEtDr03(iP->hcalTowerSumEtConeDR03());
    outPhoton->SetHcalDepth1TowerSumEtDr03(iP->hcalDepth1TowerSumEtConeDR03());
    outPhoton->SetHcalDepth2TowerSumEtDr03(iP->hcalDepth2TowerSumEtConeDR03());
    outPhoton->SetSolidConeTrkIsoDr03(iP->trkSumPtSolidConeDR03());
    outPhoton->SetHollowConeTrkIsoDr03(iP->trkSumPtHollowConeDR03());
    outPhoton->SetSolidConeNTrkDr03(iP->nTrkSolidConeDR03());
    outPhoton->SetHollowConeNTrkDr03(iP->nTrkHollowConeDR03());
    outPhoton->SetHCalIsoTowDr03(iP->hcalTowerSumEtConeDR03() +
				 (iP->hadronicOverEm()-iP->hadTowOverEm())
				 *iP->superCluster()->energy()/cosh(iP->superCluster()->eta()));    

    // isolation variables for dR=0.4
    outPhoton->SetEcalRecHitIsoDr04(iP->ecalRecHitSumEtConeDR04());
    outPhoton->SetHcalTowerSumEtDr04(iP->hcalTowerSumEtConeDR04());
    outPhoton->SetHcalDepth1TowerSumEtDr04(iP->hcalDepth1TowerSumEtConeDR04());
    outPhoton->SetHcalDepth2TowerSumEtDr04(iP->hcalDepth2TowerSumEtConeDR04());
    outPhoton->SetSolidConeTrkIsoDr04(iP->trkSumPtSolidConeDR04());
    outPhoton->SetHollowConeTrkIsoDr04(iP->trkSumPtHollowConeDR04());
    outPhoton->SetSolidConeNTrkDr04(iP->nTrkSolidConeDR04());
    outPhoton->SetHollowConeNTrkDr04(iP->nTrkHollowConeDR04());
    outPhoton->SetHCalIsoTowDr04(iP->hcalTowerSumEtConeDR04() +
				 (iP->hadronicOverEm()-iP->hadTowOverEm())
				 *iP->superCluster()->energy()/cosh(iP->superCluster()->eta()));    

    // pflow isolation
    outPhoton->SetPFChargedHadronIso(iP->chargedHadronIso());
    outPhoton->SetPFChargedHadronIso(iP->neutralHadronIso());
    outPhoton->SetPFChargedHadronIso(iP->photonIso());    
    
    // fiducial and quality flags
    outPhoton->SetIsEB(iP->isEB());
    outPhoton->SetIsEE(iP->isEE());
    outPhoton->SetIsEBGap(iP->isEBGap());
    outPhoton->SetIsEEGap(iP->isEEGap());
    outPhoton->SetIsEBEEGap(iP->isEBEEGap());
    //deprecated, identical to supercluster preselection in 3_1_X, so set to true
    outPhoton->SetIsLooseEM(true); //deprecated
    if (!phIDCutBasedLooseToken_.isUninitialized())
      outPhoton->SetIsLoosePhoton((*phidLooseMap)[phRef]);
    if (!phIDCutBasedTightToken_.isUninitialized())
      outPhoton->SetIsTightPhoton((*phidTightMap)[phRef]);   

    // calo position
    outPhoton->SetCaloPosXYZ(iP->caloPosition().x(),iP->caloPosition().y(),iP->caloPosition().z());

    // MIP tagger information
    outPhoton->SetMipChi2(iP->mipChi2());
    outPhoton->SetMipTotEnergy(iP->mipTotEnergy());                    
    outPhoton->SetMipSlope(iP->mipSlope());                        
    outPhoton->SetMipIntercept(iP->mipIntercept());                    
    outPhoton->SetMipNhitCone(iP->mipNhitCone());                     
    outPhoton->SetMipIsHalo(iP->mipIsHalo());                      

    // make the pho-HErecHit matching 
    ThreeVector matchRhPos(0,0,0);
    double deltaPhiMin = 0.2;
    double rhoMin = 110.;
    double rhoMax = 140.;
    double rhEnMin = 1.;
    double matchedRhEnergy = -1.;
    double matchedRhTime = -1000.;
    // first do the std 2012 matching (wide DR window) - Plus side
    HERecHitMatcher(&(*iP), +1,
                    deltaPhiMin, rhoMin, rhoMax, rhEnMin,
                    matchRhPos, matchedRhEnergy, matchedRhTime,
                    hbheRecHitCol, caloGeom);
    outPhoton->SetMatchHePlusPos(matchRhPos.X(),matchRhPos.Y(),matchRhPos.Z());                      
    outPhoton->SetMatchHePlusEn(matchedRhEnergy);                      
    outPhoton->SetMatchHePlusTime(matchedRhTime);                      
    // then do the std 2012 matching (wide DR window) - Minus side
    matchRhPos.SetXYZ(0,0,0);
    matchedRhEnergy = -1.;
    matchedRhTime = -1000.;
    HERecHitMatcher(&(*iP), -1,
                    deltaPhiMin, rhoMin, rhoMax, rhEnMin,
                    matchRhPos, matchedRhEnergy, matchedRhTime,
                    hbheRecHitCol, caloGeom);
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
    HERecHitMatcher(&(*iP), +1,
                    deltaPhiMin, rhoMin, rhoMax, rhEnMin,
                    matchRhPos, matchedRhEnergy, matchedRhTime,
                    hbheRecHitCol, caloGeom);
    outPhoton->SetMatchHePlusPosDR15(matchRhPos.X(),matchRhPos.Y(),matchRhPos.Z());
    outPhoton->SetMatchHePlusEnDR15(matchedRhEnergy);                      
    outPhoton->SetMatchHePlusTimeDR15(matchedRhTime);                      
    // finally do the std 2011 matching (narrow DR window) - Minus side
    matchRhPos.SetXYZ(0,0,0);
    matchedRhEnergy = -1.;
    matchedRhTime = -1000.;
    HERecHitMatcher(&(*iP), -1,
                    deltaPhiMin, rhoMin, rhoMax, rhEnMin,
                    matchRhPos, matchedRhEnergy, matchedRhTime,
                    hbheRecHitCol, caloGeom);
    outPhoton->SetMatchHeMinusPosDR15(matchRhPos.X(),matchRhPos.Y(),matchRhPos.Z());
    outPhoton->SetMatchHeMinusEnDR15(matchedRhEnergy);                      
    outPhoton->SetMatchHeMinusTimeDR15(matchedRhTime);                      

    // make links to conversions
    if (conversionMap_) {
      const reco::ConversionRefVector &conversionRefs = iP->conversions();
      for (reco::ConversionRefVector::const_iterator conversionRef = 
             conversionRefs.begin(); conversionRef != conversionRefs.end(); ++conversionRef) {
        outPhoton->AddConversionD(conversionMap_->GetMit(*conversionRef));
      }
    }

    // make links to conversions (single leg)
    if (oneLegConversionMap_) {
      const reco::ConversionRefVector &conversionRefs = iP->conversionsOneLeg();
      for (reco::ConversionRefVector::const_iterator conversionRef = 
             conversionRefs.begin(); conversionRef != conversionRefs.end(); ++conversionRef) {
        outPhoton->AddConversionS(oneLegConversionMap_->GetMit(*conversionRef));
      }
    }

    // make link to supercluster
    if (barrelSuperClusterMap_ && endcapSuperClusterMap_ && iP->superCluster().isNonnull()) {
      if (barrelSuperClusterMap_->HasMit(iP->superCluster()))
        outPhoton->SetSuperCluster(barrelSuperClusterMap_->GetMit(iP->superCluster()));        
      else if(endcapSuperClusterMap_->HasMit(iP->superCluster())) 
        outPhoton->SetSuperCluster(endcapSuperClusterMap_->GetMit(iP->superCluster()));
    }

    //CP // make link to pf supercluster (DOES NOT EXIST ANYMORE)
    //CP if (pfSuperClusterMap_ && iP->pfSuperCluster().isNonnull()) {
    //CP   if (pfSuperClusterMap_->HasMit(iP->pfSuperCluster())) 
    //CP 	outPhoton->SetPFSuperCluster(pfSuperClusterMap_->GetMit(iP->pfSuperCluster()));
    //CP   
    //CP   //CP // horrible stuff: mark PF superclusters with fraction of energy that overlaps with egamma
    //CP   //CP // supercluster
    //CP   //CP if (pfClusterMap_ && iP->superCluster().isNonnull()) {
    //CP   //CP 	for (reco::CaloCluster_iterator pfcit = iP->pfSuperCluster()->clustersBegin();
    //CP   //CP 	     pfcit!=iP->pfSuperCluster()->clustersEnd(); ++pfcit) {
    //CP   //CP 	  float eoverlap = pfclusters.getPFSuperclusterOverlap(**pfcit,*iP->superCluster());
    //CP   //CP 	  if(pfClusterMap_->GetMit(*pfcit)) const_cast<mithep::BasicCluster*>(pfClusterMap_->GetMit(*pfcit))->SetMatchedEnergy(eoverlap);
    //CP   //CP 	}
    //CP   //CP }	
    //CP }
    
    //CP // horrible stuff: make links to PFCandidates to try and recover pflow clustering when pflow id
    //CP // failed...
    //CP if (pfCandMap_ && iP->superCluster().isNonnull()) {
    //CP   std::vector<PFCandidatePtr> inmust;
    //CP   std::vector<PFCandidatePtr> outmust;
    //CP   std::pair<double,double> scsize = ggPFPhotons::SuperClusterSize(*iP->superCluster(),
    //CP 								      pEBRecHits,
    //CP 								      pEERecHits,
    //CP 								      geometryEB,
    //CP 								      geometryEE);
    //CP   ggPFPhotons::recoPhotonClusterLink(*iP->superCluster(),
    //CP 					 inmust, 
    //CP 				       	 outmust,
    //CP 					 pPFCands,
    //CP 					 scsize.first,
    //CP 					 scsize.second);
    //CP   
    //CP   for (std::vector<PFCandidatePtr>::const_iterator pfit = inmust.begin(); pfit!=inmust.end(); ++pfit)
    //CP 	outPhoton->AddPFPhotonInMustache(pfCandMap_->GetMit(*pfit));
    //CP   for (std::vector<PFCandidatePtr>::const_iterator pfit = outmust.begin(); pfit!=outmust.end(); ++pfit)
    //CP 	outPhoton->AddPFPhotonOutOfMustache(pfCandMap_->GetMit(*pfit));
    //CP }

    // add electron to map
    edm::Ptr<reco::Photon> thePtr(hPhotonProduct,photonIndex);
    photonMap_->Add(thePtr,outPhoton);
  }
  photons_->Trim();
}

//--------------------------------------------------------------------------------------------------
void FillerPhotons::HERecHitMatcher(const reco::Photon* pho, int zSide,
                                    double deltaPhiMin, double rhoMin, double rhoMax,
				    double rhEnMin, ThreeVector& matchRhPos,
				    double& matchedRhEnergy, double& matchedRhTime,
                                    const HBHERecHitCollection* hbheRecHitCol,
				    const CaloGeometry* caloGeom)
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
  if (!pho->isEB()) 
    return;

  // loop on the HE rechits 
  for (HBHERecHitCollection::const_iterator hh = hbheRecHitCol->begin();
       hh != hbheRecHitCol->end(); hh++) {
    HcalDetId id(hh->detid());
    
    // discard the rh if not in HBHE
    if (id.subdet()!=HcalEndcap) 
      continue;

    // get global 3d point for rechit
    const CaloCellGeometry *hbhe_cell = caloGeom->getGeometry(hh->id());
    Global3DPoint hbhe_position = hbhe_cell->getPosition();

    // discard the rh if not in right side
    if (hbhe_position.z() * zSide < 0) 
      continue;

    // discard the rh if not enough energetic
    if (hh->energy() < rhEnMin)
      continue;

    // discard the rh if not in the rho window around ECAL EB
    double rho = sqrt(hbhe_position.x()*hbhe_position.x() + hbhe_position.y()*hbhe_position.y());

    if ((rho < rhoMin) || (rho > rhoMax))
      continue;
	  
    // discard the rh if not in the phi window around the photon
    double corrDeltaPhi = TMath::Abs(pho->phi()-hbhe_position.phi());
    if (corrDeltaPhi > TMath::Pi())
      corrDeltaPhi = TMath::TwoPi() - corrDeltaPhi;     
    if (corrDeltaPhi > deltaPhiMin) 
      continue;
    
    // update the selected rechit
    if (hh->energy() > matchedRhEnergy) {
      matchedRhEnergy = hh->energy();
      matchedRhTime = hh->time();
      matchRhPos.SetXYZ(hbhe_position.x(),hbhe_position.y(),hbhe_position.z());
    } 
  }
  
  return;
}
