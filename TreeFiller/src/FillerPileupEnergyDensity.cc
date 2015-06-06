#include "MitProd/TreeFiller/interface/FillerPileupEnergyDensity.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/Common/interface/Handle.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataTree/interface/PileupEnergyDensityCol.h"
#include "MitProd/ObjectService/interface/ObjectService.h"

mithep::FillerPileupEnergyDensity::FillerPileupEnergyDensity(edm::ParameterSet const& cfg, edm::ConsumesCollector& collector, mithep::ObjectService* os, char const* name, bool active/* = true*/) :
  BaseFiller(cfg, os, name, active),
  edmToken_{ // be careful with the ordering!
  GetToken<double>(collector, "edmName", ""),
    GetToken<double>(collector, "edmNameLowEta", ""),
    GetToken<double>(collector, "edmNameRandom", ""),
    GetToken<double>(collector, "edmNameRandomLowEta", ""),
    GetToken<double>(collector, "edmNameFixedGridAll", ""),
    GetToken<double>(collector, "edmNameFixedGridFastjetAll", ""),
    GetToken<double>(collector, "edmNameKt6CaloJets", ""),
    GetToken<double>(collector, "edmNameKt6CaloJetsCentral", ""),
    GetToken<double>(collector, "edmNameKt6PFJets", ""),
    GetToken<double>(collector, "edmNameKt6PFJetsCentralChargedPileUp", ""),
    GetToken<double>(collector, "edmNameKt6PFJetsCentralNeutral", ""),
    GetToken<double>(collector, "edmNameKt6PFJetsCentralNeutralTight", "")
  },
  mitName_(Conf().getUntrackedParameter<std::string>("mitName", mithep::Names::gkPileupEnergyDensityBrn)),
  rhos_(new mithep::PileupEnergyDensityArr)
{
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

  unsigned const nAlgos = mithep::PileupEnergyDensity::nAlgos;

  mithep::PileupEnergyDensity* rho = rhos_->AddNew();

  for (unsigned iA = 0; iA != nAlgos; ++iA) {
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
