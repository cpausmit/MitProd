// $Id: FillerBasicClusters.cc,v 1.20 2012/05/05 16:49:59 paus Exp $

#include "MitProd/TreeFiller/interface/FillerBasicClusters.h"
#include "DataFormats/CaloRecHit/interface/CaloClusterFwd.h"
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
#include "RecoEgamma/EgammaTools/interface/ggPFClusters.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerBasicClusters::FillerBasicClusters(const ParameterSet &cfg, const char *name, bool active) : 
  BaseFiller(cfg,name,active),
  edmName_(Conf().getUntrackedParameter<string>("edmName","hybridSuperClusters")),
  mitName_(Conf().getUntrackedParameter<string>("mitName","BasicClusters")),
  barrelEcalRecHitName_(Conf().getUntrackedParameter<string>("barrelEcalRecHitName","")),
  endcapEcalRecHitName_(Conf().getUntrackedParameter<string>("endcapEcalRecHitName","")),
  basicClusterMapName_(Conf().getUntrackedParameter<string>("basicClusterMapName",
                                                            "BasicClusterMap")),
  pfClusters_(Conf().getUntrackedParameter<bool>("pfClusters",false)),
  basicClusters_(new mithep::BasicClusterArr(100)),
  basicClusterMap_(new mithep::BasicClusterMap)
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

  basicClusters_->Delete();
  basicClusterMap_->Reset();

  Handle<reco::CaloClusterCollection> hBasicClusterProduct;
  GetProduct(edmName_, hBasicClusterProduct, event);
  basicClusterMap_->SetEdmProductId(hBasicClusterProduct.id().id());
  const reco::CaloClusterCollection &inBasicClusters = *(hBasicClusterProduct.product());  

  edm::Handle< EcalRecHitCollection > pEBRecHits;
  event.getByLabel(barrelEcalRecHitName_, pEBRecHits );
  edm::Handle< EcalRecHitCollection > pEERecHits;
  event.getByLabel( endcapEcalRecHitName_, pEERecHits );
  
  edm::ESHandle<CaloGeometry> pGeometry;
  setup.get<CaloGeometryRecord>().get(pGeometry);
 
  const CaloSubdetectorGeometry *geometryEB = pGeometry->getSubdetectorGeometry(DetId::Ecal, EcalBarrel);
  const CaloSubdetectorGeometry *geometryEE = pGeometry->getSubdetectorGeometry(DetId::Ecal, EcalEndcap);

  EcalClusterLazyTools lazyTools(event, setup, edm::InputTag(barrelEcalRecHitName_), 
                                 edm::InputTag(endcapEcalRecHitName_));

  ggPFClusters pfclusters(pEBRecHits, pEERecHits, geometryEB, geometryEE);

  EcalClusterLocal local;                                 
                                 
  //loop over rechits and make map of energy values
  std::map<DetId,float> hitEnergies;

  for (EcalRecHitCollection::const_iterator it = pEBRecHits->begin(); it!=pEBRecHits->end(); ++it) {
    hitEnergies.insert(std::pair<DetId,float>(it->id(),it->energy()));
  }

  for (EcalRecHitCollection::const_iterator it = pEERecHits->begin(); it!=pEERecHits->end(); ++it) {
    hitEnergies.insert(std::pair<DetId,float>(it->id(),it->energy()));
  }

  // loop through all basic clusters
  for (reco::CaloClusterCollection::const_iterator inBC = inBasicClusters.begin(); 
       inBC != inBasicClusters.end(); ++inBC) {

    mithep::BasicCluster *outBasicCluster = basicClusters_->Allocate();
    new (outBasicCluster) mithep::BasicCluster();

    outBasicCluster->SetXYZ(inBC->x(),inBC->y(),inBC->z());
    outBasicCluster->SetEnergy(inBC->energy());   
    outBasicCluster->SetNHits(inBC->size());

    outBasicCluster->SetCovEtaPhi(lazyTools.covariances(*inBC)[1]);
    outBasicCluster->SetCoviEtaiEta(lazyTools.localCovariances(*inBC)[0]);
    outBasicCluster->SetCoviEtaiPhi(lazyTools.localCovariances(*inBC)[1]);
    outBasicCluster->SetCoviPhiiPhi(lazyTools.localCovariances(*inBC)[2]);
    outBasicCluster->SetZernike20(lazyTools.zernike20(*inBC));
    outBasicCluster->SetZernike42(lazyTools.zernike42(*inBC));
    outBasicCluster->SetEtaLat(lazyTools.lat(*inBC)[0]);
    outBasicCluster->SetPhiLat(lazyTools.lat(*inBC)[1]);
    outBasicCluster->SetLat(lazyTools.lat(*inBC)[2]);


    outBasicCluster->SetE1x3(lazyTools.e1x3(*inBC));
    outBasicCluster->SetE3x1(lazyTools.e3x1(*inBC));
    outBasicCluster->SetE1x5(lazyTools.e1x5(*inBC));
    outBasicCluster->SetE2x2(lazyTools.e2x2(*inBC));
    outBasicCluster->SetE3x2(lazyTools.e3x2(*inBC));
    outBasicCluster->SetE3x3(lazyTools.e3x3(*inBC));
    outBasicCluster->SetE4x4(lazyTools.e4x4(*inBC));
    outBasicCluster->SetE5x5(lazyTools.e5x5(*inBC));
    outBasicCluster->SetE2x5Right(lazyTools.e2x5Right(*inBC));
    outBasicCluster->SetE2x5Left(lazyTools.e2x5Left(*inBC));
    outBasicCluster->SetE2x5Top(lazyTools.e2x5Top(*inBC));
    outBasicCluster->SetE2x5Bottom(lazyTools.e2x5Bottom(*inBC));
    outBasicCluster->SetE2x5Max(lazyTools.e2x5Max(*inBC));
    outBasicCluster->SetELeft(lazyTools.eLeft(*inBC));
    outBasicCluster->SetERight(lazyTools.eRight(*inBC));
    outBasicCluster->SetETop(lazyTools.eTop(*inBC));
    outBasicCluster->SetEBottom(lazyTools.eBottom(*inBC));
    outBasicCluster->SetEMax(lazyTools.eMax(*inBC));
    outBasicCluster->SetE2nd(lazyTools.e2nd(*inBC));
    outBasicCluster->SetCovEtaEta(lazyTools.covariances(*inBC)[0]);
    outBasicCluster->SetCovPhiPhi(lazyTools.covariances(*inBC)[2]);

    //ecal timing information
    const reco::BasicCluster *inbcd = dynamic_cast<const reco::BasicCluster*>(&*inBC);

    if (inbcd) {
      outBasicCluster->SetTime(lazyTools.BasicClusterTime(*inbcd, event));
      outBasicCluster->SetSeedTime(lazyTools.BasicClusterSeedTime(*inbcd));
    }

    //local coordinates
    if (inBC->hitsAndFractions().at(0).first.subdetId()==EcalBarrel) {
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
    else {
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
    
    // add basic clusters to the map
    reco::CaloClusterPtr thePtr(hBasicClusterProduct, inBC-inBasicClusters.begin());
    basicClusterMap_->Add(thePtr, outBasicCluster);
  
  }
  basicClusters_->Trim();
}
