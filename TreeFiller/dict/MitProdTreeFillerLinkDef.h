#ifndef MITPROD_TREEFILLER_LINKDEF_H
#define MITPROD_TREEFILLER_LINKDEF_H

#include "MitProd/TreeFiller/interface/FillerMCEventInfo.h"
#include "MitAna/DataTree/interface/MCRunInfo.h"

#include "TString.h"

#include <map>

#ifdef __CLING__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclass;
#pragma link C++ nestedtypedef;
#pragma link C++ namespace mithep;

#pragma link C++ class std::map<TString, unsigned>;
#pragma link C++ class mithep::FillerMCEventInfo::WeightGroupHandler;
#endif

#endif
