// $Id: BaseFiller.cc,v 1.1 2008/06/18 13:23:22 paus Exp $

#include "MitProd/TreeFiller/interface/BaseFiller.h"

using namespace std;
using namespace edm;
using namespace mithep;

//-------------------------------------------------------------------------------------------------
BaseFiller::BaseFiller(const ParameterSet &cfg, const char *en, const char *mn) :
  config_(cfg),
  edmName_(cfg.getUntrackedParameter<string>("edmName",en)),
  mitName_(cfg.getUntrackedParameter<string>("mitName",mn)),
  active_ (cfg.getUntrackedParameter<bool>  ("active",true))
{
}
