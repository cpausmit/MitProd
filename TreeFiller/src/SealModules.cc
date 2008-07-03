// $Id: SealModules.cc,v 1.6 2008/07/01 14:38:33 loizides Exp $

#include "FWCore/PluginManager/interface/ModuleDef.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "MitProd/TreeFiller/interface/FillMitTree.h"

using mithep::FillMitTree;

DEFINE_SEAL_MODULE();
DEFINE_ANOTHER_FWK_MODULE(FillMitTree);
