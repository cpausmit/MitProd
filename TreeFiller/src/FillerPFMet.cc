#include "MitProd/TreeFiller/interface/FillerPFMet.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataTree/interface/PFMetCol.h"
#include "MitProd/ObjectService/interface/ObjectService.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerPFMet::FillerPFMet(const ParameterSet &cfg, edm::ConsumesCollector& collector, ObjectService* os, const char *name, bool active) : 
  BaseFiller(cfg,os,name,active),
  edmToken_(GetToken<reco::PFMETCollection>(collector, "edmName")),
  edmSingleToken_(GetToken<reco::PFMET>(collector, "edmName")),
  mitName_(Conf().getUntrackedParameter<string>("mitName",Names::gkCaloMetBrn)),
  pfMets_(new mithep::PFMetArr)
{
  // Constructor.
}

//--------------------------------------------------------------------------------------------------
FillerPFMet::~FillerPFMet()
{
  // Destructor.

  delete pfMets_;
}

//--------------------------------------------------------------------------------------------------
void FillerPFMet::BookDataBlock(TreeWriter &tws)
{
  // Add mets branch to tree.

  tws.AddBranch(mitName_,&pfMets_);
  OS()->add<mithep::PFMetArr>(pfMets_,mitName_);
}

//--------------------------------------------------------------------------------------------------
void FillerPFMet::FillDataBlock(const edm::Event      &event, 
                                const edm::EventSetup &setup)
{
  // Fill missing energy from edm collection into our collection.

  pfMets_->Delete();

  std::vector<reco::PFMET const*> inPFMets;
  
  Handle<reco::PFMETCollection> hPFMetProduct;
  if(GetProductSafe(edmToken_,hPFMetProduct, event)){
    reco::PFMETCollection const& inMetCollection = *hPFMetProduct;
    for (auto&& met : inMetCollection)
      inPFMets.push_back(&met);
  } else {
    Handle<reco::PFMET> hSingleMetProduct;
    GetProduct(edmSingleToken_, hSingleMetProduct, event);
    inPFMets.push_back(hSingleMetProduct.product());
  }
  // loop through all mets
  for (auto inPFMet : inPFMets) {
    mithep::PFMet *pfMet = pfMets_->Allocate();
    new (pfMet) mithep::PFMet(inPFMet->px(), inPFMet->py());
    
    // fill Met base class data 
    pfMet->SetSumEt(inPFMet->sumEt());
    pfMet->SetElongitudinal(inPFMet->e_longitudinal());
    for(unsigned i=0; i<inPFMet->mEtCorr().size(); i++) {
      pfMet->PushCorrectionX(inPFMet->mEtCorr()[i].mex);
      pfMet->PushCorrectionY(inPFMet->mEtCorr()[i].mey);
      pfMet->PushCorrectionSumEt(inPFMet->mEtCorr()[i].sumet);
    }
    // fill PFMet class data
    pfMet->SetPFMetSig(inPFMet->significance());
    pfMet->SetNeutralEMFraction(inPFMet->NeutralEMFraction());
    pfMet->SetNeutralHadFraction(inPFMet->NeutralHadFraction());
    pfMet->SetChargedEMFraction(inPFMet->ChargedEMFraction());
    pfMet->SetChargedHadFraction(inPFMet->ChargedHadFraction());
    pfMet->SetMuonFraction(inPFMet->MuonFraction());

  }
  pfMets_->Trim();
}
