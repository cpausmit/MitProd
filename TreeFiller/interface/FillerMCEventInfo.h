//--------------------------------------------------------------------------------------------------
// $Id: FillerMCEventInfo.h,v 1.5 2009/03/20 18:05:40 loizides Exp $
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

      void                     BookDataBlock(TreeWriter &tws);
      void                     FillDataBlock(const edm::Event &e, const edm::EventSetup &es);

    private:
      std::string              evtName_;          //event branch name
      std::string              genHepMCEvName_;   //hepmc branch name (if present)
      std::string              genEvWeightName_;  //edm event weight name
      std::string              genEvScaleName_;   //edm event scale name
      std::string              genEvProcIdName_;  //edm event process id name
      std::string              genPdfInfoName_;   //edm event pdf info name
      MCEventInfo             *eventInfo_;        //event info
  };
}
#endif
