// $Id: FillerPhotons.cc,v 1.31 2012/07/25 03:08:42 paus Exp $

#include "MitProd/TreeFiller/interface/FillerPhotons.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/EgammaCandidates/interface/Photon.h"
#include "DataFormats/EgammaCandidates/interface/PhotonFwd.h"
#include "DataFormats/EgammaCandidates/interface/Conversion.h"
#include "DataFormats/EgammaCandidates/interface/ConversionFwd.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataTree/interface/PhotonCol.h"
#include "MitProd/ObjectService/interface/ObjectService.h"
#include "RecoEgamma/EgammaTools/interface/ggPFPhotons.h"
#include "RecoEgamma/EgammaTools/interface/ggPFClusters.h"
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/CaloTopology/interface/CaloSubdetectorTopology.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHit.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "DataFormats/EcalDetId/interface/EcalSubdetector.h"
#include "TSystem.h"

using namespace std;
using namespace edm;
using namespace mithep;

//---------------------------------------------------------------------------------------------------
FillerPhotons::FillerPhotons(const edm::ParameterSet &cfg, const char *name, bool active) :
  BaseFiller                (cfg,name,active),
  edmName_                  (Conf().getUntrackedParameter<string>("edmName","photons")),
  mitName_                  (Conf().getUntrackedParameter<string>("mitName",Names::gkPhotonBrn)),
  conversionMapName_        (Conf().getUntrackedParameter<string>("conversionMapName","")),
  oneLegConversionMapName_  (Conf().getUntrackedParameter<string>("oneLegConversionMapName","")),
  barrelSuperClusterMapName_(Conf().getUntrackedParameter<string>("barrelSuperClusterMapName","")),
  endcapSuperClusterMapName_(Conf().getUntrackedParameter<string>("endcapSuperClusterMapName","")),
  pfSuperClusterMapName_    (Conf().getUntrackedParameter<string>("pfSuperClusterMapName","")),
  pfClusterMapName_         (Conf().getUntrackedParameter<string>("pfClusterMapName","")),  
  pfCandMapName_            (Conf().getUntrackedParameter<string>("pfCandMapName","")),
  photonMapName_            (Conf().getUntrackedParameter<string>("photonMapName","")),  
  phIDCutBasedTightName_    (Conf().getUntrackedParameter<string>("phIDCutBasedTightName",
								  "PhotonIDProd:PhotonCutBasedIDTight")),
  phIDCutBasedLooseName_    (Conf().getUntrackedParameter<string>("phIDCutBasedLooseName",
								  "PhotonIDProd:PhotonCutBasedIDLoose")),
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
    pfCandMap_ = OS()->get<PFCandidateMap>(pfCandMapName_);
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

  //if (!ecorr_.IsInitialized())
  //  ecorr_.Initialize(setup,std::string(TString("gbrph.root")));
  
  photons_->Delete();
  photonMap_->Reset();

  // get photon collection
  Handle<reco::PhotonCollection> hPhotonProduct;
  GetProduct(edmName_, hPhotonProduct, event);
  const reco::PhotonCollection inPhotons = *(hPhotonProduct.product());  
  
  //pf photon stuff 
  edm::Handle<EcalRecHitCollection> pEBRecHits;
  event.getByLabel("reducedEcalRecHitsEB", pEBRecHits );
  edm::Handle<EcalRecHitCollection> pEERecHits;
  event.getByLabel( "reducedEcalRecHitsEE", pEERecHits );  
  
  edm::ESHandle<CaloGeometry> pGeometry;
  setup.get<CaloGeometryRecord>().get(pGeometry);
 
  const CaloSubdetectorGeometry *geometryEB = pGeometry->getSubdetectorGeometry(DetId::Ecal, EcalBarrel);
  const CaloSubdetectorGeometry *geometryEE = pGeometry->getSubdetectorGeometry(DetId::Ecal, EcalEndcap);
  
  ggPFClusters pfclusters(pEBRecHits, pEERecHits, geometryEB, geometryEE);
  
  Handle<reco::PFCandidateCollection> pPFCands;
  event.getByLabel("particleFlow", pPFCands);
  
  // handles to get the photon ID information
  Handle<edm::ValueMap<bool> > phidLooseMap;
  if (!phIDCutBasedLooseName_.empty())
    GetProduct(phIDCutBasedLooseName_, phidLooseMap, event);

  Handle<edm::ValueMap<bool> > phidTightMap;
  if (!phIDCutBasedTightName_.empty())  
    GetProduct(phIDCutBasedTightName_, phidTightMap, event);
  
  for (reco::PhotonCollection::const_iterator iP = inPhotons.begin(); 
       iP != inPhotons.end(); ++iP) {
    
    int photonIndex = iP - inPhotons.begin();
    reco::PhotonRef phRef(hPhotonProduct, photonIndex);

    mithep::Photon *outPhoton = photons_->Allocate();
    new (outPhoton) mithep::Photon(iP->px(),iP->py(),iP->pz(),iP->energy());
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

    //isolation variables for dR=0.3
    outPhoton->SetEcalRecHitIsoDr03(iP->ecalRecHitSumEtConeDR03());
    outPhoton->SetHcalTowerSumEtDr03(iP->hcalTowerSumEtConeDR03());
    outPhoton->SetHcalDepth1TowerSumEtDr03(iP->hcalDepth1TowerSumEtConeDR03());
    outPhoton->SetHcalDepth2TowerSumEtDr03(iP->hcalDepth2TowerSumEtConeDR03());
    outPhoton->SetSolidConeTrkIsoDr03(iP->trkSumPtSolidConeDR03());
    outPhoton->SetHollowConeTrkIsoDr03(iP->trkSumPtHollowConeDR03());
    outPhoton->SetSolidConeNTrkDr03(iP->nTrkSolidConeDR03());
    outPhoton->SetHollowConeNTrkDr03(iP->nTrkHollowConeDR03());
    outPhoton->SetHCalIsoTowDr03(iP->hcalTowerSumEtConeDR03() + (iP->hadronicOverEm() - iP->hadTowOverEm())*iP->superCluster()->energy()/cosh(iP->superCluster()->eta()));    

    //isolation variables for dR=0.4
    outPhoton->SetEcalRecHitIsoDr04(iP->ecalRecHitSumEtConeDR04());
    outPhoton->SetHcalTowerSumEtDr04(iP->hcalTowerSumEtConeDR04());
    outPhoton->SetHcalDepth1TowerSumEtDr04(iP->hcalDepth1TowerSumEtConeDR04());
    outPhoton->SetHcalDepth2TowerSumEtDr04(iP->hcalDepth2TowerSumEtConeDR04());
    outPhoton->SetSolidConeTrkIsoDr04(iP->trkSumPtSolidConeDR04());
    outPhoton->SetHollowConeTrkIsoDr04(iP->trkSumPtHollowConeDR04());
    outPhoton->SetSolidConeNTrkDr04(iP->nTrkSolidConeDR04());
    outPhoton->SetHollowConeNTrkDr04(iP->nTrkHollowConeDR04());
    outPhoton->SetHCalIsoTowDr04(iP->hcalTowerSumEtConeDR04() + (iP->hadronicOverEm() - iP->hadTowOverEm())*iP->superCluster()->energy()/cosh(iP->superCluster()->eta()));    

    //pflow isolation
    outPhoton->SetPFChargedHadronIso(iP->chargedHadronIso());
    outPhoton->SetPFChargedHadronIso(iP->neutralHadronIso());
    outPhoton->SetPFChargedHadronIso(iP->photonIso());    
    
    //fiducial and quality flags
    outPhoton->SetIsEB(iP->isEB());
    outPhoton->SetIsEE(iP->isEE());
    outPhoton->SetIsEBGap(iP->isEBGap());
    outPhoton->SetIsEEGap(iP->isEEGap());
    outPhoton->SetIsEBEEGap(iP->isEBEEGap());
    //deprecated, identical to supercluster preselection in 3_1_X, so set to true
    outPhoton->SetIsLooseEM(true); //deprecated
    if (!phIDCutBasedLooseName_.empty()) outPhoton->SetIsLoosePhoton((*phidLooseMap)[phRef]);
    if (!phIDCutBasedTightName_.empty()) outPhoton->SetIsTightPhoton((*phidTightMap)[phRef]);   

    //calo position
    outPhoton->SetCaloPosXYZ(iP->caloPosition().x(),iP->caloPosition().y(),iP->caloPosition().z());

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

    // make link to pf supercluster
    if (pfSuperClusterMap_ && iP->pfSuperCluster().isNonnull()) {
      if(pfSuperClusterMap_->HasMit(iP->pfSuperCluster())) 
	 outPhoton->SetPFSuperCluster(pfSuperClusterMap_->GetMit(iP->pfSuperCluster()));
      //horrible stuff: mark PF superclusters with fraction of energy that overlaps with egamma supercluster
      if (pfClusterMap_ && iP->superCluster().isNonnull()) {
	for (reco::CaloCluster_iterator pfcit = iP->pfSuperCluster()->clustersBegin();
	     pfcit!=iP->pfSuperCluster()->clustersEnd(); ++pfcit) {
	  float eoverlap = pfclusters.getPFSuperclusterOverlap(**pfcit,*iP->superCluster() );
	  if(pfClusterMap_->GetMit(*pfcit)) const_cast<mithep::BasicCluster*>(pfClusterMap_->GetMit(*pfcit))->SetMatchedEnergy(eoverlap);
	}
      }	
    }
    
    //horrible stuff: make links to PFCandidates to try and recover pflow clustering when pflow id failed...
    if (pfCandMap_ && iP->superCluster().isNonnull()) {
      std::vector<PFCandidatePtr> inmust;
      std::vector<PFCandidatePtr> outmust;
      std::pair<double,double> scsize = ggPFPhotons::SuperClusterSize(*iP->superCluster(),
								      pEBRecHits,
								      pEERecHits,
								      geometryEB,
								      geometryEE);
      ggPFPhotons::recoPhotonClusterLink(*iP->superCluster(),
					 inmust, 
				       	 outmust,
					 pPFCands,
					 scsize.first,
					 scsize.second);
      
      for (std::vector<PFCandidatePtr>::const_iterator pfit = inmust.begin(); pfit!=inmust.end(); ++pfit)
	outPhoton->AddPFPhotonInMustache(pfCandMap_->GetMit(*pfit));
      for (std::vector<PFCandidatePtr>::const_iterator pfit = outmust.begin(); pfit!=outmust.end(); ++pfit)
	outPhoton->AddPFPhotonOutOfMustache(pfCandMap_->GetMit(*pfit));
    }
    
    ////regression energy corrections
    //std::pair<double,double> cor = ecorr_.CorrectedEnergyWithError(*iP);
    //outPhoton->SetEnergyRegr(cor.first);
    //outPhoton->SetEnergyErrRegr(cor.second);

    // add electron to map
    edm::Ptr<reco::Photon> thePtr(hPhotonProduct, iP - inPhotons.begin());
    photonMap_->Add(thePtr, outPhoton);

  }
  photons_->Trim();
}
