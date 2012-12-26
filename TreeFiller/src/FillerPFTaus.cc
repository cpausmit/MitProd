// $Id: FillerPFTaus.cc,v 1.16 2012/04/11 16:36:43 mhchan Exp $

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
  hpsActive_(Conf().getUntrackedParameter<bool>("hpsActive", false)),
  edmName_(Conf().getUntrackedParameter<string>("edmName","")),
  mitName_(Conf().getUntrackedParameter<string>("mitName",Names::gkPFTauBrn)),
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

  // Retrieve EDM names of HPS Tau Discriminators
  vector<pair<string, string> > hpsNames;
  hpsNames.push_back(make_pair("discriminationByLooseElectronRejectionName", "hpsPFTauDiscriminationByLooseElectronRejection"));
  hpsNames.push_back(make_pair("discriminationByMediumElectronRejectionName", "hpsPFTauDiscriminationByMediumElectronRejection"));
  hpsNames.push_back(make_pair("discriminationByTightElectronRejectionName", "hpsPFTauDiscriminationByTightElectronRejection"));
  hpsNames.push_back(make_pair("discriminationByMVAElectronRejectionName", "hpsPFTauDiscriminationByMVAElectronRejection"));
  hpsNames.push_back(make_pair("discriminationByLooseMuonRejectionName", "hpsPFTauDiscriminationByLooseMuonRejection"));
  hpsNames.push_back(make_pair("discriminationByMediumMuonRejectionName", "hpsPFTauDiscriminationByMediumMuonRejection"));
  hpsNames.push_back(make_pair("discriminationByTightMuonRejectionName", "hpsPFTauDiscriminationByTightMuonRejection"));
  hpsNames.push_back(make_pair("discriminationByDecayModeFindingName", "hpsPFTauDiscriminationByDecayModeFinding"));
  hpsNames.push_back(make_pair("discriminationByVLooseIsolationName", "hpsPFTauDiscriminationByVLooseIsolation"));
  hpsNames.push_back(make_pair("discriminationByLooseIsolationName", "hpsPFTauDiscriminationByLooseIsolation"));
  hpsNames.push_back(make_pair("discriminationByMediumIsolationName", "hpsPFTauDiscriminationByMediumIsolation"));
  hpsNames.push_back(make_pair("discriminationByTightIsolationName", "hpsPFTauDiscriminationByTightIsolation"));
  hpsNames.push_back(make_pair("discriminationByVLooseCombinedIsolationDBSumPtCorrName", "hpsPFTauDiscriminationByVLooseCombinedIsolationDBSumPtCorr"));
  hpsNames.push_back(make_pair("discriminationByLooseCombinedIsolationDBSumPtCorrName", "hpsPFTauDiscriminationByLooseCombinedIsolationDBSumPtCorr"));
  hpsNames.push_back(make_pair("discriminationByMediumCombinedIsolationDBSumPtCorrName", "hpsPFTauDiscriminationByMediumCombinedIsolationDBSumPtCorr"));
  hpsNames.push_back(make_pair("discriminationByTightCombinedIsolationDBSumPtCorrName", "hpsPFTauDiscriminationByTightCombinedIsolationDBSumPtCorr"));
  hpsNames.push_back(make_pair("discriminationByRawCombinedIsolationDBSumPtCorrName", "hpsPFTauDiscriminationByRawCombinedIsolationDBSumPtCorr"));
  hpsNames.push_back(make_pair("mva2rawElectronRejectionName", "hpsPFTauDiscriminationByMVA2rawElectronRejection"));
  hpsNames.push_back(make_pair("mva2rawElectronRejectionCategoryName", "hpsPFTauDiscriminationByMVA2rawElectronRejection:category"));
  hpsNames.push_back(make_pair("mva2LooseElectronRejectionName", "hpsPFTauDiscriminationByMVA2LooseElectronRejection"));
  hpsNames.push_back(make_pair("mva2MediumElectronRejectionName", "hpsPFTauDiscriminationByMVA2MediumElectronRejection"));
  hpsNames.push_back(make_pair("mva2TightElectronRejectionName", "hpsPFTauDiscriminationByMVA2TightElectronRejection"));
  hpsNames.push_back(make_pair("mva3rawElectronRejectionName", "hpsPFTauDiscriminationByMVA3rawElectronRejection"));
  hpsNames.push_back(make_pair("mva3rawElectronRejectionCategoryName", "hpsPFTauDiscriminationByMVA3rawElectronRejection:category"));
  hpsNames.push_back(make_pair("mva3LooseElectronRejectionName", "hpsPFTauDiscriminationByMVA3LooseElectronRejection"));
  hpsNames.push_back(make_pair("mva3MediumElectronRejectionName", "hpsPFTauDiscriminationByMVA3MediumElectronRejection"));
  hpsNames.push_back(make_pair("mva3TightElectronRejectionName", "hpsPFTauDiscriminationByMVA3TightElectronRejection"));
  hpsNames.push_back(make_pair("mva3VTightElectronRejectionName", "hpsPFTauDiscriminationByMVA3VTightElectronRejection"));
  hpsNames.push_back(make_pair("looseCombinedIsolationDBSumPtCorr3HitsName", "hpsPFTauDiscriminationByLooseCombinedIsolationDBSumPtCorr3Hits"));
  hpsNames.push_back(make_pair("mediumCombinedIsolationDBSumPtCorr3HitsName", "hpsPFTauDiscriminationByMediumCombinedIsolationDBSumPtCorr3Hits"));
  hpsNames.push_back(make_pair("tightCombinedIsolationDBSumPtCorr3HitsName", "hpsPFTauDiscriminationByTightCombinedIsolationDBSumPtCorr3Hits"));
  hpsNames.push_back(make_pair("rawCombinedIsolationDBSumPtCorr3HitsName", "hpsPFTauDiscriminationByRawCombinedIsolationDBSumPtCorr3Hits"));
  hpsNames.push_back(make_pair("looseMuonRejection2Name", "hpsPFTauDiscriminationByLooseMuonRejection2"));
  hpsNames.push_back(make_pair("mediumMuonRejection2Name", "hpsPFTauDiscriminationByMediumMuonRejection2"));
  hpsNames.push_back(make_pair("tightMuonRejection2Name", "hpsPFTauDiscriminationByTightMuonRejection2"));

  for(vector<pair<string, string> >::iterator it = hpsNames.begin(); it != hpsNames.end(); it++)
  {
    string paramName = it->first;
    string defaultEDMName = it->second;
    hpsHandles_[paramName].name = Conf().getUntrackedParameter<string>(paramName, defaultEDMName);
  }
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

  // Handles for HPS discriminator
  if(hpsActive_)
  {
    for(map<string, PFTauDiscHandle>::iterator it = hpsHandles_.begin(); it != hpsHandles_.end(); it++)
    {
      PFTauDiscHandle &hpsDisc = it->second;
      GetProductSafe(hpsDisc.name, hpsDisc.handle, event);
    }
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
      tau->SetDiscriminationByLooseElectronRejection(hpsHandles_["discriminationByLooseElectronRejectionName"].value(tauRef));
      tau->SetDiscriminationByMediumElectronRejection(hpsHandles_["discriminationByMediumElectronRejectionName"].value(tauRef));
      tau->SetDiscriminationByTightElectronRejection(hpsHandles_["discriminationByTightElectronRejectionName"].value(tauRef));
      tau->SetDiscriminationByMVAElectronRejection(hpsHandles_["discriminationByMVAElectronRejectionName"].value(tauRef));
      tau->SetDiscriminationByLooseMuonRejection(hpsHandles_["discriminationByLooseMuonRejectionName"].value(tauRef));
      tau->SetDiscriminationByMediumMuonRejection(hpsHandles_["discriminationByMediumMuonRejectionName"].value(tauRef));
      tau->SetDiscriminationByTightMuonRejection(hpsHandles_["discriminationByTightMuonRejectionName"].value(tauRef));
      tau->SetDiscriminationByDecayModeFinding(hpsHandles_["discriminationByDecayModeFindingName"].value(tauRef));
      tau->SetDiscriminationByVLooseIsolation(hpsHandles_["discriminationByVLooseIsolationName"].value(tauRef));
      tau->SetDiscriminationByLooseIsolation(hpsHandles_["discriminationByLooseIsolationName"].value(tauRef));
      tau->SetDiscriminationByMediumIsolation(hpsHandles_["discriminationByMediumIsolationName"].value(tauRef));
      tau->SetDiscriminationByTightIsolation(hpsHandles_["discriminationByTightIsolationName"].value(tauRef));
      tau->SetDiscriminationByVLooseCombinedIsolationDBSumPtCorr(hpsHandles_["discriminationByVLooseCombinedIsolationDBSumPtCorrName"].value(tauRef));
      tau->SetDiscriminationByLooseCombinedIsolationDBSumPtCorr(hpsHandles_["discriminationByLooseCombinedIsolationDBSumPtCorrName"].value(tauRef));
      tau->SetDiscriminationByMediumCombinedIsolationDBSumPtCorr(hpsHandles_["discriminationByMediumCombinedIsolationDBSumPtCorrName"].value(tauRef));
      tau->SetDiscriminationByTightCombinedIsolationDBSumPtCorr(hpsHandles_["discriminationByTightCombinedIsolationDBSumPtCorrName"].value(tauRef));
      tau->SetDiscriminationByRawCombinedIsolationDBSumPtCorr(hpsHandles_["discriminationByRawCombinedIsolationDBSumPtCorrName"].value(tauRef));
      tau->SetMVA2rawElectronRejection(hpsHandles_["mva2rawElectronRejectionName"].value(tauRef));
      tau->SetMVA2rawElectronRejectionCategory(hpsHandles_["mva2rawElectronRejectionCategoryName"].value(tauRef));
      tau->SetMVA2LooseElectronRejection(hpsHandles_["mva2LooseElectronRejectionName"].value(tauRef));
      tau->SetMVA2MediumElectronRejection(hpsHandles_["mva2MediumElectronRejectionName"].value(tauRef));
      tau->SetMVA2TightElectronRejection(hpsHandles_["mva2TightElectronRejectionName"].value(tauRef));
      tau->SetMVA3rawElectronRejection(hpsHandles_["mva3rawElectronRejectionName"].value(tauRef));
      tau->SetMVA3rawElectronRejectionCategory(hpsHandles_["mva3rawElectronRejectionCategoryName"].value(tauRef));
      tau->SetMVA3LooseElectronRejection(hpsHandles_["mva3LooseElectronRejectionName"].value(tauRef));
      tau->SetMVA3MediumElectronRejection(hpsHandles_["mva3MediumElectronRejectionName"].value(tauRef));
      tau->SetMVA3TightElectronRejection(hpsHandles_["mva3TightElectronRejectionName"].value(tauRef));
      tau->SetMVA3VTightElectronRejection(hpsHandles_["mva3VTightElectronRejectionName"].value(tauRef));
      tau->SetLooseCombinedIsolationDBSumPtCorr3Hits(hpsHandles_["looseCombinedIsolationDBSumPtCorr3HitsName"].value(tauRef));
      tau->SetMediumCombinedIsolationDBSumPtCorr3Hits(hpsHandles_["mediumCombinedIsolationDBSumPtCorr3HitsName"].value(tauRef));
      tau->SetTightCombinedIsolationDBSumPtCorr3Hits(hpsHandles_["tightCombinedIsolationDBSumPtCorr3HitsName"].value(tauRef));
      tau->SetRawCombinedIsolationDBSumPtCorr3Hits(hpsHandles_["rawCombinedIsolationDBSumPtCorr3HitsName"].value(tauRef));
      tau->SetLooseMuonRejection2(hpsHandles_["looseMuonRejection2Name"].value(tauRef));
      tau->SetMediumMuonRejection2(hpsHandles_["mediumMuonRejection2Name"].value(tauRef));
      tau->SetTightMuonRejection2(hpsHandles_["tightMuonRejection2Name"].value(tauRef));
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
