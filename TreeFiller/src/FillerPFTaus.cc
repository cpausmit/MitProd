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
  hpsNames.push_back(make_pair("discriminationByLooseElectronRejectionName",
			       "hpsPFTauDiscriminationByLooseElectronRejection"));
  hpsNames.push_back(make_pair("discriminationByMediumElectronRejectionName",
			       "hpsPFTauDiscriminationByMediumElectronRejection"));
  hpsNames.push_back(make_pair("discriminationByTightElectronRejectionName",
			       "hpsPFTauDiscriminationByTightElectronRejection"));
  hpsNames.push_back(make_pair("discriminationByMVAElectronRejectionName",
			       "hpsPFTauDiscriminationByMVAElectronRejection"));
  hpsNames.push_back(make_pair("discriminationByLooseMuonRejectionName",
			       "hpsPFTauDiscriminationByLooseMuonRejection"));
  hpsNames.push_back(make_pair("discriminationByMediumMuonRejectionName",
			       "hpsPFTauDiscriminationByMediumMuonRejection"));
  hpsNames.push_back(make_pair("discriminationByTightMuonRejectionName",
			       "hpsPFTauDiscriminationByTightMuonRejection"));
  hpsNames.push_back(make_pair("discriminationByDecayModeFindingName",
			       "hpsPFTauDiscriminationByDecayModeFinding"));
  hpsNames.push_back(make_pair("discriminationByVLooseIsolationName",
			       "hpsPFTauDiscriminationByVLooseIsolation"));
  hpsNames.push_back(make_pair("discriminationByLooseIsolationName",
			       "hpsPFTauDiscriminationByLooseIsolation"));
  hpsNames.push_back(make_pair("discriminationByMediumIsolationName",
			       "hpsPFTauDiscriminationByMediumIsolation"));
  hpsNames.push_back(make_pair("discriminationByTightIsolationName",
			       "hpsPFTauDiscriminationByTightIsolation"));
  hpsNames.push_back(make_pair("discriminationByVLooseCombinedIsolationDBSumPtCorrName",
			       "hpsPFTauDiscriminationByVLooseCombinedIsolationDBSumPtCorr"));
  hpsNames.push_back(make_pair("discriminationByLooseCombinedIsolationDBSumPtCorrName",
			       "hpsPFTauDiscriminationByLooseCombinedIsolationDBSumPtCorr"));
  hpsNames.push_back(make_pair("discriminationByMediumCombinedIsolationDBSumPtCorrName",
			       "hpsPFTauDiscriminationByMediumCombinedIsolationDBSumPtCorr"));
  hpsNames.push_back(make_pair("discriminationByTightCombinedIsolationDBSumPtCorrName",
			       "hpsPFTauDiscriminationByTightCombinedIsolationDBSumPtCorr"));
  hpsNames.push_back(make_pair("discriminationByRawCombinedIsolationDBSumPtCorrName",
			       "hpsPFTauDiscriminationByRawCombinedIsolationDBSumPtCorr"));
  hpsNames.push_back(make_pair("mva2rawElectronRejectionName",
			       "hpsPFTauDiscriminationByMVA2rawElectronRejection"));
  hpsNames.push_back(make_pair("mva2rawElectronRejectionCategoryName",
			       "hpsPFTauDiscriminationByMVA2rawElectronRejection:category"));
  hpsNames.push_back(make_pair("mva2LooseElectronRejectionName",
			       "hpsPFTauDiscriminationByMVA2LooseElectronRejection"));
  hpsNames.push_back(make_pair("mva2MediumElectronRejectionName",
			       "hpsPFTauDiscriminationByMVA2MediumElectronRejection"));
  hpsNames.push_back(make_pair("mva2TightElectronRejectionName",
			       "hpsPFTauDiscriminationByMVA2TightElectronRejection"));
  hpsNames.push_back(make_pair("mva3rawElectronRejectionName",
			       "hpsPFTauDiscriminationByMVA3rawElectronRejection"));
  hpsNames.push_back(make_pair("mva3rawElectronRejectionCategoryName",
			       "hpsPFTauDiscriminationByMVA3rawElectronRejection:category"));
  hpsNames.push_back(make_pair("mva3LooseElectronRejectionName",
			       "hpsPFTauDiscriminationByMVA3LooseElectronRejection"));
  hpsNames.push_back(make_pair("mva3MediumElectronRejectionName",
			       "hpsPFTauDiscriminationByMVA3MediumElectronRejection"));
  hpsNames.push_back(make_pair("mva3TightElectronRejectionName",
			       "hpsPFTauDiscriminationByMVA3TightElectronRejection"));
  hpsNames.push_back(make_pair("mva3VTightElectronRejectionName",
			       "hpsPFTauDiscriminationByMVA3VTightElectronRejection"));
  hpsNames.push_back(make_pair("looseCombinedIsolationDBSumPtCorr3HitsName",
			       "hpsPFTauDiscriminationByLooseCombinedIsolationDBSumPtCorr3Hits"));
  hpsNames.push_back(make_pair("mediumCombinedIsolationDBSumPtCorr3HitsName",
			       "hpsPFTauDiscriminationByMediumCombinedIsolationDBSumPtCorr3Hits"));
  hpsNames.push_back(make_pair("tightCombinedIsolationDBSumPtCorr3HitsName",
			       "hpsPFTauDiscriminationByTightCombinedIsolationDBSumPtCorr3Hits"));
  hpsNames.push_back(make_pair("rawCombinedIsolationDBSumPtCorr3HitsName",
			       "hpsPFTauDiscriminationByRawCombinedIsolationDBSumPtCorr3Hits"));
  hpsNames.push_back(make_pair("looseMuonRejection2Name",
			       "hpsPFTauDiscriminationByLooseMuonRejection2"));
  hpsNames.push_back(make_pair("mediumMuonRejection2Name",
			       "hpsPFTauDiscriminationByMediumMuonRejection2"));
  hpsNames.push_back(make_pair("tightMuonRejection2Name",
			       "hpsPFTauDiscriminationByTightMuonRejection2"));

  for (vector<pair<string, string> >::iterator it = hpsNames.begin();
       it != hpsNames.end(); it++) {
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
  if (hpsActive_) {
    for (map<string, PFTauDiscHandle>::iterator it = hpsHandles_.begin();
	 it != hpsHandles_.end(); it++) {
      PFTauDiscHandle &hpsDisc = it->second;
      GetProductSafe(hpsDisc.name, hpsDisc.handle, event);
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
    if (hpsActive_)  {
      outTau->SetDiscriminationByLooseElectronRejection(hpsHandles_["discriminationByLooseElectronRejectionName"].value(tRef));
      outTau->SetDiscriminationByMediumElectronRejection(hpsHandles_["discriminationByMediumElectronRejectionName"].value(tRef));
      outTau->SetDiscriminationByTightElectronRejection(hpsHandles_["discriminationByTightElectronRejectionName"].value(tRef));
      outTau->SetDiscriminationByMVAElectronRejection(hpsHandles_["discriminationByMVAElectronRejectionName"].value(tRef));
      outTau->SetDiscriminationByLooseMuonRejection(hpsHandles_["discriminationByLooseMuonRejectionName"].value(tRef));
      outTau->SetDiscriminationByMediumMuonRejection(hpsHandles_["discriminationByMediumMuonRejectionName"].value(tRef));
      outTau->SetDiscriminationByTightMuonRejection(hpsHandles_["discriminationByTightMuonRejectionName"].value(tRef));
      outTau->SetDiscriminationByDecayModeFinding(hpsHandles_["discriminationByDecayModeFindingName"].value(tRef));
      outTau->SetDiscriminationByVLooseIsolation(hpsHandles_["discriminationByVLooseIsolationName"].value(tRef));
      outTau->SetDiscriminationByLooseIsolation(hpsHandles_["discriminationByLooseIsolationName"].value(tRef));
      outTau->SetDiscriminationByMediumIsolation(hpsHandles_["discriminationByMediumIsolationName"].value(tRef));
      outTau->SetDiscriminationByTightIsolation(hpsHandles_["discriminationByTightIsolationName"].value(tRef));
      outTau->SetDiscriminationByVLooseCombinedIsolationDBSumPtCorr(hpsHandles_["discriminationByVLooseCombinedIsolationDBSumPtCorrName"].value(tRef));
      outTau->SetDiscriminationByLooseCombinedIsolationDBSumPtCorr(hpsHandles_["discriminationByLooseCombinedIsolationDBSumPtCorrName"].value(tRef));
      outTau->SetDiscriminationByMediumCombinedIsolationDBSumPtCorr(hpsHandles_["discriminationByMediumCombinedIsolationDBSumPtCorrName"].value(tRef));
      outTau->SetDiscriminationByTightCombinedIsolationDBSumPtCorr(hpsHandles_["discriminationByTightCombinedIsolationDBSumPtCorrName"].value(tRef));
      outTau->SetDiscriminationByRawCombinedIsolationDBSumPtCorr(hpsHandles_["discriminationByRawCombinedIsolationDBSumPtCorrName"].value(tRef));
      outTau->SetMVA2rawElectronRejection(hpsHandles_["mva2rawElectronRejectionName"].value(tRef));
      outTau->SetMVA2rawElectronRejectionCategory(hpsHandles_["mva2rawElectronRejectionCategoryName"].value(tRef));
      outTau->SetMVA2LooseElectronRejection(hpsHandles_["mva2LooseElectronRejectionName"].value(tRef));
      outTau->SetMVA2MediumElectronRejection(hpsHandles_["mva2MediumElectronRejectionName"].value(tRef));
      outTau->SetMVA2TightElectronRejection(hpsHandles_["mva2TightElectronRejectionName"].value(tRef));
      outTau->SetMVA3rawElectronRejection(hpsHandles_["mva3rawElectronRejectionName"].value(tRef));
      outTau->SetMVA3rawElectronRejectionCategory(hpsHandles_["mva3rawElectronRejectionCategoryName"].value(tRef));
      outTau->SetMVA3LooseElectronRejection(hpsHandles_["mva3LooseElectronRejectionName"].value(tRef));
      outTau->SetMVA3MediumElectronRejection(hpsHandles_["mva3MediumElectronRejectionName"].value(tRef));
      outTau->SetMVA3TightElectronRejection(hpsHandles_["mva3TightElectronRejectionName"].value(tRef));
      outTau->SetMVA3VTightElectronRejection(hpsHandles_["mva3VTightElectronRejectionName"].value(tRef));
      outTau->SetLooseCombinedIsolationDBSumPtCorr3Hits(hpsHandles_["looseCombinedIsolationDBSumPtCorr3HitsName"].value(tRef));
      outTau->SetMediumCombinedIsolationDBSumPtCorr3Hits(hpsHandles_["mediumCombinedIsolationDBSumPtCorr3HitsName"].value(tRef));
      outTau->SetTightCombinedIsolationDBSumPtCorr3Hits(hpsHandles_["tightCombinedIsolationDBSumPtCorr3HitsName"].value(tRef));
      outTau->SetRawCombinedIsolationDBSumPtCorr3Hits(hpsHandles_["rawCombinedIsolationDBSumPtCorr3HitsName"].value(tRef));
      outTau->SetLooseMuonRejection2(hpsHandles_["looseMuonRejection2Name"].value(tRef));
      outTau->SetMediumMuonRejection2(hpsHandles_["mediumMuonRejection2Name"].value(tRef));
      outTau->SetTightMuonRejection2(hpsHandles_["tightMuonRejection2Name"].value(tRef));
    }

    // link electron track directly
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
	  << "Error! Jet unmapped collection " << edmName_ << endl;
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
    throw edm::Exception(edm::errors::Configuration, "FillerPFCandidates::FillDataBlock()\n")
      << "Error! MITHEP Object " 
      << "not found in AssociationMaps (" << typeid(*this).name() << ")." << std::endl;
  
  return mitPart;
}
