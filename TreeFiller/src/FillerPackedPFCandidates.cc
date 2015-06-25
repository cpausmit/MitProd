#include "MitProd/TreeFiller/interface/FillerPackedPFCandidates.h"

#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataTree/interface/PFCandidateCol.h"
#include "MitAna/DataTree/interface/Photon.h"
#include "MitAna/DataTree/interface/Electron.h"
#include "MitAna/DataTree/interface/Muon.h"
#include "MitProd/ObjectService/interface/ObjectService.h"

mithep::FillerPackedPFCandidates::FillerPackedPFCandidates(const edm::ParameterSet &cfg, edm::ConsumesCollector& collector, mithep::ObjectService* os, const char *name, bool active) :
  BaseFiller(cfg, os, name, active),
  edmToken_(GetToken<pat::PackedCandidateCollection>(collector, cfg, "edmName", "packedPFCandidates")),
  mitName_              (cfg.getUntrackedParameter<std::string>("mitName", mithep::Names::gkPFCandidatesBrn)),
  pfCandMapName_        (cfg.getUntrackedParameter<std::string>("pfCandMapName", "")),
  pfNoPileupCandMapName_(cfg.getUntrackedParameter<std::string>("pfNoPileupCandMapName", "")),
  fillPfNoPileup_       (cfg.getUntrackedParameter<bool>("fillPfNoPileup", true)),
  electronMapName_      (cfg.getUntrackedParameter<std::string>("electronMapName", "")),
  muonMapName_          (cfg.getUntrackedParameter<std::string>("muonMapName", "")),
  photonMapName_        (cfg.getUntrackedParameter<std::string>("photonMapName", "")),
  pfCandMap_            (new mithep::PFCandidateMap),
  pfNoPileupCandMap_    (new mithep::PFCandidateMap),
  pfCands_              (new mithep::PFCandidateArr(16)),
  electronMap_          (0),
  muonMap_              (0),
  photonMap_            (0)
{
  // Constructor.
}

mithep::FillerPackedPFCandidates::~FillerPackedPFCandidates() 
{
  // Destructor.

  delete pfCands_;
  delete pfCandMap_;
  delete pfNoPileupCandMap_;
}

void
mithep::FillerPackedPFCandidates::BookDataBlock(mithep::TreeWriter &tws)
{ 
  // Add particle-flow candidate branch to tree and get pointers to maps.

  tws.AddBranch(mitName_, &pfCands_);
  OS()->add(pfCands_, mitName_);

  if (!pfCandMapName_.empty()) {
    pfCandMap_->SetBrName(mitName_);
    OS()->add(pfCandMap_, pfCandMapName_);
  }
  if (!pfNoPileupCandMapName_.empty()) {
    pfNoPileupCandMap_->SetBrName(mitName_);
    OS()->add(pfNoPileupCandMap_, pfNoPileupCandMapName_);
  }

  if (!electronMapName_.empty()) {
    electronMap_ = OS()->get<mithep::CandidateMap>(electronMapName_);
    if (electronMap_)
      AddBranchDep(mitName_, electronMap_->GetBrName());
  }
  if (!muonMapName_.empty()) {
    muonMap_ = OS()->get<mithep::CandidateMap>(muonMapName_);
    if (muonMap_)
      AddBranchDep(mitName_, muonMap_->GetBrName());
  }
  if (!photonMapName_.empty()) {
    photonMap_ = OS()->get<mithep::CandidateMap>(photonMapName_);
    if (photonMap_)
      AddBranchDep(mitName_, photonMap_->GetBrName());
  }
}

void
mithep::FillerPackedPFCandidates::FillDataBlock(edm::Event const& event, edm::EventSetup const&)
{
  // Fill particle flow candidate information. 

  pfCands_->Delete();
  pfCandMap_->Reset();
  pfNoPileupCandMap_->Reset();

  // get PF Candidates
  edm::Handle<pat::PackedCandidateCollection> hPackedCandProduct;
  GetProduct(edmToken_, hPackedCandProduct, event);  
  pat::PackedCandidateCollection const& inPackedCands = *hPackedCandProduct;

  unsigned iPart = 0;
  for (auto&& inPart : inPackedCands) {
    mithep::PFCandidate *outPfCand = pfCands_->AddNew();

    outPfCand->SetPtEtaPhiM(inPart.pt(), inPart.eta(), inPart.phi(), inPart.mass());

    reco::CandidatePtr ptr(hPackedCandProduct, iPart);
    ++iPart;

    // fill standard variables
    outPfCand->SetCharge(inPart.charge());
    // fill source vertex
    outPfCand->SetVertex(inPart.vertex().x(), inPart.vertex().y(), inPart.vertex().z());
    
    // fill pf type enum
    switch (std::abs(inPart.pdgId())) {
    case 211:
      outPfCand->SetPFType(mithep::PFCandidate::eHadron);
      break;
    case 11:
      outPfCand->SetPFType(mithep::PFCandidate::eElectron);
      break;
    case 13:
      outPfCand->SetPFType(mithep::PFCandidate::eMuon);
      break;
    case 22:
      outPfCand->SetPFType(mithep::PFCandidate::eGamma);
      break;
    case 130:
      outPfCand->SetPFType(mithep::PFCandidate::eNeutralHadron);
      break;
    case 1:
      outPfCand->SetPFType(mithep::PFCandidate::eHadronHF);
      break;
    case 2:
      outPfCand->SetPFType(mithep::PFCandidate::eEGammaHF);
      break;
    case 0:
    default:
      outPfCand->SetPFType(mithep::PFCandidate::eX);
      break;
    }

    outPfCand->SetFlag(mithep::PFCandidate::ePFNoPileup, inPart.fromPV() > 0);

    // add to exported pf candidate map
    pfCandMap_->Add(ptr, outPfCand);
    
    // add pf No Pileup map
    if (fillPfNoPileup_ && outPfCand->Flag(mithep::PFCandidate::ePFNoPileup))
      pfNoPileupCandMap_->Add(ptr, outPfCand);
  }

  pfCands_->Trim();
}

void
mithep::FillerPackedPFCandidates::ResolveLinks(edm::Event const&, edm::EventSetup const&)
{
  if (!electronMap_ && !muonMap_ && !photonMap_)
    return;
  
  for (auto& ptrcand : pfCandMap_->FwdMap()) {
    auto& ptr = ptrcand.first;
    auto* outPfCand = ptrcand.second;
    
    if (electronMap_) {
      mithep::Electron const* electron = static_cast<mithep::Electron const*>(electronMap_->GetMit(ptr, false));
      if (electron)
        outPfCand->SetElectron(electron);
    }
    if (muonMap_) {
      mithep::Muon const* muon = static_cast<mithep::Muon const*>(muonMap_->GetMit(ptr, false));
      if (muon)
        outPfCand->SetMuon(muon);
    }
    if (photonMap_) {
      mithep::Photon const* photon = static_cast<mithep::Photon const*>(photonMap_->GetMit(ptr, false));
      if (photon)
        outPfCand->SetPhoton(photon);
    }
  }
}

DEFINE_MITHEP_TREEFILLER(FillerPackedPFCandidates);
