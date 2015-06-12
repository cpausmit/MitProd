//--------------------------------------------------------------------------------------------------
// FillerPileupEnergyDensity
//
// Implementation of a filler to fill Pileup Energy Density
//
// Authors: A.Levin, J.Bendavid, C.Paus, C.Loizides, Y.Iiyama
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERPILEUPENERGYDENSITY_H
#define MITPROD_TREEFILLER_FILLERPILEUPENERGYDENSITY_H

#include "MitAna/DataTree/interface/PileupEnergyDensityFwd.h"
#include "MitAna/DataTree/interface/PileupEnergyDensity.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"

namespace mithep
{
  class FillerPileupEnergyDensity : public BaseFiller
  {
  public:
    FillerPileupEnergyDensity(edm::ParameterSet const&, edm::ConsumesCollector&, ObjectService*, char const*, bool = true);
    ~FillerPileupEnergyDensity();

    void BookDataBlock(TreeWriter &) override;
    void FillDataBlock(edm::Event const&, edm::EventSetup const&) override;

  private:
    edm::EDGetTokenT<double> edmToken_[mithep::PileupEnergyDensity::nAlgos]; //edm tokens of various rho values

    std::string mitName_; //mit name of PileupEnergyDensities

    mithep::PileupEnergyDensityArr* rhos_; //array of PileupEnergyDensitys
  };
}
#endif
