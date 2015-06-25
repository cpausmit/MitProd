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
#include "MitProd/ObjectService/interface/ObjectService.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerTrackJets::FillerTrackJets(const ParameterSet &cfg, edm::ConsumesCollector& collector, ObjectService* os, const char *name, bool active) : 
  BaseFiller(cfg,os,name,active),
  flavorMatchingActive_(cfg.getUntrackedParameter<bool>("flavorMatchingActive",true)),
  bTaggingActive_(cfg.getUntrackedParameter<bool>("bTaggingActive",true)),
  jetToVertexActive_(cfg.getUntrackedParameter<bool>("jetToVertexActive",true)),
  jetCorrectionsActive_(cfg.getUntrackedParameter<bool>("jetCorrectionsActive",true)),
  edmToken_(GetToken<reco::TrackJetCollection>(collector, cfg, "edmName","recoPFJets:iterativeCone5PFJets")),
  jetToVertexAlphaToken_(GetToken<std::vector<double> >(collector, cfg, "jetToVertexAlphaName","jetToVertexAlpha")),
  jetToVertexBetaToken_(GetToken<std::vector<double> >(collector, cfg, "jetToVertexBetaName","jetToVertexBetaName")),
  flavorMatchingByReferenceToken_(GetToken<reco::JetMatchedPartonsCollection>(collector, cfg, "flavorMatchingByReferenceName","srcByReference")),
  bJetTagsToken_{
    GetToken<reco::JetTagCollection>(collector, cfg, "JetProbabilityBJetTagsName", ""),
    GetToken<reco::JetTagCollection>(collector, cfg, "JetBProbabilityBJetTagsName", ""),
    GetToken<reco::JetTagCollection>(collector, cfg, "SimpleSecondaryVertexHighEffBJetTagsName", ""),
    GetToken<reco::JetTagCollection>(collector, cfg, "SimpleSecondaryVertexHighPurBJetTagsName", ""),
    GetToken<reco::JetTagCollection>(collector, cfg, "CombinedSecondaryVertexBJetTagsName", ""),
    GetToken<reco::JetTagCollection>(collector, cfg, "CombinedSecondaryVertexV2BJetTagsName", ""),
    GetToken<reco::JetTagCollection>(collector, cfg, "CombinedSecondaryVertexSoftLeptonBJetTagsName", ""),
    GetToken<reco::JetTagCollection>(collector, cfg, "CombinedInclusiveSecondaryVertexV2BJetTagsName", ""),
    GetToken<reco::JetTagCollection>(collector, cfg, "CombinedMVABJetTagsName", ""),
    GetToken<reco::JetTagCollection>(collector, cfg, "TrackCountingHighEffBJetTagsName", ""),
    GetToken<reco::JetTagCollection>(collector, cfg, "TrackCountingHighPurBJetTagsName", "")
  },
  L2JetCorrectorName_(cfg.getUntrackedParameter<string>
                      ("L2JetCorrectorName","L2JetCorrectorName")),
  L3JetCorrectorName_(cfg.getUntrackedParameter<string>
                      ("L3JetCorrectorName","L3JetCorrectorName")),
  flavorMatchingDefinition_(cfg.getUntrackedParameter<string>
                   ("flavorMatchingDefinition","Algorithmic")),
  mitName_(cfg.getUntrackedParameter<string>("mitName","ItrCone5PFJets")), 
  trackMapName_(cfg.getUntrackedParameter<string>("trackMapName","trackMapName")),
  vertexMapName_(cfg.getUntrackedParameter<string>("vertexMapName","")),
  jetMapName_(cfg.getUntrackedParameter<string>("jetMapName","TrackJetMap")),
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
  
  reco::TrackJetCollection const& inJets = *hJetProduct;

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
    
    mithep::TrackJet *jet = jets_->AddNew();

    jet->SetRawPtEtaPhiM(inJet->p4().pt(), inJet->p4().eta(), inJet->p4().phi(), inJet->p4().mass());

    //add to map
    edm::Ptr<reco::Jet> thePtr(hJetProduct, inJet - inJets.begin());
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

DEFINE_MITHEP_TREEFILLER(FillerTrackJets);
