#include "MitProd/TreeFiller/interface/FillerGenJets.h"
#include "DataFormats/JetReco/interface/GenJet.h"
#include "SimDataFormats/JetMatching/interface/JetFlavour.h"
#include "SimDataFormats/JetMatching/interface/JetFlavourMatching.h"
#include "SimDataFormats/JetMatching/interface/MatchedPartons.h"
#include "MitAna/DataTree/interface/GenJetCol.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitProd/ObjectService/interface/ObjectService.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerGenJets::FillerGenJets(const ParameterSet &cfg, edm::ConsumesCollector& collector, ObjectService* os, const char *name, bool active) : 
  BaseFiller(cfg,os,name,active),
  flavorMatchingActive_(Conf().getUntrackedParameter<bool>("flavorMatchingActive",true)),
  edmToken_(GetToken<reco::GenJetCollection>(collector, "edmName", "genjets")),
  flavorMatchingByReferenceToken_(GetToken<reco::JetMatchedPartonsCollection>(collector, "flavorMatchingByReferenceName", "srcByReference")),
  mitName_(Conf().getUntrackedParameter<string>("mitName","GenJets")), 
  flavorMatchingDefinition_(Conf().getUntrackedParameter<string>
                   ("flavorMatchingDefinition","Algorithmic")),
  genjets_(new mithep::GenJetArr(16))
{
  // Constructor.
}

//--------------------------------------------------------------------------------------------------
FillerGenJets::~FillerGenJets()
{
  // Destructor.

  delete genjets_;
}

//--------------------------------------------------------------------------------------------------
void FillerGenJets::BookDataBlock(TreeWriter &tws)
{
  // Add jets branch to tree.

  tws.AddBranch(mitName_,&genjets_);
  OS()->add<mithep::GenJetArr>(genjets_,mitName_);
}

//--------------------------------------------------------------------------------------------------
void FillerGenJets::FillDataBlock(const edm::Event      &event, 
                                  const edm::EventSetup &setup)
{
  // Fill jets from edm collection into our collection.

  genjets_->Delete();

  // handle for the jet collection
  Handle<reco::GenJetCollection> hGenJetProduct;
  GetProduct(edmToken_, hGenJetProduct, event);

  // handles for jet flavour matching 
  Handle<reco::JetMatchedPartonsCollection> hPartonMatchingProduct;  
  if (flavorMatchingActive_) 
    GetProduct(flavorMatchingByReferenceToken_, hPartonMatchingProduct, event);

  reco::GenJetCollection const& inJets = *hGenJetProduct;
  
  // loop through all jets
  for (reco::GenJetCollection::const_iterator inJet = inJets.begin(); 
       inJet != inJets.end(); ++inJet) {
    
    reco::GenJetRef jetRef(hGenJetProduct, inJet - inJets.begin());    
    
    mithep::GenJet *jet = genjets_->Allocate();
    new (jet) mithep::GenJet(inJet->p4().x(),inJet->p4().y(),inJet->p4().z(),inJet->p4().e());
    jet->SetHadEnergy(inJet->hadEnergy());
    jet->SetEmEnergy(inJet->emEnergy());
    jet->SetInvisibleEnergy(inJet->invisibleEnergy());
    jet->SetAuxiliaryEnergy(inJet->auxiliaryEnergy());

    if (flavorMatchingActive_) {
      unsigned int iJet = inJet - inJets.begin();
      const reco::JetMatchedPartonsCollection *matchedPartons = hPartonMatchingProduct.product();
      reco::MatchedPartons matchedParton = (*matchedPartons)[matchedPartons->key(iJet)];
      int flavorPhysDef = (matchedParton.physicsDefinitionParton().isNonnull())?
        matchedParton.physicsDefinitionParton()->pdgId():0;
      int flavorAlgDef = (matchedParton.algoDefinitionParton().isNonnull())?
        matchedParton.algoDefinitionParton()->pdgId():0;
      
      if (flavorMatchingDefinition_ == "Algorithmic")
        jet->SetMatchedMCFlavor(flavorAlgDef);
      else if(flavorMatchingDefinition_ == "Physics")
        jet->SetMatchedMCFlavor(flavorPhysDef);
      else
        jet->SetMatchedMCFlavor(0);

    }     
  }

  genjets_->Trim();
}
