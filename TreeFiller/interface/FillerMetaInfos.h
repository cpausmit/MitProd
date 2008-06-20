//--------------------------------------------------------------------------------------------------
// $Id: FillerMetaInfos.h,v 1.1 2008/06/19 16:53:43 loizides Exp $
//
// FillerMetaInfos
//
// Authors: C.Loizides
//--------------------------------------------------------------------------------------------------

#ifndef TREEFILLER_FILLERMETAINFOS_H
#define TREEFILLER_FILLERMETAINFOS_H

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "MitAna/DataTree/interface/EventHeader.h"
#include "MitAna/DataTree/interface/RunInfo.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"

namespace mithep 
{
  class FillerMetaInfos : public BaseFiller
  {
    public:
      FillerMetaInfos(const edm::ParameterSet&);
      ~FillerMetaInfos();

      void BookDataBlock(TreeWriter &tws);
      void FillDataBlock(const edm::Event&, const edm::EventSetup&);
      void FillRunInfo(const edm::Event&, const edm::EventSetup&);
  
    private:
      std::string              evtName_;
      std::string              runName_;
      EventHeader             *eventHeader_;
      RunInfo                 *runInfo_;
      TTree                   *runTree_;
      Int_t                    runEntries_;
      std::map<UInt_t,Int_t>   runmap_;
  };
}
#endif
