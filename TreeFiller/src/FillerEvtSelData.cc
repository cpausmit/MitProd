#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/Common/interface/TriggerResults.h"

#include "MitProd/TreeFiller/interface/FillerEvtSelData.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataTree/interface/EvtSelData.h"
#include "MitProd/ObjectService/interface/ObjectService.h"

#include <algorithm>

mithep::FillerEvtSelData::FillerEvtSelData(edm::ParameterSet const& cfg, edm::ConsumesCollector& collector, mithep::ObjectService* os, char const* name/* = "EvtSelData"*/,  bool active/* = true*/) : 
  BaseFiller(cfg, os, "EvtSelData",active),
  mitName_(cfg.getUntrackedParameter<string>("mitName", mithep::Names::gkEvtSelDataBrn)),
  HBHENoiseFilterToken_(GetToken<bool>(collector, cfg, "HBHENoiseFilterName", "")),
  ECALDeadCellFilterToken_(GetToken<bool>(collector, cfg, "ECALDeadCellFilterName", "")),
  TrackingFailureFilterToken_(GetToken<bool>(collector, cfg, "trackingFailureFilterName", "")),
  EEBadScFilterToken_(GetToken<bool>(collector, cfg, "EEBadScFilterName", "")),
  ECALaserCorrFilterToken_(GetToken<bool>(collector, cfg, "ECALaserCorrFilterName", "")),
  ManyStripClusToken_(GetToken<bool>(collector, cfg, "tkManyStripClusName", "")),
  TooManyStripClusToken_(GetToken<bool>(collector, cfg, "tkTooManyStripClusName", "")),
  LogErrorTooManyClustersToken_(GetToken<bool>(collector, cfg, "tkLogErrorTooManyClustersName", "")),
  BeamHaloSummaryToken_(GetToken<reco::BeamHaloSummary>(collector, cfg, "BeamHaloSummaryName", "")),
  patFilterResultsToken_(GetToken<edm::TriggerResults>(collector, cfg, "patFilterResultsName", "TriggerResults::PAT")),
  evtSelData_(new EvtSelData())
{
  filterLabels_[kHBHENoiseFilter] = cfg.getUntrackedParameter<std::string>("HBHENoiseFilterName", "");
  filterLabels_[kECALDeadCellFilter] = cfg.getUntrackedParameter<std::string>("ECALDeadCellFilterName", "");
  filterLabels_[kTrackingFailureFilter] = cfg.getUntrackedParameter<std::string>("trackingFailureFilterName", "");
  filterLabels_[kEEBadScFilter] = cfg.getUntrackedParameter<std::string>("EEBadScFilterName", "");
  filterLabels_[kECALaserCorrFilter] = cfg.getUntrackedParameter<std::string>("ECALaserCorrFilterName", "");
  filterLabels_[kManyStripClusFilter] = cfg.getUntrackedParameter<std::string>("tkManyStripClusName", "");
  filterLabels_[kTooManyStripClusFilter] = cfg.getUntrackedParameter<std::string>("tkTooManyStripClusName", "");
  filterLabels_[kLogErrorTooManyClustersFilter] = cfg.getUntrackedParameter<std::string>("tkLogErrorTooManyClustersName", "");
  filterLabels_[kCSCTightHaloFilter] = cfg.getUntrackedParameter<std::string>("BeamHaloSummaryName", "");
  filterLabels_[kCSCLooseHaloFilter] = cfg.getUntrackedParameter<std::string>("BeamHaloSummaryName", "");
}

mithep::FillerEvtSelData::~FillerEvtSelData()
{
  delete evtSelData_;
}

void
mithep::FillerEvtSelData::BookDataBlock(TreeWriter& tws)
{
  tws.AddBranch(mitName_, &evtSelData_);
  OS()->add(evtSelData_, mitName_);
}

void
mithep::FillerEvtSelData::FillDataBlock(edm::Event const& event, 
                                        edm::EventSetup const&)
{
  bool pass[nEvtSelFilters];
  std::fill_n(pass, nEvtSelFilters, false);

  if (patFilterResultsToken_.isUninitialized()) {
  // PAT filters not applied -> Read the MET filters boolean decisions
    edm::Handle<bool> HBHENoiseFilterHandle;
    GetProduct(HBHENoiseFilterToken_, HBHENoiseFilterHandle, event);
    pass[kHBHENoiseFilter] = *HBHENoiseFilterHandle;
  
    edm::Handle<bool> ECALDeadCellFilterHandle;
    GetProduct(ECALDeadCellFilterToken_, ECALDeadCellFilterHandle, event);
    pass[kECALDeadCellFilter] = *ECALDeadCellFilterHandle;
  
    edm::Handle<bool> TrackingFailureFilterHandle;
    GetProduct(TrackingFailureFilterToken_, TrackingFailureFilterHandle, event);
    pass[kTrackingFailureFilter] = *TrackingFailureFilterHandle;

    edm::Handle<bool> EEBadScFilterHandle;
    GetProduct(EEBadScFilterToken_, EEBadScFilterHandle, event);
    pass[kEEBadScFilter] = *EEBadScFilterHandle;

    edm::Handle<bool> ECALaserCorrFilterHandle;
    GetProduct(ECALaserCorrFilterToken_, ECALaserCorrFilterHandle, event);
    pass[kECALaserCorrFilter] = *ECALaserCorrFilterHandle;

    edm::Handle<bool> ManyStripClusHandle;
    GetProduct(ManyStripClusToken_, ManyStripClusHandle, event);
    edm::Handle<bool> TooManyStripClusHandle;
    GetProduct(TooManyStripClusToken_, TooManyStripClusHandle, event);
    edm::Handle<bool> LogErrorTooManyClustersHandle;
    GetProduct(LogErrorTooManyClustersToken_, LogErrorTooManyClustersHandle, event);
    //Odd tracking filter :three filters (with inverted logic)
    pass[kManyStripClusFilter] = !(*ManyStripClusHandle);
    pass[kTooManyStripClusFilter] = !(*TooManyStripClusHandle);
    pass[kLogErrorTooManyClustersFilter] = !(*LogErrorTooManyClustersHandle);

    //Read the beam halo summary
    edm::Handle<reco::BeamHaloSummary> BeamHaloSummaryHandle;
    GetProduct(BeamHaloSummaryToken_ , BeamHaloSummaryHandle, event);
    pass[kCSCTightHaloFilter] = !BeamHaloSummaryHandle->CSCTightHaloId();
    pass[kCSCLooseHaloFilter] = !BeamHaloSummaryHandle->CSCLooseHaloId();
  }
  else {
    // Filters already applied at PAT - retrieve as trigger results
    edm::Handle<edm::TriggerResults> patFilterResultsHandle;
    GetProduct(patFilterResultsToken_, patFilterResultsHandle, event);
    edm::TriggerResults const& patFilterResults = *patFilterResultsHandle;

    auto&& filterNames = event.triggerNames(patFilterResults);

    for (unsigned iF = 0; iF != nEvtSelFilters; ++iF) {
      if (iF == kCSCLooseHaloFilter)
        continue;

      unsigned index = filterNames.triggerIndex(filterLabels_[iF]);
      if (index == filterNames.size())
        throw edm::Exception(edm::errors::Configuration, "FillerEvtSelData::FillDataBlock")
          << "Unknown filter label " << filterLabels_[iF];

      pass[iF] = patFilterResults.accept(index);
    }
    
    pass[kCSCLooseHaloFilter] = pass[kCSCTightHaloFilter];
  }

  int selWord = 0;
  for (unsigned iF = 0; iF != nEvtSelFilters; ++iF) {
    if (pass[iF])
      selWord |= (1 << iF);
  }

  //Store the bit word in the container
  evtSelData_->SetFiltersWord(selWord);
}

DEFINE_MITHEP_TREEFILLER(FillerEvtSelData);
