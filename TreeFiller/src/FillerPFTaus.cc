// $Id: FillerPFTaus.cc,v 1.3 2009/06/15 15:00:26 loizides Exp $

#include "MitProd/TreeFiller/interface/FillerPFTaus.h"
#include "DataFormats/Common/interface/RefToPtr.h"
#include "DataFormats/TauReco/interface/PFTau.h"
#include "DataFormats/TauReco/interface/PFTauFwd.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataTree/interface/PFTauCol.h"
#include "MitProd/ObjectService/interface/ObjectService.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerPFTaus::FillerPFTaus(const ParameterSet &cfg, const char *name, bool active) : 
  BaseFiller(cfg,name,active),
  edmName_(Conf().getUntrackedParameter<string>("edmName","")),
  mitName_(Conf().getUntrackedParameter<string>("mitName",Names::gkPFTauBrn)), 
  trackMapName_(Conf().getUntrackedParameter<string>("trackMapName","TracksMapName")), 
  jetMapName_(Conf().getUntrackedParameter<string>("jetMapName","JetMapName")), 
  pfCandMapName_(Conf().getUntrackedParameter<string>("pfCandMapName","")),
  trackMap_(0),
  jetMap_(0),
  pfCandMap_(0),
  taus_(new mithep::PFTauArr(16))
{
  // Constructor.
}

//--------------------------------------------------------------------------------------------------
FillerPFTaus::~FillerPFTaus()
{
  // Destructor.

  delete taus_;
}

//--------------------------------------------------------------------------------------------------
void FillerPFTaus::BookDataBlock(TreeWriter &tws, const edm::EventSetup &es)
{
  // Add taus branch to tree.

  tws.AddBranch(mitName_,&taus_);
  OS()->add<mithep::PFTauArr>(taus_,mitName_);

  if (!trackMapName_.empty()) {
    trackMap_ = OS()->get<TrackMap>(trackMapName_);
    if (trackMap_)
      AddBranchDep(mitName_, trackMap_->GetBrName());
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
}

//--------------------------------------------------------------------------------------------------
void FillerPFTaus::FillDataBlock(const edm::Event      &event, 
                                 const edm::EventSetup &setup)
{
  // Fill taus from edm collection into our collection.

  taus_->Delete();

  // handle for the tau collection
  Handle<reco::PFTauCollection> hTauProduct;
  GetProduct(edmName_, hTauProduct, event);
  
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
    
    // add track references
    if (trackMap_) {
      // electron preid track reference
      if (inTau->electronPreIDTrack().isNonnull())
        tau->SetElectronTrack(trackMap_->GetMit(refToPtr(inTau->electronPreIDTrack())));
    }

    const reco::PFTauTagInfo *tagInfo = inTau->pfTauTagInfoRef().get();

    // add source calojet reference
    if (jetMap_) {
      tau->SetPFJet(jetMap_->GetMit(refToPtr(tagInfo->pfjetRef())));
    }

    // add pf candidate references
    if (pfCandMap_) {
      if (inTau->leadPFCand().isNonnull())
        tau->SetLeadPFCand(pfCandMap_->GetMit(refToPtr(inTau->leadPFCand())));
        
      if (inTau->leadPFChargedHadrCand().isNonnull()) {
        const mithep::PFCandidate *pfc = 
          pfCandMap_->GetMit(refToPtr(inTau->leadPFChargedHadrCand()));
        tau->SetLeadChargedHadronPFCand(pfc);
      }
        
      if (inTau->leadPFNeutralCand().isNonnull())
        tau->SetLeadNeutralPFCand(pfCandMap_->GetMit(refToPtr(inTau->leadPFNeutralCand())));
        
      for (uint i=0; i<inTau->signalPFCands().size(); ++i) {
        const PFCandidate *signalCand = pfCandMap_->GetMit(refToPtr(inTau->signalPFCands().at(i)));
        tau->AddSignalPFCand(signalCand);
      }
      
      for (uint i=0; i<inTau->isolationPFCands().size(); ++i) {
        const PFCandidate *isoCand = pfCandMap_->GetMit(refToPtr(inTau->isolationPFCands().at(i)));
        tau->AddIsoPFCand(isoCand);
      }
    }
  }      
  taus_->Trim();
}
