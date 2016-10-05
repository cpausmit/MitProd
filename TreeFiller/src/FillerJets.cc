#include "MitProd/TreeFiller/interface/FillerJets.h"

#include "SimDataFormats/JetMatching/interface/MatchedPartons.h"

mithep::FillerJets::FillerJets(edm::ParameterSet const& cfg, edm::ConsumesCollector& collector, ObjectService* os, char const* name, bool active/* = true*/) :
  BaseFiller(cfg, os, name, active),
  flavorMatchingActive_(cfg.getUntrackedParameter<bool>("flavorMatchingActive", true)),
  bTaggingActive_(cfg.getUntrackedParameter<bool>("bTaggingActive", true)),
  jetToVertexActive_(cfg.getUntrackedParameter<bool>("jetToVertexActive", true)),
  jetCorrectionsActive_(cfg.getUntrackedParameter<bool>("jetCorrectionsActive", true)),
  edmToken_(GetToken<reco::JetView>(collector, cfg, "edmName")),
  jetToVertexAlphaToken_(GetToken<std::vector<double> >(collector, cfg, "jetToVertexAlphaName", jetToVertexActive_)), //jetToVertexAlpha
  jetToVertexBetaToken_(GetToken<std::vector<double> >(collector, cfg, "jetToVertexBetaName", jetToVertexActive_)), //jetToVertexBetaName
  flavorMatchingByReferenceToken_(GetToken<reco::JetMatchedPartonsCollection>(collector, cfg, "flavorMatchingByReferenceName", flavorMatchingActive_)), //srcByReference
  bJetTagsToken_{},
  rhoToken_(GetToken<double>(collector, cfg, "rhoName", jetCorrectionsActive_)),
  mitName_(cfg.getUntrackedParameter<string>("mitName")),
  L2JetCorrectorName_(cfg.getUntrackedParameter<std::string>("L2JetCorrectorName", "L2JetCorrectorName")),
  L3JetCorrectorName_(cfg.getUntrackedParameter<std::string>("L3JetCorrectorName", "L3JetCorrectorName")),
  jetMapName_(cfg.getUntrackedParameter<string>("jetMapName")),
  flavorMatchingDefinition_{},
  rho_(0.),
  L2Corrector_(0),
  L3Corrector_(0),
  jetMap_(new JetMap),
  jets_(0)
{
  for (unsigned iA = 0; iA != mithep::Jet::nBTagAlgos; ++iA) {
    std::string paramName(mithep::Jet::BTagAlgoName(iA) + std::string("BJetTagsName"));
    bJetTagsToken_[iA] = GetToken<reco::JetTagCollection>(collector, cfg, paramName, false);
  }

  std::string defName(cfg.getUntrackedParameter<std::string>("flavorMatchingDefinition", "Algorithmic"));
  if (defName == "Algorithmic")
    flavorMatchingDefinition_ = kAlgorithmic;
  else if (defName == "Physics")
    flavorMatchingDefinition_ = kPhysics;
  else
    throw edm::Exception(edm::errors::Configuration, "FillerJets::Ctor")
      << "Invalid flavor matching definition";
}

mithep::FillerJets::~FillerJets()
{
  delete jetMap_;
  delete jets_;
}

void
mithep::FillerJets::BookDataBlock(TreeWriter& tws)
{
  tws.AddBranch(mitName_, &jets_);
  OS()->add(jets_, mitName_);

  if (!jetMapName_.empty()) {
    jetMap_->SetBrName(mitName_);
    OS()->add(jetMap_, jetMapName_);
  }

  BookAdditional(tws);

  // Set "MustDelete" bit of Array so that Reset() in the FillDataBlock will flush the content
  jets_->SetBit(15);
}

void
mithep::FillerJets::FillDataBlock(edm::Event const& event, edm::EventSetup const& setup)
{
  // with MustDelete bit set, Reset() is equivalent to Delete()
  jets_->Reset();
  jetMap_->Reset();

  edm::Handle<reco::JetView> hJetsH;
  GetProduct(edmToken_, hJetsH, event);
  auto& inJets = *hJetsH;

  // handles for jet flavour matching
  reco::JetMatchedPartonsCollection const* matchedPartons = 0;
  if (flavorMatchingActive_) {
    edm::Handle<reco::JetMatchedPartonsCollection> hMatchH;
    GetProduct(flavorMatchingByReferenceToken_, hMatchH, event);
    matchedPartons = hMatchH.product();
  }

  reco::JetTagCollection const* bJetTags[mithep::Jet::nBTagAlgos] = {};

  if (bTaggingActive_)
    initBJetTags(event, bJetTags);

  if (jetCorrectionsActive_)
    initCorrections(event, setup);

  std::vector<double> const* jvAlpha = 0;
  std::vector<double> const* jvBeta = 0;
  if (jetToVertexActive_) {
    edm::Handle<std::vector<double>> jvAlphaH;
    edm::Handle<std::vector<double>> jvBetaH;
    GetProduct(jetToVertexAlphaToken_, jvAlphaH, event);
    GetProduct(jetToVertexBetaToken_, jvBetaH, event);
    jvAlpha = jvAlphaH.product();
    jvBeta = jvBetaH.product();
  }

  PrepareSpecific(event, setup);

  // loop through all jets
  for (unsigned iJ = 0; iJ != inJets.size(); ++iJ) {
    auto& inJet = inJets.at(iJ);
    auto&& baseRef = inJets.refAt(iJ);

    mithep::Jet* outJet = AddNew();

    jetMap_->Add(inJets.ptrAt(iJ), outJet);

    // for pat::Jets, p4() is already corrected - fixed in FillSpecific
    outJet->SetRawPtEtaPhiM(inJet.p4().pt(), inJet.p4().eta(), inJet.p4().phi(), inJet.p4().mass());

    //fill jet moments
    outJet->SetSigmaEta(TMath::Sqrt(inJet.etaetaMoment()));
    outJet->SetSigmaPhi(TMath::Sqrt(inJet.phiphiMoment()));

    //fill jet area
    outJet->SetJetArea(inJet.jetArea());

    FillSpecific(*outJet, baseRef);

    if (jetToVertexActive_) {
      //compute alpha and beta parameter for jets
      outJet->SetAlpha(jvAlpha->at(iJ));
      outJet->SetBeta(jvBeta->at(iJ));
    }

    if (bTaggingActive_)
      setBJetTags(*outJet, baseRef, bJetTags);

    if (jetCorrectionsActive_)
      setCorrections(*outJet, inJet);

    // get the Monte Carlo flavour matching information
    if (flavorMatchingActive_) {
      auto&& matchedParton = (*matchedPartons)[matchedPartons->key(iJ)];

      outJet->SetMatchedMCFlavor(0);
      switch (flavorMatchingDefinition_) {
      case kAlgorithmic:
        if (matchedParton.algoDefinitionParton().isNonnull())
          outJet->SetMatchedMCFlavor(matchedParton.algoDefinitionParton()->pdgId());
        break;

      case kPhysics:
        if (matchedParton.physicsDefinitionParton().isNonnull())
          outJet->SetMatchedMCFlavor(matchedParton.physicsDefinitionParton()->pdgId());
        break;

      default:
        break;
      }
    }
  }

  jets_->Trim();
}

void
mithep::FillerJets::FillPostRunBlock(edm::Run const&, edm::EventSetup const&)
{
  // Unset the "MustDelete" bit before writing the branch to the tree
  jets_->ResetBit(15);
}

void
mithep::FillerJets::initBJetTags(edm::Event const& event, reco::JetTagCollection const* bJetTags[mithep::Jet::nBTagAlgos])
{
  for (unsigned iT = 0; iT != mithep::Jet::nBTagAlgos; ++iT) {
    if (!bJetTagsToken_[iT].isUninitialized()) {
      edm::Handle<reco::JetTagCollection> hBJetTags;
      GetProduct(bJetTagsToken_[iT], hBJetTags, event);
      bJetTags[iT] = hBJetTags.product();
    }
  }
}

void
mithep::FillerJets::setBJetTags(mithep::Jet& outJet, reco::JetBaseRef const& baseRef, reco::JetTagCollection const* bJetTags[mithep::Jet::nBTagAlgos]) const
{
  for (unsigned iT = 0; iT != mithep::Jet::nBTagAlgos; ++iT) {
    if (bJetTags[iT])
      outJet.SetBJetTagsDisc((*bJetTags[iT])[baseRef], iT);
  }
}

void
mithep::FillerJets::initCorrections(edm::Event const& event, edm::EventSetup const& setup)
{
  edm::Handle<double> hRho;
  GetProduct(rhoToken_, hRho, event);
  rho_ = *hRho;

  L2Corrector_ = JetCorrector::getJetCorrector(L2JetCorrectorName_, setup);
  L3Corrector_ = JetCorrector::getJetCorrector(L3JetCorrectorName_, setup);
}

void
mithep::FillerJets::setCorrections(mithep::Jet& outJet, reco::Jet const&)
{
  // jet corrections

  double L1Scale = std::max((outJet.RawMom().pt() - rho_ * outJet.JetArea()) / outJet.RawMom().pt(), 0.);
  outJet.SetL1OffsetCorrectionScale(L1Scale);
  outJet.EnableCorrection(mithep::Jet::L1);

  FourVector p4(outJet.RawMom().Px(), outJet.RawMom().Py(), outJet.RawMom().Pz(), outJet.RawMom().E());

  p4 *= L1Scale;

  double L2Scale = L2Corrector_->correction(p4);
  outJet.SetL2RelativeCorrectionScale(L2Scale);
  outJet.EnableCorrection(mithep::Jet::L2);

  p4 *= L2Scale;

  double L3Scale = L3Corrector_->correction(p4);
  outJet.SetL3AbsoluteCorrectionScale(L3Scale);
  outJet.EnableCorrection(mithep::Jet::L3);
}
