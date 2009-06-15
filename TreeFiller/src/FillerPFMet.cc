// $Id: FillerPFMet.cc,v 1.2 2009/03/15 11:20:41 loizides Exp $

#include "MitProd/TreeFiller/interface/FillerPFMet.h"
#include "DataFormats/METReco/interface/PFMET.h"
#include "DataFormats/METReco/interface/PFMETCollection.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataTree/interface/PFMetCol.h"
#include "MitProd/ObjectService/interface/ObjectService.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerPFMet::FillerPFMet(const ParameterSet &cfg, const char *name, bool active) : 
  BaseFiller(cfg,name,active),
  edmName_(Conf().getUntrackedParameter<string>("edmName","met")),
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

  Handle<reco::PFMETCollection> hCaloMetProduct;
  GetProduct(edmName_, hCaloMetProduct, event);

  const reco::PFMETCollection inPFMets = *(hCaloMetProduct.product());  

  // loop through all mets
  for (reco::PFMETCollection::const_iterator inPFMet = inPFMets.begin(); 
       inPFMet != inPFMets.end(); ++inPFMet) {
    
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
    pfMet->SetNeutralEMFraction(inPFMet->NeutralEMFraction());
    pfMet->SetNeutralHadFraction(inPFMet->NeutralHadFraction());
    pfMet->SetChargedEMFraction(inPFMet->ChargedEMFraction());
    pfMet->SetChargedHadFraction(inPFMet->ChargedHadFraction());
    pfMet->SetMuonFraction(inPFMet->MuonFraction());

  }
  pfMets_->Trim();
}
