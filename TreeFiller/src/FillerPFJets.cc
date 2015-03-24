#include "MitProd/TreeFiller/interface/FillerPFJets.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"
#include "DataFormats/JetReco/interface/PFJet.h"
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

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerPFJets::FillerPFJets(const ParameterSet &cfg, edm::ConsumesCollector& collector, ObjectService* os, const char *name, bool active) : 
  BaseFiller(cfg,os,name,active),
  flavorMatchingActive_(Conf().getUntrackedParameter<bool>("flavorMatchingActive",true)),
  bTaggingActive_(Conf().getUntrackedParameter<bool>("bTaggingActive",true)),
  jetToVertexActive_(Conf().getUntrackedParameter<bool>("jetToVertexActive",true)),
  jetCorrectionsActive_(Conf().getUntrackedParameter<bool>("jetCorrectionsActive",true)),
  fastJetCorrectionsActive_(Conf().getUntrackedParameter<bool>("fastJetCorrectionsActive",false)),
  edmToken_(GetToken<reco::PFJetCollection>(collector, "edmName","recoPFJets:iterativeCone5PFJets")),
  jetToVertexAlphaToken_(GetToken<std::vector<double> >(collector, "jetToVertexAlphaName","jetToVertexAlpha")),
  jetToVertexBetaToken_(GetToken<std::vector<double> >(collector, "jetToVertexBetaName","jetToVertexBetaName")),
  rhoToken_(GetToken<double>(collector, "rhoName")),
  flavorMatchingByReferenceToken_(GetToken<reco::JetMatchedPartonsCollection>(collector, "flavorMatchingByReferenceName","srcByReference")),
  jetProbabilityBJetTagsToken_(GetToken<reco::JetTagCollection>(collector, "JetProbabilityBJetTagsName","jetProbabilityBJetTags")),  
  jetBProbabilityBJetTagsToken_(GetToken<reco::JetTagCollection>(collector, "JetBProbabilityBJetTagsName","jetBProbabilityBJetTags")),   
  simpleSecondaryVertexHighEffBJetTagsToken_(GetToken<reco::JetTagCollection>(collector, "SimpleSecondaryVertexHighEffBJetTagsName","simpleSecondaryVertexHighEffBJetTags")),                   
  simpleSecondaryVertexHighPurBJetTagsToken_(GetToken<reco::JetTagCollection>(collector, "SimpleSecondaryVertexHighPurBJetTagsName","simpleSecondaryVertexHighPurBJetTags")),                   
  combinedSecondaryVertexBJetTagsToken_(GetToken<reco::JetTagCollection>(collector, "CombinedSecondaryVertexBJetTagsName","combinedSecondaryVertexBJetTags")),
  combinedSecondaryVertexMVABJetTagsToken_(GetToken<reco::JetTagCollection>(collector, "CombinedSecondaryVertexMVABJetTagsName","combinedSecondaryVertexMVABJetTags")),
  trackCountingHighEffBJetTagsToken_(GetToken<reco::JetTagCollection>(collector, "TrackCountingHighEffBJetTagsName","trackCountingHighEffBJetTags")),
  trackCountingHighPurBJetTagsToken_(GetToken<reco::JetTagCollection>(collector, "TrackCountingHighPurBJetTagsName","trackCountingHighPurBJetTags")),
  mitName_(Conf().getUntrackedParameter<string>("mitName","ItrCone5PFJets")), 
  L2JetCorrectorName_(Conf().getUntrackedParameter<string>
                      ("L2JetCorrectorName","L2JetCorrectorName")),
  L3JetCorrectorName_(Conf().getUntrackedParameter<string>
                      ("L3JetCorrectorName","L3JetCorrectorName")),
  flavorMatchingDefinition_(Conf().getUntrackedParameter<string>
                   ("flavorMatchingDefinition","Algorithmic")),
  pfCandMapName_(Conf().getUntrackedParameter<string>("pfCandMapName","pfCandMapName")),
  pfCandMap_(0),
  jetMapName_(Conf().getUntrackedParameter<string>("jetMapName","PFJetMap")),
  jetMap_(new mithep::PFJetMap),
  jets_(new mithep::PFJetArr(16))
{
  // Constructor.
}

//--------------------------------------------------------------------------------------------------
FillerPFJets::~FillerPFJets()
{
  // Destructor.

  delete jets_;
  delete jetMap_;
}

//--------------------------------------------------------------------------------------------------
void FillerPFJets::BookDataBlock(TreeWriter &tws)
{
  // Add jets branch to tree.

  tws.AddBranch(mitName_,&jets_);
  OS()->add<mithep::PFJetArr>(jets_,mitName_);

  // if a map name has been given
  if (!pfCandMapName_.empty()) {
    // find the pf candidate map
    pfCandMap_ = OS()->get<PFCandidateMap>(pfCandMapName_);
    // if the map exists in the object service
    if (pfCandMap_) {
      AddBranchDep(mitName_,pfCandMap_->GetBrName());
    }
  }
  if (!jetMapName_.empty()) {
    jetMap_->SetBrName(mitName_);
    OS()->add<PFJetMap>(jetMap_,jetMapName_);
  }
}

//--------------------------------------------------------------------------------------------------
void FillerPFJets::FillDataBlock(const edm::Event      &event, 
                                 const edm::EventSetup &setup)
{
  // Fill jets from edm collection into our collection.

  jets_->Delete();
  jetMap_->Reset();

  // handle for the Jet Collection
  Handle<reco::PFJetCollection> hJetProduct;
  GetProduct(edmToken_, hJetProduct, event);
  // handle for rho
  Handle<double> rho;
  GetProduct(rhoToken_, rho, event);

  // handles for jet flavour matching 
  Handle<reco::JetMatchedPartonsCollection> hPartonMatchingProduct;  
  if (flavorMatchingActive_) 
    GetProduct(flavorMatchingByReferenceToken_, hPartonMatchingProduct, event);

  Handle<reco::JetTagCollection> hJetProbabilityBJetTags;
  Handle<reco::JetTagCollection> hJetBProbabilityBJetTags;
  Handle<reco::JetTagCollection> hSimpleSecondaryVertexHighEffBJetTags;
  Handle<reco::JetTagCollection> hSimpleSecondaryVertexHighPurBJetTags;
  Handle<reco::JetTagCollection> hCombinedSecondaryVertexBJetTags;
  Handle<reco::JetTagCollection> hCombinedSecondaryVertexMVABJetTags;
  Handle<reco::JetTagCollection> hTrackCountingHighEffBJetTags;
  Handle<reco::JetTagCollection> hTrackCountingHighPurBJetTags;

  if (bTaggingActive_) {
    GetProduct(jetProbabilityBJetTagsToken_, hJetProbabilityBJetTags, event);    
    GetProduct(jetBProbabilityBJetTagsToken_, hJetBProbabilityBJetTags, event);        
    GetProduct(simpleSecondaryVertexHighEffBJetTagsToken_,hSimpleSecondaryVertexHighEffBJetTags,event);
    GetProduct(simpleSecondaryVertexHighPurBJetTagsToken_,hSimpleSecondaryVertexHighPurBJetTags,event);
    GetProduct(combinedSecondaryVertexBJetTagsToken_, hCombinedSecondaryVertexBJetTags, event);    
    GetProduct(combinedSecondaryVertexMVABJetTagsToken_, hCombinedSecondaryVertexMVABJetTags, event);
    GetProduct(trackCountingHighEffBJetTagsToken_, hTrackCountingHighEffBJetTags, event);    
    GetProduct(trackCountingHighPurBJetTagsToken_, hTrackCountingHighPurBJetTags, event);    
  }
  
  const reco::PFJetCollection inJets = *(hJetProduct.product());  

  // Handles to Jet to Vertex Association
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

  // Define Jet Correction Services
  const JetCorrector* correctorL2 = 0; 
  const JetCorrector* correctorL3 = 0; 
  if (jetCorrectionsActive_) {
    correctorL2 = JetCorrector::getJetCorrector (L2JetCorrectorName_,setup);
    correctorL3 = JetCorrector::getJetCorrector (L3JetCorrectorName_,setup);
  }

  // loop through all jets
  for (reco::PFJetCollection::const_iterator inJet = inJets.begin(); 
       inJet != inJets.end(); ++inJet) {
    
    reco::PFJetRef   jetRef(hJetProduct, inJet - inJets.begin());    
    reco::JetBaseRef jetBaseRef(jetRef);
    
    mithep::PFJet *jet = jets_->Allocate();

    new (jet) mithep::PFJet(inJet->p4().x(),inJet->p4().y(),inJet->p4().z(),inJet->p4().e());

    // add to map
    edm::Ptr<reco::PFJet> thePtr(hJetProduct, inJet - inJets.begin());
    jetMap_->Add(thePtr,jet);

    // fill jet moments
    jet->SetSigmaEta(TMath::Sqrt(inJet->etaetaMoment()));
    jet->SetSigmaPhi(TMath::Sqrt(inJet->phiphiMoment()));


    // fill pfjet-specific quantities
    jet->SetChargedHadronEnergy(inJet->chargedHadronEnergy());
    jet->SetNeutralHadronEnergy(inJet->neutralHadronEnergy());
    jet->SetChargedEmEnergy(inJet->chargedEmEnergy());
    jet->SetChargedMuEnergy(inJet->chargedMuEnergy());
    jet->SetNeutralEmEnergy(inJet->neutralEmEnergy());
    jet->SetChargedMultiplicity(inJet->chargedMultiplicity());
    jet->SetNeutralMultiplicity(inJet->neutralMultiplicity());
    jet->SetMuonMultiplicity(inJet->muonMultiplicity());
     
    if (jetToVertexActive_) {
      // compute alpha and beta parameter for jets
      jet->SetAlpha((*it_jv_alpha));
      jet->SetBeta((*it_jv_beta));      
    }

    // fill the area anyway 
    jet->SetJetArea(inJet->jetArea());

    // Jet Corrections
    if (fastJetCorrectionsActive_) {
      double l1Scale = (inJet->pt() - (*rho)*inJet->jetArea())/inJet->pt();
      l1Scale = (l1Scale>0) ? l1Scale : 0.0;
      jet->SetL1OffsetCorrectionScale( l1Scale);
      jet->EnableCorrection(mithep::PFJet::L1);
    }
    if (jetCorrectionsActive_) {
      double L2Scale = correctorL2->correction(inJet->p4());
      double L3Scale = correctorL3->correction(inJet->p4()*L2Scale);
      jet->SetL2RelativeCorrectionScale(L2Scale);
      jet->SetL3AbsoluteCorrectionScale(L3Scale);     
      jet->EnableCorrection(mithep::PFJet::L2);
      jet->EnableCorrection(mithep::PFJet::L3);     
    }
    
    if (bTaggingActive_) {
      jet->SetJetProbabilityBJetTagsDisc((*(hJetProbabilityBJetTags.product()))[jetBaseRef]);
      jet->SetJetBProbabilityBJetTagsDisc((*(hJetBProbabilityBJetTags.product()))[jetBaseRef]);
      jet->SetSimpleSecondaryVertexHighEffBJetTagsDisc((*(hSimpleSecondaryVertexHighEffBJetTags.product()))[jetBaseRef]);       
      jet->SetSimpleSecondaryVertexHighPurBJetTagsDisc((*(hSimpleSecondaryVertexHighPurBJetTags.product()))[jetBaseRef]);       
      jet->SetCombinedSecondaryVertexBJetTagsDisc(
        (*(hCombinedSecondaryVertexBJetTags.product()))[jetBaseRef]);   
      jet->SetCombinedSecondaryVertexMVABJetTagsDisc(
        (*(hCombinedSecondaryVertexMVABJetTags.product()))[jetBaseRef]); 
      jet->SetTrackCountingHighEffBJetTagsDisc(
        (*(hTrackCountingHighEffBJetTags.product()))[jetBaseRef]);  
      jet->SetTrackCountingHighPurBJetTagsDisc(
        (*(hTrackCountingHighPurBJetTags.product()))[jetBaseRef]); 
    }

    // get the Monte Carlo flavour matching information
    if (flavorMatchingActive_) {
      unsigned int iJet = inJet - inJets.begin();
      const reco::JetMatchedPartonsCollection *matchedPartons = hPartonMatchingProduct.product();
      assert(matchedPartons->size()==inJets.size());
      reco::MatchedPartons matchedParton = (*matchedPartons)[matchedPartons->key(iJet)];
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
      //printf(" Adding pfCandMap (%s) : %d\n",mitName_.data(),pfCandMap_->GetEntries());
      for (uint i=0; i<inJet->numberOfDaughters(); ++i) {
        const reco::CandidatePtr   candPtr = inJet->daughterPtr(i);
        const reco::PFCandidatePtr pfPtr(candPtr);

	//printf(" GetMit (%s) : %d\n",mitName_.data(),pfCandMap_->GetEntries());
	
        const mithep::PFCandidate *constituent = pfCandMap_->GetMit(pfPtr);
	jet->AddPFCand(constituent);
      }
    }
    if (jetToVertexActive_) {
      it_jv_alpha++; 
      it_jv_beta++;
    }
  }      
  jets_->Trim();
}
