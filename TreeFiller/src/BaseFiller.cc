// $Id: BaseFiller.cc,v 1.2 2008/06/18 19:17:21 loizides Exp $

#include "MitProd/TreeFiller/interface/BaseFiller.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
BaseFiller::BaseFiller(const ParameterSet &cfg, const char *name) :
  name_(name),
  config_(cfg.exists(name) ? cfg.getUntrackedParameter<ParameterSet>(name) : ParameterSet()),
  active_(config_.getUntrackedParameter<bool>("active",true))
{
}
