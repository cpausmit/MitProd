// $Id: FillerCaloJets.cc,v 1.8 2008/09/09 12:51:38 sixie Exp $

#include "MitProd/TreeFiller/interface/FillerCaloJets.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/Common/interface/Handle.h"
#include "MitAna/DataTree/interface/Names.h"
#include "SimDataFormats/JetMatching/interface/JetFlavour.h"
#include "SimDataFormats/JetMatching/interface/JetFlavourMatching.h"
#include "SimDataFormats/JetMatching/interface/MatchedPartons.h"
#include "SimDataFormats/JetMatching/interface/JetMatchedPartons.h"

#include "DataFormats/JetReco/interface/Jet.h"
#include "DataFormats/BTauReco/interface/JetTag.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerCaloJets::FillerCaloJets(const ParameterSet &cfg, const char *name, bool active) : 
  BaseFiller(cfg,name,active),
  flavorMatchingActive_(Conf().getUntrackedParameter<bool>("flavorMatchingActive",true)),
  bTaggingActive_(Conf().getUntrackedParameter<bool>("bTaggingActive",true)),
  edmName_(Conf().getUntrackedParameter<string>("edmName","recoCaloJets:iterativeCone5CaloJets")),
  mitName_(Conf().getUntrackedParameter<string>("mitName",Names::gkCaloJetBrn)),  
  flavorMatchingByReferenceName_(Conf().getUntrackedParameter<string>
                   ("flavorMatchingByReferenceName","srcByReference")),
  flavorMatchingDefinition_(Conf().getUntrackedParameter<string>
                   ("flavorMatchingDefinition","Algorithmic")),
  jetProbabilityBJetTagsName_(Conf().getUntrackedParameter<string>
                   ("jetProbabilityBJetTagsName","jetProbabilityBJetTags")),  
  jetBProbabilityBJetTagsName_(Conf().getUntrackedParameter<string>
                   ("jetBProbabilityBJetTagsName","jetBProbabilityBJetTags")),   
  simpleSecondaryVertexBJetTagsName_(Conf().getUntrackedParameter<string>
                   ("simpleSecondaryVertexBJetTagsName","simpleSecondaryVertexBJetTags")),
  combinedSecondaryVertexBJetTagsName_(Conf().getUntrackedParameter<string>
                   ("combinedSecondaryVertexBJetTagsName","combinedSecondaryVertexBJetTags")),
  combinedSecondaryVertexMVABJetTagsName_(Conf().getUntrackedParameter<string>
                   ("combinedSecondaryVertexMVABJetTagsName","combinedSecondaryVertexMVABJetTags")),
  impactParameterMVABJetTagsName_(Conf().getUntrackedParameter<string>
                   ("impactParameterMVABJetTagsName","impactParameterMVABJetTags")),
  trackCountingHighEffBJetTagsName_(Conf().getUntrackedParameter<string>
                   ("trackCountingHighEffBJetTagsName","trackCountingHighEffBJetTags")),
  trackCountingHighPurBJetTagsName_(Conf().getUntrackedParameter<string>
                   ("trackCountingHighPurBJetTagsName","trackCountingHighPurBJetTagsName")),
  softMuonBJetTagsName_(Conf().getUntrackedParameter<string>
                   ("softMuonBJetTagsName","softMuonBJetTagsName")),
  softMuonNoIPBJetTagsName_(Conf().getUntrackedParameter<string>
                   ("softMuonNoIPBJetTagsName","softMuonNoIPBJetTagsName")),
  softElectronBJetTagsName_(Conf().getUntrackedParameter<string>
                   ("softElectronBJetTagsName_","softElectronBJetTagsName_")),
  caloTowerMapName_(Conf().getUntrackedParameter<string>("caloTowerMapName","CaloTowerMap")),
  caloTowerMap_(0),
  jets_(new mithep::JetArr(16))
{
  // Constructor.
}

//--------------------------------------------------------------------------------------------------
FillerCaloJets::~FillerCaloJets()
{
  // Destructor.

  delete jets_;
}

//--------------------------------------------------------------------------------------------------
void FillerCaloJets::BookDataBlock(TreeWriter &tws)
{
  // Add jets branch to tree.

  tws.AddBranch(mitName_.c_str(),&jets_);

  if (!caloTowerMapName_.empty())
    caloTowerMap_ = OS()->get<CaloTowerMap>(caloTowerMapName_.c_str());
}

//--------------------------------------------------------------------------------------------------
void FillerCaloJets::FillDataBlock(const edm::Event      &event, 
                                   const edm::EventSetup &setup)
{
  // Fill jets from edm collection into our collection.

  jets_->Reset();

  //Handle for the Jet Collection
  Handle<reco::CaloJetCollection> hJetProduct;
  GetProduct(edmName_, hJetProduct, event);

  //Handles for jet flavour matching 
  Handle<reco::JetMatchedPartonsCollection> hPartonMatchingProduct;  
  if (flavorMatchingActive_) 
    GetProduct(flavorMatchingByReferenceName_, hPartonMatchingProduct, event);

  Handle<reco::JetTagCollection> hJetProbabilityBJetTags;
  Handle<reco::JetTagCollection> hJetBProbabilityBJetTags;
  Handle<reco::JetTagCollection> hSimpleSecondaryVertexBJetTags;
  Handle<reco::JetTagCollection> hCombinedSecondaryVertexBJetTags;
  Handle<reco::JetTagCollection> hCombinedSecondaryVertexMVABJetTags;
  Handle<reco::JetTagCollection> hImpactParameterMVABJetTags;
  Handle<reco::JetTagCollection> hTrackCountingHighEffBJetTags;
  Handle<reco::JetTagCollection> hTrackCountingHighPurBJetTags;
  Handle<reco::JetTagCollection> hSoftMuonBJetTags;
  Handle<reco::JetTagCollection> hSoftMuonNoIPBJetTags;
  Handle<reco::JetTagCollection> hSoftElectronBJetTags;

  if (bTaggingActive_) {
    GetProduct("jetProbabilityBJetTags", hJetProbabilityBJetTags, event);    
    GetProduct("jetBProbabilityBJetTags", hJetBProbabilityBJetTags, event);    
    GetProduct("simpleSecondaryVertexBJetTags", hSimpleSecondaryVertexBJetTags, event);    
    GetProduct("combinedSecondaryVertexBJetTags", hCombinedSecondaryVertexBJetTags, event);    
    GetProduct("combinedSecondaryVertexMVABJetTags", hCombinedSecondaryVertexMVABJetTags, event);
    GetProduct("impactParameterMVABJetTags", hImpactParameterMVABJetTags, event);    
    GetProduct("trackCountingHighEffBJetTags", hTrackCountingHighEffBJetTags, event);    
    GetProduct("trackCountingHighPurBJetTags", hTrackCountingHighPurBJetTags, event);    
    GetProduct("softMuonBJetTags", hSoftMuonBJetTags, event);    
    GetProduct("softMuonNoIPBJetTags", hSoftMuonNoIPBJetTags, event);    
    GetProduct("softElectronBJetTags", hSoftElectronBJetTags, event);    
  }

  const reco::CaloJetCollection inJets = *(hJetProduct.product());  

  // loop through all jets
  for (reco::CaloJetCollection::const_iterator inJet = inJets.begin(); 
       inJet != inJets.end(); ++inJet) {
    
    reco::CaloJetRef jetRef(hJetProduct, inJet - inJets.begin());    
    reco::JetBaseRef jetBaseRef(jetRef);

    

    mithep::Jet *jet = jets_->Allocate();
    new (jet) mithep::Jet(inJet->p4().x(),
                          inJet->p4().y(),
                          inJet->p4().z(),
                          inJet->p4().e());

    jet->SetMaxEInEmTowers (inJet->maxEInEmTowers());	 
    jet->SetMaxEInHadTowers(inJet->maxEInHadTowers());
    jet->SetEnergyFractionH(inJet->energyFractionHadronic());
    jet->SetHadEnergyInHB  (inJet->hadEnergyInHB());
    jet->SetHadEnergyInHO  (inJet->hadEnergyInHO());
    jet->SetHadEnergyInHE  (inJet->hadEnergyInHE());
    jet->SetHadEnergyInHF  (inJet->hadEnergyInHF());
    jet->SetEmEnergyInEB   (inJet->emEnergyInEB());
    jet->SetEmEnergyInEE   (inJet->emEnergyInEE());
    jet->SetEmEnergyInHF   (inJet->emEnergyInHF());
    jet->SetTowersArea     (inJet->towersArea());
    jet->SetN              (inJet->nConstituents());
    jet->SetN60		   (inJet->n60());
    jet->SetN90		   (inJet->n90());    


      
    if (bTaggingActive_) {
     //  cout << "jet " << inJet->eta() << " " << inJet->phi() << " "
//            << (*(hJetProbabilityBJetTags.product()))[jetBaseRef] <<  " "
//            << (*(hJetProbabilityBJetTags.product()))[jetBaseRef] << " "
//            << (*(hJetBProbabilityBJetTags.product()))[jetBaseRef] << " "
//            << (*(hSimpleSecondaryVertexBJetTags.product()))[jetBaseRef] << " "
//            << (*(hCombinedSecondaryVertexBJetTags.product()))[jetBaseRef]   << " " 
//            << (*(hCombinedSecondaryVertexMVABJetTags.product()))[jetBaseRef] << " "
//            << (*(hImpactParameterMVABJetTags.product()))[jetBaseRef] << " "
//            << (*(hTrackCountingHighEffBJetTags.product()))[jetBaseRef] << " "
//            << (*(hTrackCountingHighPurBJetTags.product()))[jetBaseRef] << " "
//            << (*(hSoftMuonBJetTags.product()))[jetBaseRef] << " "
//            << (*(hSoftMuonNoIPBJetTags.product()))[jetBaseRef] << " "
//            << (*(hSoftElectronBJetTags.product()))[jetBaseRef] << " "
//            << endl;

      jet->SetJetProbabilityBJetTagsDisc((*(hJetProbabilityBJetTags.product()))[jetBaseRef]);
      jet->SetJetBProbabilityBJetTagsDisc((*(hJetProbabilityBJetTags.product()))[jetBaseRef]);
      jet->SetSimpleSecondaryVertexBJetTagsDisc(
        (*(hJetBProbabilityBJetTags.product()))[jetBaseRef]);       
      jet->SetCombinedSecondaryVertexBJetTagsDisc(
        (*(hSimpleSecondaryVertexBJetTags.product()))[jetBaseRef]);   
      jet->SetCombinedSecondaryVertexMVABJetTagsDisc(
        (*(hCombinedSecondaryVertexBJetTags.product()))[jetBaseRef]); 
      jet->SetImpactParameterMVABJetTagsDisc(
        (*(hImpactParameterMVABJetTags.product()))[jetBaseRef]);  
      jet->SetTrackCountingHighEffBJetTagsDisc(
        (*(hTrackCountingHighEffBJetTags.product()))[jetBaseRef]);  
      jet->SetTrackCountingHighPurBJetTagsDisc(
        (*(hTrackCountingHighPurBJetTags.product()))[jetBaseRef]); 
      jet->SetSoftMuonBJetTagsDisc((*(hSoftMuonBJetTags.product()))[jetBaseRef]);
      jet->SetSoftMuonNoIPBJetTagsDisc((*(hSoftMuonNoIPBJetTags.product()))[jetBaseRef]); 
      jet->SetSoftElectronBJetTagsDisc((*(hSoftElectronBJetTags.product()))[jetBaseRef]); 
    }

    //Get the Monte Carlo Flavour Matching information
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

    //Add CaloTower Refs
    if (caloTowerMap_) {
      std::vector<CaloTowerDetId> ctidVector = inJet->getTowerIndices();
      for(std::vector<CaloTowerDetId>::const_iterator ctid = ctidVector.begin(); ctid != ctidVector.end(); ++ctid) {
        mithep::CaloTower *caloTower = caloTowerMap_->GetMit(*ctid);
        jet->AddTower(caloTower);
      }
    }
  }
  
  jets_->Trim();
}
