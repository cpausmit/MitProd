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
FillerPileupEnergyDensity::FillerPileupEnergyDensity(const ParameterSet &cfg, const char *name, bool active) :
  BaseFiller                           (cfg,name,active),
  edmName_                             (Conf().getUntrackedParameter<edm::InputTag>("edmName")),
  edmNameLowEta_                       (Conf().getUntrackedParameter<edm::InputTag>("edmNameLowEta")),
  edmNameRandom_                       (Conf().getUntrackedParameter<edm::InputTag>("edmNameRandom")),
  edmNameRandomLowEta_                 (Conf().getUntrackedParameter<edm::InputTag>("edmNameRandomLowEta")),
  edmNameFixedGridAll_                 (Conf().getUntrackedParameter<edm::InputTag>("edmNameFixedGridAll")),
  edmNameFixedGridFastjetAll_          (Conf().getUntrackedParameter<edm::InputTag>("edmNameFixedGridFastjetAll")),
  edmNameKt6CaloJets_                  (Conf().getUntrackedParameter<edm::InputTag>("edmNameKt6CaloJets")),
  edmNameKt6CaloJetsCentral_           (Conf().getUntrackedParameter<edm::InputTag>("edmNameKt6CaloJetsCentral")),
  edmNameKt6PFJets_                    (Conf().getUntrackedParameter<edm::InputTag>("edmNameKt6PFJets")),
  edmNameKt6PFJetsCentralChargedPileUp_(Conf().getUntrackedParameter<edm::InputTag>("edmNameKt6PFJetsCentralChargedPileUp")),
  edmNameKt6PFJetsCentralNeutral_      (Conf().getUntrackedParameter<edm::InputTag>("edmNameKt6PFJetsCentralNeutral")),
  edmNameKt6PFJetsCentralNeutralTight_ (Conf().getUntrackedParameter<edm::InputTag>("edmNameKt6PFJetsCentralNeutralTight")),
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
  event.getByLabel(edmName_,hRho);

  Handle<double> hRhoLowEta;
  event.getByLabel(edmNameLowEta_,hRhoLowEta);

  Handle<double> hRhoRandom;
  event.getByLabel(edmNameRandom_,hRhoRandom);

  Handle<double> hRhoRandomLowEta;
  event.getByLabel(edmNameRandomLowEta_,hRhoRandomLowEta);

  Handle<double> hRhoFixedGridAll;
  event.getByLabel(edmNameFixedGridAll_,hRhoFixedGridAll);

  Handle<double> hRhoFixedGridFastjetAll;
  event.getByLabel(edmNameFixedGridFastjetAll_,hRhoFixedGridFastjetAll);

  Handle<double> hRhoKt6CaloJets;
  event.getByLabel(edmNameKt6CaloJets_,hRhoKt6CaloJets);

  Handle<double> hRhoKt6CaloJetsCentral;
  event.getByLabel(edmNameKt6CaloJetsCentral_,hRhoKt6CaloJetsCentral);

  Handle<double> hRhoKt6PFJets;
  event.getByLabel(edmNameKt6PFJets_,hRhoKt6PFJets);

  Handle<double> hRhoKt6PFJetsCentralChargedPileUp;
  event.getByLabel(edmNameKt6PFJetsCentralChargedPileUp_,hRhoKt6PFJetsCentralChargedPileUp);

  Handle<double> hRhoKt6PFJetsCentralNeutral;
  event.getByLabel(edmNameKt6PFJetsCentralNeutral_,hRhoKt6PFJetsCentralNeutral);

  Handle<double> hRhoKt6PFJetsCentralNeutralTight;
  event.getByLabel(edmNameKt6PFJetsCentralNeutralTight_,hRhoKt6PFJetsCentralNeutralTight);

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
