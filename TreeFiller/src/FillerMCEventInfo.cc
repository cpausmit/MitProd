// $Id: FillerMCEventInfo.cc,v 1.13 2008/08/18 11:16:13 sixie Exp $

#include "MitProd/TreeFiller/interface/FillerMCEventInfo.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/Common/interface/Handle.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataTree/interface/MCEventInfo.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerMCEventInfo::FillerMCEventInfo(const ParameterSet &cfg, bool active) : 
  BaseFiller(cfg,"MCEventInfo",active),
  evtName_(Conf().getUntrackedParameter<string>("evtName",Names::gkMCEvtInfoBrn)),
  datasetName_(Conf().getUntrackedParameter<string>("datasetName","")),
  eventInfo_(new MCEventInfo())
{
  // Constructor.
}

//--------------------------------------------------------------------------------------------------
FillerMCEventInfo::~FillerMCEventInfo()
{
  // Destructor.

  delete eventInfo_;
}

//--------------------------------------------------------------------------------------------------
void FillerMCEventInfo::BookDataBlock(TreeWriter &tws)
{
  // Create run info tre and book our branches.

  // add branches to main tree
  tws.AddBranch(evtName_.c_str(),"mithep::MCEventInfo",&eventInfo_);
}

//--------------------------------------------------------------------------------------------------
void FillerMCEventInfo::FillDataBlock(const edm::Event &event, 
                                      const edm::EventSetup &setup)
{
  // Fill our data structures.

  eventInfo_->SetWeight(1);

  // can assign different weight based on datasetName_ and event content here
}
