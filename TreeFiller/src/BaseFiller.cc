// $Id: BaseFiller.cc,v 1.4 2008/06/20 17:52:57 loizides Exp $

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
  active_(config_.getUntrackedParameter<bool>("active",active))
{
}

//--------------------------------------------------------------------------------------------------
void BaseFiller::PrintErrorAndExit(const char *msg) const
{
  Error("PrintErrorAndExit", msg);
  gSystem->Exit(1);
}
