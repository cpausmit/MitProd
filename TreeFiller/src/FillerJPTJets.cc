#include "MitProd/TreeFiller/interface/FillerJPTJets.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"
#include "DataFormats/JetReco/interface/JPTJet.h"
#include "SimDataFormats/JetMatching/interface/JetFlavour.h"
#include "SimDataFormats/JetMatching/interface/JetFlavourMatching.h"
#include "SimDataFormats/JetMatching/interface/MatchedPartons.h"
#include "JetMETCorrections/Objects/interface/JetCorrector.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataTree/interface/CaloJetCol.h"
#include "MitEdm/DataFormats/interface/RefToBaseToPtr.h"
#include "MitProd/ObjectService/interface/ObjectService.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerJPTJets::FillerJPTJets(const ParameterSet &cfg, edm::ConsumesCollector& collector, ObjectService* os, const char *name, bool active) : 
  BaseFiller(cfg,os,name,active),
  flavorMatchingActive_(Conf().getUntrackedParameter<bool>("flavorMatchingActive",true)),
  bTaggingActive_(Conf().getUntrackedParameter<bool>("bTaggingActive",true)),
  jetToVertexActive_(Conf().getUntrackedParameter<bool>("jetToVertexActive",true)),
  jetCorrectionsActive_(Conf().getUntrackedParameter<bool>("jetCorrectionsActive",true)),
  edmToken_(GetToken<reco::JPTJetCollection>(collector, "edmName", "recoJPTJets:iterativeCone5JPTJets:RECO")),
  edmFallbackToken_(GetToken<reco::JPTJetCollection>(collector, "edmFallbackName", "recoJPTJets:iterativeCone5JPTJets")),
  jetToVertexAlphaToken_(GetToken<std::vector<double> >(collector,"jetToVertexAlphaName","jetToVertexAlpha")),
  jetToVertexBetaToken_(GetToken<std::vector<double> >(collector,"jetToVertexBetaName","jetToVertexBetaName")),
  flavorMatchingByReferenceToken_(GetToken<reco::JetMatchedPartonsCollection>(collector,"flavorMatchingByReferenceName","srcByReference")),
  bJetTagsToken_{
    GetToken<reco::JetTagCollection>(collector, "JetProbabilityBJetTagsName", ""),
    GetToken<reco::JetTagCollection>(collector, "JetBProbabilityBJetTagsName", ""),
    GetToken<reco::JetTagCollection>(collector, "SimpleSecondaryVertexHighEffBJetTagsName", ""),
    GetToken<reco::JetTagCollection>(collector, "SimpleSecondaryVertexHighPurBJetTagsName", ""),
    GetToken<reco::JetTagCollection>(collector, "CombinedSecondaryVertexBJetTagsName", ""),
    GetToken<reco::JetTagCollection>(collector, "CombinedSecondaryVertexV2BJetTagsName", ""),
    GetToken<reco::JetTagCollection>(collector, "CombinedSecondaryVertexSoftLeptonBJetTagsName", ""),
    GetToken<reco::JetTagCollection>(collector, "CombinedInclusiveSecondaryVertexV2BJetTagsName", ""),
    GetToken<reco::JetTagCollection>(collector, "CombinedMVABJetTagsName", ""),
    GetToken<reco::JetTagCollection>(collector, "TrackCountingHighEffBJetTagsName", ""),
    GetToken<reco::JetTagCollection>(collector, "TrackCountingHighPurBJetTagsName", "")
  },
  mitName_(Conf().getUntrackedParameter<string>("mitName","ItrCone5JPTJets")), 
  flavorMatchingDefinition_(Conf().getUntrackedParameter<string>
                   ("flavorMatchingDefinition","Algorithmic")),
  L2JetCorrectorName_(Conf().getUntrackedParameter<string>
                      ("L2JetCorrectorName","L2JetCorrectorName")),
  L3JetCorrectorName_(Conf().getUntrackedParameter<string>
                      ("L3JetCorrectorName","L3JetCorrectorName")),
  caloJetMapName_(Conf().getUntrackedParameter<string>("caloJetCandMapName","caloJetMapName")),
  jetMapName_(Conf().getUntrackedParameter<string>("jetMapName","JPTJetMap")),
  caloJetMap_(0),
  jetMap_(new mithep::JPTJetMap),
  jets_(new mithep::JPTJetArr(16))
{
  // Constructor.
}

//--------------------------------------------------------------------------------------------------
FillerJPTJets::~FillerJPTJets()
{
  // Destructor.

  delete jets_;
  delete jetMap_;
}

//--------------------------------------------------------------------------------------------------
void FillerJPTJets::BookDataBlock(TreeWriter &tws)
{
  // Add jets branch to tree.

  tws.AddBranch(mitName_,&jets_);
  OS()->add(jets_, mitName_);

  if (!caloJetMapName_.empty()) {
    caloJetMap_ = OS()->get<CaloJetMap>(caloJetMapName_);
    if (caloJetMap_)
      AddBranchDep(mitName_,caloJetMap_->GetBrName());
  }
  if (!jetMapName_.empty()) {
    jetMap_->SetBrName(mitName_);
    OS()->add(jetMap_, jetMapName_);
  }
}

//--------------------------------------------------------------------------------------------------
void FillerJPTJets::FillDataBlock(const edm::Event      &event, 
                                 const edm::EventSetup &setup)
{
  // Fill jets from edm collection into our collection.

  jets_->Delete();
  jetMap_->Reset();
  
  //ugly hack to get collection from original reco if present (needed to handle things 
  //automatically between 35x and post 35x samples in the correct way)
  //Re-hacked 19.03.2015 by Y.I.
  
  // handle for the Jet Collection
  Handle<reco::JPTJetCollection> hJetProduct;
  if(!GetProductSafe(edmToken_, hJetProduct, event))
     GetProduct(edmFallbackToken_, hJetProduct, event);
  
  // handles for jet flavour matching 
  Handle<reco::JetMatchedPartonsCollection> hPartonMatchingProduct;  
  if (flavorMatchingActive_) 
    GetProduct(flavorMatchingByReferenceToken_, hPartonMatchingProduct, event);

  reco::JetTagCollection const* bJetTags[mithep::Jet::nBTagAlgos];

  if (bTaggingActive_) {
    for (unsigned iT = 0; iT != mithep::Jet::nBTagAlgos; ++iT) {
      edm::Handle<reco::JetTagCollection> hBJetTags;
      GetProductSafe(bJetTagsToken_[iT], hBJetTags, event);
      if (hBJetTags.isValid())
        bJetTags[iT] = hBJetTags.product();
      else
        bJetTags[iT] = 0;
    }
  }
  
  reco::JPTJetCollection const& inJets = *hJetProduct;

  //Handles to Jet to Vertex Association
  Handle<std::vector<double> > JV_alpha;  
  Handle<std::vector<double> > JV_beta;    
  std::vector<double>::const_iterator it_jv_alpha;
  std::vector<double>::const_iterator it_jv_beta;

  if (jetToVertexActive_) {
    GetProduct(jetToVertexAlphaToken_, JV_alpha, event); 
    GetProduct(jetToVertexBetaToken_, JV_beta, event);  
    it_jv_alpha = JV_alpha->begin();
    it_jv_beta  = JV_beta->begin();    
  }

  //Define Jet Correction Services
  const JetCorrector* correctorL2 = 0; 
  const JetCorrector* correctorL3 = 0; 
  if (jetCorrectionsActive_) {
    correctorL2 = JetCorrector::getJetCorrector (L2JetCorrectorName_,setup);
    correctorL3 = JetCorrector::getJetCorrector (L3JetCorrectorName_,setup);
  }

  // loop through all jets
  for (reco::JPTJetCollection::const_iterator inJet = inJets.begin(); 
       inJet != inJets.end(); ++inJet) {
    
    reco::JPTJetRef jetRef(hJetProduct, inJet - inJets.begin());    
    reco::JetBaseRef jetBaseRef(jetRef);
    
    mithep::JPTJet *jet = jets_->Allocate();
    new (jet) mithep::JPTJet(inJet->p4().x(),
                          inJet->p4().y(),
                          inJet->p4().z(),
                          inJet->p4().e());

    //add to map
    edm::Ptr<reco::Jet> thePtr(hJetProduct, inJet - inJets.begin());
    jetMap_->Add(thePtr, jet);

    //fill jet moments
    jet->SetSigmaEta(TMath::Sqrt(inJet->etaetaMoment()));
    jet->SetSigmaPhi(TMath::Sqrt(inJet->phiphiMoment()));


    //fill jptjet-specific quantities
    jet->SetZSPCor(inJet->getZSPCor());
    jet->SetChargedHadronEnergy(inJet->chargedHadronEnergy());
    jet->SetNeutralHadronEnergy(inJet->neutralHadronEnergy());
    jet->SetChargedEmEnergy(inJet->chargedHadronEnergy());
    jet->SetNeutralEmEnergy(inJet->neutralEmEnergy());
    jet->SetResponseOfChargedWithEff(inJet->getSpecific().mResponseOfChargedWithEff);
    jet->SetResponseOfChargedWithoutEff(inJet->getSpecific().mResponseOfChargedWithoutEff);
    jet->SetSumPtOfChargedWithEff(inJet->getSpecific().mSumPtOfChargedWithEff);
    jet->SetSumPtOfChargedWithoutEff(inJet->getSpecific().mSumPtOfChargedWithoutEff);
    jet->SetSumEnergyOfChargedWithEff(inJet->getSpecific().mSumEnergyOfChargedWithEff);
    jet->SetSumEnergyOfChargedWithoutEff(inJet->getSpecific().mSumEnergyOfChargedWithoutEff);
    jet->SetR2momtr(inJet->getSpecific().R2momtr);
    jet->SetEta2momtr(inJet->getSpecific().Eta2momtr);
    jet->SetPhi2momtr(inJet->getSpecific().Phi2momtr);
    jet->SetPout(inJet->getSpecific().Pout);
    jet->SetZch(inJet->getSpecific().Zch);
    jet->SetChargedMultiplicity(inJet->chargedMultiplicity());
    jet->SetMuonMultiplicity(inJet->muonMultiplicity());
    jet->SetElectronMultiplicity(inJet->elecMultiplicity());
     
    if (jetToVertexActive_) {
      //compute alpha and beta parameter for jets
      jet->SetAlpha((*it_jv_alpha));
      jet->SetBeta((*it_jv_beta));      
    }

    //Jet Corrections
    if (jetCorrectionsActive_) {
      double L2Scale = correctorL2->correction(inJet->p4());
      double L3Scale = correctorL3->correction(inJet->p4()*L2Scale);
      jet->SetL2RelativeCorrectionScale(L2Scale);
      jet->SetL3AbsoluteCorrectionScale(L3Scale);     
      jet->EnableCorrection(mithep::Jet::L2);
      jet->EnableCorrection(mithep::Jet::L3);     
    }
    
    if (bTaggingActive_) {
      for (unsigned iT = 0; iT != mithep::Jet::nBTagAlgos; ++iT) {
        if (bJetTags[iT])
          jet->SetBJetTagsDisc((*bJetTags[iT])[jetBaseRef], iT);
      }
    }

    // get the Monte Carlo flavour matching information
    if (flavorMatchingActive_) {
      unsigned int iJet = inJet - inJets.begin();
      const reco::JetMatchedPartonsCollection *matchedPartons = hPartonMatchingProduct.product();
      reco::MatchedPartons matchedParton = (*matchedPartons)[matchedPartons->key(iJet)];
      int flavorPhysDef = (matchedParton.physicsDefinitionParton().isNonnull())?
        matchedParton.physicsDefinitionParton()->pdgId():0;
      int flavorAlgDef = (matchedParton.algoDefinitionParton().isNonnull())?
        matchedParton.algoDefinitionParton()->pdgId():0;
      
      if (flavorMatchingDefinition_ == "Algorithmic") {
        jet->SetMatchedMCFlavor(flavorAlgDef);
      } else if(flavorMatchingDefinition_ == "Physics") {
        jet->SetMatchedMCFlavor(flavorPhysDef);
      } else {
        jet->SetMatchedMCFlavor(0);
      }
    }

    // fill ref to original jet
    if (caloJetMap_ && inJet->getCaloJetRef().isNonnull()) {
      const edm::Ptr<reco::Jet> caloJetPtr(mitedm::refToBaseToPtr(inJet->getCaloJetRef()));
      const mithep::Jet *originalJet = caloJetMap_->GetMit(caloJetPtr);
      jet->SetOriginalJet(originalJet);
    }
    if (jetToVertexActive_) {
      it_jv_alpha++; 
      it_jv_beta++;
    }
  }      
  jets_->Trim();
}

DEFINE_MITHEP_TREEFILLER(FillerJPTJets);
