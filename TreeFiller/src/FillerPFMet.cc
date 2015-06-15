#include "MitProd/TreeFiller/interface/FillerPFMet.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataTree/interface/PFMetCol.h"
#include "MitProd/ObjectService/interface/ObjectService.h"

template<class MET>
mithep::FillerPFMet<MET>::FillerPFMet(edm::ParameterSet const& cfg, edm::ConsumesCollector& collector, mithep::ObjectService* os, char const* name, bool active) : 
  BaseFiller(cfg, os, name, active),
  edmToken_(GetToken<MetCollection>(collector, "edmName")),
  edmSingleToken_(GetToken<MET>(collector, "edmName")),
  mitName_(Conf().getUntrackedParameter("mitName", std::string(Names::gkCaloMetBrn))),
  pfMets_(new mithep::PFMetArr)
{
  // Constructor.
}

template<class MET>
mithep::FillerPFMet<MET>::~FillerPFMet()
{
  // Destructor.

  delete pfMets_;
}

template<class MET>
void
mithep::FillerPFMet<MET>::BookDataBlock(TreeWriter& tws)
{
  // Add mets branch to tree.

  tws.AddBranch(mitName_, &pfMets_);
  OS()->add(pfMets_, mitName_);
}

template<class MET>
void
mithep::FillerPFMet<MET>::FillDataBlock(edm::Event const& event, edm::EventSetup const&)
{
  // Fill missing energy from edm collection into our collection.

  pfMets_->Delete();

  std::vector<MET const*> inMets;
  
  edm::Handle<MetCollection> hPFMetProduct;
  if(GetProductSafe(edmToken_, hPFMetProduct, event)){
    MetCollection const& inMetCollection = *hPFMetProduct;
    for (auto&& met : inMetCollection)
      inMets.push_back(&met);
  }
  else {
    edm::Handle<MET> hSingleMetProduct;
    GetProduct(edmSingleToken_, hSingleMetProduct, event);
    inMets.push_back(hSingleMetProduct.product());
  }

  // loop through all mets
  for (auto* inMet : inMets) {
    mithep::PFMet* pfMet = FillBasicData(inMet);

    pfMet->SetElongitudinal(inMet->e_longitudinal());

    // fill PFMet class data
    pfMet->SetPFMetSig(inMet->significance());

    // XYZEtFraction and XYZFraction are synonymous - some are implemented for pat while some aren't..
    pfMet->SetNeutralEMFraction(inMet->NeutralEMFraction());
    pfMet->SetNeutralHadFraction(inMet->NeutralHadEtFraction());
    pfMet->SetChargedEMFraction(inMet->ChargedEMEtFraction());
    pfMet->SetChargedHadFraction(inMet->ChargedHadEtFraction());
    pfMet->SetMuonFraction(inMet->MuonEtFraction());

  }
  pfMets_->Trim();
}

template<class MET>
mithep::PFMet*
mithep::FillerPFMet<MET>::FillBasicData(MET const* inMet)
{
  return 0;
}

#include "DataFormats/METReco/interface/PFMET.h"
#include "DataFormats/PatCandidates/interface/MET.h"

namespace mithep 
{
  template<>
  mithep::PFMet*
  mithep::FillerPFMet<reco::PFMET>::FillBasicData(reco::PFMET const* inMet)
  {
    mithep::PFMet *pfMet = pfMets_->Allocate();
    new (pfMet) mithep::PFMet(inMet->px(), inMet->py());
    
    pfMet->SetSumEt(inMet->sumEt());

    for(unsigned i=0; i<inMet->mEtCorr().size(); i++) {
      pfMet->PushCorrectionX(inMet->mEtCorr()[i].mex);
      pfMet->PushCorrectionY(inMet->mEtCorr()[i].mey);
      pfMet->PushCorrectionSumEt(inMet->mEtCorr()[i].sumet);
    }

    return pfMet;
  }

  template<>
  mithep::PFMet*
  mithep::FillerPFMet<pat::MET>::FillBasicData(pat::MET const* inMet)
  {
    mithep::PFMet *pfMet = pfMets_->Allocate();
    new (pfMet) mithep::PFMet(inMet->shiftedPx(pat::MET::NoShift, pat::MET::Raw), inMet->shiftedPy(pat::MET::NoShift, pat::MET::Raw));
    
    pfMet->SetSumEt(inMet->shiftedSumEt(pat::MET::NoShift, pat::MET::Raw));

    for (auto corr : {pat::MET::Type1, pat::MET::Type1p2}) {
      pfMet->PushCorrectionX(inMet->shiftedPx(pat::MET::NoShift, corr) - pfMet->Mex());
      pfMet->PushCorrectionY(inMet->shiftedPy(pat::MET::NoShift, corr) - pfMet->Mey());
      pfMet->PushCorrectionSumEt(inMet->shiftedSumEt(pat::MET::NoShift, corr) - pfMet->SumEt());
    }

    return pfMet;
  }

  typedef mithep::FillerPFMet<reco::PFMET> FillerPFMetFromPFMET;
  typedef mithep::FillerPFMet<pat::MET> FillerPFMetFromPATMET;
}

DEFINE_MITHEP_TREEFILLER(FillerPFMetFromPFMET);
DEFINE_MITHEP_TREEFILLER(FillerPFMetFromPATMET);
