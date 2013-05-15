// $Id: FillerSuperClusters.cc,v 1.17 2012/12/28 17:27:21 pharris Exp $

#include "MitProd/TreeFiller/interface/FillerSuperClusters.h"
#include "DataFormats/EgammaReco/interface/SuperClusterFwd.h"
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
#include "Geometry/CaloTopology/interface/EcalPreshowerTopology.h"
#include "Geometry/EcalAlgo/interface/EcalPreshowerGeometry.h"
#include "RecoCaloTools/Navigation/interface/EcalPreshowerNavigator.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerSuperClusters::FillerSuperClusters(const ParameterSet &cfg, const char *name, bool active) : 
  BaseFiller            (cfg,name,active),
  edmName_              (Conf().getUntrackedParameter<string>("edmName","hybridSuperClusters")),
  mitName_              (Conf().getUntrackedParameter<string>("mitName","SuperClusters")),
  basicClusterMapName_  (Conf().getUntrackedParameter<string>("basicClusterMapName", 
                                                              "BasicClusterMap")),
  psClusterMapName_     (Conf().getUntrackedParameter<string>("psClusterMapName", 
							      "")),                                                            
  psXClusterMapName_    (Conf().getUntrackedParameter<string>("psXClusterMapName", 
							      "")),                                                            
  psYClusterMapName_    (Conf().getUntrackedParameter<string>("psYClusterMapName", 
							      "")),                                                            
  caloTowerDetIdMapName_(Conf().getUntrackedParameter<string>("caloTowerDetIdMapName", 
							      "CaloTowerDetIdMap")),                                                            
  superClusterMapName_  (Conf().getUntrackedParameter<string>("superClusterMapName", 
							      "SuperClusterMap")),
  superClusterIdMapName_(Conf().getUntrackedParameter<string>("superClusterIdMapName", 
                                                              "SuperClusterIdMap")),
  caloTowerName_        (Conf().getUntrackedParameter<string>("caloTowerName","towerMaker")),
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
  OS()->add<SuperClusterArr>(superClusters_,mitName_);

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
    OS()->add<SuperClusterMap>(superClusterMap_,superClusterMapName_);
  }
  if (!superClusterIdMapName_.empty()) {
    superClusterIdMap_->SetBrName(mitName_);
    OS()->add<SuperClusterIdMap>(superClusterIdMap_,superClusterIdMapName_);
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
  GetProduct(edmName_, hSuperClusterProduct, event);
  
  Handle<CaloTowerCollection> hCaloTowerProduct;
  if(caloTowerName_.size() > 0) GetProduct(caloTowerName_, hCaloTowerProduct, event);

  superClusterMap_->SetEdmProductId(hSuperClusterProduct.id().id());
  const reco::SuperClusterCollection inSuperClusters = *(hSuperClusterProduct.product());  

  //lazy tools for supercluster ecal timing
  EcalClusterLazyTools lazyTools(event, setup, edm::InputTag("reducedEcalRecHitsEB"), 
                                 edm::InputTag("reducedEcalRecHitsEE"));

  //mustache id
  reco::Mustache mustache;

  //es shape variables
  edm::Handle<EcalRecHitCollection> hESRecHits;
  event.getByLabel("reducedEcalRecHitsES" , hESRecHits);

  edm::ESHandle<CaloGeometry> pGeometry;
  setup.get<CaloGeometryRecord>().get(pGeometry);
 
  edm::ESHandle<CaloTopology> pTopology;
  setup.get<CaloTopologyRecord>().get(pTopology);

  const CaloSubdetectorGeometry *geometryES = pGeometry->getSubdetectorGeometry(DetId::Ecal, EcalPreshower);
  EcalPreshowerTopology topology_p(pGeometry);

  //map of preshower rechits for shape calculations
  std::map<DetId, EcalRecHit> esmap;
  if (hESRecHits.isValid()) {
    EcalRecHitCollection::const_iterator it;
    for (it = hESRecHits->begin(); it != hESRecHits->end(); ++it) {
      // remove bad ES rechits
      if (it->recoFlag()==1 || it->recoFlag()==14 || (it->recoFlag()<=10 && it->recoFlag()>=5)) continue;
      //Make the map of DetID, EcalRecHit pairs
      esmap.insert(std::make_pair(it->id(), *it));
    }
  }

  // loop through all super clusters
  for (reco::SuperClusterCollection::const_iterator inSC = inSuperClusters.begin(); 
       inSC != inSuperClusters.end(); ++inSC) {

    mithep::SuperCluster *outSC = superClusters_->Allocate();
    new (outSC) mithep::SuperCluster();

    outSC->SetXYZ(inSC->x(),inSC->y(),inSC->z());
    outSC->SetEnergy(inSC->energy());
    outSC->SetRawEnergy(inSC->rawEnergy());
    outSC->SetPreshowerEnergy(inSC->preshowerEnergy());
    outSC->SetPhiWidth(inSC->phiWidth());
    outSC->SetEtaWidth(inSC->etaWidth());

    //Compute Hadronic Energy behind the supercluster (within DR < 0.15)
    if(caloTowerName_.size() > 0) { 
      EgammaTowerIsolation towerIsoDepth1(0.15,0.,0.,1,hCaloTowerProduct.product()) ;  
      EgammaTowerIsolation towerIsoDepth2(0.15,0.,0.,2,hCaloTowerProduct.product()) ;  
      outSC->SetHcalDepth1Energy(towerIsoDepth1.getTowerESum(&(*inSC)));
      outSC->SetHcalDepth2Energy(towerIsoDepth2.getTowerESum(&(*inSC)));
    }

    //ecal timing information
    outSC->SetTime(lazyTools.SuperClusterTime(*inSC, event));
    outSC->SetSeedTime(lazyTools.SuperClusterSeedTime(*inSC));

    //preshower shape variables
    if (inSC->seed()->hitsAndFractions().at(0).first.subdetId()==EcalEndcap) {
      std::vector<float> phoESShape = getESShape(getESHits(inSC->x(), inSC->y(), inSC->z(), esmap, *pGeometry.product(), &topology_p, 0));

      if (phoESShape.size()>=2) {
        outSC->SetPsEffWidthSigmaXX(phoESShape[0]);
        outSC->SetPsEffWidthSigmaYY(phoESShape[1]);
      }

    }


    // set the seed
    if (basicClusterMap_ && inSC->seed().isNonnull())
      outSC->SetSeed(basicClusterMap_->GetMit(inSC->seed()));
    
    // add basic clusters that belong to this super cluster and tag with mustache id
    if (basicClusterMap_) {
      std::vector<const mithep::BasicCluster*> pcs;
      std::vector<reco::CaloCluster> bcs;
      std::vector<unsigned int> insidebcs;
      std::vector<unsigned int> outsidebcs;
      for(reco::CaloCluster_iterator bc = inSC->clustersBegin(); bc != inSC->clustersEnd(); ++bc) {
        if (bc->isNonnull()) {
	  const mithep::BasicCluster *bclus = basicClusterMap_->GetMit(*bc);
	
	  // insertion sort (basic clusters sorted by energy)
	  pcs.push_back(bclus);
          bcs.push_back(**bc);
	  unsigned int iclus = pcs.size()-1;
	  while(iclus>0 && pcs[iclus]->Energy()>pcs[iclus-1]->Energy()) {
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
      for(unsigned int iclus=0; iclus<pcs.size(); iclus++)
        outSC->AddCluster(pcs[iclus]);
	
      mustache.MustacheClust(bcs,insidebcs,outsidebcs);
      for (unsigned int iclus=0; iclus<insidebcs.size(); ++iclus) {
        const_cast<mithep::BasicCluster*>(outSC->Cluster(insidebcs.at(iclus)))->SetInsideMustache(kTRUE);
      }
      for (unsigned int iclus=0; iclus<outsidebcs.size(); ++iclus) {
        const_cast<mithep::BasicCluster*>(outSC->Cluster(outsidebcs.at(iclus)))->SetInsideMustache(kFALSE);
      }
    }

    // add preshower clusters that belong to this super cluster and tag with mustache id
    if (psClusterMap_ || psXClusterMap_ || psYClusterMap_) {
      std::vector<const mithep::PsCluster*> pcs;
      std::vector<reco::CaloCluster> bcs;
      std::vector<unsigned int> insidebcs;
      std::vector<unsigned int> outsidebcs;
      for(reco::CaloCluster_iterator bc = inSC->preshowerClustersBegin(); bc != inSC->preshowerClustersEnd(); ++bc) {
        if (bc->isNonnull()) {
	  const mithep::PsCluster *psclus = 0;
          if(psClusterMap_)  psclus = psClusterMap_->GetMit(*bc);
	  if(psXClusterMap_) psclus = psXClusterMap_->GetMit(*bc);
	  if(psYClusterMap_) psclus = psYClusterMap_->GetMit(*bc);
	  
	  // insertion sort (Preshower clusters sorted by energy)
	  pcs.push_back(psclus);
          bcs.push_back(**bc);
	  unsigned int iclus = pcs.size()-1;
	  while(iclus>0 && pcs[iclus]->Energy()>pcs[iclus-1]->Energy()) {
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
      for(unsigned int iclus=0; iclus<pcs.size(); iclus++)
        outSC->AddPsClust(pcs[iclus]);

      mustache.MustacheClust(bcs,insidebcs,outsidebcs);
      for (unsigned int iclus=0; iclus<insidebcs.size(); ++iclus) {
        const_cast<mithep::PsCluster*>(outSC->PsClust(insidebcs.at(iclus)))->SetInsideMustache(kTRUE);
      }
      for (unsigned int iclus=0; iclus<outsidebcs.size(); ++iclus) {
        const_cast<mithep::PsCluster*>(outSC->PsClust(outsidebcs.at(iclus)))->SetInsideMustache(kFALSE);
      }
    }

    //compute preshower energy sums by plane
    double psenergyplane1 = 0.;
    double psenergyplane2 = 0.; 
    for(reco::CaloCluster_iterator bc = inSC->preshowerClustersBegin(); bc != inSC->preshowerClustersEnd(); ++bc) {
      const reco::PreshowerCluster *pscluster = dynamic_cast<const reco::PreshowerCluster*>(bc->get());
      if (!pscluster) continue;

      if (pscluster->plane()==1) psenergyplane1 += pscluster->energy();
      else if (pscluster->plane()==2) psenergyplane2 += pscluster->energy();
    }
    outSC->SetPreshowerEnergyPlane1(psenergyplane1);
    outSC->SetPreshowerEnergyPlane2(psenergyplane2);

    //add super cluster det ids to the id map and also fill supercluster-calotower associations
    const std::vector< std::pair<DetId, float> > &pairs = inSC->hitsAndFractions();
    for (std::vector< std::pair<DetId, float> >::const_iterator ipair = pairs.begin();
          ipair < pairs.end(); ++ipair) {

      const DetId &ihit = ipair->first;
    
      if (caloTowerDetIdMap_) {
        if (caloTowerDetIdMap_->HasMit(ihit)) {
          const mithep::CaloTower *matchedTower = caloTowerDetIdMap_->GetMit(ihit);
          if (!outSC->HasTower(matchedTower)) {
            outSC->AddTower(matchedTower);
          }
        }
      }
      
      superClusterIdMap_->Add(ihit,outSC);
    }

    // add super cluster to the map
    reco::SuperClusterRef theRef(hSuperClusterProduct, inSC-inSuperClusters.begin());
    superClusterMap_->Add(theRef, outSC);

  }
  superClusters_->Trim();
}

//horrible code below copied from globe for preshower cluster shape calculations
std::vector<float> FillerSuperClusters::getESHits(double X, double Y, double Z, std::map<DetId, EcalRecHit> rechits_map, const CaloGeometry& geometry, CaloSubdetectorTopology *topology_p, int row) {
  std::vector<float> esHits;

  const GlobalPoint point(X,Y,Z);

  const CaloSubdetectorGeometry *geometry_p ;
  geometry_p = geometry.getSubdetectorGeometry (DetId::Ecal,EcalPreshower) ;

  DetId esId1 = (dynamic_cast<const EcalPreshowerGeometry*>(geometry_p))->getClosestCellInPlane(point, 1);
  DetId esId2 = (dynamic_cast<const EcalPreshowerGeometry*>(geometry_p))->getClosestCellInPlane(point, 2);
  ESDetId esDetId1 = (esId1 == DetId(0)) ? ESDetId(0) : ESDetId(esId1);
  ESDetId esDetId2 = (esId2 == DetId(0)) ? ESDetId(0) : ESDetId(esId2);

  std::map<DetId, EcalRecHit>::iterator it;
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
    if (strip1 != ESDetId(0)) strip1 = theESNav1.north();
    if (strip2 != ESDetId(0)) strip2 = theESNav2.east();
  } else if (row == -1) {
    if (strip1 != ESDetId(0)) strip1 = theESNav1.south();
    if (strip2 != ESDetId(0)) strip2 = theESNav2.west();
  }

  // Plane 1 
  if (strip1 == ESDetId(0)) {
    for (int i=0; i<31; ++i) esHits.push_back(0);
  } else {

    it = rechits_map.find(strip1);
    if (it->second.energy() > 1.0e-10 && it != rechits_map.end()) esHits.push_back(it->second.energy());
    else esHits.push_back(0);
    //cout<<"center : "<<strip1<<" "<<it->second.energy()<<endl;      

    // east road 
    for (int i=0; i<15; ++i) {
      next = theESNav1.east();
      if (next != ESDetId(0)) {
        it = rechits_map.find(next);
        if (it->second.energy() > 1.0e-10 && it != rechits_map.end()) esHits.push_back(it->second.energy());
        else esHits.push_back(0);
        //cout<<"east "<<i<<" : "<<next<<" "<<it->second.energy()<<endl;
      } else {
        for (int j=i; j<15; j++) esHits.push_back(0);
        break;
        //cout<<"east "<<i<<" : "<<next<<" "<<0<<endl;
      }
    }

    // west road 
    theESNav1.setHome(strip1);
    theESNav1.home();
    for (int i=0; i<15; ++i) {
      next = theESNav1.west();
      if (next != ESDetId(0)) {
        it = rechits_map.find(next);
        if (it->second.energy() > 1.0e-10 && it != rechits_map.end()) esHits.push_back(it->second.energy());
        else esHits.push_back(0);
        //cout<<"west "<<i<<" : "<<next<<" "<<it->second.energy()<<endl;
      } else {
        for (int j=i; j<15; j++) esHits.push_back(0);
        break;
        //cout<<"west "<<i<<" : "<<next<<" "<<0<<endl;
      }
    }
  }

  if (strip2 == ESDetId(0)) {
    for (int i=0; i<31; ++i) esHits.push_back(0);
  } else {

    it = rechits_map.find(strip2);
    if (it->second.energy() > 1.0e-10 && it != rechits_map.end()) esHits.push_back(it->second.energy());
    else esHits.push_back(0);
    //cout<<"center : "<<strip2<<" "<<it->second.energy()<<endl;      

    // north road 
    for (int i=0; i<15; ++i) {
      next = theESNav2.north();
      if (next != ESDetId(0)) {
        it = rechits_map.find(next);
        if (it->second.energy() > 1.0e-10 && it != rechits_map.end()) esHits.push_back(it->second.energy());
        else esHits.push_back(0);
        //cout<<"north "<<i<<" : "<<next<<" "<<it->second.energy()<<endl;  
      } else {
        for (int j=i; j<15; j++) esHits.push_back(0);
        break;
        //cout<<"north "<<i<<" : "<<next<<" "<<0<<endl;
      }
    }

    // south road 
    theESNav2.setHome(strip2);
    theESNav2.home();
    for (int i=0; i<15; ++i) {
      next = theESNav2.south();
      if (next != ESDetId(0)) {
        it = rechits_map.find(next);
        if (it->second.energy() > 1.0e-10 && it != rechits_map.end()) esHits.push_back(it->second.energy());
        else esHits.push_back(0);
        //cout<<"south "<<i<<" : "<<next<<" "<<it->second.energy()<<endl;
      } else {
        for (int j=i; j<15; j++) esHits.push_back(0);
        break;
        //cout<<"south "<<i<<" : "<<next<<" "<<0<<endl;
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

  for(int ibin=0; ibin<((nBIN+1)/2); ibin++) {
    if (ibin==0) {
      esRH_F[(nBIN-1)/2] = ESHits0[ibin];
      esRH_R[(nBIN-1)/2] = ESHits0[ibin+31];
    } else {
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

