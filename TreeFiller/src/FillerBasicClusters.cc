#include "MitProd/TreeFiller/interface/FillerBasicClusters.h"
#include "DataFormats/CaloRecHit/interface/CaloCluster.h"
#include "MitAna/DataTree/interface/BasicClusterCol.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitProd/ObjectService/interface/ObjectService.h"
#include "RecoEcal/EgammaCoreTools/interface/EcalClusterLazyTools.h"
#include "RecoLocalCalo/EcalRecAlgos/interface/EcalSeverityLevelAlgo.h"
#include "DataFormats/EcalDetId/interface/EcalSubdetector.h"
#include "RecoEgamma/EgammaTools/interface/EcalClusterLocal.h"
#include "RecoEcal/EgammaCoreTools/interface/EcalClusterTools.h"
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"
//#include "RecoEgamma/EgammaTools/interface/ggPFClusters.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerBasicClusters::FillerBasicClusters(const ParameterSet &cfg, ConsumesCollector& collector, ObjectService* os, const char *name, bool active) : 
  BaseFiller(cfg,os,name,active),
  edmToken_(GetToken<reco::CaloClusterCollection>(collector, cfg, "edmName", "hybridSuperClusters")),
  barrelEcalRecHitToken_(GetToken<EcalRecHitCollection>(collector, cfg, "barrelEcalRecHitName","")),
  endcapEcalRecHitToken_(GetToken<EcalRecHitCollection>(collector, cfg, "endcapEcalRecHitName","")),
  mitName_             (cfg.getUntrackedParameter<string>("mitName","BasicClusters")),
  basicClusterMapName_ (cfg.getUntrackedParameter<string>("basicClusterMapName",
							     "BasicClusterMap")),
  basicClusters_       (new mithep::BasicClusterArr(100)),
  basicClusterMap_     (new mithep::BasicClusterMap)
{
  // Constructor.
}

//--------------------------------------------------------------------------------------------------
FillerBasicClusters::~FillerBasicClusters()
{
  // Destructor.

  delete basicClusters_;
  delete basicClusterMap_;
}

//--------------------------------------------------------------------------------------------------
void FillerBasicClusters::BookDataBlock(TreeWriter &tws)
{
  // Add BasicCluster branch and the BasicClusterMap to tree.

  tws.AddBranch(mitName_,&basicClusters_);
  OS()->add<BasicClusterArr>(basicClusters_,mitName_);

  if (!basicClusterMapName_.empty()) {
    basicClusterMap_->SetBrName(mitName_);
    OS()->add<BasicClusterMap>(basicClusterMap_,basicClusterMapName_);
  }
}

//--------------------------------------------------------------------------------------------------
void FillerBasicClusters::FillDataBlock(const edm::Event      &event, 
                                        const edm::EventSetup &setup)
{
  // Fill the BasicCluster information into our structures.

  basicClusters_  ->Delete();
  basicClusterMap_->Reset();

  Handle<reco::CaloClusterCollection> hBasicClusterProduct;
  GetProduct(edmToken_, hBasicClusterProduct, event);
  basicClusterMap_->SetEdmProductId(hBasicClusterProduct.id().id());
  const reco::CaloClusterCollection &inBasicClusters = *(hBasicClusterProduct.product());  

  edm::Handle< EcalRecHitCollection > pEBRecHits;
  GetProduct(barrelEcalRecHitToken_, pEBRecHits, event);
  edm::Handle< EcalRecHitCollection > pEERecHits;
  GetProduct(endcapEcalRecHitToken_, pEERecHits, event);
  
  edm::ESHandle<CaloGeometry> pGeometry;
  setup.get<CaloGeometryRecord>().get(pGeometry);
 
  // const CaloSubdetectorGeometry *geometryEB = pGeometry->getSubdetectorGeometry(DetId::Ecal, EcalBarrel);
  // const CaloSubdetectorGeometry *geometryEE = pGeometry->getSubdetectorGeometry(DetId::Ecal, EcalEndcap);

  EcalClusterLazyTools lazyTools(event, setup, barrelEcalRecHitToken_, endcapEcalRecHitToken_);

  //  ggPFClusters pfclusters(pEBRecHits, pEERecHits, geometryEB, geometryEE);

  EcalClusterLocal local;                                 
                                 
  // loop over rechits and make map of energy values
  std::map<DetId,float> hitEnergies;

  for (EcalRecHitCollection::const_iterator it = pEBRecHits->begin(); it!=pEBRecHits->end(); ++it)
    hitEnergies.insert(std::pair<DetId,float>(it->id(),it->energy()));

  for (EcalRecHitCollection::const_iterator it = pEERecHits->begin(); it!=pEERecHits->end(); ++it)
    hitEnergies.insert(std::pair<DetId,float>(it->id(),it->energy()));

  // loop through all basic clusters
  for (reco::CaloClusterCollection::const_iterator inBC = inBasicClusters.begin(); 
       inBC != inBasicClusters.end(); ++inBC) {

    mithep::BasicCluster *outBasicCluster = basicClusters_->AddNew();

    outBasicCluster->SetXYZ   (inBC->x(),inBC->y(),inBC->z());
    outBasicCluster->SetEnergy(inBC->energy());   
    outBasicCluster->SetNHits (inBC->size());

    outBasicCluster->SetCovEtaPhi  (lazyTools.covariances(*inBC)[1]);
    outBasicCluster->SetCoviEtaiEta(lazyTools.localCovariances(*inBC)[0]);
    outBasicCluster->SetCoviEtaiPhi(lazyTools.localCovariances(*inBC)[1]);
    outBasicCluster->SetCoviPhiiPhi(lazyTools.localCovariances(*inBC)[2]);
    outBasicCluster->SetZernike20  (lazyTools.zernike20(*inBC));
    outBasicCluster->SetZernike42  (lazyTools.zernike42(*inBC));
    outBasicCluster->SetEtaLat     (lazyTools.lat(*inBC)[0]);
    outBasicCluster->SetPhiLat     (lazyTools.lat(*inBC)[1]);
    outBasicCluster->SetLat        (lazyTools.lat(*inBC)[2]);
    outBasicCluster->SetE1x3       (lazyTools.e1x3(*inBC));
    outBasicCluster->SetE3x1       (lazyTools.e3x1(*inBC));
    outBasicCluster->SetE1x5       (lazyTools.e1x5(*inBC));
    outBasicCluster->SetE2x2       (lazyTools.e2x2(*inBC));
    outBasicCluster->SetE3x2       (lazyTools.e3x2(*inBC));
    outBasicCluster->SetE3x3       (lazyTools.e3x3(*inBC));
    outBasicCluster->SetE4x4       (lazyTools.e4x4(*inBC));
    outBasicCluster->SetE5x5       (lazyTools.e5x5(*inBC));
    outBasicCluster->SetE2x5Right  (lazyTools.e2x5Right(*inBC));
    outBasicCluster->SetE2x5Left   (lazyTools.e2x5Left(*inBC));
    outBasicCluster->SetE2x5Top    (lazyTools.e2x5Top(*inBC));
    outBasicCluster->SetE2x5Bottom (lazyTools.e2x5Bottom(*inBC));
    outBasicCluster->SetE2x5Max    (lazyTools.e2x5Max(*inBC));
    outBasicCluster->SetELeft      (lazyTools.eLeft(*inBC));
    outBasicCluster->SetERight     (lazyTools.eRight(*inBC));
    outBasicCluster->SetETop       (lazyTools.eTop(*inBC));
    outBasicCluster->SetEBottom    (lazyTools.eBottom(*inBC));
    outBasicCluster->SetEMax       (lazyTools.eMax(*inBC));
    outBasicCluster->SetE2nd       (lazyTools.e2nd(*inBC));
    outBasicCluster->SetCovEtaEta  (lazyTools.covariances(*inBC)[0]);
    outBasicCluster->SetCovPhiPhi  (lazyTools.covariances(*inBC)[2]);

    // ecal timing information
    const reco::BasicCluster *inbcd = dynamic_cast<const reco::BasicCluster*>(&*inBC);
    if (inbcd) {
      if (inBC->hitsAndFractions().at(0).first.subdetId() == EcalBarrel ||
	  inBC->hitsAndFractions().at(0).first.subdetId() == EcalEndcap   ) {
	outBasicCluster->SetTime(lazyTools.BasicClusterTime(*inbcd, event));
	outBasicCluster->SetSeedTime(lazyTools.BasicClusterSeedTime(*inbcd));
      }
    }

    // local crystal information

    // patch for now to protect against cluster with negative energy
    if (inBC->energy()<0) {
      std::cout << " WARNING - cluster energy is negative!"
		<< " Skipping crystal info for this cluster." << std::endl;
    }
    else {
  
      if      (inBC->hitsAndFractions().at(0).first.subdetId() == EcalBarrel) {
        float etacry, phicry, thetatilt, phitilt;
        int ieta, iphi;
        local.localCoordsEB(*inBC,setup,etacry,phicry,ieta,iphi,thetatilt,phitilt);
        outBasicCluster->SetEtaCry(etacry);
        outBasicCluster->SetPhiCry(phicry);
        outBasicCluster->SetIEta(ieta);
        outBasicCluster->SetIPhi(iphi);
        outBasicCluster->SetThetaAxis(thetatilt);
        outBasicCluster->SetPhiAxis(phitilt);
  
        EBDetId edet(inBC->hitsAndFractions().at(0).first);
  
        outBasicCluster->SetSeedIEta(edet.ieta());
        outBasicCluster->SetSeedIPhi(edet.iphi());
      }
      else if (inBC->hitsAndFractions().at(0).first.subdetId() == EcalEndcap) {
        float xcry, ycry, thetatilt, phitilt;
        int ix, iy;
        local.localCoordsEE(*inBC,setup,xcry,ycry,ix,iy,thetatilt,phitilt);
        outBasicCluster->SetXCry(xcry);
        outBasicCluster->SetYCry(ycry);
        outBasicCluster->SetIX(ix);
        outBasicCluster->SetIY(iy);
        outBasicCluster->SetThetaAxis(thetatilt);
        outBasicCluster->SetPhiAxis(phitilt);
  
        EEDetId edet(inBC->hitsAndFractions().at(0).first);

        outBasicCluster->SetSeedIX(edet.ix());
        outBasicCluster->SetSeedIY(edet.iy());
      }
      else {                             // this is for clusters in the preshower
        outBasicCluster->SetXCry(-99);
        outBasicCluster->SetYCry(-99);
        outBasicCluster->SetIX(-99);
        outBasicCluster->SetIY(-99);
        outBasicCluster->SetThetaAxis(-99);
        outBasicCluster->SetPhiAxis(-99);
        outBasicCluster->SetSeedIX(-99);
        outBasicCluster->SetSeedIY(-99);
      }
    }
  
    // add basic clusters to the map
    reco::CaloClusterPtr thePtr(hBasicClusterProduct, inBC-inBasicClusters.begin());
    basicClusterMap_->Add(thePtr, outBasicCluster);
  }
  basicClusters_->Trim();
}

DEFINE_MITHEP_TREEFILLER(FillerBasicClusters);
