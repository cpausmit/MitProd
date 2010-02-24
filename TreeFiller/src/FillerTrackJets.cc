// $Id: FillerTrackJets.cc,v 1.9 2009/09/25 08:42:51 loizides Exp $

#include "MitProd/TreeFiller/interface/FillerTrackJets.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/JetReco/interface/TrackJet.h"
#include "DataFormats/BTauReco/interface/JetTag.h"
#include "DataFormats/Common/interface/RefToPtr.h"
#include "SimDataFormats/JetMatching/interface/JetFlavour.h"
#include "SimDataFormats/JetMatching/interface/JetFlavourMatching.h"
#include "SimDataFormats/JetMatching/interface/MatchedPartons.h"
#include "SimDataFormats/JetMatching/interface/JetMatchedPartons.h"
#include "JetMETCorrections/Objects/interface/JetCorrector.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataTree/interface/TrackJetCol.h"
#include "MitProd/ObjectService/interface/ObjectService.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerTrackJets::FillerTrackJets(const ParameterSet &cfg, const char *name, bool active) : 
  BaseFiller(cfg,name,active),
  flavorMatchingActive_(Conf().getUntrackedParameter<bool>("flavorMatchingActive",true)),
  bTaggingActive_(Conf().getUntrackedParameter<bool>("bTaggingActive",true)),
  jetToVertexActive_(Conf().getUntrackedParameter<bool>("jetToVertexActive",true)),
  jetCorrectionsActive_(Conf().getUntrackedParameter<bool>("jetCorrectionsActive",true)),
  edmName_(Conf().getUntrackedParameter<string>("edmName","recoPFJets:iterativeCone5PFJets")),
  mitName_(Conf().getUntrackedParameter<string>("mitName","ItrCone5PFJets")), 
  jetToVertexAlphaName_(Conf().getUntrackedParameter<string>
                        ("jetToVertexAlphaName","jetToVertexAlpha")),
  jetToVertexBetaName_(Conf().getUntrackedParameter<string>
                       ("jetToVertexBetaName","jetToVertexBetaName")),
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
  combinedSecondaryVertexBJetTagsName_(Conf().getUntrackedParameter<string>
                   ("CombinedSecondaryVertexBJetTagsName","combinedSecondaryVertexBJetTags")),
  combinedSecondaryVertexMVABJetTagsName_(Conf().getUntrackedParameter<string>
                   ("CombinedSecondaryVertexMVABJetTagsName","combinedSecondaryVertexMVABJetTags")),
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
void FillerTrackJets::BookDataBlock(TreeWriter &tws, const edm::EventSetup &es)
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
  GetProduct(edmName_, hJetProduct, event);

  // handles for jet flavour matching 
  Handle<reco::JetMatchedPartonsCollection> hPartonMatchingProduct;  
  if (flavorMatchingActive_) 
    GetProduct(flavorMatchingByReferenceName_, hPartonMatchingProduct, event);

  Handle<reco::JetTagCollection> hJetProbabilityBJetTags;
  Handle<reco::JetTagCollection> hJetBProbabilityBJetTags;
  Handle<reco::JetTagCollection> hSimpleSecondaryVertexBJetTags;
  Handle<reco::JetTagCollection> hCombinedSecondaryVertexBJetTags;
  Handle<reco::JetTagCollection> hCombinedSecondaryVertexMVABJetTags;
  Handle<reco::JetTagCollection> hTrackCountingHighEffBJetTags;
  Handle<reco::JetTagCollection> hTrackCountingHighPurBJetTags;
  Handle<reco::JetTagCollection> hSoftMuonBJetTags;
  Handle<reco::JetTagCollection> hSoftMuonByIP3dBJetTags;
  Handle<reco::JetTagCollection> hSoftMuonByPtBJetTags;
  Handle<reco::JetTagCollection> hSoftElectronByIP3dBJetTags;
  Handle<reco::JetTagCollection> hSoftElectronByPtBJetTags;

  if (bTaggingActive_) {
    GetProduct(jetProbabilityBJetTagsName_, hJetProbabilityBJetTags, event);    
    GetProduct(jetBProbabilityBJetTagsName_, hJetBProbabilityBJetTags, event);    
    GetProduct(simpleSecondaryVertexBJetTagsName_, hSimpleSecondaryVertexBJetTags, event);    
    GetProduct(combinedSecondaryVertexBJetTagsName_, hCombinedSecondaryVertexBJetTags, event);    
    GetProduct(combinedSecondaryVertexMVABJetTagsName_, hCombinedSecondaryVertexMVABJetTags, event);
    GetProduct(trackCountingHighEffBJetTagsName_, hTrackCountingHighEffBJetTags, event);    
    GetProduct(trackCountingHighPurBJetTagsName_, hTrackCountingHighPurBJetTags, event);    
    GetProduct(softMuonBJetTagsName_, hSoftMuonBJetTags, event);    
    GetProduct(softMuonByIP3dBJetTagsName_, hSoftMuonByIP3dBJetTags, event);
    GetProduct(softMuonByPtBJetTagsName_, hSoftMuonByPtBJetTags, event);   
    GetProduct(softElectronByIP3dBJetTagsName_, hSoftElectronByIP3dBJetTags, event);
    GetProduct(softElectronByPtBJetTagsName_, hSoftElectronByPtBJetTags, event);    
  }
  
  const reco::TrackJetCollection inJets = *(hJetProduct.product());  

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
      jet->SetSoftMuonBJetTagsDisc((*(hSoftMuonBJetTags.product()))[jetBaseRef]);
      jet->SetSoftMuonByIP3dBJetTagsDisc((*(hSoftMuonByIP3dBJetTags.product()))[jetBaseRef]); 
      jet->SetSoftMuonByPtBJetTagsDisc((*(hSoftMuonByPtBJetTags.product()))[jetBaseRef]); 
      jet->SetSoftElectronByIP3dBJetTagsDisc((*(hSoftElectronByIP3dBJetTags.product()))[jetBaseRef]);
      jet->SetSoftElectronByPtBJetTagsDisc((*(hSoftElectronByPtBJetTags.product()))[jetBaseRef]); 
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
