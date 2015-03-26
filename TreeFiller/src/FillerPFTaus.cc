// $Id: FillerPFTaus.cc,v 1.16 2012/04/11 16:36:43 mhchan Exp $

#include "MitProd/TreeFiller/interface/FillerPFTaus.h"
#include "DataFormats/Common/interface/RefToPtr.h"
#include "DataFormats/TauReco/interface/PFTau.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataTree/interface/PFTauCol.h"
#include "MitProd/ObjectService/interface/ObjectService.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerPFTaus::FillerPFTaus(const ParameterSet &cfg, edm::ConsumesCollector& collector, ObjectService* os, const char *name, bool active) : 
  BaseFiller(cfg,os,name,active),
  hpsActive_(Conf().getUntrackedParameter<bool>("hpsActive", false)),
  edmToken_(GetToken<reco::PFTauCollection>(collector, "edmName","")),
  hpsTokens_(),
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
  std::string hpsNames[][2] = {
    {"discriminationByLooseElectronRejectionName", "hpsPFTauDiscriminationByLooseElectronRejection"},
    {"discriminationByMediumElectronRejectionName", "hpsPFTauDiscriminationByMediumElectronRejection"},
    {"discriminationByTightElectronRejectionName", "hpsPFTauDiscriminationByTightElectronRejection"},
    {"discriminationByMVAElectronRejectionName", "hpsPFTauDiscriminationByMVAElectronRejection"},
    {"discriminationByLooseMuonRejectionName", "hpsPFTauDiscriminationByLooseMuonRejection"},
    {"discriminationByMediumMuonRejectionName", "hpsPFTauDiscriminationByMediumMuonRejection"},
    {"discriminationByTightMuonRejectionName", "hpsPFTauDiscriminationByTightMuonRejection"},
    {"discriminationByDecayModeFindingName", "hpsPFTauDiscriminationByDecayModeFinding"},
    {"discriminationByVLooseIsolationName", "hpsPFTauDiscriminationByVLooseIsolation"},
    {"discriminationByLooseIsolationName", "hpsPFTauDiscriminationByLooseIsolation"},
    {"discriminationByMediumIsolationName", "hpsPFTauDiscriminationByMediumIsolation"},
    {"discriminationByTightIsolationName", "hpsPFTauDiscriminationByTightIsolation"},
    {"discriminationByVLooseCombinedIsolationDBSumPtCorrName", "hpsPFTauDiscriminationByVLooseCombinedIsolationDBSumPtCorr"},
    {"discriminationByLooseCombinedIsolationDBSumPtCorrName", "hpsPFTauDiscriminationByLooseCombinedIsolationDBSumPtCorr"},
    {"discriminationByMediumCombinedIsolationDBSumPtCorrName", "hpsPFTauDiscriminationByMediumCombinedIsolationDBSumPtCorr"},
    {"discriminationByTightCombinedIsolationDBSumPtCorrName", "hpsPFTauDiscriminationByTightCombinedIsolationDBSumPtCorr"},
    {"discriminationByRawCombinedIsolationDBSumPtCorrName", "hpsPFTauDiscriminationByRawCombinedIsolationDBSumPtCorr"},
    {"mva2rawElectronRejectionName", "hpsPFTauDiscriminationByMVA2rawElectronRejection"},
    {"mva2rawElectronRejectionCategoryName", "hpsPFTauDiscriminationByMVA2rawElectronRejection:category"},
    {"mva2LooseElectronRejectionName", "hpsPFTauDiscriminationByMVA2LooseElectronRejection"},
    {"mva2MediumElectronRejectionName", "hpsPFTauDiscriminationByMVA2MediumElectronRejection"},
    {"mva2TightElectronRejectionName", "hpsPFTauDiscriminationByMVA2TightElectronRejection"},
    {"mva3rawElectronRejectionName", "hpsPFTauDiscriminationByMVA3rawElectronRejection"},
    {"mva3rawElectronRejectionCategoryName", "hpsPFTauDiscriminationByMVA3rawElectronRejection:category"},
    {"mva3LooseElectronRejectionName", "hpsPFTauDiscriminationByMVA3LooseElectronRejection"},
    {"mva3MediumElectronRejectionName", "hpsPFTauDiscriminationByMVA3MediumElectronRejection"},
    {"mva3TightElectronRejectionName", "hpsPFTauDiscriminationByMVA3TightElectronRejection"},
    {"mva3VTightElectronRejectionName", "hpsPFTauDiscriminationByMVA3VTightElectronRejection"},
    {"looseCombinedIsolationDBSumPtCorr3HitsName", "hpsPFTauDiscriminationByLooseCombinedIsolationDBSumPtCorr3Hits"},
    {"mediumCombinedIsolationDBSumPtCorr3HitsName", "hpsPFTauDiscriminationByMediumCombinedIsolationDBSumPtCorr3Hits"},
    {"tightCombinedIsolationDBSumPtCorr3HitsName", "hpsPFTauDiscriminationByTightCombinedIsolationDBSumPtCorr3Hits"},
    {"rawCombinedIsolationDBSumPtCorr3HitsName", "hpsPFTauDiscriminationByRawCombinedIsolationDBSumPtCorr3Hits"},
    {"looseMuonRejection2Name", "hpsPFTauDiscriminationByLooseMuonRejection2"},
    {"mediumMuonRejection2Name", "hpsPFTauDiscriminationByMediumMuonRejection2"},
    {"tightMuonRejection2Name", "hpsPFTauDiscriminationByTightMuonRejection2"}
  };

  for(unsigned iN = 0; iN != sizeof(hpsNames) / sizeof(std::string) / 2; ++iN)
  {
    string& paramName = hpsNames[iN][0];
    string& defaultEDMName = hpsNames[iN][1];
    hpsTokens_[paramName] = GetToken<reco::PFTauDiscriminator>(collector, paramName, defaultEDMName, true);
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
  GetProduct(edmToken_, hTauProduct, event);

  // Handles for HPS discriminator
  map<string, edm::Handle<reco::PFTauDiscriminator> > hpsHandles;
  if(hpsActive_)
  {
    for(map<string, edm::EDGetTokenT<reco::PFTauDiscriminator> >::iterator it = hpsTokens_.begin(); it != hpsTokens_.end(); it++){
      // handle map entries created on the fly
      GetProductSafe(it->second, hpsHandles[it->first], event);
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
      auto discVal = [&hpsHandles, &tauRef](std::string const& discName)->double {
        std::map<std::string, edm::Handle<reco::PFTauDiscriminator> >::const_iterator itr(hpsHandles.find(discName));
        if(itr == hpsHandles.end()) return 0.;
        edm::Handle<reco::PFTauDiscriminator> const& handle(itr->second);
        if(handle.isValid())
          return (*handle)[tauRef];
        else
          return 0.;
      };
      
      tau->SetDiscriminationByLooseElectronRejection(discVal("discriminationByLooseElectronRejectionName"));
      tau->SetDiscriminationByMediumElectronRejection(discVal("discriminationByMediumElectronRejectionName"));
      tau->SetDiscriminationByTightElectronRejection(discVal("discriminationByTightElectronRejectionName"));
      tau->SetDiscriminationByMVAElectronRejection(discVal("discriminationByMVAElectronRejectionName"));
      tau->SetDiscriminationByLooseMuonRejection(discVal("discriminationByLooseMuonRejectionName"));
      tau->SetDiscriminationByMediumMuonRejection(discVal("discriminationByMediumMuonRejectionName"));
      tau->SetDiscriminationByTightMuonRejection(discVal("discriminationByTightMuonRejectionName"));
      tau->SetDiscriminationByDecayModeFinding(discVal("discriminationByDecayModeFindingName"));
      tau->SetDiscriminationByVLooseIsolation(discVal("discriminationByVLooseIsolationName"));
      tau->SetDiscriminationByLooseIsolation(discVal("discriminationByLooseIsolationName"));
      tau->SetDiscriminationByMediumIsolation(discVal("discriminationByMediumIsolationName"));
      tau->SetDiscriminationByTightIsolation(discVal("discriminationByTightIsolationName"));
      tau->SetDiscriminationByVLooseCombinedIsolationDBSumPtCorr(discVal("discriminationByVLooseCombinedIsolationDBSumPtCorrName"));
      tau->SetDiscriminationByLooseCombinedIsolationDBSumPtCorr(discVal("discriminationByLooseCombinedIsolationDBSumPtCorrName"));
      tau->SetDiscriminationByMediumCombinedIsolationDBSumPtCorr(discVal("discriminationByMediumCombinedIsolationDBSumPtCorrName"));
      tau->SetDiscriminationByTightCombinedIsolationDBSumPtCorr(discVal("discriminationByTightCombinedIsolationDBSumPtCorrName"));
      tau->SetDiscriminationByRawCombinedIsolationDBSumPtCorr(discVal("discriminationByRawCombinedIsolationDBSumPtCorrName"));
      tau->SetMVA2rawElectronRejection(discVal("mva2rawElectronRejectionName"));
      tau->SetMVA2rawElectronRejectionCategory(discVal("mva2rawElectronRejectionCategoryName"));
      tau->SetMVA2LooseElectronRejection(discVal("mva2LooseElectronRejectionName"));
      tau->SetMVA2MediumElectronRejection(discVal("mva2MediumElectronRejectionName"));
      tau->SetMVA2TightElectronRejection(discVal("mva2TightElectronRejectionName"));
      tau->SetMVA3rawElectronRejection(discVal("mva3rawElectronRejectionName"));
      tau->SetMVA3rawElectronRejectionCategory(discVal("mva3rawElectronRejectionCategoryName"));
      tau->SetMVA3LooseElectronRejection(discVal("mva3LooseElectronRejectionName"));
      tau->SetMVA3MediumElectronRejection(discVal("mva3MediumElectronRejectionName"));
      tau->SetMVA3TightElectronRejection(discVal("mva3TightElectronRejectionName"));
      tau->SetMVA3VTightElectronRejection(discVal("mva3VTightElectronRejectionName"));
      tau->SetLooseCombinedIsolationDBSumPtCorr3Hits(discVal("looseCombinedIsolationDBSumPtCorr3HitsName"));
      tau->SetMediumCombinedIsolationDBSumPtCorr3Hits(discVal("mediumCombinedIsolationDBSumPtCorr3HitsName"));
      tau->SetTightCombinedIsolationDBSumPtCorr3Hits(discVal("tightCombinedIsolationDBSumPtCorr3HitsName"));
      tau->SetRawCombinedIsolationDBSumPtCorr3Hits(discVal("rawCombinedIsolationDBSumPtCorr3HitsName"));
      tau->SetLooseMuonRejection2(discVal("looseMuonRejection2Name"));
      tau->SetMediumMuonRejection2(discVal("mediumMuonRejection2Name"));
      tau->SetTightMuonRejection2(discVal("tightMuonRejection2Name"));
    }

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
        tau->SetLeadPFCand(pfCandMap_->GetMit(inTau->leadPFCand()));
        
      if (inTau->leadPFChargedHadrCand().isNonnull()) {
        const mithep::PFCandidate *pfc = 
          pfCandMap_->GetMit(inTau->leadPFChargedHadrCand());
        tau->SetLeadChargedHadronPFCand(pfc);
      }
        
      if (inTau->leadPFNeutralCand().isNonnull())
        tau->SetLeadNeutralPFCand(pfCandMap_->GetMit(inTau->leadPFNeutralCand()));
        
      for (uint i=0; i<inTau->signalPFCands().size(); ++i) {
        const PFCandidate *signalCand = pfCandMap_->GetMit(inTau->signalPFCands().at(i));
        tau->AddSignalPFCand(signalCand);
      }

      for (uint i=0; i<inTau->signalPFChargedHadrCands().size(); ++i) {
	const PFCandidate *signalCand = pfCandMap_->GetMit(inTau->signalPFChargedHadrCands().at(i));
	tau->AddSignalPFChargedHadrCand(signalCand);
      }

      for (uint i=0; i<inTau->signalPFNeutrHadrCands().size(); ++i) {
	const PFCandidate *signalCand = pfCandMap_->GetMit(inTau->signalPFNeutrHadrCands().at(i));
	tau->AddSignalPFNeutrHadrCand(signalCand);
      }

      for (uint i=0; i<inTau->signalPFGammaCands().size(); ++i) {
	const PFCandidate *signalCand = pfCandMap_->GetMit(inTau->signalPFGammaCands().at(i));
	tau->AddSignalPFGammaCand(signalCand);
      }
      
      for (uint i=0; i<inTau->isolationPFCands().size(); ++i) {
        const PFCandidate *isoCand = pfCandMap_->GetMit(inTau->isolationPFCands().at(i));
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
    throw edm::Exception(edm::errors::Configuration, "FillerPFTaus::FillDataBlock()\n")
    << "Error! MITHEP Object " 
    << "not found in AssociationMaps (" << typeid(*this).name() << ")." << std::endl;
    
  return mitPart;
}
