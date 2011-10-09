// $Id: FillerPhotons.cc,v 1.24 2011/10/08 18:54:48 bendavid Exp $

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
#include "HiggsAnalysis/HiggsToGammaGamma/interface/PhotonFix.h"
#include "TSystem.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerPhotons::FillerPhotons(const edm::ParameterSet &cfg, const char *name, bool active) :
  BaseFiller(cfg,name,active),
  edmName_(Conf().getUntrackedParameter<string>("edmName","photons")),
  mitName_(Conf().getUntrackedParameter<string>("mitName",Names::gkPhotonBrn)),
  conversionMapName_(Conf().getUntrackedParameter<string>("conversionMapName","")),
  barrelSuperClusterMapName_(Conf().getUntrackedParameter<string>("barrelSuperClusterMapName","")),
  endcapSuperClusterMapName_(Conf().getUntrackedParameter<string>("endcapSuperClusterMapName","")),
  phIDCutBasedTightName_(Conf().getUntrackedParameter<string>("phIDCutBasedTightName",
                                                              "PhotonIDProd:PhotonCutBasedIDTight")),
  phIDCutBasedLooseName_(Conf().getUntrackedParameter<string>("phIDCutBasedLooseName",
                                                              "PhotonIDProd:PhotonCutBasedIDLoose")),
  enablePhotonFix_(Conf().getUntrackedParameter<bool>("enablePhotonFix")),                                                           
  photons_(new mithep::PhotonArr(16)), 
  conversionMap_(0),
  barrelSuperClusterMap_(0),
  endcapSuperClusterMap_(0)
{
  // Constructor.
  if (enablePhotonFix_) {
    PhotonFix::initialiseParameters(Conf());
  }

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
    conversionMap_ = OS()->get<ConversionMap>(conversionMapName_);
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
}

//--------------------------------------------------------------------------------------------------
void FillerPhotons::FillDataBlock(const edm::Event      &event, 
                                  const edm::EventSetup &setup)
{
  // Fill photon array.

  if (!ecorr_.IsInitialized()) ecorr_.Initialize(setup,std::string(gSystem->Getenv("CMSSW_BASE") + TString("/src/MitPhysics/data/gbrph.root")));
  if (enablePhotonFix_) PhotonFix::initialiseGeometry(setup);
  
  photons_->Delete();

  // get photon collection
  Handle<reco::PhotonCollection> hPhotonProduct;
  GetProduct(edmName_, hPhotonProduct, event);
  const reco::PhotonCollection inPhotons = *(hPhotonProduct.product());  
  
  // handles to get the photon ID information
  Handle<edm::ValueMap<bool> > phidLooseMap;
  GetProduct(phIDCutBasedLooseName_, phidLooseMap, event);
  Handle<edm::ValueMap<bool> > phidTightMap;
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

    //isolation variables for dR=0.3
    outPhoton->SetEcalRecHitIsoDr03(iP->ecalRecHitSumEtConeDR03());
    outPhoton->SetHcalTowerSumEtDr03(iP->hcalTowerSumEtConeDR03());
    outPhoton->SetHcalDepth1TowerSumEtDr03(iP->hcalDepth1TowerSumEtConeDR03());
    outPhoton->SetHcalDepth2TowerSumEtDr03(iP->hcalDepth2TowerSumEtConeDR03());
    outPhoton->SetSolidConeTrkIsoDr03(iP->trkSumPtSolidConeDR03());
    outPhoton->SetHollowConeTrkIsoDr03(iP->trkSumPtHollowConeDR03());
    outPhoton->SetSolidConeNTrkDr03(iP->nTrkSolidConeDR03());
    outPhoton->SetHollowConeNTrkDr03(iP->nTrkHollowConeDR03());
    
    //isolation variables for dR=0.4
    outPhoton->SetEcalRecHitIsoDr04(iP->ecalRecHitSumEtConeDR04());
    outPhoton->SetHcalTowerSumEtDr04(iP->hcalTowerSumEtConeDR04());
    outPhoton->SetHcalDepth1TowerSumEtDr04(iP->hcalDepth1TowerSumEtConeDR04());
    outPhoton->SetHcalDepth2TowerSumEtDr04(iP->hcalDepth2TowerSumEtConeDR04());
    outPhoton->SetSolidConeTrkIsoDr04(iP->trkSumPtSolidConeDR04());
    outPhoton->SetHollowConeTrkIsoDr04(iP->trkSumPtHollowConeDR04());
    outPhoton->SetSolidConeNTrkDr04(iP->nTrkSolidConeDR04());
    outPhoton->SetHollowConeNTrkDr04(iP->nTrkHollowConeDR04());

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
    outPhoton->SetIsLoosePhoton((*phidLooseMap)[phRef]);
    outPhoton->SetIsTightPhoton((*phidTightMap)[phRef]);   

    //calo position
    outPhoton->SetCaloPosXYZ(iP->caloPosition().x(),iP->caloPosition().y(),iP->caloPosition().z());
    
    // make links to conversions
    if (iP->hasConversionTracks() && conversionMap_) {
      reco::ConversionRefVector conversionRefs = iP->conversions();
      for (reco::ConversionRefVector::const_iterator conversionRef = 
             conversionRefs.begin(); conversionRef != conversionRefs.end(); ++conversionRef) {
        outPhoton->AddConversion(conversionMap_->GetMit(*conversionRef));
      }
    }

    // make link to supercluster
    if (barrelSuperClusterMap_ && endcapSuperClusterMap_ && iP->superCluster().isNonnull()) {
      if(barrelSuperClusterMap_->HasMit(iP->superCluster())) {
        outPhoton->SetSuperCluster(barrelSuperClusterMap_->GetMit(iP->superCluster()));        
      } else {
        outPhoton->SetSuperCluster(endcapSuperClusterMap_->GetMit(iP->superCluster()));
      }
    }
    
    //regression energy corrections
    std::pair<double,double> cor = ecorr_.CorrectedEnergyWithError(*iP);
    outPhoton->SetEnergyRegr(cor.first);
    outPhoton->SetEnergyErrRegr(cor.second);
    
    //photonfix energy corrections
    if (enablePhotonFix_) {
      PhotonFix pfix(*iP);
      outPhoton->SetEnergyPhoFix(pfix.fixedEnergy());
      outPhoton->SetEnergyErrPhoFix(pfix.sigmaEnergy());
    }
    
    
  }
  photons_->Trim();
}
