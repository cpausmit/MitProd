#include "MitProd/TreeFiller/interface/FillerPFJets.h"

#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/PatCandidates/interface/Jet.h"

mithep::FillerPFJets::FillerPFJets(edm::ParameterSet const& cfg, edm::ConsumesCollector& collector, mithep::ObjectService* os, char const* name, bool active/* = true*/) : 
  FillerJets(cfg, collector, os, name, active),
  fillFromPAT_(cfg.getUntrackedParameter<bool>("fillFromPAT", false)),
  bJetTagsName_{},
  pfCandMapName_(cfg.getUntrackedParameter<std::string>("pfCandMapName", "")),
  pfCandMap_(0)
{
  jets_ = new mithep::PFJetArr(32);
  
  if (bTaggingActive_ && fillFromPAT_) {
    for (unsigned iA = 0; iA != mithep::Jet::nBTagAlgos; ++iA) {
      std::string paramName(mithep::Jet::BTagAlgoName(iA) + std::string("BJetTagsName"));
      bJetTagsName_[iA] = cfg.getUntrackedParameter<std::string>(paramName, "");
    }
  }
}

mithep::FillerPFJets::~FillerPFJets()
{
  delete jets_;
}

void
mithep::FillerPFJets::PrepareLinks()
{
  // find the pf candidate map
  if (!pfCandMapName_.empty()) {
    pfCandMap_ = OS()->get<mithep::PFCandidateMap>(pfCandMapName_);
    AddBranchDep(mitName_, pfCandMap_->GetBrName());
  }
}

void
mithep::FillerPFJets::FillSpecific(mithep::Jet& outBaseJet, reco::JetBaseRef const& inJetRef)
{
  auto& outJet = static_cast<mithep::PFJet&>(outBaseJet);

  if (fillFromPAT_) {
    auto* inJet = dynamic_cast<pat::Jet const*>(inJetRef.get());
    if (!inJet)
      throw edm::Exception(edm::errors::Configuration, "FillerPFJets::FillSpecific)")
        << "fillFromPAT set on non-PAT input";

    // pat::Jet::p4() returns a corrected p4. Uncorrect here
    double toRaw = inJet->jecFactor("Uncorrected");
    auto&& rawP4 = inJet->p4() * toRaw;
    outJet.SetRawPtEtaPhiM(rawP4.pt(), rawP4.eta(), rawP4.phi(), rawP4.mass());

    fillPATJetVariables(outJet, *inJet);
  }
  else {
    auto* inJet = dynamic_cast<reco::PFJet const*>(inJetRef.get());
    if (!inJet)
      throw edm::Exception(edm::errors::Configuration, "FillerPFJets::FillSpecific)")
        << "fillFromPAT not set but cannot case jet to PFJet";

    fillPFJetVariables(outJet, *inJet);
  }
}

void
mithep::FillerPFJets::ResolveLinks(edm::Event const& event, edm::EventSetup const& setup)
{
  if (!pfCandMap_)
    return;

  // add PFCandidate refs
  for (auto& mapElem : jetMap_->FwdMap()) {
    auto&& jPtr = mapElem.first;
    auto& inJet = *jPtr;
    auto& outJet = static_cast<mithep::PFJet&>(*mapElem.second);

    for (unsigned iD = 0; iD != inJet.numberOfDaughters(); ++iD) {
      mithep::PFCandidate* pfCand = pfCandMap_->GetMit(inJet.daughterPtr(iD), false);

      if (pfCand)
        outJet.AddPFCand(pfCand);
      else {
        // First two daughters of the AK8 jets in miniAOD are Soft Drop subjets.
        // Being a bit general here..
        auto* subJet = dynamic_cast<reco::Jet const*>(inJet.daughter(iD));
        if (!subJet)
          throw edm::Exception(edm::errors::LogicError, "FillerPFJets::FillDataBlock()")
            << "Daughter " << iD << " not in the list of PF Candidates but not a subjet";

        for (unsigned iSD = 0; iSD != subJet->numberOfDaughters(); ++iSD) {
          auto* subJetConstituent = pfCandMap_->GetMit(subJet->daughterPtr(iSD));
          if (subJetConstituent)
            outJet.AddPFCand(subJetConstituent);
        }
      }
    }
  }
}

void
mithep::FillerPFJets::fillPFJetVariables(mithep::PFJet& outJet, reco::PFJet const& inPFJet)
{
  // fill pfjet-specific quantities
  outJet.SetChargedHadronEnergy(inPFJet.chargedHadronEnergy());
  outJet.SetNeutralHadronEnergy(inPFJet.neutralHadronEnergy());
  outJet.SetChargedEmEnergy(inPFJet.chargedEmEnergy());
  outJet.SetChargedMuEnergy(inPFJet.chargedMuEnergy());
  outJet.SetNeutralEmEnergy(inPFJet.neutralEmEnergy());
  outJet.SetChargedMultiplicity(inPFJet.chargedMultiplicity());
  outJet.SetNeutralMultiplicity(inPFJet.neutralMultiplicity());
  outJet.SetMuonMultiplicity(inPFJet.muonMultiplicity());
}

void
mithep::FillerPFJets::fillPATJetVariables(mithep::PFJet& outJet, pat::Jet const& inPFJet)
{
  // fill pfjet-specific quantities
  outJet.SetChargedHadronEnergy(inPFJet.chargedHadronEnergy());
  outJet.SetNeutralHadronEnergy(inPFJet.neutralHadronEnergy());
  outJet.SetChargedEmEnergy(inPFJet.chargedEmEnergy());
  outJet.SetChargedMuEnergy(inPFJet.chargedMuEnergy());
  outJet.SetNeutralEmEnergy(inPFJet.neutralEmEnergy());
  outJet.SetChargedMultiplicity(inPFJet.chargedMultiplicity());
  outJet.SetNeutralMultiplicity(inPFJet.neutralMultiplicity());
  outJet.SetMuonMultiplicity(inPFJet.muonMultiplicity());
}

void
mithep::FillerPFJets::initBJetTags(edm::Event const& event, reco::JetTagCollection const* bJetTags[mithep::Jet::nBTagAlgos])
{
  if (!fillFromPAT_)
    FillerJets::initBJetTags(event, bJetTags);
}

void
mithep::FillerPFJets::setBJetTags(mithep::Jet& outJet, reco::JetBaseRef const& baseRef, reco::JetTagCollection const* bJetTags[mithep::Jet::nBTagAlgos]) const
{
  if (fillFromPAT_) {
    pat::Jet const& inJet = static_cast<pat::Jet const&>(*baseRef);

    for (unsigned iT = 0; iT != mithep::Jet::nBTagAlgos; ++iT)
      outJet.SetBJetTagsDisc(inJet.bDiscriminator(bJetTagsName_[iT]), iT);
  }
  else
    FillerJets::setBJetTags(outJet, baseRef, bJetTags);
}

void
mithep::FillerPFJets::initCorrections(edm::Event const& event, edm::EventSetup const& setup)
{
  if (!fillFromPAT_)
    FillerJets::initCorrections(event, setup);
}

void
mithep::FillerPFJets::setCorrections(mithep::Jet& outJet, reco::Jet const& inJet)
{
  if (fillFromPAT_) {
    auto& inPATJet = static_cast<pat::Jet const&>(inJet);

    double toRaw = inPATJet.jecFactor("Uncorrected");
    double toL1 = inPATJet.jecFactor("L1FastJet");
    double toL2 = inPATJet.jecFactor("L2Relative");
    double toL3 = inPATJet.jecFactor("L3Absolute");

    outJet.SetL1OffsetCorrectionScale(toL1 / toRaw);
    outJet.EnableCorrection(mithep::Jet::L1);

    outJet.SetL2RelativeCorrectionScale(toL2 / toL1);
    outJet.EnableCorrection(mithep::Jet::L2);

    outJet.SetL3AbsoluteCorrectionScale(toL3 / toL2);
    outJet.EnableCorrection(mithep::Jet::L3);
  }
  else
    FillerJets::setCorrections(outJet, inJet);
}

DEFINE_MITHEP_TREEFILLER(FillerPFJets);

