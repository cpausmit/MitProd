//--------------------------------------------------------------------------------------------------
// FillerJets
//
// Template for fillers of various jet types
//
// Authors: Y.Iiyama
//--------------------------------------------------------------------------------------------------

#ifndef FILLERJETS_INSTANCE
#warning This header is protected from accidental inclusions.
#abort // there is no such preprocessor - will therefore abort.
#endif

#ifndef MITPROD_TREEFILLER_FILLERJETS_H
#define MITPROD_TREEFILLER_FILLERJETS_H

#include "MitProd/TreeFiller/interface/AssociationMap.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"
#include "MitProd/ObjectService/interface/ObjectService.h"
#include "MitAna/DataTree/interface/Jet.h"
#include "MitAna/DataCont/interface/Array.h"

#include "DataFormats/JetReco/interface/JetCollection.h"
#include "DataFormats/Common/interface/View.h"
#include "DataFormats/BTauReco/interface/JetTag.h"
#include "SimDataFormats/JetMatching/interface/JetMatchedPartons.h"
#include "SimDataFormats/JetMatching/interface/MatchedPartons.h"
#include "JetMETCorrections/Objects/interface/JetCorrector.h"

namespace mithep {

  template<class MITJET>
  class FillerJets : public BaseFiller {  
  public:
    FillerJets(edm::ParameterSet const&, edm::ConsumesCollector&, ObjectService*, char const* name, bool active = true);
    ~FillerJets();

    void BookDataBlock(TreeWriter&) override;
    virtual void BookAdditional(TreeWriter&) {}
    void FillDataBlock(edm::Event const&, edm::EventSetup const&) override;
    virtual void PrepareSpecific(edm::Event const&, edm::EventSetup const&) {}
    virtual void FillSpecific(MITJET&, reco::JetBaseRef const&) {}

    typedef mithep::AssociationMap<edm::Ptr<reco::Jet>, MITJET*> JetAssociationMap;
    typedef mithep::Array<MITJET> ProductArray;
  
  protected:
    enum FlavorMatchingDef {
      kAlgorithmic,
      kPhysics,
      nFlavorMatchingDefs
    };

    virtual void initBJetTags(edm::Event const&, reco::JetTagCollection const* [mithep::Jet::nBTagAlgos]);
    virtual void setBJetTags(mithep::Jet&, reco::JetBaseRef const&, reco::JetTagCollection const* [mithep::Jet::nBTagAlgos]) const;
    virtual void initCorrections(edm::Event const&, edm::EventSetup const&);
    virtual void setCorrections(mithep::Jet&, reco::Jet const&);

    bool flavorMatchingActive_;     //=true if flavor matching is done  
    bool bTaggingActive_;           //=true if bTagging info is filled
    bool jetToVertexActive_;        //=true if jet to vertex info is done
    bool fastJetCorrectionsActive_; //=true if L1 corrections are done
    bool jetCorrectionsActive_;     //=true if L2 & L3 corrections are done

    edm::EDGetTokenT<reco::JetView> edmToken_; //edm name of jets collection
    edm::EDGetTokenT<std::vector<double> > jetToVertexAlphaToken_; //edm name of jet to vertex alpha coll
    edm::EDGetTokenT<std::vector<double> > jetToVertexBetaToken_; //edm name of jet to vertex beta coll
    edm::EDGetTokenT<reco::JetMatchedPartonsCollection> flavorMatchingByReferenceToken_; //source of flavor matching
    edm::EDGetTokenT<reco::JetTagCollection> bJetTagsToken_[mithep::Jet::nBTagAlgos]; //bjet algo discriminant
    edm::EDGetTokenT<double> rhoToken_; //pileup energy density

    std::string mitName_;                         //mit name of jets collection
    std::string L2JetCorrectorName_;              //label of the L2JetCorrection service
    std::string L3JetCorrectorName_;              //label of the L3JetCorrection service
    std::string jetMapName_;                      //name of exported PFJetMap
    FlavorMatchingDef flavorMatchingDefinition_;        //type of flavor matching
    double rho_;
    JetCorrector const* L2Corrector_;
    JetCorrector const* L3Corrector_;

    JetAssociationMap* jetMap_;      //export map
    ProductArray* jets_;        //array of Jets
  };

}

template<class MITJET>
mithep::FillerJets<MITJET>::FillerJets(edm::ParameterSet const& cfg, edm::ConsumesCollector& collector, ObjectService* os, char const* name, bool active/* = true*/) :
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
  jetMap_(new JetAssociationMap),
  jets_(new ProductArray(16))
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

template<class MITJET>
mithep::FillerJets<MITJET>::~FillerJets()
{
  delete jetMap_;
  delete jets_;
}

template<class MITJET>
void
mithep::FillerJets<MITJET>::BookDataBlock(TreeWriter& tws)
{
  tws.AddBranch(mitName_, &jets_);
  OS()->add(jets_, mitName_);

  if (!jetMapName_.empty()) {
    jetMap_->SetBrName(mitName_);
    OS()->add(jetMap_, jetMapName_);
  }

  BookAdditional(tws);
}

template<class MITJET>
void
mithep::FillerJets<MITJET>::FillDataBlock(edm::Event const& event, edm::EventSetup const& setup)
{
  jets_->Delete();
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
    
    auto* outJet = jets_->AddNew();

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

template<class MITJET>
void
mithep::FillerJets<MITJET>::initBJetTags(edm::Event const& event,
                                         reco::JetTagCollection const* bJetTags[mithep::Jet::nBTagAlgos])
{
  for (unsigned iT = 0; iT != mithep::Jet::nBTagAlgos; ++iT) {
    edm::Handle<reco::JetTagCollection> hBJetTags;
    if (GetProductSafe(bJetTagsToken_[iT], hBJetTags, event))
      bJetTags[iT] = hBJetTags.product();
  }
}

template<class MITJET>
void
mithep::FillerJets<MITJET>::setBJetTags(mithep::Jet& outJet, reco::JetBaseRef const& baseRef,
                                        reco::JetTagCollection const* bJetTags[mithep::Jet::nBTagAlgos]) const
{
  for (unsigned iT = 0; iT != mithep::Jet::nBTagAlgos; ++iT) {
    if (bJetTags[iT])
      outJet.SetBJetTagsDisc((*bJetTags[iT])[baseRef], iT);
  }
}

template<class MITJET>
void
mithep::FillerJets<MITJET>::initCorrections(edm::Event const& event, edm::EventSetup const& setup)
{
  edm::Handle<double> hRho;
  GetProduct(rhoToken_, hRho, event);
  rho_ = *hRho;

  L2Corrector_ = JetCorrector::getJetCorrector(L2JetCorrectorName_, setup);
  L3Corrector_ = JetCorrector::getJetCorrector(L3JetCorrectorName_, setup);
}

template<class MITJET>
void
mithep::FillerJets<MITJET>::setCorrections(mithep::Jet& outJet, reco::Jet const&)
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

#endif
