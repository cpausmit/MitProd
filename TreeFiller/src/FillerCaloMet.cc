// $Id: FillerCaloMet.cc,v 1.5 2008/07/13 08:46:04 loizides Exp $

#include "MitProd/TreeFiller/interface/FillerCaloMet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/METReco/interface/CaloMET.h"
#include "MitAna/DataTree/interface/Names.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerCaloMet::FillerCaloMet(const ParameterSet &cfg, const char *name, bool active) : 
  BaseFiller(cfg,name,active),
  edmName_(Conf().getUntrackedParameter<string>("edmName","met")),
  mitName_(Conf().getUntrackedParameter<string>("mitName",Names::gkCaloMetBrn)),
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

  tws.AddBranch(mitName_.c_str(),&caloMets_);
}

//--------------------------------------------------------------------------------------------------
void FillerCaloMet::FillDataBlock(const edm::Event      &event, 
                                  const edm::EventSetup &setup)
{
  // Fill missing energy from edm collection into our collection.

  caloMets_->Reset();

  Handle<reco::CaloMETCollection> hCaloMetProduct;
  GetProduct(edmName_, hCaloMetProduct, event);

  const reco::CaloMETCollection inCaloMets = *(hCaloMetProduct.product());  

  // loop through all mets
  for (reco::CaloMETCollection::const_iterator inCaloMet = inCaloMets.begin(); 
       inCaloMet != inCaloMets.end(); ++inCaloMet) {
    
    mithep::CaloMet *caloMet = caloMets_->Allocate();
    new (caloMet) mithep::CaloMet(inCaloMet->px(), inCaloMet->py());
    
    // Fill Met base class data 
    caloMet->SetSumEt(inCaloMet->sumEt());
    caloMet->SetMetSig(inCaloMet->mEtSig());    
    caloMet->SetE_longitudinal(inCaloMet->e_longitudinal());
    for(unsigned i=0; i<inCaloMet->mEtCorr().size(); i++) {
      caloMet->PushCorrectionX(inCaloMet->mEtCorr()[i].mex);
      caloMet->PushCorrectionY(inCaloMet->mEtCorr()[i].mey);
      caloMet->PushCorrectionSumEt(inCaloMet->mEtCorr()[i].sumet);
    }
    // Fill CaloMet class data
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
    caloMet->SetMetSignificance(inCaloMet->metSignificance());
    caloMet->SetCaloSumEtInpHF(inCaloMet->CaloSETInpHF());
    caloMet->SetCaloSumEtInmHF(inCaloMet->CaloSETInmHF());    
    caloMet->SetCaloMetInpHF(inCaloMet->CaloMETInpHF());
    caloMet->SetCaloMetInmHF(inCaloMet->CaloMETInmHF());
    caloMet->SetCaloMetPhiInpHF(inCaloMet->CaloMETPhiInpHF());
    caloMet->SetCaloMetPhiInmHF(inCaloMet->CaloMETPhiInmHF());
  }

  caloMets_->Trim();
}
