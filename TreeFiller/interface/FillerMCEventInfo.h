//--------------------------------------------------------------------------------------------------
// $Id: FillerMCEventInfo.h,v 1.8 2009/07/14 16:03:22 sixie Exp $
//
// FillerMCEventInfo
//
// Implementation of a filler that stores MC related quantities, like the pdf info, in the
// bambu records.
//
// Authors: C.Loizides
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERMCEVENTINFO_H
#define MITPROD_TREEFILLER_FILLERMCEVENTINFO_H

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

      void                     BookDataBlock(TreeWriter &tws);
      void                     FillDataBlock(const edm::Event &e, const edm::EventSetup &es);

    private:
      std::string              evtName_;              //mit event branch name
      std::string              genHepMCEvName_;       //hepmc branch name (if present)
      std::string              genEvtInfoName_;       //edm event info name 
      bool                     flavorHistoryActive_;  //=true if flavor history is filled
      std::string              flavorHistName_;       //edm flavor history name
      MCEventInfo             *eventInfo_;            //event info
  };
}
#endif
