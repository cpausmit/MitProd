// $Id: SealModules.cc,v 1.5 2008/06/18 13:23:23 paus Exp $

#include "FWCore/PluginManager/interface/ModuleDef.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "MitProd/TreeFiller/interface/FillMitTree.h"

using mithep::FillMitTree;

DEFINE_SEAL_MODULE();
DEFINE_ANOTHER_FWK_MODULE(FillMitTree);

