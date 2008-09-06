// $Id: FillerCaloTowers.cc,v 1.1 2008/08/08 11:12:38 sixie Exp $

#include "MitProd/TreeFiller/interface/FillerCaloTowers.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "MitAna/DataTree/interface/Names.h"
#include "DataFormats/CaloTowers/interface/CaloTower.h"
#include "DataFormats/CaloTowers/interface/CaloTowerFwd.h"
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/Records/interface/IdealGeometryRecord.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerCaloTowers::FillerCaloTowers(const ParameterSet &cfg, const char *name, bool active) : 
  BaseFiller(cfg,name,active),
  edmName_(Conf().getUntrackedParameter<string>("edmName","towerMaker")),
  mitName_(Conf().getUntrackedParameter<string>("mitName","CaloTowers")),
  caloTowerMapName_(Conf().getUntrackedParameter<string>("caloTowerMapName",
                                                            "CaloTowerClusterMap")),
  caloTowers_(new mithep::CaloTowerArr(1000))
{
  // Constructor.
}

//--------------------------------------------------------------------------------------------------
FillerCaloTowers::~FillerCaloTowers()
{
  // Destructor.

  delete caloTowers_;
}

//--------------------------------------------------------------------------------------------------
void FillerCaloTowers::BookDataBlock(TreeWriter &tws)
{
  // Add CaloTower branch to tree.

  tws.AddBranch(mitName_.c_str(),&caloTowers_);
  OS()->add<CaloTowerArr>(caloTowers_,mitName_.c_str());

}

//--------------------------------------------------------------------------------------------------
void FillerCaloTowers::FillDataBlock(const edm::Event      &event, 
                                   const edm::EventSetup &setup)
{
  // Fill the CaloTower Data Block

  caloTowers_->Reset();

  Handle<CaloTowerCollection> hCaloTowerProduct;
  GetProduct(edmName_, hCaloTowerProduct, event);
  const CaloTowerCollection inCaloTowers = *(hCaloTowerProduct.product());  

  for (CaloTowerCollection::const_iterator inCaloTower = inCaloTowers.begin(); 
       inCaloTower != inCaloTowers.end(); ++inCaloTower) {
    
    mithep::CaloTower *outCaloTower = caloTowers_->Allocate();
    new (outCaloTower) mithep::CaloTower();
       
    outCaloTower->SetMomentum(inCaloTower->momentum().x(),inCaloTower->momentum().y(),
                              inCaloTower->momentum().z(), inCaloTower->energy());
    outCaloTower->SetEmEnergy(inCaloTower->emEnergy());
    outCaloTower->SetHadEnergy(inCaloTower->hadEnergy());
    outCaloTower->SetOuterEnergy(inCaloTower->outerEnergy());
    outCaloTower->SetEmLvl1(inCaloTower->emLvl1());
    outCaloTower->SetHadLvl1(inCaloTower->hadLv11());

  }

  caloTowers_->Trim();

}
