//--------------------------------------------------------------------------------------------------
// $Id: FillerMetaInfos.h,v 1.4 2008/06/25 07:39:57 loizides Exp $
//
// FillerMetaInfos
//
// Authors: C.Loizides
//--------------------------------------------------------------------------------------------------

#ifndef TREEFILLER_FILLERMETAINFOS_H
#define TREEFILLER_FILLERMETAINFOS_H

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"

namespace mithep 
{
  class RunInfo;
  class EventHeader;
  class LAHeader;

  class FillerMetaInfos : public BaseFiller
  {
    public:
      FillerMetaInfos(const edm::ParameterSet &cfg);
      ~FillerMetaInfos();

      void BookDataBlock(TreeWriter &tws);
      void FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
      void FillRunInfo(const edm::Event &e, const edm::EventSetup &es);
      void FillHltInfo(const edm::Event &e, const edm::EventSetup &es);

    private:
      std::string              evtName_;      //event branch name
      std::string              runName_;      //run info branch name
      std::string              lahName_;      //look-ahead header branch name
      std::string              hltName_;      //
      std::string              l1tName_;      //
      TreeWriter              *tws_;          //tree writer (not owned)
      EventHeader             *eventHeader_;  //event header
      LAHeader                *evtLAHeader_;  //look-ahead event header
      RunInfo                 *runInfo_;      //run info block
      TTree                   *runTree_;      //run info tree (not owned)
      TTree                   *laTree_;       //look ahead tree (not owned)
      Int_t                    runEntries_;   //number of run info entries
      UShort_t                 fileNum_;      //file number of current file
      std::map<UInt_t,Int_t>   runmap_;       //map between run number and entry number
  };
}
#endif
