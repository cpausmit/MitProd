// $Id: FillerPFTaus.cc,v 1.12 2011/10/10 20:57:40 bendavid Exp $

#include "MitProd/TreeFiller/interface/FillerPFTaus.h"
#include "DataFormats/Common/interface/RefToPtr.h"
#include "DataFormats/TauReco/interface/PFTau.h"
#include "DataFormats/TauReco/interface/PFTauFwd.h"
#include "DataFormats/TauReco/interface/PFTauDiscriminator.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataTree/interface/PFTauCol.h"
#include "MitProd/ObjectService/interface/ObjectService.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerPFTaus::FillerPFTaus(const ParameterSet &cfg, const char *name, bool active) : 
  BaseFiller(cfg,name,active),
  hpsActive_(Conf().getUntrackedParameter<bool>("hpsActive", false)),
  edmName_(Conf().getUntrackedParameter<string>("edmName","")),
  mitName_(Conf().getUntrackedParameter<string>("mitName",Names::gkPFTauBrn)),
  discriminationByLooseElectronRejectionName_(Conf().getUntrackedParameter<string>("discriminationByLooseElectronRejectionName", "hpsPFTauDiscriminationByLooseElecronRejection")),
  discriminationByMediumElectronRejectionName_(Conf().getUntrackedParameter<string>("discriminationByMediumElectronRejectionName", "hpsPFTauDiscriminationByMediumElecronRejection")),
  discriminationByTightElectronRejectionName_(Conf().getUntrackedParameter<string>("discriminationByTightElectronRejectionName", "hpsPFTauDiscriminationByTightElecronRejection")),
  discriminationByLooseMuonRejectionName_(Conf().getUntrackedParameter<string>("discriminationByLooseMuonRejectionName", "hpsPFTauDiscriminationByLooseMuonRejection")),
  discriminationByTightMuonRejectionName_(Conf().getUntrackedParameter<string>("discriminationByTightMuonRejectionName", "hpsPFTauDiscriminationByTightMuonRejection")),
  discriminationByDecayModeFindingName_(Conf().getUntrackedParameter<string>("discriminationByDecayModeFindingName", "hpsPFTauDiscriminationByDecayModeFinding")),
  discriminationByVLooseIsolationName_(Conf().getUntrackedParameter<string>("discriminationByVLooseIsolationName", "hpsPFTauDiscriminationByVLooseIsolation")),
  discriminationByLooseIsolationName_(Conf().getUntrackedParameter<string>("discriminationByLooseIsolationName", "hpsPFTauDiscriminationByLooseIsolation")),
  discriminationByMediumIsolationName_(Conf().getUntrackedParameter<string>("discriminationByMediumIsolationName", "hpsPFTauDiscriminationByMediumIsolation")),
  discriminationByTightIsolationName_(Conf().getUntrackedParameter<string>("discriminationByTightIsolationName", "hpsPFTauDiscriminationByTightIsolation")),
  discriminationByVLooseCombinedIsolationDBSumPtCorrName_(Conf().getUntrackedParameter<string>("discriminationByVLooseCombinedIsolationDBSumPtCorrName", "hpsPFTauDiscriminationByVLooseCombinedIsolationDBSumPtCorr")),
  discriminationByLooseCombinedIsolationDBSumPtCorrName_(Conf().getUntrackedParameter<string>("discriminationByLooseCombinedIsolationDBSumPtCorrName", "hpsPFTauDiscriminationByLooseCombinedIsolationDBSumPtCorr")),
  discriminationByMediumCombinedIsolationDBSumPtCorrName_(Conf().getUntrackedParameter<string>("discriminationByMediumCombinedIsolationDBSumPtCorrName", "hpsPFTauDiscriminationByMediumCombinedIsolationDBSumPtCorr")),
  discriminationByTightCombinedIsolationDBSumPtCorrName_(Conf().getUntrackedParameter<string>("discriminationByTightCombinedIsolationDBSumPtCorrName", "hpsPFTauDiscriminationByTightCombinedIsolationDBSumPtCorr")),
  trackMapNames_(Conf().exists("trackMapNames") ? 
                    Conf().getUntrackedParameter<vector<string> >("trackMapNames") : 
                    vector<string>()),
  jetMapName_(Conf().getUntrackedParameter<string>("jetMapName","JetMapName")), 
  pfCandMapName_(Conf().getUntrackedParameter<string>("pfCandMapName","")),
  tauMapName_(Conf().getUntrackedParameter<string>("tauMapName","")),
  allowMissingTrackRef_(Conf().getUntrackedParameter<bool>("allowMissingTrackRef",false)),
  jetMap_(0),
  pfCandMap_(0),
  tauMap_(new mithep::PFTauMap),
  taus_(new mithep::PFTauArr(16))
{
  // Constructor.
}

//--------------------------------------------------------------------------------------------------
FillerPFTaus::~FillerPFTaus()
{
  // Destructor.
  delete taus_;
  delete tauMap_;
}

//--------------------------------------------------------------------------------------------------
void FillerPFTaus::BookDataBlock(TreeWriter &tws)
{
  // Add taus branch to tree.

  tws.AddBranch(mitName_,&taus_);
  OS()->add<mithep::PFTauArr>(taus_,mitName_);

  for (std::vector<std::string>::const_iterator bmapName = trackMapNames_.begin();
        bmapName!=trackMapNames_.end(); ++bmapName) {
    if (!bmapName->empty()) {
      const TrackMap *map = OS()->get<TrackMap>(*bmapName);
      if (map) {
        trackMaps_.push_back(map);
        AddBranchDep(mitName_,map->GetBrName());
      }
    }
  }

  if (!jetMapName_.empty()) {
    jetMap_ = OS()->get<PFJetMap>(jetMapName_);
    if (jetMap_)
      AddBranchDep(mitName_, jetMap_->GetBrName());
  }

  if (!pfCandMapName_.empty()) {
    pfCandMap_ = OS()->get<PFCandidateMap>(pfCandMapName_);
    if (pfCandMap_)
      AddBranchDep(mitName_, pfCandMap_->GetBrName());
  }
  if (!tauMapName_.empty()) {
    tauMap_->SetBrName(mitName_);
    OS()->add<PFTauMap>(tauMap_,tauMapName_);
  }
}

//--------------------------------------------------------------------------------------------------
void FillerPFTaus::FillDataBlock(const edm::Event      &event, 
                                 const edm::EventSetup &setup)
{
  // Fill taus from edm collection into our collection.

  taus_  ->Delete();
  tauMap_->Reset();

  // handle for the tau collection
  Handle<reco::PFTauCollection> hTauProduct;
  GetProduct(edmName_, hTauProduct, event);
  
  // Handles for HPS discriminants
  Handle<reco::PFTauDiscriminator> hDiscriminationByLooseElectronRejection;
  Handle<reco::PFTauDiscriminator> hDiscriminationByMediumElectronRejection;
  Handle<reco::PFTauDiscriminator> hDiscriminationByTightElectronRejection;
  Handle<reco::PFTauDiscriminator> hDiscriminationByLooseMuonRejection;
  Handle<reco::PFTauDiscriminator> hDiscriminationByTightMuonRejection;
  Handle<reco::PFTauDiscriminator> hDiscriminationByDecayModeFinding;
  Handle<reco::PFTauDiscriminator> hDiscriminationByVLooseIsolation;
  Handle<reco::PFTauDiscriminator> hDiscriminationByLooseIsolation;
  Handle<reco::PFTauDiscriminator> hDiscriminationByMediumIsolation;
  Handle<reco::PFTauDiscriminator> hDiscriminationByTightIsolation;
  Handle<reco::PFTauDiscriminator> hDiscriminationByVLooseCombinedIsolationDBSumPtCorr;
  Handle<reco::PFTauDiscriminator> hDiscriminationByLooseCombinedIsolationDBSumPtCorr;
  Handle<reco::PFTauDiscriminator> hDiscriminationByMediumCombinedIsolationDBSumPtCorr;
  Handle<reco::PFTauDiscriminator> hDiscriminationByTightCombinedIsolationDBSumPtCorr;
  
  if(hpsActive_)
  {
    GetProduct(discriminationByLooseElectronRejectionName_, hDiscriminationByLooseElectronRejection, event);
    GetProduct(discriminationByMediumElectronRejectionName_, hDiscriminationByMediumElectronRejection, event);
    GetProduct(discriminationByTightElectronRejectionName_, hDiscriminationByTightElectronRejection, event);
    GetProduct(discriminationByLooseMuonRejectionName_, hDiscriminationByLooseMuonRejection, event);
    GetProduct(discriminationByTightMuonRejectionName_, hDiscriminationByTightMuonRejection, event);
    GetProduct(discriminationByDecayModeFindingName_, hDiscriminationByDecayModeFinding, event);
    GetProduct(discriminationByVLooseIsolationName_, hDiscriminationByVLooseIsolation, event);
    GetProduct(discriminationByLooseIsolationName_, hDiscriminationByLooseIsolation, event);
    GetProduct(discriminationByMediumIsolationName_, hDiscriminationByMediumIsolation, event);
    GetProduct(discriminationByTightIsolationName_, hDiscriminationByTightIsolation, event);
    GetProduct(discriminationByVLooseCombinedIsolationDBSumPtCorrName_, hDiscriminationByVLooseCombinedIsolationDBSumPtCorr, event);
    GetProduct(discriminationByLooseCombinedIsolationDBSumPtCorrName_, hDiscriminationByLooseCombinedIsolationDBSumPtCorr, event);
    GetProduct(discriminationByMediumCombinedIsolationDBSumPtCorrName_, hDiscriminationByMediumCombinedIsolationDBSumPtCorr, event);
    GetProduct(discriminationByTightCombinedIsolationDBSumPtCorrName_, hDiscriminationByTightCombinedIsolationDBSumPtCorr, event);
  }

  const reco::PFTauCollection inTaus = *(hTauProduct.product());  
  // loop through all taus
  for (reco::PFTauCollection::const_iterator inTau = inTaus.begin(); 
       inTau != inTaus.end(); ++inTau) {
    
    reco::PFTauRef tauRef(hTauProduct, inTau - inTaus.begin());
    
    mithep::PFTau *tau = taus_->Allocate();
    new (tau) mithep::PFTau(inTau->px(),
                          inTau->py(),
                          inTau->pz(),
                          inTau->energy());

    tau->SetMomAlt(inTau->alternatLorentzVect().x(),
                   inTau->alternatLorentzVect().y(),
                   inTau->alternatLorentzVect().z(),
                   inTau->alternatLorentzVect().e());

    // fill pftau-specific quantities
    tau->SetCharge(inTau->charge());
    tau->SetBremRecoveryEOverP(inTau->bremsRecoveryEOverPLead());
    tau->SetCaloCompatibility(inTau->caloComp());
    tau->SetECalStripSumEOverP(inTau->ecalStripSumEOverPLead());
    tau->SetEMFraction(inTau->emFraction());
    tau->SetElectronPreIDDecision(inTau->electronPreIDDecision());
    tau->SetElectronPreIDOutput(inTau->electronPreIDOutput());
    tau->SetHCal3x3EOverP(inTau->hcal3x3OverPLead());
    tau->SetHCalMaxEOverP(inTau->hcalMaxOverPLead());
    tau->SetHCalTotalEOverP(inTau->hcalTotOverPLead());
    tau->SetIsoChargedHadronPtSum(inTau->isolationPFChargedHadrCandsPtSum());
    tau->SetIsoGammaEtSum(inTau->isolationPFGammaCandsEtSum());
    tau->SetLeadPFCandSignD0Sig(inTau->leadPFChargedHadrCandsignedSipt());
    tau->SetMaxHCalPFClusterEt(inTau->maximumHCALPFClusterEt());
    tau->SetMuonDecision(inTau->muonDecision());
    tau->SetSegmentCompatibility(inTau->segComp());
    
    // fill HPS discriminants
    if(hpsActive_)
    {
      tau->SetDiscriminationByLooseElectronRejection((*(hDiscriminationByLooseElectronRejection.product()))[tauRef]);
      tau->SetDiscriminationByMediumElectronRejection((*(hDiscriminationByMediumElectronRejection.product()))[tauRef]);
      tau->SetDiscriminationByTightElectronRejection((*(hDiscriminationByTightElectronRejection.product()))[tauRef]);
      tau->SetDiscriminationByLooseMuonRejection((*(hDiscriminationByLooseMuonRejection.product()))[tauRef]);
      tau->SetDiscriminationByTightMuonRejection((*(hDiscriminationByTightMuonRejection.product()))[tauRef]);
      tau->SetDiscriminationByDecayModeFinding((*(hDiscriminationByDecayModeFinding.product()))[tauRef]);
      tau->SetDiscriminationByVLooseIsolation((*(hDiscriminationByVLooseIsolation.product()))[tauRef]);
      tau->SetDiscriminationByLooseIsolation((*(hDiscriminationByLooseIsolation.product()))[tauRef]);
      tau->SetDiscriminationByMediumIsolation((*(hDiscriminationByMediumIsolation.product()))[tauRef]);
      tau->SetDiscriminationByTightIsolation((*(hDiscriminationByTightIsolation.product()))[tauRef]);
      tau->SetDiscriminationByVLooseCombinedIsolationDBSumPtCorr((*(hDiscriminationByVLooseCombinedIsolationDBSumPtCorr.product()))[tauRef]);
      tau->SetDiscriminationByLooseCombinedIsolationDBSumPtCorr((*(hDiscriminationByLooseCombinedIsolationDBSumPtCorr.product()))[tauRef]);
      tau->SetDiscriminationByMediumCombinedIsolationDBSumPtCorr((*(hDiscriminationByMediumCombinedIsolationDBSumPtCorr.product()))[tauRef]);
      tau->SetDiscriminationByTightCombinedIsolationDBSumPtCorr((*(hDiscriminationByTightCombinedIsolationDBSumPtCorr.product()))[tauRef]);
    }

    // add track references
//     if (trackMap_) {
//       // electron preid track reference
//       if (inTau->electronPreIDTrack().isNonnull()) {
//         if (!allowMissingTrackRef_ || trackMap_->HasMit(refToPtrHack(inTau->electronPreIDTrack()))) {
//           tau->SetElectronTrack(trackMap_->GetMit(refToPtrHack(inTau->electronPreIDTrack())));
//         }
//       }
//     }

     if (inTau->electronPreIDTrack().isNonnull()) {
       const mithep::Track *theTrack = getMitTrack(refToPtrHack(inTau->electronPreIDTrack()),allowMissingTrackRef_);
       tau->SetElectronTrack(theTrack);
     }

     // add source pfjet reference ( only filled since cmssw 311x )
     if (jetMap_) {
       tau->SetPFJet(jetMap_->GetMit(refToPtrHack(inTau->jetRef())));
     }

    // add pf candidate references
    if (pfCandMap_) {
      if (inTau->leadPFCand().isNonnull())
        tau->SetLeadPFCand(pfCandMap_->GetMit(refToPtrHack(inTau->leadPFCand())));
        
      if (inTau->leadPFChargedHadrCand().isNonnull()) {
        const mithep::PFCandidate *pfc = 
          pfCandMap_->GetMit(refToPtrHack(inTau->leadPFChargedHadrCand()));
        tau->SetLeadChargedHadronPFCand(pfc);
      }
        
      if (inTau->leadPFNeutralCand().isNonnull())
        tau->SetLeadNeutralPFCand(pfCandMap_->GetMit(refToPtrHack(inTau->leadPFNeutralCand())));
        
      for (uint i=0; i<inTau->signalPFCands().size(); ++i) {
        const PFCandidate *signalCand = pfCandMap_->GetMit(refToPtrHack(inTau->signalPFCands().at(i)));
        tau->AddSignalPFCand(signalCand);
      }

      for (uint i=0; i<inTau->signalPFChargedHadrCands().size(); ++i) {
	const PFCandidate *signalCand = pfCandMap_->GetMit(refToPtrHack(inTau->signalPFChargedHadrCands().at(i)));
	tau->AddSignalPFChargedHadrCand(signalCand);
      }

      for (uint i=0; i<inTau->signalPFNeutrHadrCands().size(); ++i) {
	const PFCandidate *signalCand = pfCandMap_->GetMit(refToPtrHack(inTau->signalPFNeutrHadrCands().at(i)));
	tau->AddSignalPFNeutrHadrCand(signalCand);
      }

      for (uint i=0; i<inTau->signalPFGammaCands().size(); ++i) {
	const PFCandidate *signalCand = pfCandMap_->GetMit(refToPtrHack(inTau->signalPFGammaCands().at(i)));
	tau->AddSignalPFGammaCand(signalCand);
      }
      
      for (uint i=0; i<inTau->isolationPFCands().size(); ++i) {
        const PFCandidate *isoCand = pfCandMap_->GetMit(refToPtrHack(inTau->isolationPFCands().at(i)));
        tau->AddIsoPFCand(isoCand);
      }
    }
    // add Tau to map
    edm::Ptr<reco::PFTau> thePtr(hTauProduct, inTau - inTaus.begin());
    tauMap_->Add(thePtr, tau);
  }      
  
  taus_->Trim();
}

//--------------------------------------------------------------------------------------------------
const mithep::Track *FillerPFTaus::getMitTrack(mitedm::TrackPtr ptr, bool allowmissing) const
{
  // Return our particle referenced by the edm pointer.

  mithep::Track *mitPart = 0;
  for (std::vector<const mithep::TrackMap*>::const_iterator bmap = trackMaps_.begin();
        bmap!=trackMaps_.end(); ++bmap) {
    const mithep::TrackMap *theMap = *bmap;
    if (theMap->HasMit(ptr)) {
      mitPart = theMap->GetMit(ptr);
      return mitPart;
    }
  }
  
  if (!mitPart && !allowmissing)
    throw edm::Exception(edm::errors::Configuration, "FillerPFCandidates::FillDataBlock()\n")
    << "Error! MITHEP Object " 
    << "not found in AssociationMaps (" << typeid(*this).name() << ")." << std::endl;
    
  return mitPart;
}
