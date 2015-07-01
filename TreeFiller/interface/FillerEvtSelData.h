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

#include "DataFormats/METReco/interface/BeamHaloSummary.h"

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

    enum EvtSelFilter {
      kHBHENoiseFilter,
      kECALDeadCellFilter,
      kTrackingFailureFilter,
      kEEBadScFilter,
      kECALaserCorrFilter,
      kManyStripClusFilter,
      kTooManyStripClusFilter,
      kLogErrorTooManyClustersFilter,
      kCSCTightHaloFilter,
      kCSCLooseHaloFilter,
      nEvtSelFilters
    };

  private:    
    std::string              mitName_;                        //mit branch name
    std::string              filterLabels_[nEvtSelFilters];
    edm::EDGetTokenT<edm::TriggerResults> patFilterResultsToken_; //filter results for MET filters already applied during PAT
    edm::EDGetTokenT<bool>   HBHENoiseFilterToken_;            //input edm HBHENoiseFilter decision 
    edm::EDGetTokenT<bool>   ECALDeadCellFilterToken_;         //input edm ECALDeadCellFilter decision 
    edm::EDGetTokenT<bool>   TrackingFailureFilterToken_;      //input edm trackingFailureFilter decision 
    edm::EDGetTokenT<bool>   EEBadScFilterToken_;              //input edm EEBadScFilter decision 
    edm::EDGetTokenT<bool>   ECALaserCorrFilterToken_;         //input edm ECALaserCorrFilter decision 
    edm::EDGetTokenT<bool>   ManyStripClusToken_;            //input edm trackingOddFilter decision [1]
    edm::EDGetTokenT<bool>   TooManyStripClusToken_;         //input edm trackingOddFilter decision [2]
    edm::EDGetTokenT<bool>   LogErrorTooManyClustersToken_;  //input edm trackingOddFilter decision [3]
    edm::EDGetTokenT<reco::BeamHaloSummary>   BeamHaloSummaryToken_; //input edm BeamHalo summary 
    EvtSelData*              evtSelData_;                    //event selection data object
  };
}
#endif
