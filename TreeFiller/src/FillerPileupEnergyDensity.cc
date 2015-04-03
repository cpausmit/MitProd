#include "MitProd/TreeFiller/interface/FillerPileupEnergyDensity.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/Common/interface/Handle.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataTree/interface/PileupEnergyDensityCol.h"
#include "MitProd/ObjectService/interface/ObjectService.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerPileupEnergyDensity::FillerPileupEnergyDensity(const ParameterSet &cfg, edm::ConsumesCollector& collector, ObjectService* os, const char *name, bool active) :
  BaseFiller                           (cfg,os,name,active),
  edmToken_(GetToken<double>(collector, "edmName")),
  edmNameLowEtaToken_(GetToken<double>(collector, "edmNameLowEta")),
  edmNameRandomToken_(GetToken<double>(collector, "edmNameRandom")),
  edmNameRandomLowEtaToken_(GetToken<double>(collector, "edmNameRandomLowEta")),
  edmNameFixedGridAllToken_(GetToken<double>(collector, "edmNameFixedGridAll")),
  edmNameFixedGridFastjetAllToken_(GetToken<double>(collector, "edmNameFixedGridFastjetAll")),
  edmNameKt6CaloJetsToken_(GetToken<double>(collector, "edmNameKt6CaloJets")),
  edmNameKt6CaloJetsCentralToken_(GetToken<double>(collector, "edmNameKt6CaloJetsCentral")),
  edmNameKt6PFJetsToken_(GetToken<double>(collector, "edmNameKt6PFJets")),
  edmNameKt6PFJetsCentralChargedPileUpToken_(GetToken<double>(collector, "edmNameKt6PFJetsCentralChargedPileUp")),
  edmNameKt6PFJetsCentralNeutralToken_(GetToken<double>(collector, "edmNameKt6PFJetsCentralNeutral")),
  edmNameKt6PFJetsCentralNeutralTightToken_(GetToken<double>(collector, "edmNameKt6PFJetsCentralNeutralTight")),
  mitName_                             (Conf().getUntrackedParameter<string>("mitName",Names::gkPileupEnergyDensityBrn)),
  rhos_                                (new mithep::PileupEnergyDensityArr)
{
  // Constructor.
}

//--------------------------------------------------------------------------------------------------
FillerPileupEnergyDensity::~FillerPileupEnergyDensity()
{
  // Destructor.

  delete rhos_;
}

//--------------------------------------------------------------------------------------------------
void FillerPileupEnergyDensity::BookDataBlock(TreeWriter &tws)
{
  // Add pileup branch to tree.
  tws.AddBranch(mitName_,&rhos_);
  OS()->add<mithep::PileupEnergyDensityArr>(rhos_,mitName_);
}

//--------------------------------------------------------------------------------------------------
void FillerPileupEnergyDensity::FillDataBlock(const edm::Event      &event,
                                              const edm::EventSetup &setup)
{
  // Fill missing energy from edm collection into our collection.

  rhos_->Delete();

  Handle<double> hRho;
  GetProduct(edmToken_, hRho, event);

  Handle<double> hRhoLowEta;
  GetProduct(edmNameLowEtaToken_, hRhoLowEta, event);

  Handle<double> hRhoRandom;
  GetProduct(edmNameRandomToken_, hRhoRandom, event);

  Handle<double> hRhoRandomLowEta;
  GetProduct(edmNameRandomLowEtaToken_, hRhoRandomLowEta, event);

  Handle<double> hRhoFixedGridAll;
  GetProduct(edmNameFixedGridAllToken_, hRhoFixedGridAll, event);

  Handle<double> hRhoFixedGridFastjetAll;
  GetProduct(edmNameFixedGridFastjetAllToken_, hRhoFixedGridFastjetAll, event);

  Handle<double> hRhoKt6CaloJets;
  GetProduct(edmNameKt6CaloJetsToken_, hRhoKt6CaloJets, event);

  Handle<double> hRhoKt6CaloJetsCentral;
  GetProduct(edmNameKt6CaloJetsCentralToken_, hRhoKt6CaloJetsCentral, event);

  Handle<double> hRhoKt6PFJets;
  GetProduct(edmNameKt6PFJetsToken_, hRhoKt6PFJets, event);

  Handle<double> hRhoKt6PFJetsCentralChargedPileUp;
  GetProduct(edmNameKt6PFJetsCentralChargedPileUpToken_, hRhoKt6PFJetsCentralChargedPileUp, event);

  Handle<double> hRhoKt6PFJetsCentralNeutral;
  GetProduct(edmNameKt6PFJetsCentralNeutralToken_, hRhoKt6PFJetsCentralNeutral, event);

  Handle<double> hRhoKt6PFJetsCentralNeutralTight;
  GetProduct(edmNameKt6PFJetsCentralNeutralTightToken_, hRhoKt6PFJetsCentralNeutralTight, event);

  mithep::PileupEnergyDensity *rho = rhos_->AddNew();

  rho->SetRho(*hRho);
  rho->SetRhoLowEta(*hRhoLowEta);
  rho->SetRhoRandom(*hRhoRandom);
  rho->SetRhoRandomLowEta(*hRhoRandomLowEta);

  if (hRhoFixedGridAll.isValid())
    rho->SetRhoFixedGridAll(*hRhoFixedGridAll);
  else
    rho->SetRhoFixedGridAll(-9999);

  if (hRhoFixedGridFastjetAll.isValid())
    rho->SetRhoFixedGridFastjetAll(*hRhoFixedGridFastjetAll);
  else
    rho->SetRhoFixedGridFastjetAll(-9999);

  rho->SetRhoKt6CaloJets                  (*hRhoKt6CaloJets);
  rho->SetRhoKt6CaloJetsCentral           (*hRhoKt6CaloJetsCentral);
  rho->SetRhoKt6PFJets                    (*hRhoKt6PFJets);
  rho->SetRhoKt6PFJetsCentralChargedPileUp(*hRhoKt6PFJetsCentralChargedPileUp);
  rho->SetRhoKt6PFJetsCentralNeutral      (*hRhoKt6PFJetsCentralNeutral);
  rho->SetRhoKt6PFJetsCentralNeutralTight (*hRhoKt6PFJetsCentralNeutralTight);

  rhos_->Trim();
}
