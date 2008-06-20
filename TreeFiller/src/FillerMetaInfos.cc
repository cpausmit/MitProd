// $Id: FillerMetaInfos.cc,v 1.1 2008/06/19 16:53:43 loizides Exp $

#include "MitProd/TreeFiller/interface/FillerMetaInfos.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/Common/interface/Handle.h"
#include "MitAna/DataTree/interface/Names.h"

using namespace std;
using namespace edm;
using namespace mithep;

//-------------------------------------------------------------------------------------------------
FillerMetaInfos::FillerMetaInfos(const ParameterSet &cfg) : 
  BaseFiller(cfg,"MetaInfos"),
  evtName_(Conf().getUntrackedParameter<string>("evtName",Names::gkEvtHeaderBrn)),
  runName_(Conf().getUntrackedParameter<string>("runName",Names::gkRunInfoBrn)),
  eventHeader_(new EventHeader()),
  runInfo_(new RunInfo()),
  runTree_(0),
  runEntries_(0)
{
}

//-------------------------------------------------------------------------------------------------
FillerMetaInfos::~FillerMetaInfos()
{
  delete eventHeader_;
  eventHeader_=0;
  delete runInfo_;
  runInfo_=0;
  runTree_=0;
}

//-------------------------------------------------------------------------------------------------
void FillerMetaInfos::BookDataBlock(TreeWriter &tws)
{
  tws.AddBranch(evtName_.c_str(),"mithep::EventHeader",&eventHeader_);
  tws.AddBranchToTree(Names::gkRunTreeName, runName_.c_str(),"mithep::RunInfo",&runInfo_);
  tws.SetAutoFill(Names::gkRunTreeName,0);
  runTree_=tws.GetTree(Names::gkRunTreeName);
}

//-------------------------------------------------------------------------------------------------
void FillerMetaInfos::FillDataBlock(const edm::Event      &event, 
                                    const edm::EventSetup &setup)
{
  // fill event header
  eventHeader_->SetEvtNum(event.id().event());
  eventHeader_->SetLumiSec(event.luminosityBlock());
  UInt_t runnum = event.id().run();
  eventHeader_->SetRunNum(runnum);

  // should check here if file was closed and
  // reset map if needed. This will all come, 
  // eventually split over methods or even in 
  // separate filler.

  map<UInt_t,Int_t>::iterator riter = runmap_.find(runnum);
  if (riter != runmap_.end()) {
    Int_t runentry = riter->second;
    eventHeader_->SetRunEntry(runentry);
    return;
  }

  // fill new run info
  Int_t runentry = runEntries_;
  eventHeader_->SetRunEntry(runentry);
  runmap_.insert(pair<UInt_t,Int_t>(runnum,runentry));

  runInfo_->SetRunNum(runnum);
  runTree_->Fill();
  ++runEntries_;
}
