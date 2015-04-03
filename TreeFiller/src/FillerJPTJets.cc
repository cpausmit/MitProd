// $Id: FillerJPTJets.cc,v 1.4 2010/09/22 08:39:34 bendavid Exp $

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
#include "MitAna/DataTree/interface/JPTJetCol.h"
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
  jetProbabilityBJetTagsToken_(GetToken<reco::JetTagCollection>(collector,"JetProbabilityBJetTagsName","jetProbabilityBJetTags")),  
  jetBProbabilityBJetTagsToken_(GetToken<reco::JetTagCollection>(collector,"JetBProbabilityBJetTagsName","jetBProbabilityBJetTags")),   
  simpleSecondaryVertexBJetTagsToken_(GetToken<reco::JetTagCollection>(collector,"SimpleSecondaryVertexBJetTagsName","simpleSecondaryVertexBJetTags")),
  combinedSecondaryVertexBJetTagsToken_(GetToken<reco::JetTagCollection>(collector,"CombinedSecondaryVertexBJetTagsName","combinedSecondaryVertexBJetTags")),
  combinedSecondaryVertexMVABJetTagsToken_(GetToken<reco::JetTagCollection>(collector,"CombinedSecondaryVertexMVABJetTagsName","combinedSecondaryVertexMVABJetTags")),
  trackCountingHighEffBJetTagsToken_(GetToken<reco::JetTagCollection>(collector,"TrackCountingHighEffBJetTagsName","trackCountingHighEffBJetTags")),
  trackCountingHighPurBJetTagsToken_(GetToken<reco::JetTagCollection>(collector,"TrackCountingHighPurBJetTagsName","trackCountingHighPurBJetTags")),
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
  OS()->add<mithep::JPTJetArr>(jets_,mitName_);

  if (!caloJetMapName_.empty()) {
    caloJetMap_ = OS()->get<CaloJetMap>(caloJetMapName_);
    if (caloJetMap_)
      AddBranchDep(mitName_,caloJetMap_->GetBrName());
  }
  if (!jetMapName_.empty()) {
    jetMap_->SetBrName(mitName_);
    OS()->add<JPTJetMap>(jetMap_,jetMapName_);
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

  Handle<reco::JetTagCollection> hJetProbabilityBJetTags;
  Handle<reco::JetTagCollection> hJetBProbabilityBJetTags;
  Handle<reco::JetTagCollection> hSimpleSecondaryVertexBJetTags;
  Handle<reco::JetTagCollection> hCombinedSecondaryVertexBJetTags;
  Handle<reco::JetTagCollection> hCombinedSecondaryVertexMVABJetTags;
  Handle<reco::JetTagCollection> hTrackCountingHighEffBJetTags;
  Handle<reco::JetTagCollection> hTrackCountingHighPurBJetTags;

  if (bTaggingActive_) {
    GetProduct(jetProbabilityBJetTagsToken_, hJetProbabilityBJetTags, event);    
    GetProduct(jetBProbabilityBJetTagsToken_, hJetBProbabilityBJetTags, event);    
    GetProduct(simpleSecondaryVertexBJetTagsToken_, hSimpleSecondaryVertexBJetTags, event);    
    GetProduct(combinedSecondaryVertexBJetTagsToken_, hCombinedSecondaryVertexBJetTags, event);    
    GetProduct(combinedSecondaryVertexMVABJetTagsToken_, hCombinedSecondaryVertexMVABJetTags, event);
    GetProduct(trackCountingHighEffBJetTagsToken_, hTrackCountingHighEffBJetTags, event);    
    GetProduct(trackCountingHighPurBJetTagsToken_, hTrackCountingHighPurBJetTags, event);    
  }
  
  const reco::JPTJetCollection inJets = *(hJetProduct.product());  

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
    edm::Ptr<reco::JPTJet> thePtr(hJetProduct, inJet - inJets.begin());
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
      jet->EnableCorrection(mithep::JPTJet::L2);
      jet->EnableCorrection(mithep::JPTJet::L3);     
    }
    
    if (bTaggingActive_) {
      jet->SetJetProbabilityBJetTagsDisc((*(hJetProbabilityBJetTags.product()))[jetBaseRef]);
      jet->SetJetBProbabilityBJetTagsDisc((*(hJetBProbabilityBJetTags.product()))[jetBaseRef]);
      jet->SetSimpleSecondaryVertexBJetTagsDisc(
        (*(hSimpleSecondaryVertexBJetTags.product()))[jetBaseRef]);       
      jet->SetCombinedSecondaryVertexBJetTagsDisc(
        (*(hCombinedSecondaryVertexBJetTags.product()))[jetBaseRef]);   
      jet->SetCombinedSecondaryVertexMVABJetTagsDisc(
        (*(hCombinedSecondaryVertexMVABJetTags.product()))[jetBaseRef]); 
      jet->SetTrackCountingHighEffBJetTagsDisc(
        (*(hTrackCountingHighEffBJetTags.product()))[jetBaseRef]);  
      jet->SetTrackCountingHighPurBJetTagsDisc(
        (*(hTrackCountingHighPurBJetTags.product()))[jetBaseRef]); 
      //jet->SetSoftMuonBJetTagsDisc((*(hSoftMuonBJetTags.product()))[jetBaseRef]);
      //jet->SetSoftMuonByIP3dBJetTagsDisc((*(hSoftMuonByIP3dBJetTags.product()))[jetBaseRef]); 
      //jet->SetSoftMuonByPtBJetTagsDisc((*(hSoftMuonByPtBJetTags.product()))[jetBaseRef]); 
      //jet->SetSoftElectronByIP3dBJetTagsDisc((*(hSoftElectronByIP3dBJetTags.product()))[jetBaseRef]);
      //jet->SetSoftElectronByPtBJetTagsDisc((*(hSoftElectronByPtBJetTags.product()))[jetBaseRef]); 
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
      const edm::Ptr<reco::CaloJet> caloJetPtr(mitedm::refToBaseToPtr(inJet->getCaloJetRef()));
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
