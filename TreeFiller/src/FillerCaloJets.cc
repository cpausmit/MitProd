// $Id: FillerCaloJets.cc,v 1.21 2009/07/07 08:31:16 bendavid Exp $

#include "MitProd/TreeFiller/interface/FillerCaloJets.h"
#include "DataFormats/JetReco/interface/Jet.h"
#include "DataFormats/BTauReco/interface/JetTag.h"
#include "SimDataFormats/JetMatching/interface/JetFlavour.h"
#include "SimDataFormats/JetMatching/interface/JetFlavourMatching.h"
#include "SimDataFormats/JetMatching/interface/MatchedPartons.h"
#include "SimDataFormats/JetMatching/interface/JetMatchedPartons.h"
#include "JetMETCorrections/Objects/interface/JetCorrector.h"
#include "MitAna/DataTree/interface/CaloJetCol.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitProd/ObjectService/interface/ObjectService.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerCaloJets::FillerCaloJets(const ParameterSet &cfg, const char *name, bool active) : 
  BaseFiller(cfg,name,active),
  flavorMatchingActive_(Conf().getUntrackedParameter<bool>("flavorMatchingActive",true)),
  bTaggingActive_(Conf().getUntrackedParameter<bool>("bTaggingActive",true)),
  jetToVertexActive_(Conf().getUntrackedParameter<bool>("jetToVertexActive",true)),
  jetCorrectionsActive_(Conf().getUntrackedParameter<bool>("jetCorrectionsActive",true)),
  edmName_(Conf().getUntrackedParameter<string>("edmName","recoCaloJets:iterativeCone5CaloJets")),
  mitName_(Conf().getUntrackedParameter<string>("mitName",Names::gkCaloJetBrn)), 
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
  caloTowerMapName_(Conf().getUntrackedParameter<string>("caloTowerMapName","CaloTowerMap")),
  jetMapName_(Conf().getUntrackedParameter<string>("jetMapName","CaloJetMap")),
  caloTowerMap_(0),
  jetMap_(new mithep::CaloJetMap),
  jets_(new mithep::CaloJetArr(16))
{
  // Constructor.
}

//--------------------------------------------------------------------------------------------------
FillerCaloJets::~FillerCaloJets()
{
  // Destructor.

  delete jets_;
  delete jetMap_;
}

//--------------------------------------------------------------------------------------------------
void FillerCaloJets::BookDataBlock(TreeWriter &tws)
{
  // Add jets branch to tree.

  tws.AddBranch(mitName_,&jets_);
  OS()->add<mithep::CaloJetArr>(jets_,mitName_);

  if (!caloTowerMapName_.empty()) {
    caloTowerMap_ = OS()->get<CaloTowerMap>(caloTowerMapName_);
    if (caloTowerMap_)
      AddBranchDep(mitName_, caloTowerMap_->GetBrName());
  }
  if (!jetMapName_.empty()) {
    jetMap_->SetBrName(mitName_);
    OS()->add<CaloJetMap>(jetMap_,jetMapName_);
  }
}

//--------------------------------------------------------------------------------------------------
void FillerCaloJets::FillDataBlock(const edm::Event      &event, 
                                   const edm::EventSetup &setup)
{
  // Fill jets from edm collection into our collection.

  jets_->Delete();
  jetMap_->Reset();

  // handle for the jet collection
  Handle<reco::CaloJetCollection> hJetProduct;
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
  
  const reco::CaloJetCollection inJets = *(hJetProduct.product());  

  // handle to Jet to Vertex association
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

  // define jet correction services
  const JetCorrector* correctorL2 = 0; 
  const JetCorrector* correctorL3 = 0; 
  if (jetCorrectionsActive_) {
    correctorL2 = JetCorrector::getJetCorrector (L2JetCorrectorName_,setup);
    correctorL3 = JetCorrector::getJetCorrector (L3JetCorrectorName_,setup);
  }

  // loop through all jets
  for (reco::CaloJetCollection::const_iterator inJet = inJets.begin(); 
       inJet != inJets.end(); ++inJet) {
    
    reco::CaloJetRef jetRef(hJetProduct, inJet - inJets.begin());    
    reco::JetBaseRef jetBaseRef(jetRef);
    
    mithep::CaloJet *jet = jets_->Allocate();
    new (jet) mithep::CaloJet(inJet->p4().x(),
                          inJet->p4().y(),
                          inJet->p4().z(),
                          inJet->p4().e());

    // add to map
    edm::Ptr<reco::CaloJet> thePtr(hJetProduct, inJet - inJets.begin());
    jetMap_->Add(thePtr, jet);

    // fill calojet-specific quantities
    jet->SetMaxEInEmTowers (inJet->maxEInEmTowers());	 
    jet->SetMaxEInHadTowers(inJet->maxEInHadTowers());
    jet->SetEnergyFractionH(inJet->energyFractionHadronic());
    jet->SetEnergyFractionEm(inJet->emEnergyFraction());
    jet->SetHadEnergyInHB  (inJet->hadEnergyInHB());
    jet->SetHadEnergyInHO  (inJet->hadEnergyInHO());
    jet->SetHadEnergyInHE  (inJet->hadEnergyInHE());
    jet->SetHadEnergyInHF  (inJet->hadEnergyInHF());
    jet->SetEmEnergyInEB   (inJet->emEnergyInEB());
    jet->SetEmEnergyInEE   (inJet->emEnergyInEE());
    jet->SetEmEnergyInHF   (inJet->emEnergyInHF());
    jet->SetTowersArea     (inJet->towersArea());
     
    if (jetToVertexActive_) {
      // compute alpha and beta parameter for jets
      jet->SetAlpha((*it_jv_alpha));
      jet->SetBeta((*it_jv_beta));      
    }

    // jet corrections
    if (jetCorrectionsActive_) {
      double L2Scale = correctorL2->correction(inJet->p4());
      double L3Scale = correctorL3->correction(inJet->p4()*L2Scale);
      jet->SetL2RelativeCorrectionScale(L2Scale);
      jet->SetL3AbsoluteCorrectionScale(L3Scale);     
      jet->EnableCorrection(mithep::CaloJet::L2);
      jet->EnableCorrection(mithep::CaloJet::L3);     
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

    // add CaloTower references
    if (caloTowerMap_) {
      std::vector<CaloTowerDetId> ctidVector = inJet->getTowerIndices();
      for(std::vector<CaloTowerDetId>::const_iterator ctid = ctidVector.begin(); 
          ctid != ctidVector.end(); ++ctid) {
        mithep::CaloTower *caloTower = caloTowerMap_->GetMit(*ctid);
        jet->AddTower(caloTower);
      }
    }
    if (jetToVertexActive_) {
      it_jv_alpha++; 
      it_jv_beta++;
    }
  }      
  jets_->Trim();
}
