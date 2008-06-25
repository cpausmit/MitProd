//--------------------------------------------------------------------------------------------------
// $Id: FillerMetaInfos.h,v 1.3 2008/06/24 14:24:55 loizides Exp $
//
// FillerMetaInfos
//
// Authors: C.Loizides
//--------------------------------------------------------------------------------------------------

#ifndef TREEFILLER_FILLERMETAINFOS_H
#define TREEFILLER_FILLERMETAINFOS_H

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "MitAna/DataTree/interface/EventHeader.h"
#include "MitAna/DataTree/interface/LAHeader.h"
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
      TreeWriter              *tws_;          //tree writer (not owned)
      std::string              evtName_;      //event branch name
      std::string              runName_;      //run info branch name
      std::string              lahName_;      //look-ahead header branch name
      EventHeader             *eventHeader_;  //event header
      LAHeader                *evtLAHeader_;  //look-ahead event header
      RunInfo                 *runInfo_;      //run info block
      TTree                   *runTree_;      //run info tree
      TTree                   *laTree_;       //look ahead tree
      Int_t                    runEntries_;   //number of run info entries
      UShort_t                 fileNum_;      //file number of current file
      std::map<UInt_t,Int_t>   runmap_;       //map between run number and entry number
  };
}
#endif
