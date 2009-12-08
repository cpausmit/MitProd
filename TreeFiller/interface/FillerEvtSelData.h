//--------------------------------------------------------------------------------------------------
// $Id: FillerEvtSelData.h,v 1.10 2009/09/25 08:42:50 loizides Exp $
//
// FillerEvtSelData
//
// Fill event selection data object.
//
// Authors: C.Loizides
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLEREVTSELDATA_H
#define MITPROD_TREEFILLER_FILLEREVTSELDATA_H

#include "MitProd/TreeFiller/interface/BaseFiller.h"

namespace mithep 
{
  class EvtSelData;

  class FillerEvtSelData : public BaseFiller
  {
    public:
      FillerEvtSelData(const edm::ParameterSet &cfg, 
                       const char *name="EvtSelData", bool active=1);
      ~FillerEvtSelData();

      void                     BookDataBlock(TreeWriter &tws, const edm::EventSetup &es);
      void                     FillDataBlock(const edm::Event &e, const edm::EventSetup &es);

    private:
      std::string              mitName_;       //mit branch name
      std::string              edmName_;       //edm branch name
      EvtSelData              *evtSelData_;    //event selection data object
  };
}
#endif
