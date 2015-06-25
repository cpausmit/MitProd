#include "MitProd/TreeFiller/interface/FillerSuperClusters.h"
#include "DataFormats/EgammaReco/interface/SuperCluster.h"
#include "MitAna/DataTree/interface/BasicCluster.h"
#include "DataFormats/EgammaReco/interface/PreshowerCluster.h" 
#include "DataFormats/EgammaReco/interface/PreshowerClusterFwd.h" 
#include "MitAna/DataTree/interface/SuperClusterCol.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitProd/ObjectService/interface/ObjectService.h"
#include "RecoEgamma/EgammaIsolationAlgos/interface/EgammaTowerIsolation.h"
#include "RecoEcal/EgammaCoreTools/interface/Mustache.h"
#include "Geometry/CaloEventSetup/interface/CaloTopologyRecord.h"
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/CaloTopology/interface/CaloSubdetectorTopology.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHit.h"
#include "RecoEcal/EgammaCoreTools/interface/EcalClusterLazyTools.h"
#include "RecoEcal/EgammaCoreTools/interface/EcalClusterTools.h"
#include "Geometry/CaloTopology/interface/EcalPreshowerTopology.h"
#include "Geometry/EcalAlgo/interface/EcalPreshowerGeometry.h"
#include "RecoCaloTools/Navigation/interface/EcalPreshowerNavigator.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerSuperClusters::FillerSuperClusters(const ParameterSet &cfg, edm::ConsumesCollector& collector, ObjectService* os, const char *name, bool active) : 
  BaseFiller(cfg,os,name,active),
  edmToken_(GetToken<reco::SuperClusterCollection>(collector, cfg, "edmName","hybridSuperClusters")),
  //YI caloTowerToken_(GetToken<CaloTowerCollection>(collector, cfg, "caloTowerName","towerMaker")),
  ebRecHitsToken_(GetToken<EcalRecHitCollection>(collector, cfg, "ebRecHitsName", "reducedEcalRecHitsEB")),
  eeRecHitsToken_(GetToken<EcalRecHitCollection>(collector, cfg, "eeRecHitsName", "reducedEcalRecHitsEE")),
  esRecHitsToken_(GetToken<EcalRecHitCollection>(collector, cfg, "esRecHitsName", "reducedEcalRecHitsES")),
  mitName_              (cfg.getUntrackedParameter<string>("mitName","SuperClusters")),
  basicClusterMapName_  (cfg.getUntrackedParameter<string>("basicClusterMapName", "BasicClusterMap")),
  psClusterMapName_     (cfg.getUntrackedParameter<string>("psClusterMapName", "")),
  psXClusterMapName_    (cfg.getUntrackedParameter<string>("psXClusterMapName", "")),
  psYClusterMapName_    (cfg.getUntrackedParameter<string>("psYClusterMapName", "")),
  caloTowerDetIdMapName_(cfg.getUntrackedParameter<string>("caloTowerDetIdMapName", "CaloTowerDetIdMap")),
  superClusterMapName_  (cfg.getUntrackedParameter<string>("superClusterMapName", "SuperClusterMap")),
  superClusterIdMapName_(cfg.getUntrackedParameter<string>("superClusterIdMapName", "SuperClusterIdMap")),
  basicClusterMap_      (0),
  psClusterMap_         (0),
  psXClusterMap_        (0),
  psYClusterMap_        (0),
  caloTowerDetIdMap_    (0),
  superClusters_        (new mithep::SuperClusterArr(25)),
  superClusterMap_      (new mithep::SuperClusterMap),
  superClusterIdMap_    (new mithep::SuperClusterIdMap)
{
  // Constructor.
}

//--------------------------------------------------------------------------------------------------
FillerSuperClusters::~FillerSuperClusters()
{
  // Destructor.

  delete superClusters_;
  delete superClusterMap_;
  delete superClusterIdMap_;
}

//--------------------------------------------------------------------------------------------------
void FillerSuperClusters::BookDataBlock(TreeWriter &tws)
{
  // Add super cluster branch to tree and get pointers to maps.

  tws.AddBranch(mitName_,&superClusters_);
  OS()->add(superClusters_,mitName_);

  if (!basicClusterMapName_.empty()) {
    basicClusterMap_ = OS()->get<BasicClusterMap>(basicClusterMapName_);
    if (basicClusterMap_)
      AddBranchDep(mitName_,basicClusterMap_->GetBrName());
  }

  if (!psClusterMapName_.empty()) {
    psClusterMap_ = OS()->get<PsClusterMap>(psClusterMapName_);
    if (psClusterMap_)
      AddBranchDep(mitName_,psClusterMap_->GetBrName());
  }
  
  if (!psXClusterMapName_.empty()) {
    psXClusterMap_ = OS()->get<PsClusterMap>(psXClusterMapName_);
    if (psXClusterMap_)
      AddBranchDep(mitName_,psXClusterMap_->GetBrName());
  }
  
  if (!psYClusterMapName_.empty()) {
    psYClusterMap_ = OS()->get<PsClusterMap>(psYClusterMapName_);
    if (psYClusterMap_)
      AddBranchDep(mitName_,psYClusterMap_->GetBrName());
  }

  if (!caloTowerDetIdMapName_.empty()) {
    caloTowerDetIdMap_ = OS()->get<CaloTowerDetIdMap>(caloTowerDetIdMapName_);
    if (caloTowerDetIdMap_)
      AddBranchDep(mitName_,caloTowerDetIdMap_->GetBrName());
  }
  
  if (!superClusterMapName_.empty()) {
    superClusterMap_->SetBrName(mitName_);
    OS()->add(superClusterMap_,superClusterMapName_);
  }
  if (!superClusterIdMapName_.empty()) {
    superClusterIdMap_->SetBrName(mitName_);
    OS()->add(superClusterIdMap_,superClusterIdMapName_);
  }
}

//--------------------------------------------------------------------------------------------------
void FillerSuperClusters::FillDataBlock(const edm::Event      &event, 
                                        const edm::EventSetup &setup)
{
  // Fill the collection.

  superClusters_->Delete();
  superClusterMap_->Reset();
  superClusterIdMap_->Reset();

  Handle<reco::SuperClusterCollection> hSuperClusterProduct;
  GetProduct(edmToken_, hSuperClusterProduct, event);
  
  //CP Handle<CaloTowerCollection> hCaloTowerProduct;
  //CP if (!caloTowerToken_.isUninitialized())
  //CP   GetProduct(caloTowerToken_, hCaloTowerProduct, event);

  superClusterMap_->SetEdmProductId(hSuperClusterProduct.id().id());
  reco::SuperClusterCollection const& inSuperClusters = *hSuperClusterProduct;

  EcalClusterLazyTools* lazyTools = 0;
  EcalRecHitCollection const* ebRecHitCollection = 0;
  EcalRecHitCollection const* eeRecHitCollection = 0;

  if (!ebRecHitsToken_.isUninitialized() && !eeRecHitsToken_.isUninitialized()) {
    lazyTools = new EcalClusterLazyTools(event, setup, ebRecHitsToken_, eeRecHitsToken_);

    // ecal recHit collections for further ecal timing
    edm::Handle<EcalRecHitCollection> hEBRecHits;
    GetProduct(ebRecHitsToken_, hEBRecHits, event);
    ebRecHitCollection = hEBRecHits.product();
    edm::Handle<EcalRecHitCollection> hEERecHits;
    GetProduct(eeRecHitsToken_, hEERecHits, event);
    eeRecHitCollection = hEERecHits.product();
  }

  // mustache id
  reco::Mustache mustache;

  // es shape variables
  edm::Handle<EcalRecHitCollection> hESRecHits;
  GetProductSafe(esRecHitsToken_, hESRecHits, event);

  edm::ESHandle<CaloGeometry> pGeometry;
  setup.get<CaloGeometryRecord>().get(pGeometry);
 
  edm::ESHandle<CaloTopology> pTopology;
  setup.get<CaloTopologyRecord>().get(pTopology);

  //CP const CaloSubdetectorGeometry *geometryES =
  //CP   pGeometry->getSubdetectorGeometry(DetId::Ecal, EcalPreshower);
  EcalPreshowerTopology topology_p(pGeometry);

  // map of preshower rechits for shape calculations
  std::map<DetId, EcalRecHit const*> esmap;
  if (hESRecHits.isValid()) {
    auto& esRecHits = *hESRecHits;
    EcalRecHitCollection::const_iterator it;
    for (auto esHit : esRecHits) {
      // remove bad ES rechits
      if (esHit.recoFlag()==1 || esHit.recoFlag()==14 || (esHit.recoFlag()<=10 && esHit.recoFlag()>=5))
        continue;
      // make the map of DetID, EcalRecHit pairs
      esmap.insert(std::make_pair(esHit.id(), &esHit));
    }
  }

  // loop through all super clusters
  unsigned iSC = 0;
  for (auto&& inSC : inSuperClusters) {
    reco::SuperClusterRef ref(hSuperClusterProduct, iSC);
    ++iSC;

    mithep::SuperCluster *outSC = superClusters_->AddNew();

    outSC->SetXYZ(inSC.x(),inSC.y(),inSC.z());
    outSC->SetEnergy(inSC.energy());
    outSC->SetRawEnergy(inSC.rawEnergy());
    outSC->SetPreshowerEnergy(inSC.preshowerEnergy());
    outSC->SetPhiWidth(inSC.phiWidth());
    outSC->SetEtaWidth(inSC.etaWidth());

    auto hitsAndFractions = inSC.hitsAndFractions();

    // compute roundness and angle of the supercluster
    if (ebRecHitCollection && hitsAndFractions.size() != 0 &&
        hitsAndFractions.at(0).first.subdetId() == EcalBarrel) {
      std::vector<float> showerShapesBarrel = 
        EcalClusterTools::roundnessBarrelSuperClusters(inSC, *ebRecHitCollection, 0);
      outSC->SetRoundness(showerShapesBarrel[0]);
      outSC->SetAngle(showerShapesBarrel[1]);
    }

    //CP //Compute Hadronic Energy behind the supercluster (within DR < 0.15)
    //CP if(!caloTowerToken_.isUninitialized()) { 
    //CP   EgammaTowerIsolation towerIsoDepth1(0.15,0.,0.,1,hCaloTowerProduct.product()) ;  
    //CP   EgammaTowerIsolation towerIsoDepth2(0.15,0.,0.,2,hCaloTowerProduct.product()) ;  
    //CP   outSC->SetHcalDepth1Energy(towerIsoDepth1.getTowerESum(&(*inSC)));
    //CP   outSC->SetHcalDepth2Energy(towerIsoDepth2.getTowerESum(&(*inSC)));
    //CP }

    if (lazyTools && ebRecHitCollection && eeRecHitCollection) {
      // ecal timing information
      outSC->SetTime(lazyTools->SuperClusterTime(inSC, event));
      double seedTime = lazyTools->SuperClusterSeedTime(inSC);
      outSC->SetSeedTime(seedTime);

      // further ecal timing information
      double xtalEnergyThr = 1.; // 1 GeV Energy threshold (remove noise in timing calculation)
      // default values if the span time calculation fails
      double SCLeadTimeSpan = 1000;
      double SCSubLeadTimeSpan = 1000;
      SCTimeSpanCalculator(hitsAndFractions, xtalEnergyThr, seedTime, SCLeadTimeSpan, SCSubLeadTimeSpan, 
                           ebRecHitCollection, eeRecHitCollection);
      outSC->SetLeadTimeSpan(SCLeadTimeSpan);
      outSC->SetSubLeadTimeSpan(SCSubLeadTimeSpan);
    }

    auto seed = inSC.seed();
    if (seed.isNonnull()) {
      auto seedHitsAndFractions = seed->hitsAndFractions();

      // preshower shape variables
      if (seedHitsAndFractions.size() != 0 && esmap.size() != 0 &&
          seedHitsAndFractions.at(0).first.subdetId() == EcalEndcap) {
        std::vector<float> phoESShape = getESShape(getESHits(inSC.x(), inSC.y(), inSC.z(),
                                                             esmap, *pGeometry.product(),
                                                             &topology_p, 0));

        if (phoESShape.size()>=2) {
          outSC->SetPsEffWidthSigmaXX(phoESShape[0]);
          outSC->SetPsEffWidthSigmaYY(phoESShape[1]);
        }

      }
    }

    // add basic clusters that belong to this super cluster and tag with mustache id
    if (basicClusterMap_) {
      if(seed.isNonnull())
        outSC->SetSeed(basicClusterMap_->GetMit(seed));

      std::vector<const mithep::BasicCluster*> pcs;
      std::vector<reco::CaloCluster> bcs;
      std::vector<unsigned int> insidebcs;
      std::vector<unsigned int> outsidebcs;
      for (auto bc = inSC.clustersBegin(); bc != inSC.clustersEnd(); ++bc) {
        if (bc->isNonnull()) {
          const mithep::BasicCluster *bclus = basicClusterMap_->GetMit(*bc);
	
          // insertion sort (basic clusters sorted by energy)
          pcs.push_back(bclus);
          bcs.push_back(**bc);
          unsigned int iclus = pcs.size()-1;
          while (iclus>0 && pcs[iclus]->Energy()>pcs[iclus-1]->Energy()) {
            reco::CaloCluster tmp_bcs = bcs[iclus];
            bcs[iclus]   = bcs[iclus-1];
            bcs[iclus-1] = tmp_bcs;
	    
            const mithep::BasicCluster *tmp_pcs = pcs[iclus];
            pcs[iclus]   = pcs[iclus-1];
            pcs[iclus-1] = tmp_pcs;
	    
            iclus--;
          }
        }
      }
      for (unsigned int iclus=0; iclus<pcs.size(); iclus++)
        outSC->AddCluster(pcs[iclus]);
	
      mustache.MustacheClust(bcs,insidebcs,outsidebcs);
      for (unsigned int iclus=0; iclus<insidebcs.size(); ++iclus) {
        const_cast<mithep::BasicCluster*>(outSC->Cluster(insidebcs.at(iclus)))
          ->SetInsideMustache(kTRUE);
      }
      for (unsigned int iclus=0; iclus<outsidebcs.size(); ++iclus) {
        const_cast<mithep::BasicCluster*>(outSC->Cluster(outsidebcs.at(iclus)))
          ->SetInsideMustache(kFALSE);
      }
    }

    // add preshower clusters that belong to this super cluster and tag with mustache id
    if (psClusterMap_ || psXClusterMap_ || psYClusterMap_) {
      std::vector<const mithep::PsCluster*> pcs;
      std::vector<reco::CaloCluster> bcs;
      std::vector<unsigned int> insidebcs;
      std::vector<unsigned int> outsidebcs;
      for (auto bc = inSC.preshowerClustersBegin(); bc != inSC.preshowerClustersEnd(); ++bc) {
        if (bc->isNonnull()) {
          const mithep::PsCluster *psclus = 0;
          if (psClusterMap_)
            psclus = psClusterMap_->GetMit(*bc);
          if (psXClusterMap_)
            psclus = psXClusterMap_->GetMit(*bc);
          if (psYClusterMap_)
            psclus = psYClusterMap_->GetMit(*bc);
	  
          // insertion sort (Preshower clusters sorted by energy)
          pcs.push_back(psclus);
          bcs.push_back(**bc);
          unsigned int iclus = pcs.size()-1;
          while (iclus>0 && pcs[iclus]->Energy()>pcs[iclus-1]->Energy()) {
            reco::CaloCluster tmp_bcs = bcs[iclus];
            bcs[iclus]   = bcs[iclus-1];
            bcs[iclus-1] = tmp_bcs;
	    
            const mithep::PsCluster *tmp_pcs = pcs[iclus];
            pcs[iclus]   = pcs[iclus-1];
            pcs[iclus-1] = tmp_pcs;
	    
            iclus--;
          }
        }
      }      
      for (unsigned int iclus=0; iclus<pcs.size(); iclus++)
        outSC->AddPsClust(pcs[iclus]);

      mustache.MustacheClust(bcs,insidebcs,outsidebcs);
      for (unsigned int iclus=0; iclus<insidebcs.size(); ++iclus) {
        const_cast<mithep::PsCluster*>(outSC->PsClust(insidebcs.at(iclus)))
          ->SetInsideMustache(kTRUE);
      }
      for (unsigned int iclus=0; iclus<outsidebcs.size(); ++iclus) {
        const_cast<mithep::PsCluster*>(outSC->PsClust(outsidebcs.at(iclus)))
          ->SetInsideMustache(kFALSE);
      }
    }

    // compute preshower energy sums by plane
    if (inSC.preshowerClusters().isAvailable()) {
      double psenergyplane1 = 0.;
      double psenergyplane2 = 0.; 
      for(auto bc = inSC.preshowerClustersBegin(); bc != inSC.preshowerClustersEnd(); ++bc) {
        auto pscluster = dynamic_cast<const reco::PreshowerCluster*>(bc->get());

        if (!pscluster)
          continue;

        if      (pscluster->plane()==1)
          psenergyplane1 += pscluster->energy();
        else if (pscluster->plane()==2)
          psenergyplane2 += pscluster->energy();
      }
      outSC->SetPreshowerEnergyPlane1(psenergyplane1);
      outSC->SetPreshowerEnergyPlane2(psenergyplane2);
    }

    // add super cluster det ids to the id map and also fill supercluster-calotower associations
    for (auto&& hitAndFraction : hitsAndFractions) {
      auto& id = hitAndFraction.first;
   
      if (caloTowerDetIdMap_) {
        if (caloTowerDetIdMap_->HasMit(id)) {
          const mithep::CaloTower *matchedTower = caloTowerDetIdMap_->GetMit(id);
          if (!outSC->HasTower(matchedTower)) {
            outSC->AddTower(matchedTower);
          }
        }
      }
      
      superClusterIdMap_->Add(id,outSC);
    }

    // add super cluster to the map
    superClusterMap_->Add(ref, outSC);
  }

  superClusters_->Trim();

  delete lazyTools;
}

// horrible code below copied from globe for preshower cluster shape calculations
std::vector<float> FillerSuperClusters::getESHits(double X, double Y, double Z,
                                                  std::map<DetId, EcalRecHit const*> const& rechits_map,
                                                  const CaloGeometry& geometry,
                                                  CaloSubdetectorTopology *topology_p, int row)
{
  std::vector<float> esHits;

  const GlobalPoint point(X,Y,Z);

  const CaloSubdetectorGeometry *geometry_p = geometry.getSubdetectorGeometry(DetId::Ecal, EcalPreshower);

  DetId esId1 = (dynamic_cast<const EcalPreshowerGeometry*>(geometry_p))->getClosestCellInPlane(point, 1);
  DetId esId2 = (dynamic_cast<const EcalPreshowerGeometry*>(geometry_p))->getClosestCellInPlane(point, 2);

  ESDetId esDetId1 = (esId1 == DetId(0)) ? ESDetId(0) : ESDetId(esId1);
  ESDetId esDetId2 = (esId2 == DetId(0)) ? ESDetId(0) : ESDetId(esId2);

  std::map<DetId, EcalRecHit const*>::const_iterator it;
  std::map<DetId, EcalRecHit const*>::const_iterator mapEnd = rechits_map.end();
  ESDetId next;
  ESDetId strip1;
  ESDetId strip2;

  strip1 = esDetId1;
  strip2 = esDetId2;
    
  EcalPreshowerNavigator theESNav1(strip1, topology_p);
  theESNav1.setHome(strip1);
    
  EcalPreshowerNavigator theESNav2(strip2, topology_p);
  theESNav2.setHome(strip2);

  if (row == 1) {
    if (strip1 != ESDetId(0))
      strip1 = theESNav1.north();
    if (strip2 != ESDetId(0))
      strip2 = theESNav2.east();
  } else if (row == -1) {
    if (strip1 != ESDetId(0))
      strip1 = theESNav1.south();
    if (strip2 != ESDetId(0))
      strip2 = theESNav2.west();
  }

  // Plane 1 
  if (strip1 == ESDetId(0)) {
    for (int i=0; i<31; ++i)
      esHits.push_back(0);
  }
  else {

    it = rechits_map.find(strip1);
    if (it != mapEnd && it->second->energy() > 1.0e-10)
      esHits.push_back(it->second->energy());
    else
      esHits.push_back(0);

    // east road 
    for (int i=0; i<15; ++i) {
      next = theESNav1.east();
      if (next != ESDetId(0)) {
        it = rechits_map.find(next);
        if (it != mapEnd && it->second->energy() > 1.0e-10)
          esHits.push_back(it->second->energy());
        else
          esHits.push_back(0);
      }
      else {
        for (int j=i; j<15; j++)
          esHits.push_back(0);
        break;
      }
    }

    // west road 
    theESNav1.setHome(strip1);
    theESNav1.home();
    for (int i=0; i<15; ++i) {
      next = theESNav1.west();
      if (next != ESDetId(0)) {
        it = rechits_map.find(next);
        if (it != mapEnd && it->second->energy() > 1.0e-10)
          esHits.push_back(it->second->energy());
        else
          esHits.push_back(0);
      }
      else {
        for (int j=i; j<15; j++)
          esHits.push_back(0);
        break;
      }
    }
  }

  if (strip2 == ESDetId(0)) {
    for (int i=0; i<31; ++i)
      esHits.push_back(0);
  }
  else {

    it = rechits_map.find(strip2);
    if (it != mapEnd && it->second->energy() > 1.0e-10)
      esHits.push_back(it->second->energy());
    else
      esHits.push_back(0);

    // north road 
    for (int i=0; i<15; ++i) {
      next = theESNav2.north();
      if (next != ESDetId(0)) {
        it = rechits_map.find(next);
        if (it != mapEnd && it->second->energy() > 1.0e-10)
          esHits.push_back(it->second->energy());
        else
          esHits.push_back(0);
      }
      else {
        for (int j=i; j<15; j++)
          esHits.push_back(0);
        break;
      }
    }

    // south road 
    theESNav2.setHome(strip2);
    theESNav2.home();

    for (int i=0; i<15; ++i) {
      next = theESNav2.south();
      if (next != ESDetId(0)) {
        it = rechits_map.find(next);
        if (it != mapEnd && it->second->energy() > 1.0e-10)
          esHits.push_back(it->second->energy());
        else
          esHits.push_back(0);
      }
      else {
        for (int j=i; j<15; j++)
          esHits.push_back(0);
        break;
      }
    }
  }

  return esHits;
}

std::vector<float> FillerSuperClusters::getESShape(std::vector<float> ESHits0)
{
  std::vector<float> esShape;
    
  const int nBIN = 21;
  float esRH_F[nBIN];
  float esRH_R[nBIN];
  for (int idx=0; idx<nBIN; idx++) {
    esRH_F[idx] = 0.;
    esRH_R[idx] = 0.;
  }

  for (int ibin=0; ibin<((nBIN+1)/2); ibin++) {
    if (ibin==0) {
      esRH_F[(nBIN-1)/2] = ESHits0[ibin];
      esRH_R[(nBIN-1)/2] = ESHits0[ibin+31];
    }
    else {
      esRH_F[(nBIN-1)/2+ibin] = ESHits0[ibin];
      esRH_F[(nBIN-1)/2-ibin] = ESHits0[ibin+15];
      esRH_R[(nBIN-1)/2+ibin] = ESHits0[ibin+31];
      esRH_R[(nBIN-1)/2-ibin] = ESHits0[ibin+31+15];
    }
  } 

  // ---- Effective Energy Deposit Width ---- //
  double EffWidthSigmaXX = 0.;
  double EffWidthSigmaYY = 0.;
  double totalEnergyXX   = 0.;
  double totalEnergyYY   = 0.;
  double EffStatsXX      = 0.;
  double EffStatsYY      = 0.;
  for (int id_X=0; id_X<21; id_X++) {
    totalEnergyXX  += esRH_F[id_X];
    EffStatsXX     += esRH_F[id_X]*(id_X-10)*(id_X-10);
    totalEnergyYY  += esRH_R[id_X];
    EffStatsYY     += esRH_R[id_X]*(id_X-10)*(id_X-10);
  }
  EffWidthSigmaXX  = (totalEnergyXX>0.)  ? sqrt(fabs(EffStatsXX  / totalEnergyXX))   : 0.;
  EffWidthSigmaYY  = (totalEnergyYY>0.)  ? sqrt(fabs(EffStatsYY  / totalEnergyYY))   : 0.;

  esShape.push_back(EffWidthSigmaXX);
  esShape.push_back(EffWidthSigmaYY);
    
  return esShape;
}

//--------------------------------------------------------------------------------------------------
void  FillerSuperClusters::SCTimeSpanCalculator(std::vector<std::pair<DetId, float> > const& hitsAndFractions,
                                                double xtalEnergyThr, double seedTime, 
                                                double& SCLeadTimeSpan, double& SCSubLeadTimeSpan, 
                                                const EcalRecHitCollection* ebRecHitCol,
                                                const EcalRecHitCollection* eeRecHitCol)
{
  // This function provides information on the time span of the given supercluster.  The seed is
  // used as reference, and the time span is computed as the maximal time distance between the seed
  // time and any other SC xtal time. Low energy xtal are discarded from the computation to mitigate
  // the effect of noise/PU

  // check if the SC is in the ecal fiducial region, otherwise return default time span values
  if (hitsAndFractions.at(0).first.subdetId() != EcalBarrel &&
      hitsAndFractions.at(0).first.subdetId() != EcalEndcap   )
    return;

  // initialize the time span variables 
  SCLeadTimeSpan = 0;
  SCSubLeadTimeSpan = 0;

  // loop on SuperCluster recHits
  for (auto&& hitAndFraction : hitsAndFractions) {
    // scl seed is in barrel
    if (hitsAndFractions.at(0).first.subdetId() == EcalBarrel) {
      auto itt = ebRecHitCol->find(hitAndFraction.first);
      // check xtal energy
      if ((*itt).energy() > xtalEnergyThr) {
        float timeDiff = seedTime - (*itt).time();
        if (fabs(timeDiff) > fabs(SCLeadTimeSpan)) {
          SCSubLeadTimeSpan = SCLeadTimeSpan; 
          SCLeadTimeSpan = timeDiff;
        }
        if (fabs(timeDiff) > fabs(SCSubLeadTimeSpan) && fabs(timeDiff) < fabs(SCLeadTimeSpan))
          SCSubLeadTimeSpan = timeDiff;
      }
    }
    // scl seed is in endcap
    else {
      auto itt = eeRecHitCol->find(hitAndFraction.first);
      // check xtal energy
      if ((*itt).energy() > xtalEnergyThr) {
        float timeDiff = seedTime - (*itt).time();
        if (fabs(timeDiff) > fabs(SCLeadTimeSpan)) {
          SCSubLeadTimeSpan = SCLeadTimeSpan; 
          SCLeadTimeSpan = timeDiff;
        }
        if (fabs(timeDiff) > fabs(SCSubLeadTimeSpan) && fabs(timeDiff) < fabs(SCLeadTimeSpan))
          SCSubLeadTimeSpan = timeDiff;
      }
    }
  }
  
  return;
}

DEFINE_MITHEP_TREEFILLER(FillerSuperClusters);
