#include "MitProd/TreeFiller/interface/FillerMetaInfos.h"

#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/Run.h"

#include "MitAna/DataTree/interface/EventHeader.h"
#include "MitAna/DataTree/interface/LAHeader.h"
#include "MitAna/DataTree/interface/RunInfo.h"
#include "MitAna/DataTree/interface/Names.h"

#include "MitProd/ObjectService/interface/ObjectService.h"

mithep::FillerMetaInfos::FillerMetaInfos(edm::ParameterSet const& cfg, edm::ConsumesCollector& collector, ObjectService* os, char const* name/* = "MetaInfos"*/, bool active/* = true*/) :
  BaseFiller(cfg, os, name, active),
  l1GTRRToken_(GetToken<L1GlobalTriggerReadoutRecord>(collector, cfg, "l1GtReadRecEdmName", "")),
  evtName_(cfg.getUntrackedParameter<string>("evtMitName", mithep::Names::gkEvtHeaderBrn)),
  tws_(0),
  eventHeader_(new EventHeader()),
  evtLAHeader_(0),
  runInfo_(new RunInfo()),
  runEntries_(0),
  runmap_(),
  fileNum_(0)
{
  // Constructor.
}

mithep::FillerMetaInfos::~FillerMetaInfos()
{
  // Destructor.

  delete eventHeader_;
  delete runInfo_;
}

void
mithep::FillerMetaInfos::BookDataBlock(TreeWriter& tws)
{
  // Create run info tree and book our branches.

  // add branches to main tree
  tws.AddBranch(evtName_, &eventHeader_);
  OS()->add(eventHeader_, evtName_);

  // add branches to run info tree
  tws.AddBranchToTree(mithep::Names::gkRunTreeName, mithep::Names::gkRunInfoBrn,
                      mithep::RunInfo::Class()->GetName(), &runInfo_);
  OS()->add(runInfo_, mithep::Names::gkRunInfoBrn);

  evtLAHeader_ = OS()->mod<mithep::LAHeader>(mithep::Names::gkLAHeaderBrn);

  // store pointer to tree writer 
  tws_ = &tws;
}

void
mithep::FillerMetaInfos::FillDataBlock(edm::Event const& event, edm::EventSetup const&)
{
  // Fill our data structures.

  // clear map if a new file was opened
  if (tws_->GetFileNumber() != fileNum_) {
    fileNum_ = tws_->GetFileNumber();
    runmap_.clear();
    runEntries_ = 0;
  }

  UInt_t runnum = event.id().run();

  // store look ahead information
  // not filled at first event in the file -> works as look-ahead
  if (runEntries_ > 0)
    evtLAHeader_->SetRunNum(runnum);

  // fill event header
  eventHeader_->SetEvtNum(event.id().event());
  eventHeader_->SetLumiSec(event.luminosityBlock());
  eventHeader_->SetRunNum(runnum);
  eventHeader_->SetIsMC(!event.isRealData());
  eventHeader_->SetBunchCrossing(event.bunchCrossing());
  eventHeader_->SetExpType(event.experimentType());
  eventHeader_->SetOrbitNumber(event.orbitNumber());
  eventHeader_->SetStoreNumber(event.bunchCrossing());
  eventHeader_->SetTimeStamp(event.time().value());

  // look-up if entry is in map and load existing run and hlt entries
  auto riter = runmap_.find(runnum);
  if (riter == runmap_.end()) {
    // fill new run info
    riter = runmap_.insert(RunMap::value_type(runnum, runEntries_)).first;
    ++runEntries_;    
  }
  
  Int_t runentry = riter->second;
  eventHeader_->SetRunEntry(runentry);

  // get L1 trigger readout record information if wanted
  if (l1GTRRToken_.isUninitialized())
    return;

  edm::Handle<L1GlobalTriggerReadoutRecord> gtReadoutRec;
  GetProduct(l1GTRRToken_, gtReadoutRec, event);

  auto& fdlword = gtReadoutRec->gtFdlWord();
  eventHeader_->SetIsPhysDec(fdlword.physicsDeclared() == 1);
}

void
mithep::FillerMetaInfos::FillRunBlock(edm::Run const& run, edm::EventSetup const&)
{
  runInfo_->SetRunNum(run.run());
  runInfo_->SetHltEntry(-1);
}  

DEFINE_MITHEP_TREEFILLER(FillerMetaInfos);
