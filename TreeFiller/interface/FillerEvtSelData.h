//--------------------------------------------------------------------------------------------------
// FillerEvtSelData
//
// Fill event selection data object.
//
// Authors: C.Loizides/L.Di Matteo
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLEREVTSELDATA_H
#define MITPROD_TREEFILLER_FILLEREVTSELDATA_H

#include "MitProd/TreeFiller/interface/BaseFiller.h"

namespace mithep 
{
  class EvtSelData;

  class FillerEvtSelData : public BaseFiller
  {
  public:
    FillerEvtSelData(edm::ParameterSet const&, edm::ConsumesCollector&, ObjectService*, char const* = "EvtSelData", bool = true);
    ~FillerEvtSelData();

    void BookDataBlock(TreeWriter &) override;
    void FillDataBlock(edm::Event const&, edm::EventSetup const&) override;
    void FillRunBlock(edm::Run const&, edm::EventSetup const&) override;

  private:
    typedef std::vector<std::string> VString;

    std::string               mitName_;                        //mit branch name
    VString*                  filterLabels_;
    edm::EDGetTokenT<edm::TriggerResults> patFilterResultsToken_; //filter results for MET filters already applied during PAT
    std::vector<edm::EDGetTokenT<bool>> resultTokens_;
    std::vector<unsigned> invertedFilters_;

    EvtSelData*              evtSelData_;                    //event selection data object
  };
}
#endif
