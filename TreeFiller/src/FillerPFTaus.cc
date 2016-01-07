#define MITPROD_TREEFILLER_FILLERPFTAUS
#include "MitProd/TreeFiller/interface/FillerPFTaus.h"
#include "MitProd/ObjectService/interface/ObjectService.h"

#include "DataFormats/Common/interface/RefToPtr.h"

#include "MitAna/DataTree/interface/Names.h"

template<class TAU>
mithep::FillerPFTaus<TAU>::FillerPFTaus(edm::ParameterSet const& cfg, edm::ConsumesCollector& collector, mithep::ObjectService* os, char const* name, bool active/* = true*/) :
  BaseFiller(cfg, os, name, active),
  hpsActive_(cfg.getUntrackedParameter("hpsActive", false)),
  edmToken_(GetToken<TauCollection>(collector, cfg, "edmName")),
  hpsTokens_{},
  hpsNames_{},
  mitName_(cfg.getUntrackedParameter("mitName", std::string(Names::gkPFTauBrn))),
  trackMapNames_(cfg.getUntrackedParameter("trackMapNames", std::vector<std::string>())),
  jetMapName_(cfg.getUntrackedParameter("jetMapName", std::string("JetMapName"))),
  pfCandMapName_(cfg.getUntrackedParameter("pfCandMapName", std::string())),
  tauMapName_(cfg.getUntrackedParameter("tauMapName", std::string())),
  allowMissingTrackRef_(cfg.getUntrackedParameter("allowMissingTrackRef", false)),
  jetMap_(0),
  pfCandMap_(0),
  tauMap_(new mithep::PFTauMap),
  taus_(new mithep::PFTauArr(16))
{
  for (unsigned iD = 0; iD != mithep::PFTau::nDiscriminators; ++iD) {
    hpsNames_[iD] = cfg.getUntrackedParameter(std::string(mithep::PFTau::PFTauDiscriminatorName(iD)) + "Name", std::string(""));
    if (!hpsNames_[iD].empty())
      hpsTokens_[iD] = collector.consumes<reco::PFTauDiscriminator>(hpsNames_[iD]);
  }
}

template<class TAU>
mithep::FillerPFTaus<TAU>::~FillerPFTaus()
{
  delete taus_;
  delete tauMap_;
}

template<class TAU>
void
mithep::FillerPFTaus<TAU>::BookDataBlock(TreeWriter &tws)
{
  tws.AddBranch(mitName_, &taus_);
  OS()->add(taus_, mitName_);

  if (!tauMapName_.empty()) {
    tauMap_->SetBrName(mitName_);
    OS()->add(tauMap_,tauMapName_);
  }
}

template<class TAU>
void
mithep::FillerPFTaus<TAU>::PrepareLinks()
{
  // template function does not like the form OS()->get<X>
  ObjectService& os(*OS());

  for (auto&& bmapName : trackMapNames_) {
    if (!bmapName.empty()) {
      auto* map = os.get<mithep::TrackMap>(bmapName);
      if (map) {
        trackMaps_.push_back(map);
        AddBranchDep(mitName_,map->GetBrName());
      }
    }
  }

  if (!jetMapName_.empty()) {
    jetMap_ = os.get<mithep::PFJetMap>(jetMapName_);
    AddBranchDep(mitName_, jetMap_->GetBrName());
  }

  if (!pfCandMapName_.empty()) {
    pfCandMap_ = os.get<mithep::PFCandidateMap>(pfCandMapName_);
    AddBranchDep(mitName_, pfCandMap_->GetBrName());
  }
}

template<class TAU>
void
mithep::FillerPFTaus<TAU>::FillDataBlock(edm::Event const& event, 
                                         edm::EventSetup const&)
{
  // Fill taus from edm collection into our collection.

  taus_->Delete();
  tauMap_->Reset();
  
  // handle for the tau collection
  edm::Handle<TauCollection> hTauProduct;
  GetProduct(edmToken_, hTauProduct, event);
  // grab the input tau collection from CMSSW
  TauCollection const& inTaus = *hTauProduct;

  // Handles for HPS discriminator
  reco::PFTauDiscriminator const* discriminators[mithep::PFTau::nDiscriminators];
  if (hpsActive_)
    retrieveDiscriminators(discriminators, event);
  
  // loop through all CMSSW taus (input taus = inTaus)
  unsigned iTau = 0;
  for (auto&& inTau : inTaus) {
    // grab a reference to our input tau
    edm::Ref<TauCollection> tRef(hTauProduct, iTau);
    edm::Ptr<reco::BaseTau> tPtr(hTauProduct, iTau);
    ++iTau;
    
    // make our output object including allocating space
    mithep::PFTau *outTau = taus_->AddNew();

    outTau->SetPtEtaPhiM(inTau.pt(), inTau.eta(), inTau.phi(), inTau.mass());

    outTau->SetMomAlt(inTau.alternatLorentzVect().x(),
                      inTau.alternatLorentzVect().y(),
                      inTau.alternatLorentzVect().z(),
                      inTau.alternatLorentzVect().e());

    outTau->SetCharge(inTau.charge());

    // fill pftau-specific quantities
    setPFSpecific(*outTau, inTau);
    
    // fill HPS discriminants
    if (hpsActive_)
      setPFTauDiscriminators(*outTau, discriminators, tRef);

    // add source pfjet reference
    if (jetMap_)
      setPFJetRef(*outTau, inTau);

    // add pf candidate references
    if (pfCandMap_)
      setPFCandRefs(*outTau, inTau);

    // add outTau equivalent to the map
    tauMap_->Add(tPtr, outTau);
  }
  // make sure to trim off the unused memory
  taus_->Trim();
}

template<class TAU>
void
mithep::FillerPFTaus<TAU>::retrieveDiscriminators(reco::PFTauDiscriminator const* [mithep::PFTau::nDiscriminators], edm::Event const&) const
{
}

template<class TAU>
void
mithep::FillerPFTaus<TAU>::setPFSpecific(mithep::PFTau&, TAU const&) const
{
}

template<class TAU>
void
mithep::FillerPFTaus<TAU>::setPFTauDiscriminators(mithep::PFTau&, reco::PFTauDiscriminator const* [mithep::PFTau::nDiscriminators], edm::Ref<TauCollection> const&) const
{
}

template<class TAU>
void
mithep::FillerPFTaus<TAU>::setPFJetRef(mithep::PFTau&, TAU const&) const
{
}

template<class TAU>
void
mithep::FillerPFTaus<TAU>::setPFCandRefs(mithep::PFTau&, TAU const&) const
{
}

#include "DataFormats/TauReco/interface/PFTau.h"
#include "DataFormats/PatCandidates/interface/Tau.h"

namespace mithep {

  template<>
  void
  FillerPFTaus<reco::PFTau>::retrieveDiscriminators(reco::PFTauDiscriminator const* discriminators[mithep::PFTau::nDiscriminators], edm::Event const& event) const
  {
    for (unsigned iD = 0; iD != mithep::PFTau::nDiscriminators; ++iD) {
      if (!hpsTokens_[iD].isUninitialized()) {
        edm::Handle<reco::PFTauDiscriminator> handle;
        GetProduct(hpsTokens_[iD], handle, event);
        discriminators[iD] = handle.product();
      }
      else
        discriminators[iD] = 0;
    }
  }

  template<>
  void
  FillerPFTaus<reco::PFTau>::setPFSpecific(mithep::PFTau& outTau, reco::PFTau const& inTau) const
  {
    outTau.SetBremRecoveryEOverP(inTau.bremsRecoveryEOverPLead());
    outTau.SetCaloCompatibility(inTau.caloComp());
    outTau.SetECalStripSumEOverP(inTau.ecalStripSumEOverPLead());
    outTau.SetEMFraction(inTau.emFraction());
    outTau.SetElectronPreIDDecision(inTau.electronPreIDDecision());
    outTau.SetElectronPreIDOutput(inTau.electronPreIDOutput());
    outTau.SetHCal3x3EOverP(inTau.hcal3x3OverPLead());
    outTau.SetHCalMaxEOverP(inTau.hcalMaxOverPLead());
    outTau.SetHCalTotalEOverP(inTau.hcalTotOverPLead());
    outTau.SetMuonDecision(inTau.muonDecision());
    outTau.SetSegmentCompatibility(inTau.segComp());
    outTau.SetSignalConeSize(inTau.signalConeSize());
    outTau.SetBendCorrMass(inTau.bendCorrMass());

    outTau.SetIsoChargedHadronPtSum(inTau.isolationPFChargedHadrCandsPtSum());
    outTau.SetIsoGammaEtSum(inTau.isolationPFGammaCandsEtSum());
    outTau.SetLeadPFCandSignD0Sig(inTau.leadPFChargedHadrCandsignedSipt());
    outTau.SetMaxHCalPFClusterEt(inTau.maximumHCALPFClusterEt());

    outTau.SetDecayMode(mithep::PFTau::HadronicDecayMode(inTau.decayMode()));

    if (inTau.electronPreIDTrack().isNonnull()) {
      auto ptr = edm::refToPtr(inTau.electronPreIDTrack());

      mithep::Track *track = 0;
      for (auto&& trkMap : trackMaps_) {
        track = trkMap->GetMit(ptr, false); // no-throw version
        if (track)
          break;
      }

      if (!track && !allowMissingTrackRef_)
        throw edm::Exception(edm::errors::Configuration, "FillerPFTaus::FillDataBlock()\n")
          << "Error! electronPreIDTrack not found in AssociationMaps";

      outTau.SetElectronTrack(track);
    }
  }

  template<>
  void
  FillerPFTaus<pat::Tau>::setPFSpecific(mithep::PFTau& outTau, pat::Tau const& inTau) const
  {
    // Reference:
    // RecoTauTag/RecoTau/src/HPSPFRecoTauAlgorithm.cc

    double isolationPFChargedHadrCandsPtSum = 0.;
    for (auto&& candPtr : inTau.isolationChargedHadrCands())
      isolationPFChargedHadrCandsPtSum += candPtr->pt();
    outTau.SetIsoChargedHadronPtSum(isolationPFChargedHadrCandsPtSum);

    double isolationPFGammaCandsEtSum = 0.;
    for (auto&& candPtr : inTau.isolationGammaCands())
      isolationPFGammaCandsEtSum += candPtr->pt();
    outTau.SetIsoGammaEtSum(isolationPFGammaCandsEtSum);

    outTau.SetDecayMode(mithep::PFTau::HadronicDecayMode(inTau.decayMode()));

    // leadPFChargedHadrCandsignedSipt not available (no jet reference)
    // maximumHCALPFClusterEt not available (no hcal energy in packed candidate)
  }

  template<>
  void
  FillerPFTaus<reco::PFTau>::setPFTauDiscriminators(mithep::PFTau& outTau, reco::PFTauDiscriminator const* discriminators[mithep::PFTau::nDiscriminators], edm::Ref<TauCollection> const& tRef) const
  {
    for (unsigned iD = 0; iD != mithep::PFTau::nDiscriminators; ++iD) {
      if (discriminators[iD])
        outTau.SetPFTauDiscriminator((*discriminators[iD])[tRef], iD);
    }
  }

  template<>
  void
  FillerPFTaus<pat::Tau>::setPFTauDiscriminators(mithep::PFTau& outTau, reco::PFTauDiscriminator const* [mithep::PFTau::nDiscriminators], edm::Ref<TauCollection> const& tRef) const
  {
    pat::Tau const& inTau = *tRef;
    for (unsigned iD = 0; iD != mithep::PFTau::nDiscriminators; ++iD) {
      if (!hpsNames_[iD].empty())
        outTau.SetPFTauDiscriminator(inTau.tauID(hpsNames_[iD]), iD);
    }
  }

  template<>
  void
  FillerPFTaus<reco::PFTau>::setPFJetRef(mithep::PFTau& outTau, reco::PFTau const& inTau) const
  {
    try {
      outTau.SetPFJet(jetMap_->GetMit(edm::refToPtr(inTau.jetRef())));
    }
    catch(...) { 
      throw edm::Exception(edm::errors::Configuration, "FillerPFTaus:FillDataBlock()\n")
        << "Error! Jet unmapped collection";
    }
  }

  template<>
  void
  FillerPFTaus<reco::PFTau>::setPFCandRefs(mithep::PFTau& outTau, reco::PFTau const& inTau) const
  {
    if (inTau.leadPFCand().isNonnull())
      outTau.SetLeadPFCand(pfCandMap_->GetMit(reco::CandidatePtr(inTau.leadPFCand())));
        
    if (inTau.leadPFChargedHadrCand().isNonnull())
      outTau.SetLeadChargedHadronPFCand(pfCandMap_->GetMit(reco::CandidatePtr(inTau.leadPFChargedHadrCand())));
        
    if (inTau.leadPFNeutralCand().isNonnull())
      outTau.SetLeadNeutralPFCand(pfCandMap_->GetMit(reco::CandidatePtr(inTau.leadPFNeutralCand())));
        
    for (auto&& pfCandPtr : inTau.signalPFCands())
      outTau.AddSignalPFCand(pfCandMap_->GetMit(reco::CandidatePtr(pfCandPtr)));

    for (auto&& pfCandPtr : inTau.signalPFChargedHadrCands())
      outTau.AddSignalPFChargedHadrCand(pfCandMap_->GetMit(reco::CandidatePtr(pfCandPtr)));

    for (auto&& pfCandPtr : inTau.signalPFNeutrHadrCands())
      outTau.AddSignalPFNeutrHadrCand(pfCandMap_->GetMit(reco::CandidatePtr(pfCandPtr)));

    for (auto&& pfCandPtr : inTau.signalPFGammaCands())
      outTau.AddSignalPFGammaCand(pfCandMap_->GetMit(reco::CandidatePtr(pfCandPtr)));
      
    for (auto&& pfCandPtr : inTau.isolationPFCands())
      outTau.AddIsoPFCand(pfCandMap_->GetMit(reco::CandidatePtr(pfCandPtr)));
  }

  template<>
  void
  FillerPFTaus<pat::Tau>::setPFCandRefs(mithep::PFTau& outTau, pat::Tau const& inTau) const
  {
    if (inTau.leadCand().isNonnull())
      outTau.SetLeadPFCand(pfCandMap_->GetMit(inTau.leadCand()));
        
    if (inTau.leadChargedHadrCand().isNonnull())
      outTau.SetLeadChargedHadronPFCand(pfCandMap_->GetMit(inTau.leadChargedHadrCand()));
        
    if (inTau.leadNeutralCand().isNonnull())
      outTau.SetLeadNeutralPFCand(pfCandMap_->GetMit(inTau.leadNeutralCand()));
        
    for (auto&& pfCandPtr : inTau.signalCands())
      outTau.AddSignalPFCand(pfCandMap_->GetMit(pfCandPtr));

    for (auto&& pfCandPtr : inTau.signalChargedHadrCands())
      outTau.AddSignalPFChargedHadrCand(pfCandMap_->GetMit(pfCandPtr));

    for (auto&& pfCandPtr : inTau.signalNeutrHadrCands())
      outTau.AddSignalPFNeutrHadrCand(pfCandMap_->GetMit(pfCandPtr));

    for (auto&& pfCandPtr : inTau.signalGammaCands())
      outTau.AddSignalPFGammaCand(pfCandMap_->GetMit(pfCandPtr));
      
    for (auto&& pfCandPtr : inTau.isolationCands())
      outTau.AddIsoPFCand(pfCandMap_->GetMit(pfCandPtr));
  }

  typedef FillerPFTaus<reco::PFTau> FillerPFTausFromPFTaus;
  typedef FillerPFTaus<pat::Tau> FillerPFTausFromPATTaus;
}

DEFINE_MITHEP_TREEFILLER(FillerPFTausFromPFTaus);
DEFINE_MITHEP_TREEFILLER(FillerPFTausFromPATTaus);
