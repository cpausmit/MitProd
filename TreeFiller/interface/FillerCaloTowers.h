//--------------------------------------------------------------------------------------------------
// $Id: FillerCaloTowers.h,v 1.3 2008/09/10 03:30:22 loizides Exp $
//
// FillerCaloTowers
//
// Implementation of a filler to fill EDM CaloTowers into our mithep::CaloTower data structure.
//
// Authors: S.Xie
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERCALOTOWERS_H
#define MITPROD_TREEFILLER_FILLERCALOTOWERS_H

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "MitAna/DataUtil/interface/TreeWriter.h"
#include "MitAna/DataTree/interface/Collections.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"

namespace mithep 
{
  class FillerCaloTowers : public BaseFiller
  {  
    public:
      FillerCaloTowers(const edm::ParameterSet &cfg, const char *name, bool active=1);
      ~FillerCaloTowers();

      void BookDataBlock(TreeWriter &tws);
      void FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
  
    private:
      std::string                        edmName_;           //edm name of CaloTowers
      std::string                        mitName_;           //name of CaloTowers in OAK
      std::string                        caloTowerMapName_;  //name of export map
      mithep::CaloTowerArr              *caloTowers_;        //array of CaloTowers
      mithep::CaloTowerMap              *caloTowerMap_;      //map wrt CaloTowers
  };
}
#endif
