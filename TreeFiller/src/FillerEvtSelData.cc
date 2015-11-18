#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/Common/interface/TriggerResults.h"

#include "MitProd/TreeFiller/interface/FillerEvtSelData.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataTree/interface/EvtSelData.h"
#include "MitAna/DataTree/interface/RunInfo.h"
#include "MitProd/ObjectService/interface/ObjectService.h"

#include <algorithm>

mithep::FillerEvtSelData::FillerEvtSelData(edm::ParameterSet const& cfg, edm::ConsumesCollector& collector, mithep::ObjectService* os, char const* name/* = "EvtSelData"*/,  bool active/* = true*/) : 
  BaseFiller(cfg, os, "EvtSelData",active),
  mitName_(cfg.getUntrackedParameter<std::string>("mitName", mithep::Names::gkEvtSelDataBrn)),
  filterLabels_(new VString(cfg.getUntrackedParameter<VString>("filterLabels"))),
  patFilterResultsToken_(GetToken<edm::TriggerResults>(collector, cfg, "patFilterResultsName", false)), //TriggerResults::PAT
  resultTokens_(),
  invertedFilters_(),
  evtSelData_(new EvtSelData())
{
  if (patFilterResultsToken_.isUninitialized()) {
    auto&& inputLabels(cfg.getUntrackedParameter<VString>("inputLabels"));
    if (inputLabels.size() != filterLabels_->size())
      throw edm::Exception(edm::errors::Configuration, "FillerEvtSelData::Constructor") << "size of inputLabels does not match the size of filterLabels";

    auto&& invertedFilterLabels(cfg.getUntrackedParameter<VString>("invertedFilters", VString()));

    for (unsigned iF = 0; iF != inputLabels.size(); ++iF) {
      edm::InputTag tag(inputLabels[iF]);
      resultTokens_.push_back(collector.consumes<bool>(tag));

      if (std::find(invertedFilterLabels.begin(), invertedFilterLabels.end(), filterLabels_->at(iF)) != invertedFilterLabels.end())
        invertedFilters_.push_back(iF);
    }
  }
}

mithep::FillerEvtSelData::~FillerEvtSelData()
{
  delete filterLabels_;
  delete evtSelData_;
}

void
mithep::FillerEvtSelData::BookDataBlock(TreeWriter& tws)
{
  tws.AddBranch(mitName_, &evtSelData_);
  OS()->add(evtSelData_, mitName_);

  tws.AddTree("EvtSelBits");
  tws.AddBranchToTree("EvtSelBits", "FilterLabels", TClass::GetClass(typeid(*filterLabels_))->GetName(), &filterLabels_, 32 * 32, 0);
  tws.SetAutoFill("EvtSelBits", false);
  tws.GetTree("EvtSelBits")->Fill();
}

void
mithep::FillerEvtSelData::FillDataBlock(edm::Event const& event, 
                                        edm::EventSetup const&)
{
  int selWord = 0;

  if (patFilterResultsToken_.isUninitialized()) {
    // PAT filters not applied -> Read the MET filters boolean decisions

    for (unsigned iB = 0; iB != resultTokens_.size(); ++iB) {
      edm::Handle<bool> resultHandle;
      GetProduct(resultTokens_[iB], resultHandle, event);

      if (std::find(invertedFilters_.begin(), invertedFilters_.end(), iB) == invertedFilters_.end()) {
        if (*resultHandle)
          selWord |= (1 << iB);
      }
      else {
        if (!(*resultHandle))
          selWord |= (1 << iB);
      }
    }
  }
  else {
    // Filters already applied at PAT - retrieve as trigger results
    edm::Handle<edm::TriggerResults> patFilterResultsHandle;
    GetProduct(patFilterResultsToken_, patFilterResultsHandle, event);
    edm::TriggerResults const& patFilterResults = *patFilterResultsHandle;

    auto&& filterNames = event.triggerNames(patFilterResults);

    unsigned iB = 0;
    for (auto& label : *filterLabels_) {
      unsigned index = filterNames.triggerIndex("Flag_" + label);
      if (index == filterNames.size())
        throw edm::Exception(edm::errors::Configuration, "FillerEvtSelData::FillDataBlock")
          << "Unknown filter label " << label;

      if (patFilterResults.accept(index))
        selWord |= (1 << iB);

      ++iB;
    }
  }

  //Store the bit word in the container
  evtSelData_->SetFiltersWord(selWord);
}

void
mithep::FillerEvtSelData::FillRunBlock(edm::Run const& run, edm::EventSetup const&)
{
  auto* runInfo = OS()->mod<mithep::RunInfo>(mithep::Names::gkRunInfoBrn);
  runInfo->SetEvtSelBitsEntry(0);
}

DEFINE_MITHEP_TREEFILLER(FillerEvtSelData);
