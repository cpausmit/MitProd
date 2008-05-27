// #Id:$

#include "FWCore/PluginManager/interface/ModuleDef.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "MitProd/TreeFiller/interface/FillGenParts.h"

using mithep::FillGenParts;

DEFINE_SEAL_MODULE();
DEFINE_ANOTHER_FWK_MODULE(FillGenParts);
