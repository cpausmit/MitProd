// $Id: FillerPFJets.cc,v 1.13 2011/03/01 17:27:22 mzanetti Exp $

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
FillerPFJets::FillerPFJets(const ParameterSet &cfg, const char *name, bool active) : 
  BaseFiller(cfg,name,active),
  flavorMatchingActive_(Conf().getUntrackedParameter<bool>("flavorMatchingActive",true)),
  bTaggingActive_(Conf().getUntrackedParameter<bool>("bTaggingActive",true)),
  jetToVertexActive_(Conf().getUntrackedParameter<bool>("jetToVertexActive",true)),
  jetCorrectionsActive_(Conf().getUntrackedParameter<bool>("jetCorrectionsActive",true)),
  fastJetCorrectionsActive_(Conf().getUntrackedParameter<bool>("fastJetCorrectionsActive",false)),
  edmName_(Conf().getUntrackedParameter<string>("edmName","recoPFJets:iterativeCone5PFJets")),
  mitName_(Conf().getUntrackedParameter<string>("mitName","ItrCone5PFJets")), 
  jetToVertexAlphaName_(Conf().getUntrackedParameter<string>
                        ("jetToVertexAlphaName","jetToVertexAlpha")),
  jetToVertexBetaName_(Conf().getUntrackedParameter<string>
                       ("jetToVertexBetaName","jetToVertexBetaName")),
  rhoName_(Conf().getUntrackedParameter<edm::InputTag> ("rhoName")),
  L2JetCorrectorName_(Conf().getUntrackedParameter<string>
                      ("L2JetCorrectorName","L2JetCorrectorName")),
  L3JetCorrectorName_(Conf().getUntrackedParameter<string>
                      ("L3JetCorrectorName","L3JetCorrectorName")),
  flavorMatchingByReferenceName_(Conf().getUntrackedParameter<string>
                   ("flavorMatchingByReferenceName","srcByReference")),
  flavorMatchingDefinition_(Conf().getUntrackedParameter<string>
                   ("flavorMatchingDefinition","Algorithmic")),
  jetProbabilityBJetTagsName_(Conf().getUntrackedParameter<string>
                   ("JetProbabilityBJetTagsName","jetProbabilityBJetTags")),  
  jetBProbabilityBJetTagsName_(Conf().getUntrackedParameter<string>
                   ("JetBProbabilityBJetTagsName","jetBProbabilityBJetTags")),   
  simpleSecondaryVertexBJetTagsName_(Conf().getUntrackedParameter<string>
                   ("SimpleSecondaryVertexBJetTagsName","simpleSecondaryVertexBJetTags")),
  simpleSecondaryVertexHighEffBJetTagsName_(Conf().getUntrackedParameter<string>
                   ("SimpleSecondaryVertexHighEffBJetTagsName","simpleSecondaryVertexHighEffBJetTags")),                   
  simpleSecondaryVertexHighPurBJetTagsName_(Conf().getUntrackedParameter<string>
                   ("SimpleSecondaryVertexHighPurBJetTagsName","simpleSecondaryVertexHighPurBJetTags")),                   
  combinedSecondaryVertexBJetTagsName_(Conf().getUntrackedParameter<string>
                   ("CombinedSecondaryVertexBJetTagsName","combinedSecondaryVertexBJetTags")),
  combinedSecondaryVertexMVABJetTagsName_(Conf().getUntrackedParameter<string>
                   ("CombinedSecondaryVertexMVABJetTagsName","combinedSecondaryVertexMVABJetTags")),
  ghostTrackBJetTagsName_(Conf().getUntrackedParameter<string>
                   ("GhostTrackBJetTagsName","ghostTrackBJetTags")),                                                         
  trackCountingHighEffBJetTagsName_(Conf().getUntrackedParameter<string>
                   ("TrackCountingHighEffBJetTagsName","trackCountingHighEffBJetTags")),
  trackCountingHighPurBJetTagsName_(Conf().getUntrackedParameter<string>
                   ("TrackCountingHighPurBJetTagsName","trackCountingHighPurBJetTags")),
  softMuonBJetTagsName_(Conf().getUntrackedParameter<string>
                   ("SoftMuonBJetTagsName","softMuonBJetTags")),
  softMuonByIP3dBJetTagsName_(Conf().getUntrackedParameter<string>
                   ("SoftMuonByIP3dBJetTagsName","softMuonByIP3dBJetTags")),
  softMuonByPtBJetTagsName_(Conf().getUntrackedParameter<string>
                   ("SoftMuonByPtBJetTagsName","softMuonByPtBJetTags")),
  softElectronByIP3dBJetTagsName_(Conf().getUntrackedParameter<string>
                   ("SoftElectronByIP3dBJetTagsName","softElectronByIP3dBJetTags")),
  softElectronByPtBJetTagsName_(Conf().getUntrackedParameter<string>
                   ("SoftElectronByPtBJetTagsName","softElectronByPtBJetTags")),
  pfCandMapName_(Conf().getUntrackedParameter<string>("pfCandMapName","pfCandMapName")),
  jetMapName_(Conf().getUntrackedParameter<string>("jetMapName","PFJetMap")),
  pfCandMap_(0),
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

  if (!pfCandMapName_.empty()) {
    pfCandMap_ = OS()->get<PFCandidateMap>(pfCandMapName_);
    if (pfCandMap_)
      AddBranchDep(mitName_,pfCandMap_->GetBrName());
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
  GetProduct(edmName_, hJetProduct, event);

//   Handle<double> rho;
//   GetProduct(rhoName_, rho, event);

  Handle<double> rho;
  event.getByLabel(rhoName_,rho);

  // handles for jet flavour matching 
  Handle<reco::JetMatchedPartonsCollection> hPartonMatchingProduct;  
  if (flavorMatchingActive_) 
    GetProduct(flavorMatchingByReferenceName_, hPartonMatchingProduct, event);

  Handle<reco::JetTagCollection> hJetProbabilityBJetTags;
  Handle<reco::JetTagCollection> hJetBProbabilityBJetTags;
  Handle<reco::JetTagCollection> hSimpleSecondaryVertexBJetTags;
  Handle<reco::JetTagCollection> hSimpleSecondaryVertexHighEffBJetTags;
  Handle<reco::JetTagCollection> hSimpleSecondaryVertexHighPurBJetTags;
  Handle<reco::JetTagCollection> hCombinedSecondaryVertexBJetTags;
  Handle<reco::JetTagCollection> hCombinedSecondaryVertexMVABJetTags;
  Handle<reco::JetTagCollection> hTrackCountingHighEffBJetTags;
  Handle<reco::JetTagCollection> hTrackCountingHighPurBJetTags;
  Handle<reco::JetTagCollection> hSoftMuonBJetTags;
  Handle<reco::JetTagCollection> hSoftMuonByIP3dBJetTags;
  Handle<reco::JetTagCollection> hSoftMuonByPtBJetTags;
  Handle<reco::JetTagCollection> hSoftElectronByIP3dBJetTags;
  Handle<reco::JetTagCollection> hSoftElectronByPtBJetTags;
  Handle<reco::JetTagCollection> hGhostTrackBJetTags;

  if (bTaggingActive_) {
    GetProduct(jetProbabilityBJetTagsName_, hJetProbabilityBJetTags, event);    
    GetProduct(jetBProbabilityBJetTagsName_, hJetBProbabilityBJetTags, event);        
    event.getByLabel(simpleSecondaryVertexBJetTagsName_,hSimpleSecondaryVertexBJetTags);
    event.getByLabel(simpleSecondaryVertexHighEffBJetTagsName_,hSimpleSecondaryVertexHighEffBJetTags);
    event.getByLabel(simpleSecondaryVertexHighPurBJetTagsName_,hSimpleSecondaryVertexHighPurBJetTags);
    GetProduct(combinedSecondaryVertexBJetTagsName_, hCombinedSecondaryVertexBJetTags, event);    
    GetProduct(combinedSecondaryVertexMVABJetTagsName_, hCombinedSecondaryVertexMVABJetTags, event);
    GetProduct(trackCountingHighEffBJetTagsName_, hTrackCountingHighEffBJetTags, event);    
    GetProduct(trackCountingHighPurBJetTagsName_, hTrackCountingHighPurBJetTags, event);    
    GetProduct(softMuonBJetTagsName_, hSoftMuonBJetTags, event);    
    GetProduct(softMuonByIP3dBJetTagsName_, hSoftMuonByIP3dBJetTags, event);
    GetProduct(softMuonByPtBJetTagsName_, hSoftMuonByPtBJetTags, event);   
    GetProduct(softElectronByIP3dBJetTagsName_, hSoftElectronByIP3dBJetTags, event);
    GetProduct(softElectronByPtBJetTagsName_, hSoftElectronByPtBJetTags, event);    
    event.getByLabel(ghostTrackBJetTagsName_,hGhostTrackBJetTags);
  }
  
  const reco::PFJetCollection inJets = *(hJetProduct.product());  

  //Handles to Jet to Vertex Association
  Handle<std::vector<double> > JV_alpha;  
  Handle<std::vector<double> > JV_beta;    
  std::vector<double>::const_iterator it_jv_alpha;
  std::vector<double>::const_iterator it_jv_beta;

  if (jetToVertexActive_) {
    GetProduct(jetToVertexAlphaName_, JV_alpha, event); 
    GetProduct(jetToVertexBetaName_, JV_beta, event);  
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
  for (reco::PFJetCollection::const_iterator inJet = inJets.begin(); 
       inJet != inJets.end(); ++inJet) {
    
    reco::PFJetRef jetRef(hJetProduct, inJet - inJets.begin());    
    reco::JetBaseRef jetBaseRef(jetRef);
    
    mithep::PFJet *jet = jets_->Allocate();
    new (jet) mithep::PFJet(inJet->p4().x(),
                          inJet->p4().y(),
                          inJet->p4().z(),
                          inJet->p4().e());

    //add to map
    edm::Ptr<reco::PFJet> thePtr(hJetProduct, inJet - inJets.begin());
    jetMap_->Add(thePtr, jet);

    //fill jet moments
    jet->SetSigmaEta(TMath::Sqrt(inJet->etaetaMoment()));
    jet->SetSigmaPhi(TMath::Sqrt(inJet->phiphiMoment()));


    //fill pfjet-specific quantities
    jet->SetChargedHadronEnergy(inJet->chargedHadronEnergy());
    jet->SetNeutralHadronEnergy(inJet->neutralHadronEnergy());
    jet->SetChargedEmEnergy(inJet->chargedEmEnergy());
    jet->SetChargedMuEnergy(inJet->chargedMuEnergy());
    jet->SetNeutralEmEnergy(inJet->neutralEmEnergy());
    jet->SetChargedMultiplicity(inJet->chargedMultiplicity());
    jet->SetNeutralMultiplicity(inJet->neutralMultiplicity());
    jet->SetMuonMultiplicity(inJet->muonMultiplicity());
     
    if (jetToVertexActive_) {
      //compute alpha and beta parameter for jets
      jet->SetAlpha((*it_jv_alpha));
      jet->SetBeta((*it_jv_beta));      
    }

    //Jet Corrections
    if (fastJetCorrectionsActive_) {
      jet->SetJetArea(inJet->jetArea());
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
//      jet->SetSimpleSecondaryVertexBJetTagsDisc((*(hSimpleSecondaryVertexBJetTags.product()))[jetBaseRef]);       
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
      jet->SetSoftMuonBJetTagsDisc((*(hSoftMuonBJetTags.product()))[jetBaseRef]);
      jet->SetSoftMuonByIP3dBJetTagsDisc((*(hSoftMuonByIP3dBJetTags.product()))[jetBaseRef]); 
      jet->SetSoftMuonByPtBJetTagsDisc((*(hSoftMuonByPtBJetTags.product()))[jetBaseRef]); 
      jet->SetSoftElectronByIP3dBJetTagsDisc((*(hSoftElectronByIP3dBJetTags.product()))[jetBaseRef]);
      jet->SetSoftElectronByPtBJetTagsDisc((*(hSoftElectronByPtBJetTags.product()))[jetBaseRef]); 
//      jet->SetGhostTrackBJetTagsDisc((*(hGhostTrackBJetTags.product()))[jetBaseRef]);       
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

    // add PFCandidate refs
    if (pfCandMap_) {
      for (uint i=0; i<inJet->numberOfDaughters(); ++i) {
        const reco::CandidatePtr candPtr = inJet->daughterPtr(i);
        const reco::PFCandidatePtr pfPtr(candPtr);
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
