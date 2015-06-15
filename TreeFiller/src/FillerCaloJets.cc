#include "MitProd/TreeFiller/interface/FillerCaloJets.h"
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
FillerCaloJets::FillerCaloJets(const ParameterSet &cfg, edm::ConsumesCollector& collector, ObjectService* os, const char *name, bool active) : 
  BaseFiller(cfg,os,name,active),
  flavorMatchingActive_(Conf().getUntrackedParameter<bool>("flavorMatchingActive",true)),
  bTaggingActive_(Conf().getUntrackedParameter<bool>("bTaggingActive",true)),
  jetToVertexActive_(Conf().getUntrackedParameter<bool>("jetToVertexActive",true)),
  jetCorrectionsActive_(Conf().getUntrackedParameter<bool>("jetCorrectionsActive",true)),
  jetIDActive_(Conf().getUntrackedParameter<bool>("jetIDActive",false)),
  edmToken_(GetToken<reco::CaloJetCollection>(collector, "edmName","recoCaloJets:iterativeCone5CaloJets")),
  jetToVertexAlphaToken_(GetToken<std::vector<double> >(collector, "jetToVertexAlphaName","jetToVertexAlpha")),
  jetToVertexBetaToken_(GetToken<std::vector<double> >(collector, "jetToVertexBetaName","jetToVertexBetaName")),
  jetIDToken_(GetToken<reco::JetIDValueMap>(collector, "jetIDName","jetIDName")),
  flavorMatchingByReferenceToken_(GetToken<reco::JetMatchedPartonsCollection>(collector, "flavorMatchingByReferenceName", "srcByReference")),
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
  L2JetCorrectorName_(Conf().getUntrackedParameter<string>
                      ("L2JetCorrectorName","L2JetCorrectorName")),
  L3JetCorrectorName_(Conf().getUntrackedParameter<string>
                      ("L3JetCorrectorName","L3JetCorrectorName")),
  flavorMatchingDefinition_(Conf().getUntrackedParameter<string>
                   ("flavorMatchingDefinition","Algorithmic")),
  mitName_(Conf().getUntrackedParameter<string>("mitName",Names::gkCaloJetBrn)), 
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
  GetProduct(edmToken_, hJetProduct, event);

  // handles for jet flavour matching 
  Handle<reco::JetMatchedPartonsCollection> hPartonMatchingProduct;  
  if (flavorMatchingActive_) 
    GetProduct(flavorMatchingByReferenceToken_, hPartonMatchingProduct, event);

  // handle for jet id variables
  Handle<reco::JetIDValueMap> hJetIDMap;
  if (jetIDActive_)
    GetProduct( jetIDToken_, hJetIDMap, event);

  Handle<reco::JetTagCollection> hJetProbabilityBJetTags;
  Handle<reco::JetTagCollection> hJetBProbabilityBJetTags;
  Handle<reco::JetTagCollection> hSimpleSecondaryVertexBJetTags;
  Handle<reco::JetTagCollection> hSimpleSecondaryVertexHighEffBJetTags;
  Handle<reco::JetTagCollection> hSimpleSecondaryVertexHighPurBJetTags;
  Handle<reco::JetTagCollection> hCombinedSecondaryVertexBJetTags;
  Handle<reco::JetTagCollection> hCombinedSecondaryVertexMVABJetTags;
  Handle<reco::JetTagCollection> hTrackCountingHighEffBJetTags;
  Handle<reco::JetTagCollection> hTrackCountingHighPurBJetTags;
  Handle<reco::JetTagCollection> hGhostTrackBJetTags;

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
  
  reco::CaloJetCollection const& inJets = *hJetProduct;
  
  // handle to Jet to Vertex association
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
    edm::Ptr<reco::Jet> thePtr(hJetProduct, inJet - inJets.begin());
    jetMap_->Add(thePtr, jet);

    //fill jet moments    
    jet->SetSigmaEta(TMath::Sqrt(inJet->etaetaMoment()));
    jet->SetSigmaPhi(TMath::Sqrt(inJet->phiphiMoment()));

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

    //fill jet id variables
    if (jetIDActive_) {
      reco::JetID jetId = (*hJetIDMap)[jetBaseRef];

      jet->SetFHPD(jetId.fHPD);
      jet->SetFRBX(jetId.fRBX);
      jet->SetN90Hits(jetId.n90Hits);
      jet->SetFSubDetector1(jetId.fSubDetector1);
      jet->SetFSubDetector2(jetId.fSubDetector1);
      jet->SetFSubDetector3(jetId.fSubDetector1);
      jet->SetFSubDetector4(jetId.fSubDetector1);
      jet->SetRestrictedEMF(jetId.restrictedEMF);
      jet->SetNHCalTowers(jetId.nHCALTowers);
      jet->SetNECalTowers(jetId.nECALTowers);
      jet->SetApproximatefHPD(jetId.approximatefHPD);
      jet->SetApproximatefRBX(jetId.approximatefRBX);
      jet->SetHitsInN90(jetId.hitsInN90);
      jet->SetNHits2RPC(jetId.numberOfHits2RPC);
      jet->SetNHits3RPC(jetId.numberOfHits3RPC);
      jet->SetNHitsRPC(jetId.numberOfHitsRPC);

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

DEFINE_MITHEP_TREEFILLER(FillerCaloJets);
