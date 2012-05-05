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
      FillerPileupEnergyDensity(const edm::ParameterSet &cfg, const char *name, bool active=1);
      ~FillerPileupEnergyDensity();

      void                  BookDataBlock(TreeWriter &tws);
      void                  FillDataBlock(const edm::Event &e, const edm::EventSetup &es);

    private:
      edm::InputTag         edmName_;                  //edm names of various rho collections
      edm::InputTag         edmNameLowEta_;            //  ..
      edm::InputTag         edmNameRandom_;
      edm::InputTag         edmNameRandomLowEta_;
      edm::InputTag         edmNameFixedGridAll_;
      edm::InputTag         edmNameFixedGridFastjetAll_;
      edm::InputTag         edmNameKt6CaloJets_;
      edm::InputTag         edmNameKt6CaloJetsCentral_;
      edm::InputTag         edmNameKt6PFJets_;
      edm::InputTag         edmNameKt6PFJetsCentralChargedPileUp_;
      edm::InputTag         edmNameKt6PFJetsCentralNeutral_;
      edm::InputTag         edmNameKt6PFJetsCentralNeutralTight_;

      std::string           mitName_;                  //mit name of PileupEnergyDensitys

      mithep::PileupEnergyDensityArr *rhos_;           //array of PileupEnergyDensitys
  };
}
#endif
