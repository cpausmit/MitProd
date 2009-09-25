// $Id: FillerSuperClusters.cc,v 1.7 2009/07/20 03:19:24 loizides Exp $

#include "MitProd/TreeFiller/interface/FillerSuperClusters.h"
#include "DataFormats/EgammaReco/interface/SuperClusterFwd.h"
#include "DataFormats/EgammaReco/interface/SuperCluster.h"
#include "MitAna/DataTree/interface/BasicCluster.h"
#include "MitAna/DataTree/interface/SuperClusterCol.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitProd/ObjectService/interface/ObjectService.h"

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
  superClusterIdMapName_(Conf().getUntrackedParameter<string>("superClusterIdMapName", 
                                                              "SuperClusterIdMap")),
  basicClusterMap_(0),
  superClusters_(new mithep::SuperClusterArr(25)),
  superClusterMap_(new mithep::SuperClusterMap),
  superClusterIdMap_(new mithep::SuperClusterIdMap)
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
void FillerSuperClusters::BookDataBlock(TreeWriter &tws, const edm::EventSetup &es)
{
  // Add super cluster branch to tree and get pointers to maps.

  tws.AddBranch(mitName_,&superClusters_);
  OS()->add<SuperClusterArr>(superClusters_,mitName_);

  if (!basicClusterMapName_.empty()) {
    basicClusterMap_ = OS()->get<BasicClusterMap>(basicClusterMapName_);
    if (basicClusterMap_)
      AddBranchDep(mitName_,basicClusterMap_->GetBrName());
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

    // set the seed
    if (basicClusterMap_ && inSC->seed().isNonnull())
      outSC->SetSeed(basicClusterMap_->GetMit(inSC->seed()));
    
    // add basic clusters that belong to this super cluster
    for(reco::CaloCluster_iterator bc = inSC->clustersBegin(); bc != inSC->clustersEnd(); ++bc) {
      if (basicClusterMap_ && bc->isNonnull())
	outSC->AddCluster(basicClusterMap_->GetMit(*bc));
    }

    // add super cluster to the map
    reco::SuperClusterRef theRef(hSuperClusterProduct, inSC-inSuperClusters.begin());
    superClusterMap_->Add(theRef, outSC);

    // add super cluster det ids to the id map
    const std::vector< std::pair<DetId, float> > &pairs = inSC->hitsAndFractions();
    for (std::vector< std::pair<DetId, float> >::const_iterator ipair = pairs.begin();
          ipair < pairs.end(); ++ipair) {

      const DetId &ihit = ipair->first;
      superClusterIdMap_->Add(ihit,outSC);
    }
  }
  superClusters_->Trim();
}
