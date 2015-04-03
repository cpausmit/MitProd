// $Id: FillerTrackJets.cc,v 1.3 2010/03/26 14:18:53 sixie Exp $

#include "MitProd/TreeFiller/interface/FillerTrackJets.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/JetReco/interface/TrackJet.h"
#include "DataFormats/Common/interface/RefToPtr.h"
#include "SimDataFormats/JetMatching/interface/JetFlavour.h"
#include "SimDataFormats/JetMatching/interface/JetFlavourMatching.h"
#include "SimDataFormats/JetMatching/interface/MatchedPartons.h"
#include "JetMETCorrections/Objects/interface/JetCorrector.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataTree/interface/TrackJetCol.h"
#include "MitProd/ObjectService/interface/ObjectService.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerTrackJets::FillerTrackJets(const ParameterSet &cfg, edm::ConsumesCollector& collector, ObjectService* os, const char *name, bool active) : 
  BaseFiller(cfg,os,name,active),
  flavorMatchingActive_(Conf().getUntrackedParameter<bool>("flavorMatchingActive",true)),
  bTaggingActive_(Conf().getUntrackedParameter<bool>("bTaggingActive",true)),
  jetToVertexActive_(Conf().getUntrackedParameter<bool>("jetToVertexActive",true)),
  jetCorrectionsActive_(Conf().getUntrackedParameter<bool>("jetCorrectionsActive",true)),
  edmToken_(GetToken<reco::TrackJetCollection>(collector, "edmName","recoPFJets:iterativeCone5PFJets")),
  jetToVertexAlphaToken_(GetToken<std::vector<double> >(collector, "jetToVertexAlphaName","jetToVertexAlpha")),
  jetToVertexBetaToken_(GetToken<std::vector<double> >(collector, "jetToVertexBetaName","jetToVertexBetaName")),
  flavorMatchingByReferenceToken_(GetToken<reco::JetMatchedPartonsCollection>(collector, "flavorMatchingByReferenceName","srcByReference")),
  jetProbabilityBJetTagsToken_(GetToken<reco::JetTagCollection>(collector, "JetProbabilityBJetTagsName","jetProbabilityBJetTags")),  
  jetBProbabilityBJetTagsToken_(GetToken<reco::JetTagCollection>(collector, "JetBProbabilityBJetTagsName","jetBProbabilityBJetTags")),   
  simpleSecondaryVertexBJetTagsToken_(GetToken<reco::JetTagCollection>(collector, "SimpleSecondaryVertexBJetTagsName","simpleSecondaryVertexBJetTags")),
  combinedSecondaryVertexBJetTagsToken_(GetToken<reco::JetTagCollection>(collector, "CombinedSecondaryVertexBJetTagsName","combinedSecondaryVertexBJetTags")),
  combinedSecondaryVertexMVABJetTagsToken_(GetToken<reco::JetTagCollection>(collector, "CombinedSecondaryVertexMVABJetTagsName","combinedSecondaryVertexMVABJetTags")),
  trackCountingHighEffBJetTagsToken_(GetToken<reco::JetTagCollection>(collector, "TrackCountingHighEffBJetTagsName","trackCountingHighEffBJetTags")),
  trackCountingHighPurBJetTagsToken_(GetToken<reco::JetTagCollection>(collector, "TrackCountingHighPurBJetTagsName","trackCountingHighPurBJetTags")),
  L2JetCorrectorName_(Conf().getUntrackedParameter<string>
                      ("L2JetCorrectorName","L2JetCorrectorName")),
  L3JetCorrectorName_(Conf().getUntrackedParameter<string>
                      ("L3JetCorrectorName","L3JetCorrectorName")),
  flavorMatchingDefinition_(Conf().getUntrackedParameter<string>
                   ("flavorMatchingDefinition","Algorithmic")),
  mitName_(Conf().getUntrackedParameter<string>("mitName","ItrCone5PFJets")), 
  trackMapName_(Conf().getUntrackedParameter<string>("trackMapName","trackMapName")),
  vertexMapName_(Conf().getUntrackedParameter<string>("vertexMapName","")),
  jetMapName_(Conf().getUntrackedParameter<string>("jetMapName","TrackJetMap")),
  trackMap_(0),
  vertexMap_(0),
  jetMap_(new mithep::TrackJetMap),
  jets_(new mithep::TrackJetArr(16))
{
  // Constructor.
}

//--------------------------------------------------------------------------------------------------
FillerTrackJets::~FillerTrackJets()
{
  // Destructor.

  delete jets_;
  delete jetMap_;
}

//--------------------------------------------------------------------------------------------------
void FillerTrackJets::BookDataBlock(TreeWriter &tws)
{
  // Add jets branch to tree.

  tws.AddBranch(mitName_,&jets_);
  OS()->add<mithep::TrackJetArr>(jets_,mitName_);

  if (!trackMapName_.empty()) {
    trackMap_ = OS()->get<TrackMap>(trackMapName_);
    if (trackMap_)
      AddBranchDep(mitName_,trackMap_->GetBrName());
  }
  if (!vertexMapName_.empty()) {
    vertexMap_ = OS()->get<VertexMap>(vertexMapName_);
    if (vertexMap_)
      AddBranchDep(mitName_,vertexMap_->GetBrName());
  }
  if (!jetMapName_.empty()) {
    jetMap_->SetBrName(mitName_);
    OS()->add<TrackJetMap>(jetMap_,jetMapName_);
  }
}

//--------------------------------------------------------------------------------------------------
void FillerTrackJets::FillDataBlock(const edm::Event      &event, 
                                 const edm::EventSetup &setup)
{
  // Fill jets from edm collection into our collection.

  jets_->Delete();
  jetMap_->Reset();

  // handle for the Jet Collection
  Handle<reco::TrackJetCollection> hJetProduct;
  GetProduct(edmToken_, hJetProduct, event);

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
  
  const reco::TrackJetCollection inJets = *(hJetProduct.product());  

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
  for (reco::TrackJetCollection::const_iterator inJet = inJets.begin(); 
       inJet != inJets.end(); ++inJet) {
    
    reco::TrackJetRef jetRef(hJetProduct, inJet - inJets.begin());    
    reco::JetBaseRef jetBaseRef(jetRef);
    
    mithep::TrackJet *jet = jets_->Allocate();
    new (jet) mithep::TrackJet(inJet->p4().x(),
                          inJet->p4().y(),
                          inJet->p4().z(),
                          inJet->p4().e());

    //add to map
    edm::Ptr<reco::TrackJet> thePtr(hJetProduct, inJet - inJets.begin());
    jetMap_->Add(thePtr, jet);

    //fill jet moments
    jet->SetSigmaEta(TMath::Sqrt(inJet->etaetaMoment()));
    jet->SetSigmaPhi(TMath::Sqrt(inJet->phiphiMoment()));

    //fill TrackJet specific quantities 
    //fill primary vertex reference
    if (inJet->primaryVertex().isNonnull()) {
      jet->SetVertex(vertexMap_->GetMit(refToPtr(inJet->primaryVertex())));
    }     


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
      jet->EnableCorrection(mithep::TrackJet::L2);
      jet->EnableCorrection(mithep::TrackJet::L3);     
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

    // add track refs
    if (trackMap_) {
      for (uint i=0; i<inJet->numberOfTracks(); ++i) {
        const edm::Ptr<reco::Track> trkPtr = inJet->track(i);
        const mithep::Track *track = trackMap_->GetMit(trkPtr);
        jet->AddTrack(track);
      }
    }
    if (jetToVertexActive_) {
      it_jv_alpha++; 
      it_jv_beta++;
    }
  }      
  jets_->Trim();
}
