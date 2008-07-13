// $Id: FillerCaloJets.cc,v 1.3 2008/07/08 14:42:50 loizides Exp $

#include "MitProd/TreeFiller/interface/FillerCaloJets.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/Common/interface/Handle.h"
#include "MitAna/DataTree/interface/Names.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerCaloJets::FillerCaloJets(const ParameterSet &cfg, const char *name, bool active) : 
  BaseFiller(cfg,name,active),
  edmName_(Conf().getUntrackedParameter<string>("edmName","recoCaloJets:iterativeCone5CaloJets")),
  mitName_(Conf().getUntrackedParameter<string>("mitName",Names::gkCaloJetBrn)),
  jets_(new mithep::JetArr(16))
{
  // Constructor.
}

//--------------------------------------------------------------------------------------------------
FillerCaloJets::~FillerCaloJets()
{
  // Destructor.

  delete jets_;
}

//--------------------------------------------------------------------------------------------------
void FillerCaloJets::BookDataBlock(TreeWriter &tws)
{
  // Add jets branch to tree.

  tws.AddBranch(mitName_.c_str(),&jets_);
}

//--------------------------------------------------------------------------------------------------
void FillerCaloJets::FillDataBlock(const edm::Event      &event, 
                                   const edm::EventSetup &setup)
{
  // Fill jets from edm collection into our collection.

  jets_->Reset();

  Handle<reco::CaloJetCollection> hJetProduct;
  GetProduct(edmName_, hJetProduct, event);

  const reco::CaloJetCollection inJets = *(hJetProduct.product());  

  // loop through all jets
  for (reco::CaloJetCollection::const_iterator inJet = inJets.begin(); 
       inJet != inJets.end(); ++inJet) {

    mithep::Jet *jet = jets_->Allocate();
    new (jet) mithep::Jet(inJet->p4().x(),
                          inJet->p4().y(),
                          inJet->p4().z(),
                          inJet->p4().e());

    jet->SetMaxEInEmTowers (inJet->maxEInEmTowers());	 
    jet->SetMaxEInHadTowers(inJet->maxEInHadTowers());
    jet->SetEnergyFractionH(inJet->energyFractionHadronic());
    jet->SetHadEnergyInHB  (inJet->hadEnergyInHB());
    jet->SetHadEnergyInHO  (inJet->hadEnergyInHO());
    jet->SetHadEnergyInHE  (inJet->hadEnergyInHE());
    jet->SetHadEnergyInHF  (inJet->hadEnergyInHF());
    jet->SetEmEnergyInEB   (inJet->emEnergyInEB());
    jet->SetEmEnergyInEE   (inJet->emEnergyInEE());
    jet->SetEmEnergyInHF   (inJet->emEnergyInHF());
    jet->SetTowersArea     (inJet->towersArea());
    jet->SetN              (inJet->nConstituents());
    jet->SetN60		   (inJet->n60());
    jet->SetN90		   (inJet->n90());
  }

  jets_->Trim();
}
