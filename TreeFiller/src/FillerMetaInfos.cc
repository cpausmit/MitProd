// $Id: FillerMetaInfos.cc,v 1.12 2008/07/31 12:34:04 loizides Exp $

#include "MitProd/TreeFiller/interface/FillerMetaInfos.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Framework/interface/TriggerNames.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataTree/interface/EventHeader.h"
#include "MitAna/DataTree/interface/LAHeader.h"
#include "MitAna/DataTree/interface/RunInfo.h"
#include <TObjectTable.h>
 

//#include "DataFormats/HLTReco/interface/TriggerEvent.h"



using namespace std;
using namespace edm;
using namespace mithep;

bool mithep::FillerMetaInfos::instance_ = 0;

//--------------------------------------------------------------------------------------------------
FillerMetaInfos::FillerMetaInfos(const ParameterSet &cfg, bool active) : 
  BaseFiller(cfg,"MetaInfos",(instance_==0||active?1:0)),
  hltActive_(Conf().getUntrackedParameter<bool>("hltActive",true)),
  evtName_(Conf().getUntrackedParameter<string>("evtName",Names::gkEvtHeaderBrn)),
  runName_(Conf().getUntrackedParameter<string>("runName",Names::gkRunInfoBrn)),
  lahName_(Conf().getUntrackedParameter<string>("lahName",Names::gkLAHeaderBrn)),
  hltName_(Conf().getUntrackedParameter<string>("hltName","TriggerResults")),
  l1tName_(Conf().getUntrackedParameter<string>("l1tName","todo")),
  tws_(0),
  eventHeader_(new EventHeader()),
  evtLAHeader_(new LAHeader()),
  runInfo_(new RunInfo()),
  runTree_(0),
  laTree_(0),
  runEntries_(0),
  fileNum_(0)
{
  // Constructor.

  instance_=1;
}

//--------------------------------------------------------------------------------------------------
FillerMetaInfos::~FillerMetaInfos()
{
  // Destructor.

  delete eventHeader_;
  delete evtLAHeader_;
  delete runInfo_;
  eventHeader_ = 0;
  evtLAHeader_ = 0;
  runInfo_     = 0;
  runTree_     = 0;
  laTree_      = 0;
}

//--------------------------------------------------------------------------------------------------
void FillerMetaInfos::BookDataBlock(TreeWriter &tws)
{
  // Create run info tre and book our branches.

  // add branches to main tree
  tws.AddBranch(evtName_.c_str(),"mithep::EventHeader",&eventHeader_);

  // add branches to run info tree
  tws.AddBranchToTree(Names::gkRunTreeName,runName_.c_str(),"mithep::RunInfo",&runInfo_);
  tws.SetAutoFill(Names::gkRunTreeName,0);
  runTree_=tws.GetTree(Names::gkRunTreeName);

  // add branches to lookahead tree
  tws.AddBranchToTree(Names::gkLATreeName,Names::gkLAHeaderBrn,"mithep::LAHeader",&evtLAHeader_);
  tws.SetAutoFill(Names::gkLATreeName,0);
  laTree_=tws.GetTree(Names::gkLATreeName);

  // store pointer to tree writer 
  tws_ = &tws;
}

//--------------------------------------------------------------------------------------------------
void FillerMetaInfos::FillDataBlock(const edm::Event &event, 
                                    const edm::EventSetup &setup)
{
  // Fill our data structures.

  // clear map if a new file was opened
  if (tws_->GetFileNumber()!=fileNum_) {
    runmap_.clear();
    fileNum_ = tws_->GetFileNumber();
    runEntries_ = 0;
  }

  UInt_t runnum = event.id().run();

  // store look ahead information
  if (runEntries_>0) {
    evtLAHeader_->SetRunNum(runnum);
    laTree_->Fill();
    //if(laTree_->GetEntries() % 100==0)
    //  gObjectTable->Print();

  }

  // fill event header
  eventHeader_->SetEvtNum(event.id().event());
  eventHeader_->SetLumiSec(event.luminosityBlock());
  eventHeader_->SetRunNum(runnum);

  if(hltActive_)
    FillHltInfo(event,setup);

  // look-up if entry is in map
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

//--------------------------------------------------------------------------------------------------
void FillerMetaInfos::FillHltInfo(const edm::Event &event, 
                                  const edm::EventSetup &setup)
{
  //

  Handle<TriggerResults> triggerResultsHLT;
  try {
    GetProduct(hltName_, triggerResultsHLT, event);

    //event.getByLabel(hltName_, triggerResultsHLT);
  } catch (cms::Exception &ex) {
    edm::LogError("FillerMetaInfos") << "Error! Cannot get trigger results with label " 
                                     << hltName_ << endl;
    throw edm::Exception(edm::errors::Configuration, "FillerMetaInfos::FillHltInfo\n")
      << "Error! Cannot get trigger results with label " << hltName_ << endl;
  }

  //This gives names of trigger paths and the accept bit.
  TriggerNames triggerNames(*(triggerResultsHLT.product())); 
  for(UInt_t i=0;i<triggerNames.size();++i) {
    //cout << i << " " << triggerNames.triggerName(i) << " " << (*triggerResultsHLT.product())[i].accept() << endl;
  }

}
