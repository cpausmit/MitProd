#define MITPROD_TREEFILLER_FILLERPFJETS
#include "MitProd/TreeFiller/interface/FillerPFJets.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"
#include "DataFormats/BTauReco/interface/JetTag.h"
#include "SimDataFormats/JetMatching/interface/JetFlavour.h"
#include "SimDataFormats/JetMatching/interface/JetFlavourMatching.h"
#include "SimDataFormats/JetMatching/interface/MatchedPartons.h"
#include "SimDataFormats/JetMatching/interface/JetMatchedPartons.h"
#include "JetMETCorrections/Objects/interface/JetCorrector.h"

#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataTree/interface/PFJetCol.h"
#include "MitAna/DataTree/interface/PileupEnergyDensity.h"
#include "MitProd/ObjectService/interface/ObjectService.h"

template<class JET>
mithep::FillerPFJets<JET>::FillerPFJets(edm::ParameterSet const& cfg, edm::ConsumesCollector& collector, mithep::ObjectService* os, const char *name, bool active) : 
  BaseFiller(cfg,os,name,active),
  flavorMatchingActive_(cfg.getUntrackedParameter("flavorMatchingActive", true)),
  bTaggingActive_(cfg.getUntrackedParameter("bTaggingActive", true)),
  jetToVertexActive_(cfg.getUntrackedParameter("jetToVertexActive", true)),
  jetCorrectionsActive_(cfg.getUntrackedParameter("jetCorrectionsActive", true)),
  fastJetCorrectionsActive_(cfg.getUntrackedParameter("fastJetCorrectionsActive", false)),
  edmToken_(GetToken<JetCollection>(collector, cfg, "edmName", "recoPFJets:iterativeCone5PFJets")),
  jetToVertexAlphaToken_(GetToken<std::vector<double> >(collector, cfg, "jetToVertexAlphaName", "jetToVertexAlpha")),
  jetToVertexBetaToken_(GetToken<std::vector<double> >(collector, cfg, "jetToVertexBetaName", "jetToVertexBetaName")),
  rhoToken_(GetToken<double>(collector, cfg, "rhoName")),
  flavorMatchingByReferenceToken_(GetToken<reco::JetMatchedPartonsCollection>(collector, cfg, "flavorMatchingByReferenceName", "srcByReference")),
  bJetTagsToken_{},
  bJetTagsName_{},
  bJetTags_{},
  mitName_(cfg.getUntrackedParameter("mitName", std::string("ItrCone5PFJets"))), 
  L2JetCorrectorName_(cfg.getUntrackedParameter("L2JetCorrectorName", std::string("L2JetCorrectorName"))),
  L3JetCorrectorName_(cfg.getUntrackedParameter("L3JetCorrectorName", std::string("L3JetCorrectorName"))),
  flavorMatchingDefinition_(cfg.getUntrackedParameter("flavorMatchingDefinition", std::string("Algorithmic"))),
  pfCandMapName_(cfg.getUntrackedParameter("pfCandMapName", std::string("pfCandMapName"))),
  pfCandMap_(0),
  jetMapName_(cfg.getUntrackedParameter("jetMapName", std::string("PFJetMap"))),
  jetMap_(new mithep::PFJetMap),
  jets_(new mithep::PFJetArr(16))
{
  for (unsigned iA = 0; iA != mithep::Jet::nBTagAlgos; ++iA) {
    bJetTagsName_[iA] = cfg.getUntrackedParameter(mithep::Jet::BTagAlgoName(iA) + std::string("BJetTagsName"), std::string(""));
    if (!bJetTagsName_[iA].empty())
      bJetTagsToken_[iA] = collector.consumes<reco::JetTagCollection>(bJetTagsName_[iA]);
  }
}

template<class JET>
mithep::FillerPFJets<JET>::~FillerPFJets()
{
  delete jets_;
  delete jetMap_;
}

template<class JET>
void
mithep::FillerPFJets<JET>::BookDataBlock(mithep::TreeWriter& tws)
{
  // Add jets branch to tree.

  mithep::ObjectService& os = *OS();

  tws.AddBranch(mitName_, &jets_);
  os.add(jets_, mitName_);

  // if a map name has been given
  if (!pfCandMapName_.empty()) {
    // find the pf candidate map
    pfCandMap_ = os.get<PFCandidateMap>(pfCandMapName_);
    // if the map exists in the object service
    if (pfCandMap_)
      AddBranchDep(mitName_,pfCandMap_->GetBrName());
  }
  if (!jetMapName_.empty()) {
    jetMap_->SetBrName(mitName_);
    os.add(jetMap_, jetMapName_);
  }
}

template<class JET>
void
mithep::FillerPFJets<JET>::FillDataBlock(edm::Event const& event, edm::EventSetup const& setup)
{
  // Fill jets from edm collection into our collection.

  jets_->Delete();
  jetMap_->Reset();

  // handle for the Jet Collection
  edm::Handle<JetCollection> hJetProduct;
  GetProduct(edmToken_, hJetProduct, event);
  JetCollection const& inJets = *hJetProduct;

  // handle for rho
  double rho = 0.;
  if (fastJetCorrectionsActive_) {
    edm::Handle<double> hRho;
    GetProduct(rhoToken_, hRho, event);
    rho = *hRho;
  }

  // handles for jet flavour matching 
  edm::Handle<reco::JetMatchedPartonsCollection> hPartonMatchingProduct;  
  reco::JetMatchedPartonsCollection const* matchedPartons = 0;
  if (flavorMatchingActive_) {
    GetProduct(flavorMatchingByReferenceToken_, hPartonMatchingProduct, event);
    matchedPartons = hPartonMatchingProduct.product();
    assert(matchedPartons->size() == inJets.size());
  }

  if (bTaggingActive_)
    initBJetTags(event);

  // Handles to Jet-to-Vertex Association
  std::vector<double> const* JV_alpha = 0;
  std::vector<double> const* JV_beta = 0;

  if (jetToVertexActive_) {
    edm::Handle<std::vector<double> > alphaHandle;
    edm::Handle<std::vector<double> > betaHandle;
    GetProduct(jetToVertexAlphaToken_, alphaHandle, event); 
    GetProduct(jetToVertexBetaToken_, betaHandle, event);  
    JV_alpha = alphaHandle.product();
    JV_beta = betaHandle.product();
  }

  // Define Jet Correction Services
  const JetCorrector* correctorL2 = 0; 
  const JetCorrector* correctorL3 = 0; 
  if (jetCorrectionsActive_) {
    correctorL2 = JetCorrector::getJetCorrector(L2JetCorrectorName_,setup);
    correctorL3 = JetCorrector::getJetCorrector(L3JetCorrectorName_,setup);
  }

  // loop through all jets
  unsigned iJet = 0;
  for (auto&& inJet : inJets) {
    edm::Ref<JetCollection> jetRef(hJetProduct, iJet);
    reco::JetBaseRef jetBaseRef(jetRef);
    edm::Ptr<reco::Jet> jetPtr(hJetProduct, iJet);
    
    mithep::PFJet *jet = jets_->AddNew();

    jet->SetRawPtEtaPhiM(inJet.p4().pt(), inJet.p4().eta(), inJet.p4().phi(), inJet.p4().mass());

    // add to map
    jetMap_->Add(jetPtr,jet);

    // fill jet moments
    jet->SetSigmaEta(TMath::Sqrt(inJet.etaetaMoment()));
    jet->SetSigmaPhi(TMath::Sqrt(inJet.phiphiMoment()));

    // fill pfjet-specific quantities
    jet->SetChargedHadronEnergy(inJet.chargedHadronEnergy());
    jet->SetNeutralHadronEnergy(inJet.neutralHadronEnergy());
    jet->SetChargedEmEnergy(inJet.chargedEmEnergy());
    jet->SetChargedMuEnergy(inJet.chargedMuEnergy());
    jet->SetNeutralEmEnergy(inJet.neutralEmEnergy());
    jet->SetChargedMultiplicity(inJet.chargedMultiplicity());
    jet->SetNeutralMultiplicity(inJet.neutralMultiplicity());
    jet->SetMuonMultiplicity(inJet.muonMultiplicity());

    // compute alpha and beta parameter for jets if vertex matching active
    if (jetToVertexActive_) {
      jet->SetAlpha(JV_alpha->at(iJet));
      jet->SetBeta(JV_beta->at(iJet));
    }

    // fill the area anyway 
    jet->SetJetArea(inJet.jetArea());

    // jet corrections
    if (fastJetCorrectionsActive_) {
      double L1Scale = std::max((inJet.pt() - rho * inJet.jetArea()) / inJet.pt(), 0.);
      jet->SetL1OffsetCorrectionScale(L1Scale);
      jet->EnableCorrection(mithep::Jet::L1);
    }
    if (jetCorrectionsActive_) {
      double L2Scale = correctorL2->correction(inJet.p4());
      double L3Scale = correctorL3->correction(inJet.p4()*L2Scale);
      jet->SetL2RelativeCorrectionScale(L2Scale);
      jet->SetL3AbsoluteCorrectionScale(L3Scale);     
      jet->EnableCorrection(mithep::Jet::L2);
      jet->EnableCorrection(mithep::Jet::L3);     
    }

    if (bTaggingActive_)
      setBJetTags(jetBaseRef, *jet);

    // get the Monte Carlo flavour matching information
    if (flavorMatchingActive_) {
      reco::MatchedPartons const& matchedParton = (*matchedPartons)[jetBaseRef];
      int flavorPhysDef = (matchedParton.physicsDefinitionParton().isNonnull())?
        matchedParton.physicsDefinitionParton()->pdgId():0;
      int flavorAlgDef = (matchedParton.algoDefinitionParton().isNonnull())?
        matchedParton.algoDefinitionParton()->pdgId():0;
      
      if (flavorMatchingDefinition_ == "Algorithmic") {
        jet->SetMatchedMCFlavor(flavorAlgDef);
      }
      else if (flavorMatchingDefinition_ == "Physics") {
        jet->SetMatchedMCFlavor(flavorPhysDef);
      }
      else {
        jet->SetMatchedMCFlavor(0);
      }
    }

    // add PFCandidate refs
    if (pfCandMap_) {
      for (unsigned iD = 0; iD != inJet.numberOfDaughters(); ++iD) {
        mithep::PFCandidate* pfCand = pfCandMap_->GetMit(inJet.daughterPtr(iD), false);
        if (pfCand)
          jet->AddPFCand(pfCand);
        else {
          // First two daughters of the AK8 jets in miniAOD are Soft Drop subjets.
          // Being a bit general here..
          reco::Jet const* subJet = dynamic_cast<reco::Jet const*>(inJet.daughter(iD));
          if (!subJet)
            throw edm::Exception(edm::errors::LogicError, "FillerPFJets::FillDataBlock()")
              << "Daughter " << iD << " not in the list of PF Candidates but not a subjet";

          for (unsigned iSD = 0; iSD != subJet->numberOfDaughters(); ++iSD)
            jet->AddPFCand(pfCandMap_->GetMit(subJet->daughterPtr(iSD)));
        }
      }
    }
    
    ++iJet;
  }      
  jets_->Trim();
}

template<class JET>
void
mithep::FillerPFJets<JET>::initBJetTags(edm::Event const&)
{
}

template<class JET>
void
mithep::FillerPFJets<JET>::setBJetTags(reco::JetBaseRef const&, mithep::PFJet&) const
{
}

#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/PatCandidates/interface/Jet.h"

namespace mithep {

  template<>
  void
  FillerPFJets<reco::PFJet>::initBJetTags(edm::Event const& event)
  {
    for (unsigned iT = 0; iT != mithep::Jet::nBTagAlgos; ++iT) {
      edm::Handle<reco::JetTagCollection> hBJetTags;
      GetProduct(bJetTagsToken_[iT], hBJetTags, event);
      bJetTags_[iT] = hBJetTags.product();
    }
  }

  template<>
  void
  FillerPFJets<reco::PFJet>::setBJetTags(reco::JetBaseRef const& inJetRef, mithep::PFJet& outJet) const
  {
    reco::PFJet const& inJet = static_cast<reco::PFJet const&>(*inJetRef);

    for (unsigned iT = 0; iT != mithep::Jet::nBTagAlgos; ++iT)
      outJet.SetBJetTagsDisc((*bJetTags_[iT])[inJetRef], iT);
  }

  template<>
  void
  FillerPFJets<pat::Jet>::setBJetTags(reco::JetBaseRef const& inJetRef, mithep::PFJet& outJet) const
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

