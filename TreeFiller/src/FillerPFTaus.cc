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
    {"discriminationByLooseElectronRejectionName",
     "hpsPFTauDiscriminationByLooseElectronRejection"},
    {"discriminationByMediumElectronRejectionName",
     "hpsPFTauDiscriminationByMediumElectronRejection"},
    {"discriminationByTightElectronRejectionName",
     "hpsPFTauDiscriminationByTightElectronRejection"},
    {"discriminationByMVAElectronRejectionName",
     "hpsPFTauDiscriminationByMVAElectronRejection"},
    {"discriminationByLooseMuonRejectionName",
     "hpsPFTauDiscriminationByLooseMuonRejection"},
    {"discriminationByMediumMuonRejectionName",
     "hpsPFTauDiscriminationByMediumMuonRejection"},
    {"discriminationByTightMuonRejectionName",
     "hpsPFTauDiscriminationByTightMuonRejection"},
    {"discriminationByDecayModeFindingName",
     "hpsPFTauDiscriminationByDecayModeFinding"},
    {"discriminationByVLooseIsolationName",
     "hpsPFTauDiscriminationByVLooseIsolation"},
    {"discriminationByLooseIsolationName",
     "hpsPFTauDiscriminationByLooseIsolation"},
    {"discriminationByMediumIsolationName",
     "hpsPFTauDiscriminationByMediumIsolation"},
    {"discriminationByTightIsolationName",
     "hpsPFTauDiscriminationByTightIsolation"},
    {"discriminationByVLooseCombinedIsolationDBSumPtCorrName",
     "hpsPFTauDiscriminationByVLooseCombinedIsolationDBSumPtCorr"},
    {"discriminationByLooseCombinedIsolationDBSumPtCorrName",
     "hpsPFTauDiscriminationByLooseCombinedIsolationDBSumPtCorr"},
    {"discriminationByMediumCombinedIsolationDBSumPtCorrName",
     "hpsPFTauDiscriminationByMediumCombinedIsolationDBSumPtCorr"},
    {"discriminationByTightCombinedIsolationDBSumPtCorrName",
     "hpsPFTauDiscriminationByTightCombinedIsolationDBSumPtCorr"},
    {"discriminationByRawCombinedIsolationDBSumPtCorrName",
     "hpsPFTauDiscriminationByRawCombinedIsolationDBSumPtCorr"},
    {"mva2rawElectronRejectionName",
     "hpsPFTauDiscriminationByMVA2rawElectronRejection"},
    {"mva2rawElectronRejectionCategoryName",
     "hpsPFTauDiscriminationByMVA2rawElectronRejection:category"},
    {"mva2LooseElectronRejectionName",
     "hpsPFTauDiscriminationByMVA2LooseElectronRejection"},
    {"mva2MediumElectronRejectionName",
     "hpsPFTauDiscriminationByMVA2MediumElectronRejection"},
    {"mva2TightElectronRejectionName",
     "hpsPFTauDiscriminationByMVA2TightElectronRejection"},
    {"mva3rawElectronRejectionName",
     "hpsPFTauDiscriminationByMVA3rawElectronRejection"},
    {"mva3rawElectronRejectionCategoryName",
     "hpsPFTauDiscriminationByMVA3rawElectronRejection:category"},
    {"mva3LooseElectronRejectionName",
     "hpsPFTauDiscriminationByMVA3LooseElectronRejection"},
    {"mva3MediumElectronRejectionName",
     "hpsPFTauDiscriminationByMVA3MediumElectronRejection"},
    {"mva3TightElectronRejectionName",
     "hpsPFTauDiscriminationByMVA3TightElectronRejection"},
    {"mva3VTightElectronRejectionName",
     "hpsPFTauDiscriminationByMVA3VTightElectronRejection"},
    {"looseCombinedIsolationDBSumPtCorr3HitsName",
     "hpsPFTauDiscriminationByLooseCombinedIsolationDBSumPtCorr3Hits"},
    {"mediumCombinedIsolationDBSumPtCorr3HitsName",
     "hpsPFTauDiscriminationByMediumCombinedIsolationDBSumPtCorr3Hits"},
    {"tightCombinedIsolationDBSumPtCorr3HitsName",
     "hpsPFTauDiscriminationByTightCombinedIsolationDBSumPtCorr3Hits"},
    {"rawCombinedIsolationDBSumPtCorr3HitsName",
     "hpsPFTauDiscriminationByRawCombinedIsolationDBSumPtCorr3Hits"},
    {"looseMuonRejection2Name",
     "hpsPFTauDiscriminationByLooseMuonRejection2"},
    {"mediumMuonRejection2Name",
     "hpsPFTauDiscriminationByMediumMuonRejection2"},
    {"tightMuonRejection2Name",
     "hpsPFTauDiscriminationByTightMuonRejection2"}
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
  typedef std::map<string, edm::Handle<reco::PFTauDiscriminator> > DiscHandleMap;
  DiscHandleMap hpsHandles;
  if(hpsActive_)
  {
    for (DiscTokenMap::iterator it = hpsTokens_.begin();
        it != hpsTokens_.end(); it++) {
      // handle map entries created on the fly
      GetProductSafe(it->second, hpsHandles[it->first], event);
    }
  }
  
  // grab the input tau collection from CMSSW
  const reco::PFTauCollection inTaus = *(hTauProduct.product());  
  
  // loop through all CMSSW taus (input taus = inTaus)
  for (reco::PFTauCollection::const_iterator iT = inTaus.begin(); 
       iT != inTaus.end(); ++iT) {
    
    // grab a reference to our input tau
    unsigned int iTau = iT-inTaus.begin();
    reco::PFTauRef tRef(hTauProduct,iTau);
    
    // make our output object including allocating space
    mithep::PFTau *outTau = taus_->Allocate();
    new (outTau) mithep::PFTau(iT->px(),iT->py(),iT->pz(),iT->energy());
    outTau->SetMomAlt(iT->alternatLorentzVect().x(),
                   iT->alternatLorentzVect().y(),
                   iT->alternatLorentzVect().z(),
                   iT->alternatLorentzVect().e());

    // fill pftau-specific quantities
    outTau->SetCharge(iT->charge());
    outTau->SetBremRecoveryEOverP(iT->bremsRecoveryEOverPLead());
    outTau->SetCaloCompatibility(iT->caloComp());
    outTau->SetECalStripSumEOverP(iT->ecalStripSumEOverPLead());
    outTau->SetEMFraction(iT->emFraction());
    outTau->SetElectronPreIDDecision(iT->electronPreIDDecision());
    outTau->SetElectronPreIDOutput(iT->electronPreIDOutput());
    outTau->SetHCal3x3EOverP(iT->hcal3x3OverPLead());
    outTau->SetHCalMaxEOverP(iT->hcalMaxOverPLead());
    outTau->SetHCalTotalEOverP(iT->hcalTotOverPLead());
    outTau->SetIsoChargedHadronPtSum(iT->isolationPFChargedHadrCandsPtSum());
    outTau->SetIsoGammaEtSum(iT->isolationPFGammaCandsEtSum());
    outTau->SetLeadPFCandSignD0Sig(iT->leadPFChargedHadrCandsignedSipt());
    outTau->SetMaxHCalPFClusterEt(iT->maximumHCALPFClusterEt());
    outTau->SetMuonDecision(iT->muonDecision());
    outTau->SetSegmentCompatibility(iT->segComp());
    
    // fill HPS discriminants
    if(hpsActive_)
    {
      auto discVal = [&hpsHandles, &tRef](std::string const& discName)->double {
        DiscHandleMap::const_iterator itr(hpsHandles.find(discName));
        if(itr == hpsHandles.end()) return 0.;
        edm::Handle<reco::PFTauDiscriminator> const& handle(itr->second);
        if(handle.isValid())
          return (*handle)[tRef];
        else
          return 0.;
      };
      
      outTau->SetDiscriminationByLooseElectronRejection(discVal("discriminationByLooseElectronRejectionName"));
      outTau->SetDiscriminationByMediumElectronRejection(discVal("discriminationByMediumElectronRejectionName"));
      outTau->SetDiscriminationByTightElectronRejection(discVal("discriminationByTightElectronRejectionName"));
      outTau->SetDiscriminationByMVAElectronRejection(discVal("discriminationByMVAElectronRejectionName"));
      outTau->SetDiscriminationByLooseMuonRejection(discVal("discriminationByLooseMuonRejectionName"));
      outTau->SetDiscriminationByMediumMuonRejection(discVal("discriminationByMediumMuonRejectionName"));
      outTau->SetDiscriminationByTightMuonRejection(discVal("discriminationByTightMuonRejectionName"));
      outTau->SetDiscriminationByDecayModeFinding(discVal("discriminationByDecayModeFindingName"));
      outTau->SetDiscriminationByVLooseIsolation(discVal("discriminationByVLooseIsolationName"));
      outTau->SetDiscriminationByLooseIsolation(discVal("discriminationByLooseIsolationName"));
      outTau->SetDiscriminationByMediumIsolation(discVal("discriminationByMediumIsolationName"));
      outTau->SetDiscriminationByTightIsolation(discVal("discriminationByTightIsolationName"));
      outTau->SetDiscriminationByVLooseCombinedIsolationDBSumPtCorr(discVal("discriminationByVLooseCombinedIsolationDBSumPtCorrName"));
      outTau->SetDiscriminationByLooseCombinedIsolationDBSumPtCorr(discVal("discriminationByLooseCombinedIsolationDBSumPtCorrName"));
      outTau->SetDiscriminationByMediumCombinedIsolationDBSumPtCorr(discVal("discriminationByMediumCombinedIsolationDBSumPtCorrName"));
      outTau->SetDiscriminationByTightCombinedIsolationDBSumPtCorr(discVal("discriminationByTightCombinedIsolationDBSumPtCorrName"));
      outTau->SetDiscriminationByRawCombinedIsolationDBSumPtCorr(discVal("discriminationByRawCombinedIsolationDBSumPtCorrName"));
      outTau->SetMVA2rawElectronRejection(discVal("mva2rawElectronRejectionName"));
      outTau->SetMVA2rawElectronRejectionCategory(discVal("mva2rawElectronRejectionCategoryName"));
      outTau->SetMVA2LooseElectronRejection(discVal("mva2LooseElectronRejectionName"));
      outTau->SetMVA2MediumElectronRejection(discVal("mva2MediumElectronRejectionName"));
      outTau->SetMVA2TightElectronRejection(discVal("mva2TightElectronRejectionName"));
      outTau->SetMVA3rawElectronRejection(discVal("mva3rawElectronRejectionName"));
      outTau->SetMVA3rawElectronRejectionCategory(discVal("mva3rawElectronRejectionCategoryName"));
      outTau->SetMVA3LooseElectronRejection(discVal("mva3LooseElectronRejectionName"));
      outTau->SetMVA3MediumElectronRejection(discVal("mva3MediumElectronRejectionName"));
      outTau->SetMVA3TightElectronRejection(discVal("mva3TightElectronRejectionName"));
      outTau->SetMVA3VTightElectronRejection(discVal("mva3VTightElectronRejectionName"));
      outTau->SetLooseCombinedIsolationDBSumPtCorr3Hits(discVal("looseCombinedIsolationDBSumPtCorr3HitsName"));
      outTau->SetMediumCombinedIsolationDBSumPtCorr3Hits(discVal("mediumCombinedIsolationDBSumPtCorr3HitsName"));
      outTau->SetTightCombinedIsolationDBSumPtCorr3Hits(discVal("tightCombinedIsolationDBSumPtCorr3HitsName"));
      outTau->SetRawCombinedIsolationDBSumPtCorr3Hits(discVal("rawCombinedIsolationDBSumPtCorr3HitsName"));
      outTau->SetLooseMuonRejection2(discVal("looseMuonRejection2Name"));
      outTau->SetMediumMuonRejection2(discVal("mediumMuonRejection2Name"));
      outTau->SetTightMuonRejection2(discVal("tightMuonRejection2Name"));
    }

    if (iT->electronPreIDTrack().isNonnull()) {
      const mithep::Track *theTrack =
        getMitTrack(refToPtr(iT->electronPreIDTrack()),allowMissingTrackRef_);
      outTau->SetElectronTrack(theTrack);
    }

    // add source pfjet reference
    if (jetMap_) {
      iT->jetRef();
      //outTau->SetPFJet(jetMap_->GetMit(refToPtrHack(iT->jetRef())));
      try {
	outTau->SetPFJet(jetMap_->GetMit(refToPtr(iT->jetRef())));
      }
      catch(...) { 
	throw edm::Exception(edm::errors::Configuration, "FillerTaus:FillDataBlock()\n")
	  << "Error! Jet unmapped collection";
      }
    }

    // add pf candidate references
    if (pfCandMap_) {
      if (iT->leadPFCand().isNonnull())
	outTau->SetLeadPFCand(pfCandMap_->GetMit(iT->leadPFCand()));
      //outTau->SetLeadPFCand(pfCandMap_->GetMit(refToPtrHack(iT->leadPFCand())));
        
      if (iT->leadPFChargedHadrCand().isNonnull()) {
        const mithep::PFCandidate *pfc = 
          pfCandMap_->GetMit(iT->leadPFChargedHadrCand());
	//pfCandMap_->GetMit(refToPtrHack(iT->leadPFChargedHadrCand()));
        outTau->SetLeadChargedHadronPFCand(pfc);
      }
        
      if (iT->leadPFNeutralCand().isNonnull())
        outTau->SetLeadNeutralPFCand(pfCandMap_->GetMit(iT->leadPFNeutralCand()));
      //outTau->SetLeadNeutralPFCand(pfCandMap_->GetMit(refToPtrHack(iT->leadPFNeutralCand())));
        
      for (uint i=0; i<iT->signalPFCands().size(); ++i) {
        const PFCandidate *signalCand =
	  pfCandMap_->GetMit(iT->signalPFCands().at(i));
	//pfCandMap_->GetMit(refToPtrHack(iT->signalPFCands().at(i)));
        outTau->AddSignalPFCand(signalCand);
      }

      for (uint i=0; i<iT->signalPFChargedHadrCands().size(); ++i) {
	const PFCandidate *signalCand =
	  pfCandMap_->GetMit(iT->signalPFChargedHadrCands().at(i));
	//pfCandMap_->GetMit(refToPtrHack(iT->signalPFChargedHadrCands().at(i)));
	outTau->AddSignalPFChargedHadrCand(signalCand);
      }

      for (uint i=0; i<iT->signalPFNeutrHadrCands().size(); ++i) {
	const PFCandidate *signalCand =
	  pfCandMap_->GetMit(iT->signalPFNeutrHadrCands().at(i));
	//pfCandMap_->GetMit(refToPtrHack(iT->signalPFNeutrHadrCands().at(i)));
	outTau->AddSignalPFNeutrHadrCand(signalCand);
      }

      for (uint i=0; i<iT->signalPFGammaCands().size(); ++i) {
	const PFCandidate *signalCand =
	  pfCandMap_->GetMit(iT->signalPFGammaCands().at(i));
	//pfCandMap_->GetMit(refToPtrHack(iT->signalPFGammaCands().at(i)));
	outTau->AddSignalPFGammaCand(signalCand);
      }
      
      for (uint i=0; i<iT->isolationPFCands().size(); ++i) {
        const PFCandidate *isoCand =
	  pfCandMap_->GetMit(iT->isolationPFCands().at(i));
	//pfCandMap_->GetMit(refToPtrHack(iT->isolationPFCands().at(i)));
        outTau->AddIsoPFCand(isoCand);
      }
    }

    // add outTau equivalent to the map
    edm::Ptr<reco::PFTau> thePtr(hTauProduct,iTau);
    tauMap_->Add(thePtr,outTau);
  }
  // make sure to trim off the unused memory
  taus_->Trim();
}

//--------------------------------------------------------------------------------------------------
const mithep::Track *FillerPFTaus::getMitTrack(mitedm::TrackPtr ptr, bool allowMissing) const
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
  
  if (!mitPart && !allowMissing)
    throw edm::Exception(edm::errors::Configuration, "FillerPFTaus::FillDataBlock()\n")
      << "Error! MITHEP Object " 
      << "not found in AssociationMaps (" << typeid(*this).name() << ")." << std::endl;

  return mitPart;
}
