#include "MitProd/TreeFiller/interface/FillerCaloJets.h"

#include "DataFormats/JetReco/interface/CaloJet.h"

mithep::FillerCaloJets::FillerCaloJets(edm::ParameterSet const& cfg, edm::ConsumesCollector& collector, mithep::ObjectService* os, char const* name, bool active/* = true*/) : 
  FillerJets(cfg, collector, os, name, active),
  jetIDActive_(cfg.getUntrackedParameter<bool>("jetIDActive", false)),
  jetIDToken_(GetToken<reco::JetIDValueMap>(collector, cfg, "jetIDName", jetIDActive_)), //jetIDName
  jetIDMap_(0),
  caloTowerMapName_(cfg.getUntrackedParameter<std::string>("caloTowerMapName", "CaloTowerMap")),
  caloTowerMap_(0)
{
  jets_ = new mithep::CaloJetArr(32);
}

mithep::FillerCaloJets::~FillerCaloJets()
{
}

void
mithep::FillerCaloJets::PrepareLinks()
{
  if (!caloTowerMapName_.empty()) {
    caloTowerMap_ = OS()->get<mithep::CaloTowerMap>(caloTowerMapName_);
    AddBranchDep(mitName_, caloTowerMap_->GetBrName());
  }
}

void
mithep::FillerCaloJets::PrepareSpecific(edm::Event const& event, edm::EventSetup const&)
{
  jetIDMap_ = 0;
  if (jetIDActive_) {
    edm::Handle<reco::JetIDValueMap> hJetIDMap;
    GetProduct(jetIDToken_, hJetIDMap, event);
    jetIDMap_ = hJetIDMap.product();
  }
}

void
mithep::FillerCaloJets::FillSpecific(mithep::Jet& outBaseJet, reco::JetBaseRef const& inJetRef)
{
  auto* inCaloJet(dynamic_cast<reco::CaloJet const*>(inJetRef.get()));
  if (!inCaloJet)
    return;

  mithep::CaloJet& outJet(static_cast<mithep::CaloJet&>(outBaseJet));

  // fill calojet-specific quantities
  outJet.SetMaxEInEmTowers (inCaloJet->maxEInEmTowers());	 
  outJet.SetMaxEInHadTowers(inCaloJet->maxEInHadTowers());
  outJet.SetEnergyFractionH(inCaloJet->energyFractionHadronic());
  outJet.SetEnergyFractionEm(inCaloJet->emEnergyFraction());
  outJet.SetHadEnergyInHB  (inCaloJet->hadEnergyInHB());
  outJet.SetHadEnergyInHO  (inCaloJet->hadEnergyInHO());
  outJet.SetHadEnergyInHE  (inCaloJet->hadEnergyInHE());
  outJet.SetHadEnergyInHF  (inCaloJet->hadEnergyInHF());
  outJet.SetEmEnergyInEB   (inCaloJet->emEnergyInEB());
  outJet.SetEmEnergyInEE   (inCaloJet->emEnergyInEE());
  outJet.SetEmEnergyInHF   (inCaloJet->emEnergyInHF());
  outJet.SetTowersArea     (inCaloJet->towersArea());

  //fill jet id variables
  if (jetIDActive_) {
    auto&& jetId = (*jetIDMap_)[inJetRef];

    outJet.SetFHPD(jetId.fHPD);
    outJet.SetFRBX(jetId.fRBX);
    outJet.SetN90Hits(jetId.n90Hits);
    outJet.SetFSubDetector1(jetId.fSubDetector1);
    outJet.SetFSubDetector2(jetId.fSubDetector1);
    outJet.SetFSubDetector3(jetId.fSubDetector1);
    outJet.SetFSubDetector4(jetId.fSubDetector1);
    outJet.SetRestrictedEMF(jetId.restrictedEMF);
    outJet.SetNHCalTowers(jetId.nHCALTowers);
    outJet.SetNECalTowers(jetId.nECALTowers);
    outJet.SetApproximatefHPD(jetId.approximatefHPD);
    outJet.SetApproximatefRBX(jetId.approximatefRBX);
    outJet.SetHitsInN90(jetId.hitsInN90);
    outJet.SetNHits2RPC(jetId.numberOfHits2RPC);
    outJet.SetNHits3RPC(jetId.numberOfHits3RPC);
    outJet.SetNHitsRPC(jetId.numberOfHitsRPC);
  }
}

void
mithep::FillerCaloJets::ResolveLinks(edm::Event const&, edm::EventSetup const&)
{
  if (!caloTowerMap_)
    return;

  // add CaloTower references
  for (auto& mapElem : jetMap_->FwdMap()) {
    auto&& jPtr = mapElem.first;
    auto& inJet = static_cast<reco::CaloJet const&>(*jPtr);
    auto& outJet = static_cast<mithep::CaloJet&>(*mapElem.second);

    auto&& ctidVector = inJet.getTowerIndices();
    for (auto&& towerId : inJet.getTowerIndices())
      outJet.AddTower(caloTowerMap_->GetMit(towerId));
  }
}      

DEFINE_MITHEP_TREEFILLER(FillerCaloJets);
