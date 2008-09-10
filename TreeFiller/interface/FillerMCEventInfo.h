//--------------------------------------------------------------------------------------------------
// $Id: FillerMCEventInfo.h,v 1.1 2008/08/29 02:49:43 loizides Exp $
//
// FillerMCEventInfo
//
// Authors: C.Loizides
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERMETAINFOS_H
#define MITPROD_TREEFILLER_FILLERMETAINFOS_H

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
