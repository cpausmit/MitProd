//--------------------------------------------------------------------------------------------------
// $Id: FillerMetaInfos.h,v 1.21 2009/07/13 10:39:35 loizides Exp $
//
// FillerMetaInfos
//
// Implementation of a filler that stores all of the meta information, as well as the L1 and HLT
// trigger info into the corresponding bambu objects.
//
// Authors: C.Loizides
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERMETAINFOS_H
#define MITPROD_TREEFILLER_FILLERMETAINFOS_H

#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"
#include "MitAna/DataTree/interface/TriggerObjectBaseFwd.h"
#include "MitAna/DataTree/interface/TriggerObjectRelFwd.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"

class THashTable;

namespace mithep 
{
  class RunInfo;
  class EventHeader;
  class LAHeader;
  class TriggerMask;

  class FillerMetaInfos : public BaseFiller
  {
    public:
      FillerMetaInfos(const edm::ParameterSet &cfg, const char *name="MetaInfos", bool active=1);
      ~FillerMetaInfos();

      void                           BookDataBlock(TreeWriter &tws);
      void                           FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
      void                           FillRunInfo(const edm::Event &e, const edm::EventSetup &es);
      void                           FillHltInfo(const edm::Event &e, const edm::EventSetup &es);
      void                           FillHltTrig(const edm::Event &e, const edm::EventSetup &es);

    private:
      std::string                    evtName_;      //event branch name
      std::string                    runName_;      //run info branch name
      std::string                    lahName_;      //look-ahead header branch name
      Bool_t                         hltActive_;    //=true if HLT info are filled
      std::vector<std::string>       hltProcNames_; //HLT process name(s)
      std::string                    hltProcName_;  //HLT process name to be used
      std::string                    hltResName_;   //HLT trigger results edm name
      std::string                    hltEvtName_;   //HLT trigger event edm name
      std::string                    hltTableName_; //HLT trigger table branch name
      std::string                    hltLabelName_; //HLT trigger label branch name
      std::string                    hltBitsName_;  //HLT trigger bits branch name
      std::string                    hltObjsName_;  //HLT trigger branch name
      TreeWriter                    *tws_;          //tree writer (not owned)
      EventHeader                   *eventHeader_;  //event header
      LAHeader                      *evtLAHeader_;  //look-ahead event header
      RunInfo                       *runInfo_;      //run info block
      TTree                         *runTree_;      //run info tree (not owned)
      TTree                         *laTree_;       //look ahead tree (not owned)
      Int_t                          runEntries_;   //number of run info entries
      std::map<UInt_t,Int_t>         runmap_;       //map between run number and entry number
      TriggerMask                   *hltBits_;      //HLT trigger bit mask 
      std::vector<std::string>      *hltTable_;     //HLT trigger table
      std::map<std::string,Short_t> *hltTabMap_;    //HLT trigger labels map
      std::vector<std::string>      *hltLabels_;    //HLT module labels
      std::map<std::string,Short_t> *hltLabMap_;    //HLT module labels map
      TriggerObjectBaseArr          *hltObjs_;      //HLT trigger objects
      TriggerObjectRelArr           *hltRels_;      //HLT trigger objects relation
      TTree                         *hltTree_;      //HLT trigger tree (not owned)
      HLTConfigProvider              hltConfig_;    //HLT config from provenance
      Int_t                          hltEntries_;   //number of hlt info entries
      UShort_t                       fileNum_;      //file number of current file

      static bool                    instance_;     //=true when one active instance
  };
}
#endif
