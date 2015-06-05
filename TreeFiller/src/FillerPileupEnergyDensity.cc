#include "MitProd/TreeFiller/interface/FillerPileupEnergyDensity.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/Common/interface/Handle.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataTree/interface/PileupEnergyDensityCol.h"
#include "MitProd/ObjectService/interface/ObjectService.h"

mithep::FillerPileupEnergyDensity::FillerPileupEnergyDensity(edm::ParameterSet const& cfg, edm::ConsumesCollector& collector, mithep::ObjectService* os, char const* name, bool active/* = true*/) :
  BaseFiller(cfg, os, name, active),
  edmToken_{ // be careful with the ordering!
    GetToken<double>(collector, "edmName"),
    GetToken<double>(collector, "edmNameLowEta"),
    GetToken<double>(collector, "edmNameRandom"),
    GetToken<double>(collector, "edmNameRandomLowEta"),
    GetToken<double>(collector, "edmNameFixedGridAll"),
    GetToken<double>(collector, "edmNameFixedGridFastjetAll"),
    GetToken<double>(collector, "edmNameKt6CaloJets"),
    GetToken<double>(collector, "edmNameKt6CaloJetsCentral"),
    GetToken<double>(collector, "edmNameKt6PFJets"),
    GetToken<double>(collector, "edmNameKt6PFJetsCentralChargedPileUp"),
    GetToken<double>(collector, "edmNameKt6PFJetsCentralNeutral"),
    GetToken<double>(collector, "edmNameKt6PFJetsCentralNeutralTight")
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

  edm::Handle<double> hRho[nTypes];
  for (unsigned iT = 0; iT != nTypes; ++iT)
    GetProduct(edmToken_[iT], hRho[iT], event);

  mithep::PileupEnergyDensity* rho = rhos_->AddNew();

  rho->SetRho(*hRho[kHighEta]);
  rho->SetRhoLowEta(*hRho[kLowEta]);
  rho->SetRhoRandom(*hRho[kRandom]);
  rho->SetRhoRandomLowEta(*hRho[kRandomLowEta]);
  rho->SetRhoFixedGridAll(*hRho[kFixedGridAll]);
  rho->SetRhoFixedGridFastjetAll(*hRho[kFixedGridFastjetAll]);
  rho->SetRhoKt6CaloJets(*hRho[kKt6CaloJets]);
  rho->SetRhoKt6CaloJetsCentral(*hRho[kKt6CaloJetsCentral]);
  rho->SetRhoKt6PFJets(*hRho[kKt6PFJets]);
  rho->SetRhoKt6PFJetsCentralChargedPileUp(*hRho[kKt6PFJetsCentralChargedPileUp]);
  rho->SetRhoKt6PFJetsCentralNeutral(*hRho[kKt6PFJetsCentralNeutral]);
  rho->SetRhoKt6PFJetsCentralNeutralTight(*hRho[kKt6PFJetsCentralNeutralTight]);

  rhos_->Trim();
}
