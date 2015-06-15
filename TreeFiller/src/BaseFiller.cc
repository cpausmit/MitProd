#include "FWCore/Framework/interface/ConsumesCollector.h"

#include "MitProd/TreeFiller/interface/BaseFiller.h"
#include "MitAna/DataTree/interface/BranchName.h"
#include "MitAna/DataTree/interface/BranchTable.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitProd/TreeFiller/interface/FillMitTree.h"
#include "MitProd/ObjectService/interface/ObjectService.h"
#include <TSystem.h>
#include <TError.h>

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
BaseFiller::BaseFiller(const ParameterSet &cfg, ObjectService* os, const char *name, bool active) :
  name_(name),
  brtname_(cfg.getUntrackedParameter<string>("brTabName",Names::gkBranchTable)),
  config_(cfg.exists(name) ? cfg.getUntrackedParameter<ParameterSet>(name) : ParameterSet()),
  active_(config_.getUntrackedParameter<bool>("active",active)),
  verify_(config_.getUntrackedParameter<bool>("verify",false)),
  verbose_(config_.getUntrackedParameter<int>("verbose",0)),
  brtable_(0),
  os_(os)
{
  // Constructor.
}

//--------------------------------------------------------------------------------------------------
void BaseFiller::AddBranchDep(const char *n, const char *d)
{
  // Add dependency between to given branch names to branch table if present.

  if (!n || !d)
    return;

  if (!brtable_) {
    brtable_ = OS()->mod<BranchTable>(brtname_.c_str());
    if (!brtable_)
      return; 
  }

  std::string nstr(n);
  if (nstr.empty())
    return;

  std::string dstr(d);
  if (dstr.empty())
    return;

  if (!brtable_->Find(n,d))
    brtable_->Add(new BranchName(n,d));
}

//--------------------------------------------------------------------------------------------------
void BaseFiller::PrintErrorAndExit(const char *msg) const
{
  // Print error message, and then terminate.

  Error("PrintErrorAndExit", msg);
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
FillerFactoryStore::singleton()
{
  static FillerFactoryStore fillerFactoryStore;
  return &fillerFactoryStore;
}
