#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/EgammaCandidates/interface/Photon.h"
#include "DataFormats/Common/interface/RefToPtr.h"

#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataTree/interface/PFCandidateCol.h"
#include "MitProd/ObjectService/interface/ObjectService.h"
#include "MitProd/TreeFiller/interface/FillerPFCandidates.h"

mithep::FillerPFCandidates::FillerPFCandidates(edm::ParameterSet const& cfg, edm::ConsumesCollector& collector, mithep::ObjectService* os, const char *name, bool active) :
  BaseFiller(cfg, os, name, active),
  edmToken_(GetToken<PFCollection>(collector, cfg, "edmName", "particleFlow")),
  edmPfNoPileupToken_(GetToken<PFCollection>(collector, cfg, "edmPfNoPileupName", "pfNoElectrons")),
  mitName_                  (cfg.getUntrackedParameter<std::string>("mitName", mithep::Names::gkPFCandidatesBrn)),
  trackerTrackMapNames_     (cfg.getUntrackedParameter<std::vector<std::string> >("trackerTrackMapNames", std::vector<std::string>())),
  gsfTrackMapName_          (cfg.getUntrackedParameter<std::string>("gsfTrackMapName", "")),
  muonMapName_              (cfg.getUntrackedParameter<std::string>("muonMapName", "")),
  electronMapName_          (cfg.getUntrackedParameter<std::string>("electronMapName", "")),
  photonMapName_            (cfg.getUntrackedParameter<std::string>("photonMapName", "")),
  barrelSuperClusterMapName_(cfg.getUntrackedParameter<std::string>("barrelSuperClusterMapName", "")),
  endcapSuperClusterMapName_(cfg.getUntrackedParameter<std::string>("endcapSuperClusterMapName", "")),
  conversionMapName_        (cfg.getUntrackedParameter<std::string>("conversionMapName", "")),
  pfCandMapName_            (cfg.getUntrackedParameter<std::string>("pfCandMapName", "")),
  pfNoPileupCandMapName_    (cfg.getUntrackedParameter<std::string>("pfNoPileupCandMapName", "")),
  allowMissingTrackRef_     (cfg.getUntrackedParameter<bool>("allowMissingTrackRef", false)),
  allowMissingClusterRef_   (cfg.getUntrackedParameter<bool>("allowMissingClusterRef", false)),
  allowMissingPhotonRef_    (cfg.getUntrackedParameter<bool>("allowMissingPhotonRef", false)),
  fillPfNoPileup_           (cfg.getUntrackedParameter<bool>("fillPfNoPileup", true)),
  gsfTrackMap_              (0),
  muonMap_                  (0),
  electronMap_              (0),
  photonMap_                (0),
  barrelSuperClusterMap_    (0),
  endcapSuperClusterMap_    (0),
  conversionMap_            (0),
  pfCandMap_                (new mithep::PFCandidateMap),
  pfNoPileupCandMap_        (0),
  pfCands_                  (new mithep::PFCandidateArr(16))
{
}

mithep::FillerPFCandidates::~FillerPFCandidates() 
{
  delete pfCands_;
  delete pfCandMap_;
  delete pfNoPileupCandMap_;
}

void
mithep::FillerPFCandidates::BookDataBlock(mithep::TreeWriter& tws)
{ 
  // Add particle-flow candidate branch to tree and get pointers to maps.

  tws.AddBranch(mitName_, &pfCands_);
  OS()->add(pfCands_, mitName_);

  if (!pfCandMapName_.empty()) {
    pfCandMap_->SetBrName(mitName_);
    OS()->add(pfCandMap_,pfCandMapName_);
  }
  if (fillPfNoPileup_ && !pfNoPileupCandMapName_.empty()) {
    pfNoPileupCandMap_ = new mithep::PFCandidateMap;
    pfNoPileupCandMap_->SetBrName(mitName_);
    OS()->add(pfNoPileupCandMap_,pfNoPileupCandMapName_);
  }
}

void
mithep::FillerPFCandidates::PrepareLinks()
{
  for (auto&& bmapName : trackerTrackMapNames_) {
    if (!bmapName.empty()) {
      auto* map = OS()->get<TrackMap>(bmapName);
      if (map) {
        trackerTrackMaps_.push_back(map);
        AddBranchDep(mitName_,map->GetBrName());
      }
    }
  }
  if (!gsfTrackMapName_.empty()) {
    gsfTrackMap_ = OS()->get<TrackMap>(gsfTrackMapName_);
    if (gsfTrackMap_)
      AddBranchDep(mitName_,gsfTrackMap_->GetBrName());
  }
  if (!muonMapName_.empty()) {
    muonMap_ = OS()->get<MuonMap>(muonMapName_);
    if (muonMap_)
      AddBranchDep(mitName_,muonMap_->GetBrName());
  }
  if (!electronMapName_.empty()) {
    electronMap_ = OS()->get<ElectronMap>(electronMapName_);
    if (electronMap_)
      AddBranchDep(mitName_,electronMap_->GetBrName());
  }
  if (!photonMapName_.empty()) {
    photonMap_ = OS()->get<PhotonMap>(photonMapName_);
    if (photonMap_)
      AddBranchDep(mitName_,photonMap_->GetBrName());
  }
  if (!barrelSuperClusterMapName_.empty()) {
    barrelSuperClusterMap_ = OS()->get<SuperClusterMap>(barrelSuperClusterMapName_);
    if (barrelSuperClusterMap_)
      AddBranchDep(mitName_,barrelSuperClusterMap_->GetBrName());
  }
  if (!endcapSuperClusterMapName_.empty()) {
    endcapSuperClusterMap_ = OS()->get<SuperClusterMap>(endcapSuperClusterMapName_);
    if (endcapSuperClusterMap_)
      AddBranchDep(mitName_,endcapSuperClusterMap_->GetBrName());
  }
  if (!conversionMapName_.empty()) {
    conversionMap_ = OS()->get<ConversionMap>(conversionMapName_);
    if (conversionMap_)
      AddBranchDep(mitName_,conversionMap_->GetBrName());
  }
}

void
mithep::FillerPFCandidates::FillDataBlock(edm::Event const& event, edm::EventSetup const&)
{
  // Fill particle flow candidate information. 

  pfCands_->Delete();
  pfCandMap_->Reset();
  pfNoPileupCandMap_->Reset();

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

  for (auto&& inPfPtr : inPfCands) {
    reco::PFCandidate const& inPf = *inPfPtr;

    mithep::PFCandidate *outPfCand = pfCands_->Allocate();
    new (outPfCand) mithep::PFCandidate(inPf.px(), inPf.py(), inPf.pz(), inPf.energy());

    // fill standard variables
    outPfCand->SetCharge(inPf.charge());
    outPfCand->SetEECal(inPf.ecalEnergy());
    outPfCand->SetEHCal(inPf.hcalEnergy());
    outPfCand->SetEECalRaw(inPf.rawEcalEnergy());
    outPfCand->SetEHCalRaw(inPf.rawHcalEnergy());
    outPfCand->SetEPS1(inPf.pS1Energy());
    outPfCand->SetEPS2(inPf.pS2Energy());
    outPfCand->SetPError(inPf.deltaP());
    outPfCand->SetMvaEPi(inPf.mva_e_pi());
    outPfCand->SetMvaEMu(inPf.mva_e_mu());
    outPfCand->SetMvaPiMu(inPf.mva_pi_mu());
    outPfCand->SetMvaGamma(inPf.mva_nothing_gamma());
    outPfCand->SetMvaNeutralH(inPf.mva_nothing_nh());
    outPfCand->SetMvaGammaNeutralH(inPf.mva_gamma_nh());
    outPfCand->SetEtaECal(inPf.positionAtECALEntrance().eta());
    outPfCand->SetPhiECal(inPf.positionAtECALEntrance().phi());
    
    // fill source vertex
    outPfCand->SetVertex(inPf.vertex().x(), inPf.vertex().y(), inPf.vertex().z());
    
    // fill pf type enum
    if (inPf.particleId()==reco::PFCandidate::X)
      outPfCand->SetPFType(mithep::PFCandidate::eX);
    else if (inPf.particleId()==reco::PFCandidate::h)
      outPfCand->SetPFType(mithep::PFCandidate::eHadron);
    else if (inPf.particleId()==reco::PFCandidate::e)
      outPfCand->SetPFType(mithep::PFCandidate::eElectron);
    else if (inPf.particleId()==reco::PFCandidate::mu)
      outPfCand->SetPFType(mithep::PFCandidate::eMuon);
    else if (inPf.particleId()==reco::PFCandidate::gamma)
      outPfCand->SetPFType(mithep::PFCandidate::eGamma);
    else if (inPf.particleId()==reco::PFCandidate::h0)
      outPfCand->SetPFType(mithep::PFCandidate::eNeutralHadron);
    else if (inPf.particleId()==reco::PFCandidate::h_HF)
      outPfCand->SetPFType(mithep::PFCandidate::eHadronHF);
    else if (inPf.particleId()==reco::PFCandidate::egamma_HF)
      outPfCand->SetPFType(mithep::PFCandidate::eEGammaHF);
    
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

    // add to exported pf candidate map
    pfCandMap_->Add(reco::CandidatePtr(inPfPtr.ptr()), outPfCand);

    // add pf No Pileup map
    if (fillPfNoPileup_) { 
      // initially set the candidate to be not part of the NoPilup collection
      outPfCand->SetFlag(mithep::PFCandidate::ePFNoPileup, false);

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

        pfNoPileupCandMap_->Add(reco::CandidatePtr(inPfNpPtr.ptr()), outPfCand);
      }
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
      outPfCand->SetTrackerTrk(thetrack);
    }    

    // linking with the GfsTracks
    if (gsfTrackMap_ && inPf.gsfTrackRef().isNonnull()) 
      outPfCand->SetGsfTrk(gsfTrackMap_->GetMit(refToPtr(inPf.gsfTrackRef())));
    
    // linking with the Muons
    if (muonMap_ && inPf.muonRef().isNonnull()) 
      outPfCand->SetMuon(muonMap_->GetMit(refToPtr(inPf.muonRef())));
    
    // linking with the Electrons
    if (electronMap_ && inPf.gsfElectronRef().isNonnull()) 
      outPfCand->SetElectron(electronMap_->GetMit(refToPtr(inPf.gsfElectronRef())));
    
    // linking with the Photons
    if (photonMap_ && inPf.photonRef().isNonnull())
      outPfCand->SetPhoton(photonMap_->GetMit(refToPtr(inPf.photonRef()), !allowMissingPhotonRef_));
    
    // linking with the SuperClusters
    if (barrelSuperClusterMap_ && endcapSuperClusterMap_ &&
    	inPf.superClusterRef().isNonnull()) {
      auto* sc = barrelSuperClusterMap_->GetMit(inPf.superClusterRef(), false);
      if (!sc)
        sc = endcapSuperClusterMap_->GetMit(inPf.superClusterRef(), false);

      if (sc)
        outPfCand->SetSCluster(sc);
      else if(!allowMissingClusterRef_)
        throw edm::Exception(edm::errors::Configuration, "FillerPFCandidates::FillDataBlock()\n")
          << "Error! Refined SuperCluster reference in unmapped collection";
    }
    
    if (conversionMap_ && inPf.conversionRef().isNonnull()) 
      outPfCand->SetConversion(conversionMap_->GetMit(inPf.conversionRef()));
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
