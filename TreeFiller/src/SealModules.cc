// #Id:$

#include "FWCore/PluginManager/interface/ModuleDef.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "MitProd/TreeFiller/interface/FillGenParts.h"
#include "MitProd/TreeFiller/interface/FillTracks.h"
#include "MitProd/TreeFiller/interface/FillMuons.h"
#include "MitProd/TreeFiller/interface/FillElectrons.h"

using mithep::FillGenParts;
using mithep::FillTracks;
using mithep::FillMuons;
using mithep::FillElectrons;

DEFINE_SEAL_MODULE();
DEFINE_ANOTHER_FWK_MODULE(FillGenParts);
DEFINE_ANOTHER_FWK_MODULE(FillTracks);
DEFINE_ANOTHER_FWK_MODULE(FillMuons);
DEFINE_ANOTHER_FWK_MODULE(FillElectrons);

