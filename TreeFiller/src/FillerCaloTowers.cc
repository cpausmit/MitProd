// $Id: FillerCaloTowers.cc,v 1.11 2009/06/15 15:00:25 loizides Exp $

#include "MitProd/TreeFiller/interface/FillerCaloTowers.h"
#include "DataFormats/CaloTowers/interface/CaloTower.h"
#include "DataFormats/CaloTowers/interface/CaloTowerFwd.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/Records/interface/IdealGeometryRecord.h"
#include "MitAna/DataTree/interface/CaloTowerCol.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitProd/ObjectService/interface/ObjectService.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerCaloTowers::FillerCaloTowers(const ParameterSet &cfg, const char *name, bool active) : 
  BaseFiller(cfg,name,active),
  edmName_(Conf().getUntrackedParameter<string>("edmName","towerMaker")),
  mitName_(Conf().getUntrackedParameter<string>("mitName","CaloTowers")),
  caloTowerMapName_(Conf().getUntrackedParameter<string>("caloTowerMapName", "CaloTowerMap")),
  caloTowers_(new mithep::CaloTowerArr(1000)),
  caloTowerMap_(new mithep::CaloTowerMap)
{
  // Constructor.
}

//--------------------------------------------------------------------------------------------------
FillerCaloTowers::~FillerCaloTowers()
{
  // Destructor.

  delete caloTowers_;
  delete caloTowerMap_;
}

//--------------------------------------------------------------------------------------------------
void FillerCaloTowers::BookDataBlock(TreeWriter &tws)
{
  // Add CaloTower branch to tree.

  Int_t brsize = tws.GetDefaultBrSize();
  if (brsize<32*1024)
    brsize=32*1024;

  tws.AddBranch(mitName_,&caloTowers_,brsize);
  OS()->add<CaloTowerArr>(caloTowers_,mitName_);

  if (!caloTowerMapName_.empty()) {
    caloTowerMap_->SetBrName(mitName_);
    OS()->add<CaloTowerMap>(caloTowerMap_,caloTowerMapName_);
  }
}

//--------------------------------------------------------------------------------------------------
void FillerCaloTowers::FillDataBlock(const edm::Event      &event, 
                                     const edm::EventSetup &setup)
{
  // Fill the CaloTower info.

  caloTowers_->Delete();
  caloTowerMap_->Reset();

  Handle<CaloTowerCollection> hCaloTowerProduct;
  GetProduct(edmName_, hCaloTowerProduct, event);
  const CaloTowerCollection inCaloTowers = *(hCaloTowerProduct.product());  

  for (CaloTowerCollection::const_iterator inCaloTower = inCaloTowers.begin(); 
       inCaloTower != inCaloTowers.end(); ++inCaloTower) {
    
    mithep::CaloTower *outCaloTower = caloTowers_->Allocate();
    new (outCaloTower) mithep::CaloTower();
       
    double mass = inCaloTower->mass();
    double deltaPos = (inCaloTower->hadPosition() - inCaloTower->emPosition()).mag();
    double deltaE = inCaloTower->energy() - inCaloTower->emEnergy() - inCaloTower->hadEnergy();
    
//     if ( mass > 1e-3)
//       throw edm::Exception(edm::errors::Configuration, "FillerCaloTowers::FillDataBlock()\n")
//          << "Error! reco::CaloTower mass = " << mass 
//          << ", not massless as assumed by mithep::CaloTower," << std::endl;
         
//     if ( deltaPos > 1e-3)
//       throw edm::Exception(edm::errors::Configuration, "FillerCaloTowers::FillDataBlock()\n")
//         << "Error! reco::CaloTower does not have identical HadPos and EmPos " 
//         << "as assumed by mithep::CaloTower, deltaPos = " << deltaPos << std::endl;
//          
    if ( deltaE > 1e-3)
      throw edm::Exception(edm::errors::Configuration, "FillerCaloTowers::FillDataBlock()\n")
        << "Error! reco::CaloTower default energy does not exclude HO " 
        << "as assumed by mithep::CaloTower, deltaE = " << deltaE << std::endl;
    
//     outCaloTower->SetEMPosition(inCaloTower->emPosition().x(),
//                                 inCaloTower->emPosition().y(),
//                                 inCaloTower->emPosition().z());
//     outCaloTower->SetHadPosition(inCaloTower->hadPosition().x(),
//                                 inCaloTower->hadPosition().y(),
//                                 inCaloTower->hadPosition().z());

    ThreeVector avgPos;
    ThreeVector emPos(inCaloTower->emPosition().x(),
                      inCaloTower->emPosition().y(),
                      inCaloTower->emPosition().z());
    ThreeVector hadPos(inCaloTower->hadPosition().x(),
                       inCaloTower->hadPosition().y(),
                       inCaloTower->hadPosition().z());
    double energyWeight = 0.0;
    if (inCaloTower->emEnergy()>0.0) {
      avgPos += inCaloTower->emEnergy()*emPos;
      energyWeight += inCaloTower->emEnergy();
    }
    if (inCaloTower->hadEnergy()>0.0) {
      avgPos += inCaloTower->hadEnergy()*hadPos;
      energyWeight += inCaloTower->hadEnergy();
    }
    if (energyWeight > 0.0)
      avgPos = (1.0/energyWeight)*avgPos;
    else
      avgPos = emPos;
      
    ThreeVectorC finalPos(avgPos.Rho(),inCaloTower->eta(),inCaloTower->phi());

    outCaloTower->SetPosition(finalPos.x(),finalPos.y(),finalPos.z());

    outCaloTower->SetEmEnergy(inCaloTower->emEnergy());
    outCaloTower->SetHadEnergy(inCaloTower->hadEnergy());
    outCaloTower->SetOuterEnergy(inCaloTower->outerEnergy());
    outCaloTower->SetMass(inCaloTower->mass());
    caloTowerMap_->Add(inCaloTower->id(),outCaloTower);
    
    if (verbose_>1) {
      printf("reco::CaloTower:   pt=%5f, eta=%5f, phi=%5f, mass=%5f\n",inCaloTower->pt(),inCaloTower->eta(),inCaloTower->phi(),inCaloTower->mass());
      printf("mithep::CaloTower: pt=%5f, eta=%5f, phi=%5f, mass=%5f\n", outCaloTower->Pt(),outCaloTower->Eta(),outCaloTower->Phi(),outCaloTower->Mass());
      printf("emPos : rho=%5f, eta=%5f, phi=%5f\n",emPos.Rho(),emPos.Eta(),emPos.Phi());
      printf("hadPos: rho=%5f, eta=%5f, phi=%5f\n",hadPos.Rho(),hadPos.Eta(),hadPos.Phi());
      printf("avgPos: rho=%5f, eta=%5f, phi=%5f\n",avgPos.Rho(),avgPos.Eta(),avgPos.Phi());
      printf("finPos: rho=%5f, eta=%5f, phi=%5f\n",finalPos.Rho(),finalPos.Eta(),finalPos.Phi());
    }

  }
  caloTowers_->Trim();
}
