#include "MitProd/TreeFiller/interface/FillerJPTJets.h"

#include "MitEdm/DataFormats/interface/RefToBaseToPtr.h"
#include "MitAna/DataTree/interface/CaloJet.h"

#include "DataFormats/JetReco/interface/JPTJet.h"

mithep::FillerJPTJets::FillerJPTJets(edm::ParameterSet const& cfg, edm::ConsumesCollector& collector, mithep::ObjectService* os, char const* name, bool active/* = true*/) :
  FillerJets(cfg, collector, os, name, active),
  caloJetMapName_(cfg.getUntrackedParameter<string>("caloJetCandMapName", "caloJetMapName")),
  caloJetMap_(0)
{
  jets_ = new mithep::JPTJetArr(32);
}

mithep::FillerJPTJets::~FillerJPTJets()
{
}

void
mithep::FillerJPTJets::PrepareLinks()
{
  if (!caloJetMapName_.empty()) {
    caloJetMap_ = OS()->get<CaloJetMap>(caloJetMapName_);
    AddBranchDep(mitName_,caloJetMap_->GetBrName());
  }
}

void
mithep::FillerJPTJets::FillSpecific(mithep::Jet& outBaseJet, reco::JetBaseRef const& inJetRef)
{
  auto* inJPTJet = dynamic_cast<reco::JPTJet const*>(inJetRef.get());
  if (!inJPTJet)
    return;

  auto& outJet = static_cast<mithep::JPTJet&>(outBaseJet);

  //fill jptjet-specific quantities
  outJet.SetZSPCor(inJPTJet->getZSPCor());
  outJet.SetChargedHadronEnergy(inJPTJet->chargedHadronEnergy());
  outJet.SetNeutralHadronEnergy(inJPTJet->neutralHadronEnergy());
  outJet.SetChargedEmEnergy(inJPTJet->chargedHadronEnergy());
  outJet.SetNeutralEmEnergy(inJPTJet->neutralEmEnergy());
  outJet.SetResponseOfChargedWithEff(inJPTJet->getSpecific().mResponseOfChargedWithEff);
  outJet.SetResponseOfChargedWithoutEff(inJPTJet->getSpecific().mResponseOfChargedWithoutEff);
  outJet.SetSumPtOfChargedWithEff(inJPTJet->getSpecific().mSumPtOfChargedWithEff);
  outJet.SetSumPtOfChargedWithoutEff(inJPTJet->getSpecific().mSumPtOfChargedWithoutEff);
  outJet.SetSumEnergyOfChargedWithEff(inJPTJet->getSpecific().mSumEnergyOfChargedWithEff);
  outJet.SetSumEnergyOfChargedWithoutEff(inJPTJet->getSpecific().mSumEnergyOfChargedWithoutEff);
  outJet.SetR2momtr(inJPTJet->getSpecific().R2momtr);
  outJet.SetEta2momtr(inJPTJet->getSpecific().Eta2momtr);
  outJet.SetPhi2momtr(inJPTJet->getSpecific().Phi2momtr);
  outJet.SetPout(inJPTJet->getSpecific().Pout);
  outJet.SetZch(inJPTJet->getSpecific().Zch);
  outJet.SetChargedMultiplicity(inJPTJet->chargedMultiplicity());
  outJet.SetMuonMultiplicity(inJPTJet->muonMultiplicity());
  outJet.SetElectronMultiplicity(inJPTJet->elecMultiplicity());
}


void
mithep::FillerJPTJets::ResolveLinks(edm::Event const& event, edm::EventSetup const&)
{
  if (!caloJetMap_)
    return;

  for (auto& mapElem : jetMap_->FwdMap()) {
    auto&& jPtr = mapElem.first;
    auto& inJet = static_cast<reco::JPTJet const&>(*jPtr);
    auto& outJet = static_cast<mithep::JPTJet&>(*mapElem.second);

    if (inJet.getCaloJetRef().isNonnull()) {
      auto&& caloJetPtr(mitedm::refToBaseToPtr(inJet.getCaloJetRef()));
      outJet.SetOriginalJet(caloJetMap_->GetMit(caloJetPtr));
    }
  }      
}

DEFINE_MITHEP_TREEFILLER(FillerJPTJets);
