// $Id: FillerCaloJets.cc,v 1.4 2008/07/13 08:46:04 loizides Exp $

#include "MitProd/TreeFiller/interface/FillerCaloJets.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/Common/interface/Handle.h"
#include "MitAna/DataTree/interface/Names.h"

#include "SimDataFormats/JetMatching/interface/JetFlavour.h"
#include "SimDataFormats/JetMatching/interface/JetFlavourMatching.h"
#include "SimDataFormats/JetMatching/interface/MatchedPartons.h"
#include "SimDataFormats/JetMatching/interface/JetMatchedPartons.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerCaloJets::FillerCaloJets(const ParameterSet &cfg, const char *name, bool active) : 
  BaseFiller(cfg,name,active),
  flavorMatchingActive_(Conf().getUntrackedParameter<bool>("flavorMatchingActive",true)),
  edmName_(Conf().getUntrackedParameter<string>("edmName","recoCaloJets:iterativeCone5CaloJets")),
  mitName_(Conf().getUntrackedParameter<string>("mitName",Names::gkCaloJetBrn)),  
  flavorMatchingByReferenceName_(Conf().getUntrackedParameter<string>
                                 ("flavorMatchingByReferenceName","srcByReference")),
  flavorMatchingDefinition_(Conf().getUntrackedParameter<string>
                             ("flavorMatchingDefinition","Algorithmic")),
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
}

//--------------------------------------------------------------------------------------------------
void FillerCaloJets::FillDataBlock(const edm::Event      &event, 
                                   const edm::EventSetup &setup)
{
  // Fill jets from edm collection into our collection.

  jets_->Reset();

  //Handles for jet flavour matching
  Handle<reco::JetMatchedPartonsCollection> hPartonMatchingProduct;
  GetProduct(flavorMatchingByReferenceName_, hPartonMatchingProduct, event);

  Handle<reco::CaloJetCollection> hJetProduct;
  GetProduct(edmName_, hJetProduct, event);

  const reco::CaloJetCollection inJets = *(hJetProduct.product());  

  // loop through all jets
  for (reco::CaloJetCollection::const_iterator inJet = inJets.begin(); 
       inJet != inJets.end(); ++inJet) {

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

    //Get the Monte Carlo Flavour Matching information
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

  // for ( reco::JetMatchedPartonsCollection::const_iterator j  = hPartonMatchingProduct->begin();
//         j != hPartonMatchingProduct->end();
//         j ++ ) {
//     const reco::Jet *aJet       = (*j).first.get();
//     const reco::MatchedPartons aMatch = (*j).second;
//     const reco::GenParticleRef theHeaviest = aMatch.heaviest() ;
//     const reco::GenParticleRef theNearest2 = aMatch.nearest_status2() ;
//     const reco::GenParticleRef theNearest3 = aMatch.nearest_status3() ;
//     const reco::GenParticleRef thePhyDef = aMatch.physicsDefinitionParton() ;
//     const reco::GenParticleRef theAlgDef = aMatch.algoDefinitionParton() ;  
//     int physDef = (thePhyDef.isNonnull())?thePhyDef->pdgId():-1;
//     int algdef = (theAlgDef.isNonnull())?theAlgDef->pdgId():-1;

//     cout << j-hPartonMatchingProduct->begin() << " " << aJet->p4().pt() << " " << aJet->p4().eta() << " " << aJet->p4().phi() << " " << physDef << " " << algdef << endl;

//   }

//   for ( reco::JetFlavourMatchingCollection::const_iterator j  = hFlavourMatchingProduct->begin();
//         j != hFlavourMatchingProduct->end();
//         j ++ ) {
//     RefToBase<reco::Jet> aJet  = (*j).first;   
//     const reco::JetFlavour aFlav = (*j).second;
//     int flavour = aFlav.getFlavour();
//     cout << j-hFlavourMatchingProduct->begin() << " " << aJet->p4().pt() << " " << aJet->p4().eta() << " " << aJet->p4().phi() << " " << flavour << endl;
//   }

  jets_->Trim();
}
