// $Id: SealModules.cc,v 1.4 2008/06/11 23:37:20 paus Exp $

#include "FWCore/PluginManager/interface/ModuleDef.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "MitProd/TreeFiller/interface/FillMitTree.h"
// This below will disappear
#include "MitProd/TreeFiller/interface/FillGenParts.h"
#include "MitProd/TreeFiller/interface/FillTracks.h"
#include "MitProd/TreeFiller/interface/FillMuons.h"
#include "MitProd/TreeFiller/interface/FillElectrons.h"
#include "MitProd/TreeFiller/interface/FillGlobalMuons.h"

using mithep::FillMitTree;
// This below will disappear
using mithep::FillGenParts;
using mithep::FillTracks;
using mithep::FillMuons;
using mithep::FillElectrons;
using mithep::FillGlobalMuons;

DEFINE_SEAL_MODULE();
DEFINE_ANOTHER_FWK_MODULE(FillMitTree);
// This below will disappear
DEFINE_ANOTHER_FWK_MODULE(FillGenParts);
DEFINE_ANOTHER_FWK_MODULE(FillTracks);
DEFINE_ANOTHER_FWK_MODULE(FillMuons);
DEFINE_ANOTHER_FWK_MODULE(FillElectrons);
DEFINE_ANOTHER_FWK_MODULE(FillGlobalMuons);
