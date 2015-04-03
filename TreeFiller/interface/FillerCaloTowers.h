//--------------------------------------------------------------------------------------------------
// $Id: FillerCaloTowers.h,v 1.9 2010/03/18 20:21:00 bendavid Exp $
//
// FillerCaloTowers
//
// Implementation of a filler to fill EDM CaloTowers into our mithep::CaloTower data structure.
//
// Authors: S.Xie
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERCALOTOWERS_H
#define MITPROD_TREEFILLER_FILLERCALOTOWERS_H

#include "MitAna/DataTree/interface/CaloTowerFwd.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"

#include "DataFormats/CaloTowers/interface/CaloTowerFwd.h"

namespace mithep 
{
  class FillerCaloTowers : public BaseFiller
  {  
    public:
      FillerCaloTowers(const edm::ParameterSet &cfg, edm::ConsumesCollector&, ObjectService*, const char *name, bool active=1);
      ~FillerCaloTowers();

      void                   BookDataBlock(TreeWriter &tws);
      void                   FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
  
    private:
      edm::EDGetTokenT<CaloTowerCollection> edmToken_;           //edm name of CaloTowers
      std::string            mitName_;           //name of CaloTowers
      std::string            caloTowerMapName_;  //name of export map
      std::string            caloTowerDetIdMapName_;  //name of export map      
      mithep::CaloTowerArr  *caloTowers_;        //array of CaloTowers
      mithep::CaloTowerMap  *caloTowerMap_;      //map wrt CaloTowers
      mithep::CaloTowerDetIdMap *caloTowerDetIdMap_; //map wrt CaloTower DetId's
  };
}
#endif
