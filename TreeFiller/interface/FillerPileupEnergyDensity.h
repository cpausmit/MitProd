//--------------------------------------------------------------------------------------------------
// $Id: FillerPileupEnergyDensity.h,v 1.3 2011/03/23 19:03:46 mzanetti Exp $
//
// FillerMet
//
// Implementation of a filler to fill Pileup Energy Density
//
// Authors: C.Loizides
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERPILEUPENERGYDENSITY_H
#define MITPROD_TREEFILLER_FILLERPILEUPENERGYDENSITY_H

#include "MitAna/DataTree/interface/PileupEnergyDensityFwd.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"

namespace mithep 
{
  class FillerPileupEnergyDensity : public BaseFiller
  {  
    public:
      FillerPileupEnergyDensity(const edm::ParameterSet &cfg, const char *name, bool active=1);
      ~FillerPileupEnergyDensity();

      void                  BookDataBlock(TreeWriter &tws);
      void 	            FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
  
    private:
      edm::InputTag               edmName_;    //edm name of rho collection
      edm::InputTag               edmNameLowEta_;    //edm name of high eta rho collection
      edm::InputTag               edmNameRandom_;    //edm name of high eta rho collection
      edm::InputTag               edmNameRandomLowEta_;    //edm name of high eta rho collection
    //std::string                 edmName_;    //edm name of met collection
      std::string                 mitName_;    //mit name of PileupEnergyDensitys

      mithep::PileupEnergyDensityArr       *rhos_;       //array of PileupEnergyDensitys
  };
}
#endif
