// $Id: Exp $

#include "MitProd/TreeFiller/interface/FillerSuperClusters.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/EgammaReco/interface/SuperClusterFwd.h"
#include "DataFormats/EgammaReco/interface/SuperCluster.h"
#include "MitAna/DataTree/interface/BasicCluster.h"
#include "MitAna/DataTree/interface/SuperCluster.h"
#include "MitAna/DataTree/interface/Names.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerSuperClusters::FillerSuperClusters(const ParameterSet &cfg, const char *name, bool active) : 
  BaseFiller(cfg,name,active),
  edmName_(Conf().getUntrackedParameter<string>("edmName","hybridSuperClusters")),
  mitName_(Conf().getUntrackedParameter<string>("mitName","SuperClusters")),
  basicClusterMapName_(Conf().getUntrackedParameter<string>("basicClusterMapName", 
                                                            "BasicClusterMap")),
  superClusterMapName_(Conf().getUntrackedParameter<string>("superClusterMapName", 
                                                            "SuperClusterMap")),
  basicClusterMap_(0),
  superClusters_(new mithep::SuperClusterArr(25)),
  superClusterMap_(new mithep::SuperClusterMap)
{
  // Constructor.
}

//--------------------------------------------------------------------------------------------------
FillerSuperClusters::~FillerSuperClusters()
{
  // Destructor.

  delete superClusters_;
  delete superClusterMap_;
}

//--------------------------------------------------------------------------------------------------
void FillerSuperClusters::BookDataBlock(TreeWriter &tws)
{
  // Load the BasicClusters and Add SuperClusters branch and the SuperClusterMap to tree. 
  tws.AddBranch(mitName_.c_str(),&superClusters_);
  if (!basicClusterMapName_.empty())
    basicClusterMap_ = OS()->get<BasicClusterMap>(basicClusterMapName_.c_str());
  
  OS()->add<SuperClusterMap>(superClusterMap_,superClusterMapName_.c_str());
  OS()->add<SuperClusterArr>(superClusters_,mitName_.c_str());
}

//--------------------------------------------------------------------------------------------------
void FillerSuperClusters::FillDataBlock(const edm::Event      &event, 
                                   const edm::EventSetup &setup)
{
  //Fill The SuperCluster DataBlock
  superClusters_->Reset();
  superClusterMap_->Reset();

  Handle<reco::SuperClusterCollection> hSuperClusterProduct;
  GetProduct(edmName_, hSuperClusterProduct, event);
  superClusterMap_->SetEdmProductId(hSuperClusterProduct.id().id());
  const reco::SuperClusterCollection inSuperClusters = *(hSuperClusterProduct.product());  

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

    //set the seed
    if (basicClusterMap_ && inSC->seed().isNonnull())
      outSC->SetSeed(basicClusterMap_->GetMit(inSC->seed()));
    
    //Add Basic Clusters that belong to this super cluster
    for(reco::basicCluster_iterator bc = inSC->clustersBegin(); bc != inSC->clustersEnd(); ++bc) {
      if (basicClusterMap_ && bc->isNonnull())
	outSC->AddCluster(basicClusterMap_->GetMit(*bc));
    }

    //Add super cluster to the map
    reco::SuperClusterRef theRef(hSuperClusterProduct, inSC-inSuperClusters.begin());
    superClusterMap_->Add(theRef, outSC);

  }

  superClusters_->Trim();
}
