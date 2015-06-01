//--------------------------------------------------------------------------------------------------
// FillerMetaInfos
//
// Implementation of a filler that stores all of the meta information, as well as the L1 and HLT
// trigger info into the corresponding bambu objects.
//
// Authors: C.Loizides
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERMETAINFOS_H
#define MITPROD_TREEFILLER_FILLERMETAINFOS_H

#include "MitProd/TreeFiller/interface/BaseFiller.h"

#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerReadoutRecord.h"

namespace mithep 
{
  class RunInfo;
  class EventHeader;
  class LAHeader;

  class FillerMetaInfos : public BaseFiller
  {
  public:
    FillerMetaInfos(edm::ParameterSet const&, edm::ConsumesCollector&, ObjectService*, char const* = "MetaInfos", bool = true);
    ~FillerMetaInfos();

    void BookDataBlock(TreeWriter&) override;
    void FillDataBlock(edm::Event const&, edm::EventSetup const&) override;
    void FillRunBlock(edm::Run const&, edm::EventSetup const&) override;

    typedef std::map<UInt_t, Int_t> RunMap;

  private:

    edm::EDGetTokenT<L1GlobalTriggerReadoutRecord> l1GTRRToken_; //l1 global readout trigger record edm token
    std::string                    evtName_;      //event branch name (must be unique)
    TreeWriter                    *tws_;          //tree writer (not owned)
    EventHeader                   *eventHeader_;  //event header
    LAHeader                      *evtLAHeader_;  //look-ahead event header
    RunInfo                       *runInfo_;      //run info block
    Int_t                          runEntries_;   //number of run info entries
    RunMap                         runmap_;       //map between run number and entry number
    UShort_t                       fileNum_;      //file number of current file
  };
}
#endif
