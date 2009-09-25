//--------------------------------------------------------------------------------------------------
// $Id: FillerCaloTowers.h,v 1.7 2009/06/15 15:00:24 loizides Exp $
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

namespace mithep 
{
  class FillerCaloTowers : public BaseFiller
  {  
    public:
      FillerCaloTowers(const edm::ParameterSet &cfg, const char *name, bool active=1);
      ~FillerCaloTowers();

      void                   BookDataBlock(TreeWriter &tws, const edm::EventSetup &es);
      void                   FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
  
    private:
      std::string            edmName_;           //edm name of CaloTowers
      std::string            mitName_;           //name of CaloTowers
      std::string            caloTowerMapName_;  //name of export map
      mithep::CaloTowerArr  *caloTowers_;        //array of CaloTowers
      mithep::CaloTowerMap  *caloTowerMap_;      //map wrt CaloTowers
  };
}
#endif
