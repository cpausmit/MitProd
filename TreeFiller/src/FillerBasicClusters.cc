// $Id: FillerBasicClusters.cc,v 1.5 2009/03/15 11:20:41 loizides Exp $

#include "MitProd/TreeFiller/interface/FillerBasicClusters.h"
#include "DataFormats/EgammaReco/interface/BasicClusterFwd.h"
#include "DataFormats/EgammaReco/interface/BasicCluster.h"
#include "MitAna/DataTree/interface/BasicClusterCol.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitProd/ObjectService/interface/ObjectService.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerBasicClusters::FillerBasicClusters(const ParameterSet &cfg, const char *name, bool active) : 
  BaseFiller(cfg,name,active),
  edmName_(Conf().getUntrackedParameter<string>("edmName","hybridSuperClusters")),
  mitName_(Conf().getUntrackedParameter<string>("mitName","BasicClusters")),
  basicClusterMapName_(Conf().getUntrackedParameter<string>("basicClusterMapName",
                                                            "BasicClusterMap")),
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

  Handle<reco::BasicClusterCollection> hBasicClusterProduct;
  GetProduct(edmName_, hBasicClusterProduct, event);
  basicClusterMap_->SetEdmProductId(hBasicClusterProduct.id().id());
  const reco::BasicClusterCollection inBasicClusters = *(hBasicClusterProduct.product());  

  // loop through all basic clusters
  for (reco::BasicClusterCollection::const_iterator inBC = inBasicClusters.begin(); 
       inBC != inBasicClusters.end(); ++inBC) {

    mithep::BasicCluster *outBasicCluster = basicClusters_->Allocate();
    new (outBasicCluster) mithep::BasicCluster();

    outBasicCluster->SetXYZ(inBC->x(),inBC->y(),inBC->z());
    outBasicCluster->SetEnergy(inBC->energy());   

    //add basic clusters to the map
    reco::BasicClusterRef theRef(hBasicClusterProduct, inBC-inBasicClusters.begin());
    basicClusterMap_->Add(theRef, outBasicCluster);
  }
  basicClusters_->Trim();
}
