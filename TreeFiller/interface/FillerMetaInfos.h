//--------------------------------------------------------------------------------------------------
// $Id: FillerMetaInfos.h,v 1.28 2009/12/02 20:28:15 loizides Exp $
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

#include "MitProd/TreeFiller/interface/HLTConfigProvider.h"
#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerReadoutSetupFwd.h"
#include "MitAna/DataTree/interface/L1TriggerMaskFwd.h"
#include "MitAna/DataTree/interface/TriggerObjectBaseFwd.h"
#include "MitAna/DataTree/interface/TriggerObjectRelFwd.h"
#include "MitAna/DataTree/interface/Types.h"
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

      void                           BookDataBlock(TreeWriter &tws, const edm::EventSetup &es);
      void                           FillDataBlock(const edm::Event &e, const edm::EventSetup &es);

    private:
      void                           FillBitAMask(BitMask128 &bits, const DecisionWord &dw);
      void                           FillBitTMask(BitMask128 &bits, const TechnicalTriggerWord &tw);
      void                           FillHltInfo(const edm::Event &e, const edm::EventSetup &es);
      void                           FillHltTrig(const edm::Event &e, const edm::EventSetup &es);
      void                           FillL1Trig(const edm::Event &e, const edm::EventSetup &es);
      void                           FillRunInfo(const edm::Event &e, const edm::EventSetup &es);
      const char                    *Istr() const;

      std::string                    evtName_;      //event branch name (must be unique)
      std::string                    runTreeName_;  //run info tree name (must be unique)
      std::string                    lahTreeName_;  //look-ahead header tree name (must be unique)
      std::string                    hltTreeName_;  //hlt tree name (must be unique)
      Bool_t                         hltActive_;    //=true if HLT info are filled
      std::string                    hltProcName_;  //HLT process name to be used
      std::string                    hltResName_;   //HLT trigger results edm name
      std::string                    hltEvtName_;   //HLT trigger event edm name
      std::string                    hltTableName_; //HLT trigger table branch name
      std::string                    hltLabelName_; //HLT trigger label branch name
      std::string                    hltBitsName_;  //HLT trigger bits branch name
      std::string                    hltObjsName_;  //HLT trigger branch name
      Bool_t                         l1Active_;     //=true if some L1 info are filled
      std::string                    l1GTRecName_;  //L1 global trigger record edm name
      std::string                    l1GTRRName_;   //l1 global readout trigger record edm name
      std::string                    l1TBitsName_;  //L1 technical bit name
      std::string                    l1ABitsName_;  //L1 algo bit name
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
      mitedm::HLTConfigProvider      hltConfig_;    //HLT config from provenance
      Int_t                          hltEntries_;   //number of hlt info entries
      UShort_t                       fileNum_;      //file number of current file
      L1TriggerMask                 *l1TBits_;      //L1 technical trigger bit mask 
      L1TriggerMask                 *l1ABits_;      //L1 algorithm trigger bit mask 
      L1TriggerMask                 *l1TBits2_;     //L1 technical trigger bit mask before masking
      L1TriggerMask                 *l1ABits2_;     //L1 algorithm trigger bit mask before masking
      L1TriggerMaskArr              *l1AbArr_;      //L1 algorithm trigger bits
      L1TriggerMaskArr              *l1TbArr_;      //L1 technical trigger bits

      static Int_t                   instance_;     //counts active instances
  };
}
#endif
