// $Id: BaseFiller.cc,v 1.3 2008/06/19 16:53:43 loizides Exp $

#include "MitProd/TreeFiller/interface/BaseFiller.h"

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
