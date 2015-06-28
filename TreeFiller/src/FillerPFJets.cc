#define FILLERPFJETS_INSTANCE
#define FILLERJETS_INSTANCE

#include "MitProd/TreeFiller/interface/FillerPFJets.h"

template<class PFJET>
mithep::FillerPFJets<PFJET>::FillerPFJets(edm::ParameterSet const& cfg, edm::ConsumesCollector& collector, mithep::ObjectService* os, char const* name, bool active/* = true*/) : 
  FillerJets<mithep::PFJet>(cfg, collector, os, name, active),
  fastJetCorrectionsActive_(cfg.getUntrackedParameter("fastJetCorrectionsActive", false)),
  rhoToken_(GetToken<double>(collector, cfg, "rhoName", fastJetCorrectionsActive_)),
  rho_(0.),
  bJetTagsName_{},
  pfCandMapName_(cfg.getUntrackedParameter("pfCandMapName", std::string("pfCandMapName"))),
  pfCandMap_(0)
{
  if (bTaggingActive_) {
    for (unsigned iA = 0; iA != mithep::Jet::nBTagAlgos; ++iA) {
      std::string paramName(mithep::Jet::BTagAlgoName(iA) + std::string("BJetTagsName"));
      bJetTagsName_[iA] = cfg.getUntrackedParameter(paramName, std::string(""));
    }
  }
}

template<class PFJET>
mithep::FillerPFJets<PFJET>::~FillerPFJets()
{
}

template<class PFJET>
void
mithep::FillerPFJets<PFJET>::PrepareLinks()
{
  // somehow g++ does not like the expression OS()-> in this function
  mithep::ObjectService& os = *OS();

  // find the pf candidate map
  if (!pfCandMapName_.empty()) {
    pfCandMap_ = os.get<mithep::PFCandidateMap>(pfCandMapName_);
    if (pfCandMap_)
      AddBranchDep(mitName_, pfCandMap_->GetBrName());
  }
}

template<class PFJET>
void
mithep::FillerPFJets<PFJET>::PrepareSpecific(edm::Event const& event, edm::EventSetup const&)
{
  rho_ = 0.;
  if (fastJetCorrectionsActive_) {
    edm::Handle<double> hRho;
    GetProduct(rhoToken_, hRho, event);
    rho_ = *hRho;
  }
}

template<class PFJET>
void
mithep::FillerPFJets<PFJET>::FillSpecific(mithep::PFJet& outJet, reco::JetBaseRef const& inJetRef)
{
  auto* inPFJet = dynamic_cast<PFJET const*>(inJetRef.get());
  if (!inPFJet)
    return;

  // fill pfjet-specific quantities
  outJet.SetChargedHadronEnergy(inPFJet->chargedHadronEnergy());
  outJet.SetNeutralHadronEnergy(inPFJet->neutralHadronEnergy());
  outJet.SetChargedEmEnergy(inPFJet->chargedEmEnergy());
  outJet.SetChargedMuEnergy(inPFJet->chargedMuEnergy());
  outJet.SetNeutralEmEnergy(inPFJet->neutralEmEnergy());
  outJet.SetChargedMultiplicity(inPFJet->chargedMultiplicity());
  outJet.SetNeutralMultiplicity(inPFJet->neutralMultiplicity());
  outJet.SetMuonMultiplicity(inPFJet->muonMultiplicity());

  // jet corrections
  if (fastJetCorrectionsActive_) {
    double L1Scale = std::max((inPFJet->pt() - rho_ * inPFJet->jetArea()) / inPFJet->pt(), 0.);
    outJet.SetL1OffsetCorrectionScale(L1Scale);
    outJet.EnableCorrection(mithep::Jet::L1);
  }
}

template<class PFJET>
void
mithep::FillerPFJets<PFJET>::ResolveLinks(edm::Event const& event, edm::EventSetup const& setup)
{
  if (!pfCandMap_)
    return;

  // add PFCandidate refs
  for (auto& mapElem : jetMap_->FwdMap()) {
    auto&& jPtr = mapElem.first;
    auto& inJet = static_cast<PFJET const&>(*jPtr);
    auto& outJet = *mapElem.second;

    for (unsigned iD = 0; iD != inJet.numberOfDaughters(); ++iD) {
      auto* pfCand = pfCandMap_->GetMit(inJet.daughterPtr(iD), false);
      if (pfCand)
        outJet.AddPFCand(pfCand);
      else {
        // First two daughters of the AK8 jets in miniAOD are Soft Drop subjets.
        // Being a bit general here..
        auto* subJet = dynamic_cast<reco::Jet const*>(inJet.daughter(iD));
        if (!subJet)
          throw edm::Exception(edm::errors::LogicError, "FillerPFJets::FillDataBlock()")
            << "Daughter " << iD << " not in the list of PF Candidates but not a subjet";

        for (unsigned iSD = 0; iSD != subJet->numberOfDaughters(); ++iSD)
          outJet.AddPFCand(pfCandMap_->GetMit(subJet->daughterPtr(iSD)));
      }
    }
  }
}

template<class PFJET>
void
mithep::FillerPFJets<PFJET>::initBJetTags(edm::Event const& event, reco::JetTagCollection const* bJetTags[mithep::Jet::nBTagAlgos])
{
  FillerJets<mithep::PFJet>::initBJetTags(event, bJetTags);
}

template<class PFJET>
void
mithep::FillerPFJets<PFJET>::setBJetTags(mithep::Jet& outJet, reco::JetBaseRef const& baseRef, reco::JetTagCollection const* bJetTags[mithep::Jet::nBTagAlgos]) const
{
  FillerJets<mithep::PFJet>::setBJetTags(outJet, baseRef, bJetTags);
}

#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/PatCandidates/interface/Jet.h"

namespace mithep {

  template<>
  void
  FillerPFJets<pat::Jet>::initBJetTags(edm::Event const&, reco::JetTagCollection const* [mithep::Jet::nBTagAlgos])
  {
  }

  template<>
  void
  FillerPFJets<pat::Jet>::setBJetTags(mithep::Jet& outJet, reco::JetBaseRef const& inJetRef, reco::JetTagCollection const* [mithep::Jet::nBTagAlgos]) const
  {
    pat::Jet const& inJet = static_cast<pat::Jet const&>(*inJetRef);

    for (unsigned iT = 0; iT != mithep::Jet::nBTagAlgos; ++iT)
      outJet.SetBJetTagsDisc(inJet.bDiscriminator(bJetTagsName_[iT]), iT);
  }

  typedef FillerPFJets<reco::PFJet> FillerPFJetsFromPFJets;
  typedef FillerPFJets<pat::Jet> FillerPFJetsFromPATJets;
}

DEFINE_MITHEP_TREEFILLER(FillerPFJetsFromPFJets);
DEFINE_MITHEP_TREEFILLER(FillerPFJetsFromPATJets);

