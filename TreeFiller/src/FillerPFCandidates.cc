#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/EgammaCandidates/interface/Photon.h"
#include "DataFormats/Common/interface/RefToPtr.h"

#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataTree/interface/PFCandidateCol.h"
#include "MitAna/DataTree/interface/Muon.h"
#include "MitAna/DataTree/interface/Electron.h"
#include "MitAna/DataTree/interface/Photon.h"
#include "MitAna/DataTree/interface/Track.h"
#include "MitAna/DataTree/interface/SuperCluster.h"
#include "MitProd/ObjectService/interface/ObjectService.h"
#include "MitProd/TreeFiller/interface/FillerPFCandidates.h"

mithep::FillerPFCandidates::FillerPFCandidates(edm::ParameterSet const& cfg, edm::ConsumesCollector& collector, mithep::ObjectService* os, const char *name, bool active) :
  BaseFiller(cfg, os, name, active),
  fillPfNoPileup_           (cfg.getUntrackedParameter<bool>("fillPfNoPileup", true)),
  fillPuppiMap_             (cfg.getUntrackedParameter<bool>("fillPuppiMap", true)),
  edmToken_                 (GetToken<PFCollection>(collector, cfg, "edmName")), //particleFlow
  edmPfNoPileupToken_       (GetToken<PFCollection>(collector, cfg, "edmPfNoPileupName", fillPfNoPileup_)), //pfNoElectrons
  edmPuppiMapTokens_        (),
  edmPuppiToken_            (GetToken<CandidateView>(collector, cfg, "edmPuppiName", fillPuppiMap_)),
  mitName_                  (cfg.getUntrackedParameter<std::string>("mitName", mithep::Names::gkPFCandidatesBrn)),
  trackerTrackMapNames_     (cfg.getUntrackedParameter<std::vector<std::string> >("trackerTrackMapNames", std::vector<std::string>())),
  gsfTrackMapName_          (cfg.getUntrackedParameter<std::string>("gsfTrackMapName", "")),
  muonMapName_              (cfg.getUntrackedParameter<std::string>("muonMapName", "")),
  electronMapName_          (cfg.getUntrackedParameter<std::string>("electronMapName", "")),
  photonMapName_            (cfg.getUntrackedParameter<std::string>("photonMapName", "")),
  barrelSuperClusterMapName_(cfg.getUntrackedParameter<std::string>("barrelSuperClusterMapName", "")),
  endcapSuperClusterMapName_(cfg.getUntrackedParameter<std::string>("endcapSuperClusterMapName", "")),
  pfCandMapName_            (cfg.getUntrackedParameter<std::string>("pfCandMapName", "")),
  pfNoPileupCandMapName_    (cfg.getUntrackedParameter<std::string>("pfNoPileupCandMapName", "")),
  puppiMapName_             (cfg.getUntrackedParameter<std::string>("puppiMapName", "")),
  allowMissingTrackRef_     (cfg.getUntrackedParameter<bool>("allowMissingTrackRef", false)),
  allowMissingClusterRef_   (cfg.getUntrackedParameter<bool>("allowMissingClusterRef", false)),
  allowMissingPhotonRef_    (cfg.getUntrackedParameter<bool>("allowMissingPhotonRef", false)),
  gsfTrackMap_              (0),
  muonMap_                  (0),
  electronMap_              (0),
  photonMap_                (0),
  barrelSuperClusterMap_    (0),
  endcapSuperClusterMap_    (0),
  pfCandMap_                (new mithep::PFCandidateMap),
  pfNoPileupCandMap_        (0),
  puppiMap_                 (0),
  pfCands_                  (new mithep::PFCandidateArr(16))
{
  if (fillPuppiMap_) {
    auto&& puppiMapNames(cfg.getUntrackedParameter<std::vector<std::string>>("edmPuppiMapNames", std::vector<std::string>()));
    for (auto& name : puppiMapNames)
      edmPuppiMapTokens_.push_back(collector.consumes<CandidatePtrMap>(edm::InputTag(name)));
  }
}

mithep::FillerPFCandidates::~FillerPFCandidates() 
{
  delete pfCands_;
  delete pfCandMap_;
  delete pfNoPileupCandMap_;
  delete puppiMap_;
}

void
mithep::FillerPFCandidates::BookDataBlock(mithep::TreeWriter& tws)
{ 
  // Add particle-flow candidate branch to tree and get pointers to maps.

  tws.AddBranch(mitName_, &pfCands_);
  OS()->add(pfCands_, mitName_);

  if (!pfCandMapName_.empty()) {
    pfCandMap_->SetBrName(mitName_);
    OS()->add(pfCandMap_, pfCandMapName_);
  }

  if (fillPfNoPileup_ && !pfNoPileupCandMapName_.empty()) {
    pfNoPileupCandMap_ = new mithep::PFCandidateMap;
    pfNoPileupCandMap_->SetBrName(mitName_);
    OS()->add(pfNoPileupCandMap_, pfNoPileupCandMapName_);
  }

  if (fillPuppiMap_ && !puppiMapName_.empty()) {
    puppiMap_ = new mithep::PFCandidateMap;
    puppiMap_->SetBrName(mitName_);
    OS()->add(puppiMap_, puppiMapName_);
  }
}

void
mithep::FillerPFCandidates::PrepareLinks()
{
  for (auto&& bmapName : trackerTrackMapNames_) {
    if (!bmapName.empty()) {
      auto* map = OS()->get<TrackMap>(bmapName);
      trackerTrackMaps_.push_back(map);
      AddBranchDep(mitName_,map->GetBrName());
    }
  }
  if (!gsfTrackMapName_.empty()) {
    gsfTrackMap_ = OS()->get<TrackMap>(gsfTrackMapName_);
    AddBranchDep(mitName_,gsfTrackMap_->GetBrName());
  }
  if (!muonMapName_.empty()) {
    muonMap_ = OS()->get<MuonMap>(muonMapName_);
    AddBranchDep(mitName_,muonMap_->GetBrName());
  }
  if (!electronMapName_.empty()) {
    electronMap_ = OS()->get<ElectronMap>(electronMapName_);
    AddBranchDep(mitName_,electronMap_->GetBrName());
  }
  if (!photonMapName_.empty()) {
    photonMap_ = OS()->get<PhotonMap>(photonMapName_);
    AddBranchDep(mitName_,photonMap_->GetBrName());
  }
  if (!barrelSuperClusterMapName_.empty()) {
    barrelSuperClusterMap_ = OS()->get<SuperClusterMap>(barrelSuperClusterMapName_);
    AddBranchDep(mitName_,barrelSuperClusterMap_->GetBrName());
  }
  if (!endcapSuperClusterMapName_.empty()) {
    endcapSuperClusterMap_ = OS()->get<SuperClusterMap>(endcapSuperClusterMapName_);
    AddBranchDep(mitName_,endcapSuperClusterMap_->GetBrName());
  }
}

void
mithep::FillerPFCandidates::FillDataBlock(edm::Event const& event, edm::EventSetup const&)
{
  // Fill particle flow candidate information. 

  pfCands_->Delete();
  pfCandMap_->Reset();
  if (pfNoPileupCandMap_)
    pfNoPileupCandMap_->Reset();
  if (puppiMap_)
    puppiMap_->Reset();

  // get PF Candidates
  edm::Handle<PFCollection> hPfCandProduct;
  GetProduct(edmToken_, hPfCandProduct, event);  
  PFCollection const& inPfCands = *hPfCandProduct;

  // get PF NoPileup Candidates
  PFCollection const* inPfNoPileupCands = 0;
  if (fillPfNoPileup_) {
    edm::Handle<PFCollection> hPfNoPileupCandProduct;
    GetProduct(edmPfNoPileupToken_, hPfNoPileupCandProduct, event);  
    inPfNoPileupCands = hPfNoPileupCandProduct.product();
  }

  std::vector<CandidatePtrMap const*> candidateMaps;
  edm::ProductID puppiCandsId;
  if (puppiMap_) {
    for (auto& token : edmPuppiMapTokens_) {
      edm::Handle<CandidatePtrMap> handle;
      GetProduct(token, handle, event);
      candidateMaps.push_back(handle.product());
    }

    edm::Handle<CandidateView> hPuppiProduct;
    GetProduct(edmPuppiToken_, hPuppiProduct, event);
    puppiCandsId = hPuppiProduct.id();
  }

  for (auto&& inPfPtr : inPfCands) {
    reco::PFCandidate const& inPf = *inPfPtr;

    mithep::PFCandidate *outPfCand = pfCands_->AddNew();

    outPfCand->SetPtEtaPhiM(inPf.pt(), inPf.eta(), inPf.phi(), inPf.mass());

    // fill standard variables
    outPfCand->SetCharge(inPf.charge());
    outPfCand->SetEECal(inPf.ecalEnergy());
    outPfCand->SetEHCal(inPf.hcalEnergy());
    // outPfCand->SetEECalRaw(inPf.rawEcalEnergy());
    // outPfCand->SetEHCalRaw(inPf.rawHcalEnergy());
    // outPfCand->SetEPS1(inPf.pS1Energy());
    // outPfCand->SetEPS2(inPf.pS2Energy());
    outPfCand->SetPError(inPf.deltaP());
    // outPfCand->SetMvaEPi(inPf.mva_e_pi());
    // outPfCand->SetMvaEMu(inPf.mva_e_mu());
    // outPfCand->SetMvaPiMu(inPf.mva_pi_mu());
    // outPfCand->SetMvaNeutralH(inPf.mva_nothing_nh());
    // outPfCand->SetMvaGammaNeutralH(inPf.mva_gamma_nh());
    outPfCand->SetEtaECal(inPf.positionAtECALEntrance().eta());
    outPfCand->SetPhiECal(inPf.positionAtECALEntrance().phi());
    
    // fill source vertex
    outPfCand->SetVertex(inPf.vertex().x(), inPf.vertex().y(), inPf.vertex().z());
    
    // fill pf type enum
    switch (inPf.particleId()) {
    case reco::PFCandidate::X:
      outPfCand->SetPFType(mithep::PFCandidate::eX);
      break;
    case reco::PFCandidate::h:
      outPfCand->SetPFType(mithep::PFCandidate::eHadron);
      break;
    case reco::PFCandidate::e:
      outPfCand->SetPFType(mithep::PFCandidate::eElectron);
      break;
    case reco::PFCandidate::mu:
      outPfCand->SetPFType(mithep::PFCandidate::eMuon);
      break;
    case reco::PFCandidate::gamma:
      outPfCand->SetPFType(mithep::PFCandidate::eGamma);
      break;
    case reco::PFCandidate::h0:
      outPfCand->SetPFType(mithep::PFCandidate::eNeutralHadron);
      break;
    case reco::PFCandidate::h_HF:
      outPfCand->SetPFType(mithep::PFCandidate::eHadronHF);
      break;
    case reco::PFCandidate::egamma_HF:
      outPfCand->SetPFType(mithep::PFCandidate::eEGammaHF);
      break;
    default:
      break;
    }
    
    // fill pf flags bitmask
    outPfCand->SetFlag(mithep::PFCandidate::eNormal,
    		       inPf.flag(reco::PFCandidate::NORMAL));
    outPfCand->SetFlag(mithep::PFCandidate::eEMPhiSModules,
    		       inPf.flag(reco::PFCandidate::E_PHI_SMODULES));
    outPfCand->SetFlag(mithep::PFCandidate::eEMEta0,
    		       inPf.flag(reco::PFCandidate::E_ETA_0));
    outPfCand->SetFlag(mithep::PFCandidate::eEMEtaModules,
    		       inPf.flag(reco::PFCandidate::E_ETA_MODULES));
    outPfCand->SetFlag(mithep::PFCandidate::eEMBarrelEndcap, 
                       inPf.flag(reco::PFCandidate::E_BARREL_ENDCAP));
    outPfCand->SetFlag(mithep::PFCandidate::eEMPreshowerEdge, 
                       inPf.flag(reco::PFCandidate::E_PRESHOWER_EDGE));
    outPfCand->SetFlag(mithep::PFCandidate::eEMPreshower, 
                       inPf.flag(reco::PFCandidate::E_PRESHOWER));
    outPfCand->SetFlag(mithep::PFCandidate::eEMEndCapEdge, 
                       inPf.flag(reco::PFCandidate::E_ENDCAP_EDGE));
    outPfCand->SetFlag(mithep::PFCandidate::eHEta0, 
                       inPf.flag(reco::PFCandidate::H_ETA_0));
    outPfCand->SetFlag(mithep::PFCandidate::eHBarrelEndcap, 
                       inPf.flag(reco::PFCandidate::H_BARREL_ENDCAP));
    outPfCand->SetFlag(mithep::PFCandidate::eHEndcapVFCal, 
                       inPf.flag(reco::PFCandidate::H_ENDCAP_VFCAL));
    outPfCand->SetFlag(mithep::PFCandidate::eHVFCalEdge, 
                       inPf.flag(reco::PFCandidate::H_VFCAL_EDGE));
    outPfCand->SetFlag(mithep::PFCandidate::eToDispVtx, 
                       inPf.flag(reco::PFCandidate::T_TO_DISP));
    outPfCand->SetFlag(mithep::PFCandidate::eFromDispVtx, 
                       inPf.flag(reco::PFCandidate::T_FROM_DISP));
    outPfCand->SetFlag(mithep::PFCandidate::eFromV0, 
                       inPf.flag(reco::PFCandidate::T_FROM_V0));
    outPfCand->SetFlag(mithep::PFCandidate::eFromGammaConv, 
                       inPf.flag(reco::PFCandidate::T_FROM_GAMMACONV));
    outPfCand->SetFlag(mithep::PFCandidate::eToConversion, 
                       inPf.flag(reco::PFCandidate::GAMMA_TO_GAMMACONV));

    // initially set the candidate to be not part of the NoPilup collection
    outPfCand->SetFlag(mithep::PFCandidate::ePFNoPileup, false);

    // add to exported pf candidate map
    pfCandMap_->Add(reco::CandidatePtr(inPfPtr.ptr()), outPfCand);

    // add pf No Pileup map
    if (fillPfNoPileup_) { 
      bool found = false;
      // try to find match with the no-pileup map
      for (auto&& inPfNpPtr : *inPfNoPileupCands) {
    	if (inPfNpPtr != inPfPtr)
          continue;

        if (found)
          edm::Exception(edm::errors::Configuration, "FillerPFCandidates:FillDataBlock()\n")
            << "PF No Pileup was double linked!! ";

        found = true;

        // set the candidate to be part of the NoPilup collection
        outPfCand->SetFlag(mithep::PFCandidate::ePFNoPileup, true);
        // add it to our map

        if (pfNoPileupCandMap_)
          pfNoPileupCandMap_->Add(reco::CandidatePtr(inPfNpPtr.ptr()), outPfCand);
      }
    }

    if (puppiMap_) {
      reco::CandidatePtr targetPtr(inPfPtr.id(), inPfPtr.get(), inPfPtr.key());

      // trace the mappings from the original PF candidate to the puppi candidate
      for (auto* map : candidateMaps) {
        try {
          reco::CandidatePtr mappedPtr(map->get(targetPtr.id(), targetPtr.key()));
          if (mappedPtr.isNonnull()) {
            // non-null target found; trace up one level
            targetPtr = mappedPtr;
          }
        }
        catch (edm::Exception&) {
          // pass
        }
      }

      if (targetPtr.id() != puppiCandsId)
          throw edm::Exception(edm::errors::Configuration, "FillerPFCandidates::FillDataBlock()\n")
            << "Candidate was not found in the list of PUPPI particles.";

      // link the identified puppi candidate to the Bambu PFCandidate
      puppiMap_->Add(targetPtr, outPfCand);
    }
  }

  pfCands_->Trim();
}

void
mithep::FillerPFCandidates::ResolveLinks(edm::Event const&, edm::EventSetup const&)
{
  for (auto& ptrcand : pfCandMap_->FwdMap()) {
    auto& inPf = static_cast<reco::PFCandidate const&>(*ptrcand.first);
    auto* outPfCand = ptrcand.second;

    // fill references to other branches
    if (inPf.trackRef().isNonnull()) {
      auto* thetrack = getMitTrack(refToPtr(inPf.trackRef()), allowMissingTrackRef_);
      outPfCand->AddRef(thetrack);
    }    

    // linking with the GfsTracks
    if (gsfTrackMap_ && inPf.gsfTrackRef().isNonnull()) 
      outPfCand->AddRef(gsfTrackMap_->GetMit(refToPtr(inPf.gsfTrackRef())));
    
    // linking with the Muons
    if (muonMap_ && inPf.muonRef().isNonnull()) 
      outPfCand->AddRef(muonMap_->GetMit(refToPtr(inPf.muonRef())));
    
    // linking with the Electrons
    if (electronMap_ && inPf.gsfElectronRef().isNonnull()) 
      outPfCand->AddRef(electronMap_->GetMit(refToPtr(inPf.gsfElectronRef())));
    
    // linking with the Photons
    if (photonMap_ && inPf.photonRef().isNonnull())
      outPfCand->AddRef(photonMap_->GetMit(refToPtr(inPf.photonRef()), !allowMissingPhotonRef_));
    
    // linking with the SuperClusters
    if (barrelSuperClusterMap_ && endcapSuperClusterMap_ &&
    	inPf.superClusterRef().isNonnull()) {
      auto* sc = barrelSuperClusterMap_->GetMit(inPf.superClusterRef(), false);
      if (!sc)
        sc = endcapSuperClusterMap_->GetMit(inPf.superClusterRef(), false);

      if (sc)
        outPfCand->AddRef(sc);
      else if(!allowMissingClusterRef_)
        throw edm::Exception(edm::errors::Configuration, "FillerPFCandidates::FillDataBlock()\n")
          << "Error! Refined SuperCluster reference in unmapped collection";
    }
  }    
}

mithep::Track const*
mithep::FillerPFCandidates::getMitTrack(mitedm::TrackPtr ptr, bool allowMissing) const
{
  // Return our particle referenced by the edm pointer.

  for (auto&& bmap : trackerTrackMaps_) {
    mithep::Track const* mitPart = bmap->GetMit(ptr, false);
    if (mitPart)
      return mitPart;
  }
  
  // make sure we got a link if it is required
  if (!allowMissing)
    throw edm::Exception(edm::errors::Configuration, "FillerPFCandidates::FillDataBlock()\n")
      << "Error! MITHEP Object " 
      << "not found in AssociationMaps (" << typeid(*this).name() << ").";
    
  return 0;
}

DEFINE_MITHEP_TREEFILLER(FillerPFCandidates);
