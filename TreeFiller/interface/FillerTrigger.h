//--------------------------------------------------------------------------------------------------
// FillerTrigger
//
// Implementation of a filler that stores L1 and HLT trigger info into the corresponding bambu
// objects.
// Produces a TTree (HLT) with the following branches:
//  . HLTMenus (std::string) -> HLT menu name
//  . HLTTriggerTable (VString) -> Names of HLT paths
//  . HLTLabels (VString) -> Names of HLT modules + names of l1extraParticles tags + names of the
//                           HLTFilters + names of the L1 algo bits + names of the L1 tech bits.
//                           Tags are saved for l1extraParticles (probably) because a single module
//                           produces different object types. Tag information is used in
//                           mithep::TriggerObject (see HLTFwkMod)
//
// Author: Y.Iiyama
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERHLT_H
#define MITPROD_TREEFILLER_FILLERHLT_H

#include "MitAna/DataTree/interface/TriggerObjectBaseFwd.h"
#include "MitAna/DataTree/interface/TriggerObjectRelFwd.h"
#include "MitAna/DataTree/interface/L1TriggerMaskFwd.h"
#include "MitAna/DataTree/interface/Types.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"

#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"
#include "L1Trigger/GlobalTriggerAnalyzer/interface/L1GtUtils.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerReadoutRecord.h"
#include "DataFormats/PatCandidates/interface/TriggerObjectStandAlone.h"
#include "DataFormats/Common/interface/TriggerResults.h"

class THashTable;

namespace mithep 
{
  class TriggerMask;

  class FillerTrigger : public BaseFiller
  {
  public:
    FillerTrigger(edm::ParameterSet const&, edm::ConsumesCollector&, ObjectService*, char const* = "HLT", bool active = true);
    ~FillerTrigger();

    void BookDataBlock(TreeWriter&) override;
    void FillDataBlock(edm::Event const&, edm::EventSetup const&) override;
    void FillRunBlock(edm::Run const&, edm::EventSetup const&) override;

  private:
    typedef std::vector<std::string> VString;
    typedef std::map<std::string, Int_t> IndexMap;

    void FillTriggerConfig(edm::Run const&, edm::EventSetup const&);
    void FillHlt(edm::Event const&);
    void FillL1t(edm::Event const&);

    edm::EDGetTokenT<trigger::TriggerEvent> hltEvtToken_;   //HLT trigger event edm name
    edm::EDGetTokenT<pat::TriggerObjectStandAloneCollection> triggerObjectsToken_;
    edm::EDGetTokenT<edm::TriggerResults> hltResToken_;   //HLT trigger results edm name
    edm::InputTag l1GtMenuLiteTag_;
    edm::EDGetTokenT<L1GlobalTriggerReadoutRecord> l1GTRRToken_; //l1 global readout trigger record edm name

    std::string                    hltProcName_;  //HLT process name to be used
    std::string                    evtName_;      //event branch name (must be unique)
    std::string                    runTreeName_;  //run info tree name (must be unique)
    std::string                    lahTreeName_;  //look-ahead header tree name (must be unique)
    std::string                    hltTreeName_;  //hlt tree name (must be unique)
    std::string                    hltTableName_; //HLT trigger table branch name
    std::string                    hltLabelName_; //HLT trigger label branch name
    std::string                    hltMenuName_;  //HLT trigger menu branch name
    std::string                    hltBitsName_;  //HLT trigger bits branch name
    std::string                    hltObjsName_;  //HLT trigger branch name
    TreeWriter                    *tws_;          //tree writer (not owned)
    Int_t                          hltEntry_;     //current hlt entry
    IndexMap                       hltMenuMap_;   //map btw hlt table name and entry number
    TriggerMask                   *hltBits_;      //HLT trigger bit mask 
    std::string                   *hltMenu_;      //HLT menu name
    VString                       *hltTable_;     //HLT trigger table
    VString                       *hltLabels_;    //HLT module labels
    IndexMap                       hltLabelMap_;  //HLT module labels map
    TriggerObjectBaseArr          *hltObjs_;      //HLT trigger objects
    TriggerObjectRelArr           *hltRels_;      //HLT trigger objects relation
    TTree                         *hltTree_;      //HLT trigger tree (not owned)
    HLTConfigProvider              hltConfig_;    //HLT config from provenance
    Bool_t                         l1Active_;     //Flag for L1 processing
    L1GtUtils                      l1GtUtils_;    //L1 utility
    std::string                    l1TBitsName_;  //L1 technical bit name
    std::string                    l1ABitsName_;  //L1 algo bit name
    L1TriggerMask                 *l1TBits_;      //L1 technical trigger bit mask 
    L1TriggerMask                 *l1ABits_;      //L1 algorithm trigger bit mask 
    L1TriggerMask                 *l1TBits2_;     //L1 technical trigger bit mask before masking
    L1TriggerMask                 *l1ABits2_;     //L1 algorithm trigger bit mask before masking
    L1TriggerMaskArr              *l1AbArr_;      //L1 algorithm trigger bits
    L1TriggerMaskArr              *l1TbArr_;      //L1 technical trigger bits
    UShort_t                       fileNum_;      //file number of current file
  };
}
#endif
