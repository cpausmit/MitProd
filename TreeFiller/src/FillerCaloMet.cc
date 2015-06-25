#include "MitProd/TreeFiller/interface/FillerCaloMet.h"
#include "DataFormats/METReco/interface/CaloMET.h"
#include "MitAna/DataTree/interface/CaloMetCol.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitProd/ObjectService/interface/ObjectService.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerCaloMet::FillerCaloMet(const ParameterSet &cfg, edm::ConsumesCollector& collector, ObjectService* os, const char *name, bool active) : 
  BaseFiller(cfg,os,name,active),
  edmToken_(GetToken<reco::CaloMETCollection>(collector, cfg, "edmName","met")),
  mitName_(cfg.getUntrackedParameter<string>("mitName",Names::gkCaloMetBrn)),
  caloMets_(new mithep::CaloMetArr)
{
  // Constructor.
}

//--------------------------------------------------------------------------------------------------
FillerCaloMet::~FillerCaloMet()
{
  // Destructor.

  delete caloMets_;
}

//--------------------------------------------------------------------------------------------------
void FillerCaloMet::BookDataBlock(TreeWriter &tws)
{
  // Add mets branch to tree.

  tws.AddBranch(mitName_,&caloMets_);
  OS()->add<mithep::CaloMetArr>(caloMets_,mitName_);
}

//--------------------------------------------------------------------------------------------------
void FillerCaloMet::FillDataBlock(const edm::Event      &event, 
                                  const edm::EventSetup &setup)
{
  // Fill missing energy from edm collection into our collection.

  caloMets_->Delete();

  Handle<reco::CaloMETCollection> hCaloMetProduct;
  GetProduct(edmToken_, hCaloMetProduct, event);

  reco::CaloMETCollection const& inCaloMets = *hCaloMetProduct;

  // loop through all mets
  for (reco::CaloMETCollection::const_iterator inCaloMet = inCaloMets.begin(); 
       inCaloMet != inCaloMets.end(); ++inCaloMet) {
    
    mithep::CaloMet *caloMet = caloMets_->Allocate();
    new (caloMet) mithep::CaloMet(inCaloMet->px(), inCaloMet->py());
    
    // fill Met base class data 
    caloMet->SetSumEt(inCaloMet->sumEt());
    caloMet->SetElongitudinal(inCaloMet->e_longitudinal());
    for(unsigned i=0; i<inCaloMet->mEtCorr().size(); i++) {
      caloMet->PushCorrectionX(inCaloMet->mEtCorr()[i].mex);
      caloMet->PushCorrectionY(inCaloMet->mEtCorr()[i].mey);
      caloMet->PushCorrectionSumEt(inCaloMet->mEtCorr()[i].sumet);
    }
    // fill CaloMet class data
    caloMet->SetCaloMetSig(inCaloMet->metSignificance());    
    caloMet->SetMaxEtInEmTowers(inCaloMet->maxEtInEmTowers());
    caloMet->SetMaxEtInHadTowers(inCaloMet->maxEtInHadTowers());
    caloMet->SetEtFractionHadronic(inCaloMet->etFractionHadronic());
    caloMet->SetEmEtFraction(inCaloMet->emEtFraction());
    caloMet->SetHadEtInHB(inCaloMet->hadEtInHB());
    caloMet->SetHadEtInHO(inCaloMet->hadEtInHO());
    caloMet->SetHadEtInHE(inCaloMet->hadEtInHE());
    caloMet->SetHadEtInHF(inCaloMet->hadEtInHF());
    caloMet->SetEmEtInEB(inCaloMet->emEtInEB());
    caloMet->SetEmEtInEE(inCaloMet->emEtInEE());
    caloMet->SetEmEtInHF(inCaloMet->emEtInHF());
    caloMet->SetCaloSumEtInpHF(inCaloMet->CaloSETInpHF());
    caloMet->SetCaloSumEtInmHF(inCaloMet->CaloSETInmHF());    
    caloMet->SetCaloMetInpHF(inCaloMet->CaloMETInpHF());
    caloMet->SetCaloMetInmHF(inCaloMet->CaloMETInmHF());
    caloMet->SetCaloMetPhiInpHF(inCaloMet->CaloMETPhiInpHF());
    caloMet->SetCaloMetPhiInmHF(inCaloMet->CaloMETPhiInmHF());
  }
  caloMets_->Trim();
}

DEFINE_MITHEP_TREEFILLER(FillerCaloMet);
