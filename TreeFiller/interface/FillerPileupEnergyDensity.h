//--------------------------------------------------------------------------------------------------
// $Id: FillerPileupEnergyDensity.h,v 1.4 2011/06/15 20:04:14 bendavid Exp $
//
// FillerMet
//
// Implementation of a filler to fill Pileup Energy Density
//
// Authors: A.Levin, J.Bendavid, C.Paus, C.Loizides
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
      FillerPileupEnergyDensity(const edm::ParameterSet &cfg, edm::ConsumesCollector&, ObjectService*, const char *name, bool active=1);
      ~FillerPileupEnergyDensity();

      void                  BookDataBlock(TreeWriter &tws);
      void                  FillDataBlock(const edm::Event &e, const edm::EventSetup &es);

    private:
      edm::EDGetTokenT<double> edmToken_;                  //edm names of various rho collections
      edm::EDGetTokenT<double> edmNameLowEtaToken_;            //  ..
      edm::EDGetTokenT<double> edmNameRandomToken_;
      edm::EDGetTokenT<double> edmNameRandomLowEtaToken_;
      edm::EDGetTokenT<double> edmNameFixedGridAllToken_;
      edm::EDGetTokenT<double> edmNameFixedGridFastjetAllToken_;
      edm::EDGetTokenT<double> edmNameKt6CaloJetsToken_;
      edm::EDGetTokenT<double> edmNameKt6CaloJetsCentralToken_;
      edm::EDGetTokenT<double> edmNameKt6PFJetsToken_;
      edm::EDGetTokenT<double> edmNameKt6PFJetsCentralChargedPileUpToken_;
      edm::EDGetTokenT<double> edmNameKt6PFJetsCentralNeutralToken_;
      edm::EDGetTokenT<double> edmNameKt6PFJetsCentralNeutralTightToken_;

      std::string           mitName_;                  //mit name of PileupEnergyDensitys

      mithep::PileupEnergyDensityArr *rhos_;           //array of PileupEnergyDensitys
  };
}
#endif
