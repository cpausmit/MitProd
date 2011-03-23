// $Id: FillerPileupEnergyDensity.cc,v 1.2 2011/03/01 17:27:22 mzanetti Exp $

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
  BaseFiller(cfg,name,active),
  //edmName_(Conf().getUntrackedParameter<string>("edmName","kt6PFJets")),
  edmName_(Conf().getUntrackedParameter<edm::InputTag>("edmName")),  
  edmNameHighEta_(Conf().getUntrackedParameter<edm::InputTag>("edmNameHighEta")),  
  mitName_(Conf().getUntrackedParameter<string>("mitName",Names::gkPileupEnergyDensityBrn)),
  rhos_(new mithep::PileupEnergyDensityArr)
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

//   Handle<double> hRho;
//   GetProduct(edmName_, hRho, event);

  Handle<double> hRho;
  event.getByLabel(edmName_,hRho);

  Handle<double> hRhoHighEta;
  event.getByLabel(edmNameHighEta_,hRhoHighEta);

  mithep::PileupEnergyDensity *rho = rhos_->AddNew();

  rho->SetRho(*hRho);
  rho->SetRhoHighEta(*hRhoHighEta);

  rhos_->Trim();
}
