#include "MitProd/TreeFiller/interface/BaseFiller.h"

#include "MitAna/DataTree/interface/BranchName.h"
#include "MitAna/DataTree/interface/BranchTable.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitProd/ObjectService/interface/ObjectService.h"
#include <TSystem.h>
#include <TError.h>

mithep::BaseFiller::BaseFiller(edm::ParameterSet const& cfg, mithep::ObjectService* os, char const* name, bool active) :
  name_(name),
  active_(cfg.getUntrackedParameter<bool>("active", active)),
  verbose_(cfg.getUntrackedParameter<int>("verbose", 0)),
  brtable_(0),
  os_(os)
{
}

void
mithep::BaseFiller::AddBranchDep(std::string const& name, std::string const& dep)
{
  // Add dependency between to given branch names to branch table if present.

  if (name.empty() || dep.empty())
    return;

  if (!brtable_) {
    brtable_ = OS()->mod<BranchTable>(mithep::Names::gkBranchTable);
    if (!brtable_)
      return; 
  }

  if (!brtable_->Find(name.c_str(), dep.c_str()))
    brtable_->Add(new BranchName(name.c_str(), dep.c_str()));
}

void
mithep::BaseFiller::PrintErrorAndExit(std::string const& msg) const
{
  Error("PrintErrorAndExit", msg.c_str());
  gSystem->Exit(1);
}


mithep::BaseFiller*
mithep::FillerFactoryStore::makeFiller(char const* className, edm::ParameterSet const& cfg, edm::ConsumesCollector& collector, ObjectService* os, char const* name, bool active) const
{
  try {
    return fillerFactories_.at(className)(cfg, collector, os, name, active);
  }
  catch (std::out_of_range& ex) {
    return 0;
  }
}

/*static*/
mithep::FillerFactoryStore*
mithep::FillerFactoryStore::singleton()
{
  static FillerFactoryStore fillerFactoryStore;
  return &fillerFactoryStore;
}
