#include "MitProd/TreeFiller/interface/FillerPileupEnergyDensity.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/Common/interface/Handle.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataTree/interface/PileupEnergyDensityCol.h"
#include "MitProd/ObjectService/interface/ObjectService.h"

mithep::FillerPileupEnergyDensity::FillerPileupEnergyDensity(edm::ParameterSet const& cfg, edm::ConsumesCollector& collector, mithep::ObjectService* os, char const* name, bool active/* = true*/) :
  BaseFiller(cfg, os, name, active),
  edmToken_{},
  mitName_(cfg.getUntrackedParameter<std::string>("mitName", mithep::Names::gkPileupEnergyDensityBrn)),
  rhos_(new mithep::PileupEnergyDensityArr)
{
  for (unsigned iA = 0; iA != mithep::PileupEnergyDensity::nAlgos; ++iA)
    edmToken_[iA] = GetToken<double>(collector, cfg, std::string("edmName") + mithep::PileupEnergyDensity::AlgoName(iA), false);
}

mithep::FillerPileupEnergyDensity::~FillerPileupEnergyDensity()
{
  delete rhos_;
}

void
mithep::FillerPileupEnergyDensity::BookDataBlock(TreeWriter& tws)
{
  // Add pileup branch to tree.
  tws.AddBranch(mitName_, &rhos_);
  OS()->add(rhos_, mitName_);
}

void 
mithep::FillerPileupEnergyDensity::FillDataBlock(edm::Event const& event,
                                                 edm::EventSetup const&)
{
  // Fill missing energy from edm collection into our collection.

  rhos_->Delete();

  mithep::PileupEnergyDensity* rho = rhos_->AddNew();

  for (unsigned iA = 0; iA != mithep::PileupEnergyDensity::nAlgos; ++iA) {
    if (edmToken_[iA].isUninitialized()) {
      rho->SetRho(-9999., iA);
      continue;
    }

    edm::Handle<double> hRho;
    GetProduct(edmToken_[iA], hRho, event);
    rho->SetRho(*hRho, iA);
  }

  rhos_->Trim();
}

DEFINE_MITHEP_TREEFILLER(FillerPileupEnergyDensity);
