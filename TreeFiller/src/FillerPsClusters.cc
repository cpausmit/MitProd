// $Id: FillerPsClusters.cc,v 1.1 2012/05/05 16:49:59 paus Exp $

#include "MitProd/TreeFiller/interface/FillerPsClusters.h"
#include "DataFormats/EgammaReco/interface/PreshowerCluster.h" 
#include "DataFormats/EgammaReco/interface/PreshowerClusterFwd.h" 
#include "DataFormats/CaloRecHit/interface/CaloClusterFwd.h"
#include "DataFormats/CaloRecHit/interface/CaloCluster.h"
#include "MitAna/DataTree/interface/PsClusterCol.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitProd/ObjectService/interface/ObjectService.h"
#include "RecoEcal/EgammaCoreTools/interface/EcalClusterLazyTools.h"
#include "RecoLocalCalo/EcalRecAlgos/interface/EcalSeverityLevelAlgo.h"
#include "DataFormats/EcalDetId/interface/EcalSubdetector.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerPsClusters::FillerPsClusters(const ParameterSet &cfg, const char *name, bool active) : 
  BaseFiller       (cfg,name,active),
  edmName_         (Conf().getUntrackedParameter<string>("edmName","hybridSuperClusters")),
  mitName_         (Conf().getUntrackedParameter<string>("mitName","PsClusters")),
  psClusterMapName_(Conf().getUntrackedParameter<string>("psClusterMapName",
							 "PsClusterMap")),
  psClusters_      (new mithep::PsClusterArr(100)),
  psClusterMap_    (new mithep::PsClusterMap)
{
  // Constructor.
}

//--------------------------------------------------------------------------------------------------
FillerPsClusters::~FillerPsClusters()
{
  // Destructor.

  delete psClusters_;
  delete psClusterMap_;
}

//--------------------------------------------------------------------------------------------------
void FillerPsClusters::BookDataBlock(TreeWriter &tws)
{
  // Add PsCluster branch and the PsClusterMap to tree.

  tws.AddBranch(mitName_,&psClusters_);
  OS()->add<PsClusterArr>(psClusters_,mitName_);

  if (!psClusterMapName_.empty()) {
    psClusterMap_->SetBrName(mitName_);
    OS()->add<PsClusterMap>(psClusterMap_,psClusterMapName_);
  }
}

//--------------------------------------------------------------------------------------------------
void FillerPsClusters::FillDataBlock(const edm::Event      &event, 
                                        const edm::EventSetup &setup)
{
  // Fill the PsCluster information into our structures.

  psClusters_->Delete();
  psClusterMap_->Reset();

  Handle<reco::PreshowerClusterCollection> hPsClusterProduct;
  GetProduct(edmName_, hPsClusterProduct, event);
  psClusterMap_->SetEdmProductId(hPsClusterProduct.id().id());
  const reco::PreshowerClusterCollection &inPsClusters = *(hPsClusterProduct.product());  

  // loop through all basic clusters
  for (reco::PreshowerClusterCollection::const_iterator inBC = inPsClusters.begin(); 
       inBC != inPsClusters.end(); ++inBC) {

    mithep::PsCluster *outPsCluster = psClusters_->Allocate();
    new (outPsCluster) mithep::PsCluster();

    outPsCluster->SetXYZ(inBC->x(),inBC->y(),inBC->z());
    outPsCluster->SetEnergy(inBC->energy());   
    outPsCluster->SetNHits(inBC->size());
    
    outPsCluster->SetPsPlane(inBC->plane());

    // add basic clusters to the map
    reco::CaloClusterPtr thePtr(hPsClusterProduct, inBC-inPsClusters.begin());
    psClusterMap_->Add(thePtr, outPsCluster);
  
  }
  psClusters_->Trim();
}
