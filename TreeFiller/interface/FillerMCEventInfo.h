//--------------------------------------------------------------------------------------------------
// $Id: FillerMCEventInfo.h,v 1.2 2008/09/10 03:30:23 loizides Exp $
//
// FillerMCEventInfo
//
// Authors: C.Loizides
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERMCEVENTINFO_H
#define MITPROD_TREEFILLER_FILLERMCEVENTINFO_H

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"

namespace mithep 
{
  class MCEventInfo;

  class FillerMCEventInfo : public BaseFiller
  {
    public:
      FillerMCEventInfo(const edm::ParameterSet &cfg, 
                        const char *name="MCEventInfo", bool active=1);
      ~FillerMCEventInfo();

      void BookDataBlock(TreeWriter &tws);
      void FillDataBlock(const edm::Event &e, const edm::EventSetup &es);

    private:
      std::string              evtName_;          //event branch name
      std::string              genEvWeightName_;  //edm event weight name
      std::string              genEvScaleName_;   //edm event scale name
      std::string              genEvProcIdName_;  //edm event process id name
      std::string              genPdfInfoName_;   //edm event pdf info name
      MCEventInfo             *eventInfo_;        //event info
  };
}
#endif
