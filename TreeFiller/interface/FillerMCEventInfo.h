//--------------------------------------------------------------------------------------------------
// $Id: FillerMCEventInfo.h,v 1.8 2008/08/18 11:16:11 sixie Exp $
//
// FillerMCEventInfo
//
// Authors: C.Loizides
//--------------------------------------------------------------------------------------------------

#ifndef TREEFILLER_FILLERMETAINFOS_H
#define TREEFILLER_FILLERMETAINFOS_H

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"

namespace mithep 
{
  class MCEventInfo;

  class FillerMCEventInfo : public BaseFiller
  {
    public:
      FillerMCEventInfo(const edm::ParameterSet &cfg, bool active=1);
      ~FillerMCEventInfo();

      void BookDataBlock(TreeWriter &tws);
      void FillDataBlock(const edm::Event &e, const edm::EventSetup &es);

    private:
      std::string              evtName_;      //event branch name
      std::string              datasetName_;  //dataset name
      MCEventInfo             *eventInfo_;    //event info
  };
}
#endif
