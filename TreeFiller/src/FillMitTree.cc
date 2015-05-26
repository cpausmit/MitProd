#include "MitProd/TreeFiller/interface/FillMitTree.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

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
#include "MitProd/TreeFiller/interface/FillerTrigger.h"
#include "MitProd/TreeFiller/interface/FillerVertexes.h"

#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataTree/interface/BranchTable.h"
#include "MitAna/DataTree/interface/LAHeader.h"
#include "MitCommon/OptIO/interface/OptInt.h"

mithep::FillMitTree::FillMitTree(edm::ParameterSet const& cfg) :
  defactive_(cfg.getUntrackedParameter<bool>("defactive", true)),
  brtable_(0),
  evtLAHeader_(0),
  acfnumber_(-1),
  tws_(new mithep::TreeWriter(mithep::Names::gkEvtTreeName, false)),
  os_(new mithep::ObjectService)
{
  // Constructor.

  tws_->AddTree(mithep::Names::gkRunTreeName);
  tws_->SetAutoFill(mithep::Names::gkRunTreeName, false);
  tws_->AddTree(mithep::Names::gkLATreeName);
  tws_->SetAutoFill(mithep::Names::gkLATreeName, false);

  if (!configure(cfg)) {
    throw edm::Exception(edm::errors::Configuration, "FillMitTree::FillMitTree()\n")
      << "Could not configure fillers." << "\n";
  }
}

mithep::FillMitTree::~FillMitTree()
{
  // Destructor.

  delete tws_;
  os_->clear();
  delete os_;
}

bool
mithep::FillMitTree::addActiveFiller(BaseFiller* bf)
{
  // Check if filler is active and add it to list of fillers. Otherwise delete it.

  if (!bf)
    return false;

  if (bf->Active()) {
    fillers_.push_back(bf);
    return true;
  }

  delete bf;

  return false;
}

void
mithep::FillMitTree::beginRun(edm::Run const& run, edm::EventSetup const& setup)
{
  // Access and copy event content.

  // first step: Loop over the data fillers of the various components
  for (auto filler : fillers_) {
    try {
      filler->FillRunBlock(run, setup);
    }
    catch (std::exception&) {
      edm::LogError("Exception") << "Exception in " << filler->Name() << "::FillRunBlock()";
      throw;
    }
  }
}

void
mithep::FillMitTree::endRun(edm::Run const&, edm::EventSetup const&)
{
  tws_->GetTree(Names::gkRunTreeName)->Fill();
}

void
mithep::FillMitTree::analyze(edm::Event const& event, 
                             edm::EventSetup const& setup)
{
  // Access and copy event content.
  
  //tree writer begin event actions
  tws_->BeginEvent(kTRUE);

  // first step: Loop over the data fillers of the various components
  for (auto filler : fillers_) {
    try {
      filler->FillDataBlock(event,setup);
    }
    catch (std::exception&) {
      edm::LogError("Exception") << "Exception in " << filler->Name() << "::FillDataBlock()";
      throw;
    }
  }

  // second step: Loop over the link resolution of the various components
  for (auto filler : fillers_) {
    try {
      filler->ResolveLinks(event,setup);
    }
    catch (std::exception&) {
      edm::LogError("Exception") << "Exception in " << filler->Name() << "::ResolveLinks()";
      throw;
    }
  }

  if (acfnumber_ == -1)
    brtable_->Rehash(brtable_->GetSize());

  if (acfnumber_ != tws_->GetFileNumber())
    tws_->StoreObject(brtable_);

  if (acfnumber_ == tws_->GetFileNumber()) {
    // Fill look-ahead tree for all events except the very first in the file
    tws_->GetTree(mithep::Names::gkLATreeName)->Fill();
  }

  //tree writer end of event actions
  tws_->EndEvent(kTRUE);

  acfnumber_ = tws_->GetFileNumber();

  os_->clearEvt();
}

void
mithep::FillMitTree::beginJob()
{
  // Access the tree and book branches.

  brtable_ = new mithep::BranchTable;
  brtable_->SetName(mithep::Names::gkBranchTable);
  brtable_->SetOwner();
  os_->add(brtable_, mithep::Names::gkBranchTable);

  // loop over the various components and book the branches
  for (auto filler : fillers_) {
    edm::LogInfo("FillMitTree::beginJob") << "Booking for " << filler->Name();
    try {
      filler->BookDataBlock(*tws_);
    }
    catch (std::exception&) {
      edm::LogError("Exception") << "Exception in " << filler->Name() << "::BookDataBlock()";
      throw;
    }
  }

  // call branch ref for the event tree
  if (tws_->GetTree())
    tws_->GetTree()->BranchRef();

  evtLAHeader_ = new mithep::LAHeader;
  tws_->AddBranchToTree(mithep::Names::gkLATreeName, mithep::Names::gkLAHeaderBrn,
                        mithep::LAHeader::Class()->GetName(), &evtLAHeader_);
  os_->add(evtLAHeader_, mithep::Names::gkLAHeaderBrn);
}

bool
mithep::FillMitTree::configure(edm::ParameterSet const& cfg)
{
  // Configure TreeWriter
  const std::string twsConfigName("TreeWriter");
  edm::ParameterSet twsConfig;
  if (cfg.existsAs<edm::ParameterSet>(twsConfigName, 0))
    twsConfig = cfg.getUntrackedParameterSet(twsConfigName);
  
  configureTreeWriter(twsConfig);
  
  // Configure our fillers according to given parameter ("fillers").

  std::vector<std::string> pars;
  if (cfg.exists("fillers"))
    pars = cfg.getUntrackedParameter<std::vector<std::string> >("fillers");
  else
    cfg.getParameterSetNames(pars, false);

  edm::ConsumesCollector collector(consumesCollector());

  // loop over psets
  for (auto&& name : pars) {
    std::string ftype("Filler" + name);
    if (cfg.existsAs<edm::ParameterSet>(name, 0)) {
      auto& next = cfg.getUntrackedParameterSet(name);

      if (!next.exists("fillerType")) {
        edm::LogError("FillMitTree") << "Cannot determine fillerType for pset named " << name;
        throw edm::Exception(edm::errors::Configuration, "FillMitTree::configure")
          << "Cannot determine fillerType for pset named " << name;
      }
      ftype = next.getUntrackedParameter<std::string>("fillerType");
    }

    edm::LogInfo("FillMitTree") << "Attempting to configure '" << ftype 
                                << "' for '" << name << "'";

    BaseFiller* filler = 0;

    if (ftype == "FillerMetaInfos")
      filler = new FillerMetaInfos(cfg, collector, os_, name.c_str(), defactive_);
    else if (ftype == "FillerTrigger")
      filler = new FillerTrigger(cfg, collector, os_, name.c_str(), defactive_);
    else if (ftype == "FillerMCParticles")
      filler = new FillerMCParticles(cfg, collector, os_, name.c_str(), defactive_);
    else if (ftype == "FillerMCEventInfo")
      filler = new FillerMCEventInfo(cfg, collector, os_, name.c_str(), defactive_);
    else if (ftype == "FillerMCVertexes")
      filler = new FillerMCVertexes(cfg, collector, os_, name.c_str(), defactive_);
    else if (ftype == "FillerEvtSelData")
      filler = new FillerEvtSelData(cfg, collector, os_, name.c_str(), defactive_);
    else if (ftype == "FillerBeamSpot")
      filler = new FillerBeamSpot(cfg, collector, os_, name.c_str(), defactive_);
    else if (ftype == "FillerVertexes")
      filler = new FillerVertexes(cfg, collector, os_, name.c_str(), defactive_);
    else if (ftype == "FillerCaloTowers")
      filler = new FillerCaloTowers(cfg, collector, os_, name.c_str(), defactive_);
    else if (ftype == "FillerGenJets")
      filler = new FillerGenJets(cfg, collector, os_, name.c_str(), defactive_);
    else if (ftype == "FillerCaloJets")
      filler = new FillerCaloJets(cfg, collector, os_, name.c_str(), defactive_);
    else if (ftype == "FillerMet")
      filler = new FillerMet(cfg, collector, os_, name.c_str(), defactive_);
    else if (ftype == "FillerGenMet")
      filler = new FillerGenMet(cfg, collector, os_, name.c_str(), defactive_);
    else if (ftype == "FillerCaloMet")
      filler = new FillerCaloMet(cfg, collector, os_, name.c_str(), defactive_);
    else if (ftype == "FillerPFMet")
      filler = new FillerPFMetFromPFMET(cfg, collector, os_, name.c_str(), defactive_);
    else if (ftype == "FillerPFMetFromPATMET")
      filler = new FillerPFMetFromPATMET(cfg, collector, os_, name.c_str(), defactive_);
    else if (ftype == "FillerBasicClusters")
      filler = new FillerBasicClusters(cfg, collector, os_, name.c_str(), defactive_);
    else if (ftype == "FillerPsClusters")
      filler = new FillerPsClusters(cfg, collector, os_, name.c_str(), defactive_);
    else if (ftype == "FillerSuperClusters")
      filler = new FillerSuperClusters(cfg, collector, os_, name.c_str(), defactive_);
    else if (ftype == "FillerPixelHits")
      filler = new FillerPixelHits(cfg, collector, os_, name.c_str(), defactive_);
    else if (ftype == "FillerPileupInfo")
      filler = new FillerPileupInfo(cfg, collector, os_, name.c_str(), defactive_);
    else if (ftype == "FillerPileupEnergyDensity")
      filler = new FillerPileupEnergyDensity(cfg, collector, os_, name.c_str(), defactive_);
    else if (ftype == "FillerStripHits")
      filler = new FillerStripHits(cfg, collector, os_, name.c_str(), defactive_);
    else if (ftype == "FillerTracks")
      filler = new FillerTracks(cfg, collector, os_, name.c_str(), defactive_);
    else if (ftype == "FillerMuons")
      filler = new FillerMuons(cfg, collector, os_, name.c_str(), defactive_);
    else if (ftype == "FillerElectrons")
      filler = new FillerElectrons(cfg, collector, os_, name.c_str(), defactive_);
    else if (ftype == "FillerConversions")
      filler = new FillerConversions(cfg, collector, os_, name.c_str(), defactive_);
    else if (ftype == "FillerConversionsDecay")
      filler = new FillerConversionsDecay(cfg, collector, os_, name.c_str(), defactive_);
    else if (ftype == "FillerPhotons") 
      filler = new FillerPhotons(cfg, collector, os_, name.c_str(), defactive_);
    else if (ftype == "FillerStableParts")
      filler = new FillerStableParts(cfg, collector, os_, name.c_str(), defactive_);
    else if (ftype == "FillerDecayParts")
      filler = new FillerDecayParts(cfg, collector, os_, name.c_str(), defactive_);
    else if (ftype == "FillerPFCandidates")
      filler = new FillerPFCandidates(cfg, collector, os_, name.c_str(), defactive_);
    else if (ftype == "FillerPackedPFCandidates")
      filler = new FillerPackedPFCandidates(cfg, collector, os_, name.c_str(), defactive_);
    else if (ftype == "FillerPFJets")
      filler = new FillerPFJetsFromPFJets(cfg, collector, os_, name.c_str(), defactive_);
    else if (ftype == "FillerPFJetsFromPATJets")
      filler = new FillerPFJetsFromPATJets(cfg, collector, os_, name.c_str(), defactive_);
    else if (ftype == "FillerJPTJets")
      filler = new FillerJPTJets(cfg, collector, os_, name.c_str(), defactive_);
    else if (ftype == "FillerCaloTaus")
      filler = new FillerCaloTaus(cfg, collector, os_, name.c_str(), defactive_);
    else if (ftype == "FillerPFTaus")
      filler = new FillerPFTausFromPFTaus(cfg, collector, os_, name.c_str(), defactive_);
    else if (ftype == "FillerPFTausFromPATTaus")
      filler = new FillerPFTausFromPATTaus(cfg, collector, os_, name.c_str(), defactive_);
    else if (ftype == "FillerTrackJets")
      filler = new FillerTrackJets(cfg, collector, os_, name.c_str(), defactive_);
    else if (ftype == "FillerEmbedWeight")
      filler = new FillerEmbedWeight(cfg, collector, os_, name.c_str(), defactive_);
    else if (ftype == "FillerDCASig")
      filler = new FillerDCASig(cfg, collector, os_, name.c_str(), defactive_);

    if (filler)
      addActiveFiller(filler);
    else {
      edm::LogError("FillMitTree") 
        << "Unknown fillerType " << ftype << " for pset named " << name;
      throw edm::Exception(edm::errors::Configuration, "FillMitTree::configure\n")
        << "Unknown fillerType " << ftype << " for pset named " << name;
    }
  }

  return true;
}

bool
mithep::FillMitTree::configureTreeWriter(edm::ParameterSet const& cfg)
{
  // Configure tree writer with options from config file.

  tws_->SetPrefix(cfg.getUntrackedParameter<std::string>("fileName", "mit-test"));
  tws_->SetBaseURL(cfg.getUntrackedParameter<std::string>("pathName", "."));
  tws_->SetMaxSize((Long64_t)cfg.getUntrackedParameter<unsigned>("maxSize", 1024) * 1024 * 1024);
  tws_->SetCompressLevel(cfg.getUntrackedParameter<unsigned>("compLevel", 9));
  tws_->SetDefaultSL(cfg.getUntrackedParameter<unsigned>("splitLevel", 99));
  tws_->SetDefaultBrSize(cfg.getUntrackedParameter<unsigned>("brSize", 16 * 1024));
  
  if (OptInt::IsActivated()) {
    OptInt::SetZipMode(cfg.getUntrackedParameter<unsigned>("zipMode", 99));
    OptInt::SetGzipFraction(cfg.getUntrackedParameter<double>("gZipThres", 1.0));
    OptInt::SetBzipFraction(cfg.getUntrackedParameter<double>("bZipThres", -1.0));
    OptInt::SetLzoFraction(cfg.getUntrackedParameter<double>("lzoThres", -1.0));
    OptInt::SetLzmaFraction(cfg.getUntrackedParameter<double>("lzmaThres", 0.95));
    OptInt::SetVerbose(cfg.getUntrackedParameter<unsigned>("optIOVerbose", 0));
  }
  else {
    if (cfg.exists("zipMode")   || cfg.exists("bZipThres") || cfg.exists("gZipThres") ||
	cfg.exists("lzoThres")  || cfg.exists("lzmaThres")) {
      edm::LogError("FillMitTree") << 
        "OptIO interface not properly pre-loaded, ignoring given settings.";
    }
  }
  
  return true;
}

void
mithep::FillMitTree::endJob()
{
  // Delete fillers.

  for (auto filler : fillers_) {
    try {
      delete filler;
    }
    catch (std::exception&) {
      edm::LogError("Exception") << "Exception in " << filler->Name() << "::~" << filler->Name() << "()";
      throw;
    }
  }

  delete brtable_;
  brtable_ = 0;
  delete evtLAHeader_;
  evtLAHeader_ = 0;

  tws_->Clear();

  os_->clear();
  
  edm::LogInfo("FillMitTree::endJob") << "Ending Job";
}
