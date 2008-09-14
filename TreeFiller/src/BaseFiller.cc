// $Id: BaseFiller.cc,v 1.5 2008/07/08 12:38:20 loizides Exp $

#include "MitProd/TreeFiller/interface/BaseFiller.h"
#include <TSystem.h>
#include <TError.h>

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
BaseFiller::BaseFiller(const ParameterSet &cfg, const char *name, bool active) :
  name_(name),
  config_(cfg.exists(name) ? cfg.getUntrackedParameter<ParameterSet>(name) : ParameterSet()),
  active_(config_.getUntrackedParameter<bool>("active",active)),
  verify_(config_.getUntrackedParameter<bool>("verify",false)),
  verbose_(config_.getUntrackedParameter<int>("verbose",0))
{
}

//--------------------------------------------------------------------------------------------------
void BaseFiller::PrintErrorAndExit(const char *msg) const
{
  Error("PrintErrorAndExit", msg);
  gSystem->Exit(1);
}
