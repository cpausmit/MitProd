#include "MitProd/TreeFiller/interface/FillMitTree.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "MitProd/ObjectService/interface/ObjectService.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"
#include "MitProd/TreeFiller/interface/FillerBasicClusters.h"
#include "MitProd/TreeFiller/interface/FillerBeamSpot.h"
#include "MitProd/TreeFiller/interface/FillerCaloJets.h"
#include "MitProd/TreeFiller/interface/FillerCaloMet.h"
#include "MitProd/TreeFiller/interface/FillerCaloTaus.h"
#include "MitProd/TreeFiller/interface/FillerCaloTowers.h"
#include "MitProd/TreeFiller/interface/FillerConversions.h"
#include "MitProd/TreeFiller/interface/FillerConversionsDecay.h"
#include "MitProd/TreeFiller/interface/FillerDecayParts.h"
#include "MitProd/TreeFiller/interface/FillerDCASig.h"
#include "MitProd/TreeFiller/interface/FillerEmbedWeight.h"
#include "MitProd/TreeFiller/interface/FillerElectrons.h"
#include "MitProd/TreeFiller/interface/FillerEvtSelData.h"
#include "MitProd/TreeFiller/interface/FillerGenJets.h"
#include "MitProd/TreeFiller/interface/FillerGenMet.h"
#include "MitProd/TreeFiller/interface/FillerJPTJets.h"
#include "MitProd/TreeFiller/interface/FillerMCEventInfo.h"
#include "MitProd/TreeFiller/interface/FillerMCParticles.h"
#include "MitProd/TreeFiller/interface/FillerMCVertexes.h"
#include "MitProd/TreeFiller/interface/FillerMet.h"
#include "MitProd/TreeFiller/interface/FillerMetaInfos.h"
#include "MitProd/TreeFiller/interface/FillerMetaInfos.h"
#include "MitProd/TreeFiller/interface/FillerMuons.h"
#include "MitProd/TreeFiller/interface/FillerPFCandidates.h"
#include "MitProd/TreeFiller/interface/FillerPackedPFCandidates.h"
#include "MitProd/TreeFiller/interface/FillerPFJets.icc"
#include "MitProd/TreeFiller/interface/FillerPFMet.icc"
#include "MitProd/TreeFiller/interface/FillerPFTaus.icc"
#include "MitProd/TreeFiller/interface/FillerPhotons.h"
#include "MitProd/TreeFiller/interface/FillerPileupInfo.h"
#include "MitProd/TreeFiller/interface/FillerPileupEnergyDensity.h"
#include "MitProd/TreeFiller/interface/FillerPixelHits.h"
#include "MitProd/TreeFiller/interface/FillerPsClusters.h"
#include "MitProd/TreeFiller/interface/FillerStableParts.h"
#include "MitProd/TreeFiller/interface/FillerStripHits.h"
#include "MitProd/TreeFiller/interface/FillerSuperClusters.h"
#include "MitProd/TreeFiller/interface/FillerTracks.h"
#include "MitProd/TreeFiller/interface/FillerTrackJets.h"
#include "MitProd/TreeFiller/interface/FillerVertexes.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataTree/interface/BranchTable.h"
#include "MitCommon/OptIO/interface/OptInt.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillMitTree::FillMitTree(const edm::ParameterSet &cfg) :
  defactive_(cfg.getUntrackedParameter<bool>("defactive",1)),
  brtable_(0),
  acfnumber_(-1),
  tws_(new TreeWriter(Names::gkEvtTreeName,0)),
  os_(new ObjectService)
{
  // Constructor.

  if (!configure(cfg)) {
    throw edm::Exception(edm::errors::Configuration, "FillMitTree::FillMitTree()\n")
      << "Could not configure fillers." << "\n";
  }
}

//--------------------------------------------------------------------------------------------------
FillMitTree::~FillMitTree()
{
  // Destructor.

  delete brtable_;
  delete tws_;
  os_->clear();
  delete os_;
}

//--------------------------------------------------------------------------------------------------
bool FillMitTree::addActiveFiller(BaseFiller *bf)
{
  // Check if filler is active and add it to list of fillers. Otherwise delete it.

  if (!bf)
    return 0;

  if (bf->Active()) {
    fillers_.push_back(bf);
    return 1;
  }

  delete bf;
  bf = 0;
  return 0;
}

//--------------------------------------------------------------------------------------------------
void FillMitTree::beginRun(edm::Run const &run, edm::EventSetup const &setup)
{
  // Access and copy event content.

  // first step: Loop over the data fillers of the various components
  for (std::vector<BaseFiller*>::const_iterator iF = fillers_.begin(); iF != fillers_.end(); ++iF) {
    try {
      (*iF)->FillRunBlock(run,setup);
    }
    catch (std::exception& exc) {
      edm::LogError("Exception") << "Exception in " << (*iF)->Name() << "::FillRunBlock()";
      throw;
    }
  }
  
}

//--------------------------------------------------------------------------------------------------
void FillMitTree::analyze(const edm::Event      &event, 
			  const edm::EventSetup &setup)
{
  // Access and copy event content.
  
  //tree writer begin event actions
  tws_->BeginEvent(kTRUE);

  // first step: Loop over the data fillers of the various components
  for (std::vector<BaseFiller*>::const_iterator iF = fillers_.begin(); iF != fillers_.end(); ++iF) {
    try {
      (*iF)->FillDataBlock(event,setup);
    }
    catch (std::exception& exc) {
      edm::LogError("Exception") << "Exception in " << (*iF)->Name() << "::FillDataBlock()";
      throw;
    }
  }

  // second step: Loop over the link resolution of the various components
  for (std::vector<BaseFiller*>::const_iterator iF = fillers_.begin(); iF != fillers_.end(); ++iF) {
    try {
      (*iF)->ResolveLinks(event,setup);
    }
    catch (std::exception& exc) {
      edm::LogError("Exception") << "Exception in " << (*iF)->Name() << "::ResolveLinks()";
      throw;
    }
  }

  if (brtable_) { // only the first FillMitTree object has to deal with the branch table
    if (acfnumber_==-1) {
      brtable_->Rehash(brtable_->GetSize());
      if (0)
        brtable_->Print();
    }
    if (acfnumber_ != tws_->GetFileNumber()) {
      tws_->StoreObject(brtable_);
      acfnumber_ = tws_->GetFileNumber();
    }
  }

  //tree writer end of event actions
  tws_->EndEvent(kTRUE);

  os_->clearEvt();
}

//--------------------------------------------------------------------------------------------------
void FillMitTree::beginJob()
{
  // Access the tree and book branches.

  brtable_ = new BranchTable;
  brtable_->SetName(Names::gkBranchTable);
  brtable_->SetOwner();
  os_->add(brtable_, brtable_->GetName());

  // loop over the various components and book the branches
  for (std::vector<BaseFiller*>::iterator iF = fillers_.begin(); iF != fillers_.end(); ++iF) {
    edm::LogInfo("FillMitTree::beginJob") << "Booking for " << (*iF)->Name() << endl;
    try {
      (*iF)->BookDataBlock(*tws_);
    }
    catch (std::exception& exc) {
      edm::LogError("Exception") << "Exception in " << (*iF)->Name() << "::BookDataBlock()";
      throw;
    }
  }

  // call branch ref for the event tree
  if (brtable_ && tws_->GetTree())
    tws_->GetTree()->BranchRef();
  
}

//--------------------------------------------------------------------------------------------------
bool FillMitTree::configure(const edm::ParameterSet &cfg)
{

  // Configure TreeWriter
  const std::string twsConfigName("TreeWriter");
  ParameterSet twsConfig;
  if (cfg.existsAs<ParameterSet>(twsConfigName,0))
    twsConfig = cfg.getUntrackedParameter<ParameterSet>(twsConfigName);
  
  configureTreeWriter(twsConfig);
  
  // Configure our fillers according to given parameter ("fillers").

  std::vector<std::string> pars;
  if (cfg.exists("fillers"))
    pars=cfg.getUntrackedParameter<vector<string> >("fillers");
  else
    cfg.getParameterSetNames(pars, false);

  edm::ConsumesCollector collector(consumesCollector());

  // loop over psets
  for (unsigned int i = 0; i<pars.size(); ++i) {

    const string name(pars.at(i));

    string ftype("Filler" + name);
    if (cfg.existsAs<ParameterSet>(name,0)) {
      ParameterSet next(cfg.getUntrackedParameter<ParameterSet>(name));
      if (!next.exists("fillerType")) {
        edm::LogError("FillMitTree") << "Cannot determine fillerType for pset named "
                                     << name << std::endl;
        throw edm::Exception(edm::errors::Configuration, "FillMitTree::configure\n")
          << "Cannot determine fillerType for pset named "
          << name << std::endl;
      }
      ftype = next.getUntrackedParameter<string>("fillerType");
    }

    edm::LogInfo("FillMitTree") << "Attempting to configure '" << ftype 
                                << "' for '" << name << "'" << std::endl;

    if (ftype.compare("FillerMetaInfos")==0) {
      FillerMetaInfos *fillerMetaInfos = new FillerMetaInfos(cfg, collector, os_, name.c_str(), defactive_);
      addActiveFiller(fillerMetaInfos);
      continue;
    }
    if (ftype.compare("FillerMCParticles")==0) {
      FillerMCParticles *fillerMCParticles = new FillerMCParticles(cfg, collector, os_, name.c_str(), defactive_);
      addActiveFiller(fillerMCParticles);
      continue;
    }
    if (ftype.compare("FillerMCEventInfo")==0) {
      FillerMCEventInfo *fillerMCEventInfo = new FillerMCEventInfo(cfg, collector, os_, name.c_str(), defactive_);
      addActiveFiller(fillerMCEventInfo);
      continue;
    }
    if (ftype.compare("FillerMCVertexes")==0) {
      FillerMCVertexes *fillerMCVertexes = new FillerMCVertexes(cfg, collector, os_, name.c_str(), defactive_);
      addActiveFiller(fillerMCVertexes);
      continue;
    }  
    if (ftype.compare("FillerEvtSelData")==0) {
      FillerEvtSelData *fillerEvtSelData = new FillerEvtSelData(cfg, collector, os_, name.c_str(), defactive_);
      addActiveFiller(fillerEvtSelData);
      continue;
    }
    if (ftype.compare("FillerBeamSpot")==0) {
      FillerBeamSpot *fillerBeamSpot = new FillerBeamSpot(cfg, collector, os_, name.c_str(), defactive_);
      addActiveFiller(fillerBeamSpot);
      continue;
    }
    if (ftype.compare("FillerVertexes")==0) {
      FillerVertexes *fillerVertexes = new FillerVertexes(cfg, collector, os_, name.c_str(), defactive_);
      addActiveFiller(fillerVertexes);
      continue;
    }  
    if (ftype.compare("FillerCaloTowers")==0) {
      FillerCaloTowers *fillerCaloTowers = new FillerCaloTowers(cfg, collector, os_, name.c_str(), defactive_);
      addActiveFiller(fillerCaloTowers);
      continue;
    }  
    if (ftype.compare("FillerGenJets")==0) {
      FillerGenJets *fillerGenJets = new FillerGenJets(cfg, collector, os_, name.c_str(), defactive_);
      addActiveFiller(fillerGenJets);
      continue;
    }  
    if (ftype.compare("FillerCaloJets")==0) {
      FillerCaloJets *fillerCaloJets = new FillerCaloJets(cfg, collector, os_, name.c_str(), defactive_);
      addActiveFiller(fillerCaloJets);
      continue;
    }  
    if (ftype.compare("FillerMet")==0) {
      FillerMet *fillerMet = new FillerMet(cfg, collector, os_, name.c_str(), defactive_);
      addActiveFiller(fillerMet);
      continue;
    }  
    if (ftype.compare("FillerGenMet")==0) {
      FillerGenMet *fillerGenMet = new FillerGenMet(cfg, collector, os_, name.c_str(), defactive_);
      addActiveFiller(fillerGenMet);
      continue;
    }  
    if (ftype.compare("FillerCaloMet")==0) {
      FillerCaloMet *fillerCaloMet = new FillerCaloMet(cfg, collector, os_, name.c_str(), defactive_);
      addActiveFiller(fillerCaloMet);
      continue;
    }
    if (ftype.compare("FillerPFMet")==0) {
      FillerPFMetFromPFMET *fillerPFMet = new FillerPFMetFromPFMET(cfg, collector, os_, name.c_str(), defactive_);
      addActiveFiller(fillerPFMet);
      continue;
    }  
    if (ftype.compare("FillerPFMetFromPATMET")==0) {
      FillerPFMetFromPATMET *fillerPFMet = new FillerPFMetFromPATMET(cfg, collector, os_, name.c_str(), defactive_);
      addActiveFiller(fillerPFMet);
      continue;
    }  
    if (ftype.compare("FillerBasicClusters")==0) {
      FillerBasicClusters *fillerBasicClusters = 
        new FillerBasicClusters(cfg, collector, os_, name.c_str(), defactive_);
      addActiveFiller(fillerBasicClusters);
      continue;
    }  
    if (ftype.compare("FillerPsClusters")==0) {
      FillerPsClusters *fillerPsClusters = 
        new FillerPsClusters(cfg, collector, os_, name.c_str(), defactive_);
      addActiveFiller(fillerPsClusters);
      continue;
    }  
    if (ftype.compare("FillerSuperClusters")==0) {
      FillerSuperClusters *fillerSuperClusters =  
        new FillerSuperClusters(cfg, collector, os_, name.c_str(), defactive_);
      addActiveFiller(fillerSuperClusters);
      continue;
    }  
    if (ftype.compare("FillerPixelHits")==0) {
      FillerPixelHits *fillerPixelHits =  
        new FillerPixelHits(cfg, collector, os_, name.c_str(), defactive_);
      addActiveFiller(fillerPixelHits);
      continue;
    }  
    if (ftype.compare("FillerPileupInfo")==0) {
      FillerPileupInfo *fillerPileupInfo =  
        new FillerPileupInfo(cfg, collector, os_, name.c_str(), defactive_);
      addActiveFiller(fillerPileupInfo);
      continue;
    }  
    if (ftype.compare("FillerPileupEnergyDensity")==0) {
      FillerPileupEnergyDensity *fillerPileupEnergyDensity =  
        new FillerPileupEnergyDensity(cfg, collector, os_, name.c_str(), defactive_);
      addActiveFiller(fillerPileupEnergyDensity);
      continue;
    }  
    if (ftype.compare("FillerStripHits")==0) {
      FillerStripHits *fillerStripHits =  
        new FillerStripHits(cfg, collector, os_, name.c_str(), defactive_);
      addActiveFiller(fillerStripHits);
      continue;
    }  
    if (ftype.compare("FillerTracks")==0) {
      FillerTracks *fillerTracks = new FillerTracks(cfg, collector, os_, name.c_str(), defactive_);
      addActiveFiller(fillerTracks);
      continue;
    }  
    if (ftype.compare("FillerMuons")==0) {
      FillerMuons *fillerMuons = new FillerMuons(cfg, collector, os_, name.c_str(), defactive_);
      addActiveFiller(fillerMuons);
      continue;
    }  
    if (ftype.compare("FillerElectrons")==0) {
      FillerElectrons *fillerElectrons = new FillerElectrons(cfg, collector, os_, name.c_str(), defactive_);
      addActiveFiller(fillerElectrons);
      continue;
    }  
    if (ftype.compare("FillerConversions")==0) {
      FillerConversions *fillerConversions = new FillerConversions(cfg, collector, os_, name.c_str(), defactive_);
      addActiveFiller(fillerConversions);
      continue;
    } 
    if (ftype.compare("FillerConversionsDecay")==0) {
      FillerConversionsDecay *fillerConversionsDecay = new FillerConversionsDecay(cfg, collector, os_, name.c_str(), defactive_);
      addActiveFiller(fillerConversionsDecay);
      continue;
    }    
    if (ftype.compare("FillerPhotons")==0) { 
      FillerPhotons *fillerPhotons = new FillerPhotons(cfg, collector, os_, name.c_str(), defactive_);
      addActiveFiller(fillerPhotons);
      continue;
    }  
    if (ftype.compare("FillerStableParts")==0) {
      FillerStableParts *fillerStableParts = new FillerStableParts(cfg, collector, os_, name.c_str(), defactive_);
      addActiveFiller(fillerStableParts);
      continue;
    }  
    if (ftype.compare("FillerDecayParts")==0) {
      FillerDecayParts *fillerDecayParts = new FillerDecayParts(cfg, collector, os_, name.c_str(), defactive_);
      addActiveFiller(fillerDecayParts);
      continue;
    }  
    if (ftype.compare("FillerPFCandidates")==0) {
      FillerPFCandidates *fillerPFCands = new FillerPFCandidates(cfg, collector, os_, name.c_str(), defactive_);
      addActiveFiller(fillerPFCands);
      continue;
    }
    if (ftype.compare("FillerPackedPFCandidates")==0) {
      FillerPackedPFCandidates *fillerPFCands = new FillerPackedPFCandidates(cfg, collector, os_, name.c_str(), defactive_);
      addActiveFiller(fillerPFCands);
      continue;
    }
    if (ftype.compare("FillerPFJets")==0) {
      FillerPFJetsFromPFJets *fillerPFJets = new FillerPFJetsFromPFJets(cfg, collector, os_, name.c_str(), defactive_);
      addActiveFiller(fillerPFJets);
      continue;
    }
    if (ftype.compare("FillerPFJetsFromPATJets")==0) {
      FillerPFJetsFromPATJets *fillerPFJets = new FillerPFJetsFromPATJets(cfg, collector, os_, name.c_str(), defactive_);
      addActiveFiller(fillerPFJets);
      continue;
    }
    if (ftype.compare("FillerJPTJets")==0) {
      FillerJPTJets *fillerJPTJets = new FillerJPTJets(cfg, collector, os_, name.c_str(), defactive_);
      addActiveFiller(fillerJPTJets);
      continue;
    }  
    if (ftype.compare("FillerCaloTaus")==0) {
      FillerCaloTaus *fillerCaloTaus = new FillerCaloTaus(cfg, collector, os_, name.c_str(), defactive_);
      addActiveFiller(fillerCaloTaus);
      continue;
    }
    if (ftype.compare("FillerPFTaus")==0) {
      FillerPFTausFromPFTaus *fillerPFTaus = new FillerPFTausFromPFTaus(cfg, collector, os_, name.c_str(), defactive_);
      addActiveFiller(fillerPFTaus);
      continue;
    }
    if (ftype.compare("FillerPFTausFromPATTaus")==0) {
      FillerPFTausFromPATTaus *fillerPFTaus = new FillerPFTausFromPATTaus(cfg, collector, os_, name.c_str(), defactive_);
      addActiveFiller(fillerPFTaus);
      continue;
    }  
    if (ftype.compare("FillerTrackJets")==0) {
      FillerTrackJets *fillerTrackJets = new FillerTrackJets(cfg, collector, os_, name.c_str(), defactive_);
      addActiveFiller(fillerTrackJets);
      continue;
    }
    if (ftype.compare("FillerEmbedWeight")==0) {
      FillerEmbedWeight *fillerEmbedWeight = new FillerEmbedWeight(cfg, collector, os_, name.c_str(), defactive_);
      addActiveFiller(fillerEmbedWeight);
      continue;
    }
    if (ftype.compare("FillerDCASig")==0) {
      FillerDCASig *fillerDCASig = new FillerDCASig(cfg, collector, os_, name.c_str(), defactive_);
      addActiveFiller(fillerDCASig);
      continue;
    }
    
    edm::LogError("FillMitTree") 
      << "Unknown fillerType " << ftype << " for pset named " << name << std::endl;
    throw edm::Exception(edm::errors::Configuration, "FillMitTree::configure\n")
      << "Unknown fillerType " << ftype << " for pset named " << name << std::endl;
  }

  return 1;
}

//--------------------------------------------------------------------------------------------------
bool FillMitTree::configureTreeWriter(const edm::ParameterSet &cfg)
{
  // Configure tree writer with options from config file.

  tws_->SetPrefix       (cfg.getUntrackedParameter<string>("fileName","mit-test"));
  tws_->SetBaseURL      (cfg.getUntrackedParameter<string>("pathName","."));
  tws_->SetMaxSize      ((Long64_t)
			 cfg.getUntrackedParameter<unsigned>("maxSize",1024)*1024*1024);
  tws_->SetCompressLevel(cfg.getUntrackedParameter<unsigned>("compLevel",9));
  tws_->SetDefaultSL    (cfg.getUntrackedParameter<unsigned>("splitLevel",99));
  tws_->SetDefaultBrSize(cfg.getUntrackedParameter<unsigned>("brSize",16*1024));
  
  if (OptInt::IsActivated()) {
    OptInt::SetZipMode     (cfg.getUntrackedParameter<unsigned>("zipMode",99));
    OptInt::SetGzipFraction(cfg.getUntrackedParameter<double>("gZipThres",1.0));
    OptInt::SetBzipFraction(cfg.getUntrackedParameter<double>("bZipThres",-1.0));
    OptInt::SetLzoFraction (cfg.getUntrackedParameter<double>("lzoThres",-1.0));
    OptInt::SetLzmaFraction(cfg.getUntrackedParameter<double>("lzmaThres",0.95));
    OptInt::SetVerbose     (cfg.getUntrackedParameter<unsigned>("optIOVerbose",0));
  }
  else {
    if (cfg.exists("zipMode")   || cfg.exists("bZipThres") || cfg.exists("gZipThres") ||
	cfg.exists("lzoThres")  || cfg.exists("lzmaThres")) {
      edm::LogError("FillMitTree") << 
        "OptIO interface not properly pre-loaded, ignoring given settings." << std::endl;
    }
  }
  
  return 1;
}

//--------------------------------------------------------------------------------------------------
void FillMitTree::endJob()
{
  // Delete fillers.

  for (std::vector<BaseFiller*>::iterator iF = fillers_.begin(); iF != fillers_.end(); ++iF) {
    try {
      delete *iF;
    }
    catch (std::exception& exc) {
      edm::LogError("Exception") << "Exception in " << (*iF)->Name() << "::~" << (*iF)->Name() << "()";
      throw;
    }
  }
  tws_->Clear();

  os_->clear();
  
  edm::LogInfo("FillMitTree::endJob") << "Ending Job" << endl;
}
