#include "MitProd/TreeFiller/interface/FillerTrigger.h"

#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/Common/interface/Handle.h"

#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataTree/interface/TriggerMask.h"
#include "MitAna/DataTree/interface/TriggerObjectBase.h"
#include "MitAna/DataTree/interface/TriggerObjectBaseCol.h"
#include "MitAna/DataTree/interface/TriggerObjectRelCol.h"
#include "MitAna/DataTree/interface/L1TriggerMaskCol.h"
#include "MitAna/DataTree/interface/RunInfo.h"
#include "MitProd/ObjectService/interface/ObjectService.h"

mithep::FillerTrigger::FillerTrigger(edm::ParameterSet const& cfg, edm::ConsumesCollector& collector, ObjectService* os, char const* name/* = "HLT"*/, bool active/* = true*/) : 
  BaseFiller(cfg, os, name, active),
  hltEvtToken_(), // set below
  triggerObjectsToken_(GetToken<pat::TriggerObjectStandAloneCollection>(collector, "hltObjsEdmName", "")),
  hltResToken_(), // set below
  l1GtMenuLiteTag_(Conf().getUntrackedParameter<string>("l1GtMenuLiteEdmName", "l1GtTriggerMenuLite")),
  l1GTRRToken_(GetToken<L1GlobalTriggerReadoutRecord>(collector, "l1GtReadRecEdmName", "")),
  hltProcName_(Conf().getUntrackedParameter<std::string>("hltProcName", "")),
  hltTreeName_(Conf().getUntrackedParameter<std::string>("hltTreeMitName", mithep::Names::gkHltTreeName)),
  hltTableName_(Conf().getUntrackedParameter<std::string>("hltTableMitName", mithep::Names::gkHltTableBrn)),
  hltLabelName_(Conf().getUntrackedParameter<std::string>("hltLabelMitName", mithep::Names::gkHltLabelBrn)),
  hltMenuName_(Conf().getUntrackedParameter<std::string>("hltMenuMitName", mithep::Names::gkHltMenuBrn)),
  hltBitsName_(Conf().getUntrackedParameter<std::string>("hltBitsMitName", mithep::Names::gkHltBitBrn)),
  hltObjsName_(Conf().getUntrackedParameter<std::string>("hltObjsMitName", mithep::Names::gkHltObjBrn)),
  tws_(0),
  hltEntry_(-1),
  hltMenuMap_(),
  hltBits_(new mithep::TriggerMask),
  hltMenu_(new std::string),
  hltTable_(new VString),
  hltLabels_(new VString),
  hltLabelMap_(),
  hltObjs_(new mithep::TriggerObjectBaseArr),
  hltRels_(new mithep::TriggerObjectRelArr),
  hltTree_(0),
  hltConfig_(),
  l1Active_(Conf().getUntrackedParameter<bool>("l1Active", true)),
  l1GtUtils_(),
  l1TBitsName_(Conf().getUntrackedParameter<string>("l1TechBitsMitName", mithep::Names::gkL1TechBitsBrn)),
  l1ABitsName_(Conf().getUntrackedParameter<string>("l1AlgoBitsMitName", mithep::Names::gkL1AlgoBitsBrn)),
  l1TBits_(l1Active_ ? new mithep::L1TriggerMask : 0),
  l1ABits_(l1Active_ ? new mithep::L1TriggerMask : 0),
  l1TBits2_(l1Active_ ? new mithep::L1TriggerMask : 0),
  l1ABits2_(l1Active_ ? new mithep::L1TriggerMask : 0),
  l1AbArr_(l1Active_ ? new mithep::L1TriggerMaskArr : 0),
  l1TbArr_(l1Active_ ? new mithep::L1TriggerMaskArr : 0),
  fileNum_(0)
{
  // Constructor.
  std::string hltResName(Conf().getUntrackedParameter<std::string>("hltResEdmName", "TriggerResults"));  
     
  //force a particular process name for trigger output if required
  //otherwise the trigger results last stored will be used (behavior dictated in the CMSSW framework)
  if (!hltProcName_.empty()) {
    auto firstColon(hltResName.find(':'));
    if(firstColon == std::string::npos)
      hltResName += "::";
    else{
      auto secondColon(hltResName.find(':', firstColon + 1));
      if(secondColon != std::string::npos)
        hltResName.erase(secondColon);
      hltResName += ":";
    }
    hltResName += hltProcName_;
  }

  hltResToken_ = collector.consumes<edm::TriggerResults>(edm::InputTag(hltResName));

  std::string hltEvtName(Conf().getUntrackedParameter<std::string>("hltEvtEdmName", "hltTriggerSummaryAOD"));
  
  if (!hltEvtName.empty()) {
    if (!triggerObjectsToken_.isUninitialized())
    throw edm::Exception(edm::errors::Configuration, "FillerTrigger::Constructor")
      << "Cannot configure TriggerEvent and TriggerObjectCollection at the same time";

    if (!hltProcName_.empty()) {
      auto firstColon = hltEvtName.find(':');
      if(firstColon == std::string::npos)
        hltEvtName += "::";
      else{
        auto secondColon(hltEvtName.find(':', firstColon + 1));
        if(secondColon != std::string::npos)
          hltEvtName.erase(secondColon);
        hltEvtName += ":";
      }
      hltEvtName += hltProcName_;
    }
  
    hltEvtToken_ = collector.consumes<trigger::TriggerEvent>(edm::InputTag(hltEvtName));
  }
}

mithep::FillerTrigger::~FillerTrigger()
{
  // Destructor.

  delete hltBits_;
  delete hltTable_;
  delete hltLabels_;
  delete hltObjs_;
  delete hltRels_;
  delete l1TBits_;
  delete l1ABits_;
  delete l1AbArr_;
  delete l1TbArr_;
}

void
mithep::FillerTrigger::BookDataBlock(TreeWriter& tws)
{
  // add HLT branches to main tree and to HLT trigger info tree
  tws.AddBranch(hltBitsName_, &hltBits_);
  if (!hltObjsName_.empty()) {
    tws.AddBranch(hltObjsName_, &hltObjs_);
    tws.AddBranch(hltObjsName_ + "Relation", &hltRels_);
  }

  tws.AddTree(hltTreeName_);
  tws.AddBranchToTree(hltTreeName_, hltTableName_,
                      TClass::GetClass(typeid(*hltTable_))->GetName(), &hltTable_, 32 * 1024, 0);
  tws.AddBranchToTree(hltTreeName_,hltLabelName_,
                      TClass::GetClass(typeid(*hltLabels_))->GetName(), &hltLabels_, 32 * 1024, 0);
  tws.AddBranchToTree(hltTreeName_,hltMenuName_,
                      TClass::GetClass(typeid(*hltMenu_))->GetName(), &hltMenu_, 32 * 1024, 0);
  tws.SetAutoFill(hltTreeName_, false);
  hltTree_ = tws.GetTree(hltTreeName_);

  if (l1Active_) {
    tws.AddBranch(l1TBitsName_, &l1TBits_);
    tws.AddBranch(l1ABitsName_, &l1ABits_);
    tws.AddBranch(l1TBitsName_ + "BeforeMask", &l1TBits2_);
    tws.AddBranch(l1ABitsName_ + "BeforeMask", &l1ABits2_);
    if (!l1GTRRToken_.isUninitialized()) {
      tws.AddBranch(l1TBitsName_ + "Bxs", &l1TbArr_);
      tws.AddBranch(l1ABitsName_ + "Bxs", &l1AbArr_);
    }
  }

  // store pointer to tree writer 
  tws_ = &tws;
}

void
mithep::FillerTrigger::FillDataBlock(edm::Event const& event, edm::EventSetup const&)
{
  // Fill our data structures.

  // clear map if a new file was opened
  if (tws_->GetFileNumber() != fileNum_) {
    fileNum_ = tws_->GetFileNumber();

    hltMenuMap_.clear();

    if (hltEntry_ != -1) {
      hltEntry_ = 0;
      hltTree_->Fill();
      hltMenuMap_[*hltMenu_] = 0;
    }

    auto runInfo = OS()->mod<mithep::RunInfo>(mithep::Names::gkRunInfoBrn);
    if (runInfo->HltEntry() != -1)
      runInfo->SetHltEntry(0);
  }

  FillHlt(event);
  if (l1Active_)
    FillL1t(event);
}

void
mithep::FillerTrigger::FillRunBlock(edm::Run const& run, edm::EventSetup const& setup)
{
  if (l1Active_) {
    l1GtUtils_.retrieveL1GtTriggerMenuLite(run, l1GtMenuLiteTag_);
    if (l1GtUtils_.l1TriggerMenu().empty())
      throw cms::Exception("ProductNotFound") << "L1GtTriggerMenuLite not found in Run";
  }

  // set table, filter names, and menu name (either from EDM or cache), set hltEntry_ to entry number (can be new)
  FillTriggerConfig(run, setup);

  auto runInfo = OS()->mod<mithep::RunInfo>(mithep::Names::gkRunInfoBrn);
  runInfo->SetHltEntry(hltEntry_);
}  

void
mithep::FillerTrigger::FillTriggerConfig(edm::Run const& run, edm::EventSetup const& setup)
{
  // Fill HLT trigger table if it changed.
  bool hltConfigChanged = false;
  if (!hltConfig_.init(run, setup, hltProcName_, hltConfigChanged)) {
    edm::LogError("FillerTrigger") << "Cannot access hlt config using PSet from trigger results";

    throw edm::Exception(edm::errors::Configuration, "FillerTrigger::FillHltInfo()\n")
      << "Cannot access hlt config using PSet from trigger results";
  }

  std::string menuName;
  if (!hltConfig_.tableName().empty())
    menuName = hltConfig_.tableName();
  else
    menuName = "unknown_hlt_config";

  auto nameEntry = hltMenuMap_.find(menuName);

  if (!hltConfigChanged) {
    if (nameEntry != hltMenuMap_.end() && nameEntry->second == hltEntry_) {
      // really nothing changed
      return;
    }
    else
      throw edm::Exception(edm::errors::Configuration, "FillerTrigger::FillHltInfo()\n")
        << "HLT configuration did not change in the new run but HLT info is not present";
  }

  // HLT configuration changed. Now what?

  if (nameEntry != hltMenuMap_.end()) {
    // This menu is already in hltTree
    hltEntry_ = nameEntry->second;
    hltTree_->GetEntry(hltEntry_);

    hltLabelMap_.clear();
    for (std::string& label : *hltLabels_) {
      unsigned size = hltLabelMap_.size();
      hltLabelMap_[label] = size;
    }

    return;
  }

  // check size of menu... < 1024
  if (hltConfig_.size() > hltBits_->MaxSize()) {
    edm::LogError("FillerTrigger") << "HLT config contains too many paths "
                               << hltConfig_.size() << " > " 
                               << hltBits_->Size();
    return;
  }

  delete hltTable_;
  hltTable_ = new VString;
  delete hltLabels_;
  hltLabels_ = new VString;
  delete hltMenu_;
  hltMenu_ = new std::string(menuName);

  hltEntry_ = hltTree_->GetEntries();
  hltMenuMap_[*hltMenu_] = hltEntry_;

  hltLabelMap_.clear();

  // TODO need a better implementation to store trigger object collection tags.
  // hltL1extraParticles is the only module that produces more than one collections.
  // Currently dumping tags into list of modules. Perhaps make a separate tree?

  // Muon and HFRing tags do not exist in L1Trigger/L1ExtraFromDigis/src/L1ExtraParticlesProd.cc

  std::string hltL1extraParticlesTag[] = {
    "hltL1extraParticles:Isolated:HLT",
    "hltL1extraParticles:NonIsolated:HLT",
    "hltL1extraParticles:Central:HLT",
    "hltL1extraParticles:Forward:HLT",
    "hltL1extraParticles:Tau:HLT",
    "hltL1extraParticles:IsoTau:HLT",
    "hltL1extraParticles:Muon:HLT",
    "hltL1extraParticles:MET:HLT",
    "hltL1extraParticles:MHT:HLT",
    "hltL1extraParticles:HFRing:HLT"
  };
  for (std::string& tag : hltL1extraParticlesTag) {
    hltLabelMap_[tag] = hltLabels_->size();
    hltLabels_->push_back(tag);
  }

  // loop over hlt paths
  for (unsigned iPath = 0; iPath != hltConfig_.size(); ++iPath) {
    hltTable_->push_back(hltConfig_.triggerName(iPath));

    VString const& mLabels(hltConfig_.moduleLabels(iPath));
    for (std::string const& modLabel : mLabels) {
      // remove characters which do not work: negation is included as !ModuleName
      // (this might require a change in the underlying CMSSW HLT fwk, so watch out).
      char const* lptr = &modLabel[0];
      if (*lptr == '!' || *lptr == '-')
        ++lptr;

      auto riter = hltLabelMap_.find(lptr);
      if (riter == hltLabelMap_.end()) {
        hltLabelMap_[lptr] = hltLabels_->size();
        hltLabels_->push_back(lptr);
      }

      std::string const& type(hltConfig_.moduleType(lptr));
      riter = hltLabelMap_.find(type);
      if (riter == hltLabelMap_.end()) {
        hltLabelMap_[type] = hltLabels_->size();
        hltLabels_->push_back(type);
      }
    }
  }

  if (l1Active_) {
    std::string triggerAlias;
    std::string triggerName;

    // get L1 algo names
    hltLabels_->push_back("xxx-L1AlgoNames-xxx");

    for (unsigned iT = 0; iT != 128; ++iT) {
      if (l1GtUtils_.l1TriggerNameFromBit(iT, L1GtUtils::AlgorithmTrigger, triggerAlias, triggerName))
        hltLabels_->push_back(triggerName);
      else
        hltLabels_->push_back(Form("UnusedL1Algo%d", iT));
    }

    // get L1 tech names
    hltLabels_->push_back("xxx-L1TechNames-xxx");

    for (unsigned iT = 0; iT != 64; ++iT) {
      if (l1GtUtils_.l1TriggerNameFromBit(iT, L1GtUtils::TechnicalTrigger, triggerAlias, triggerName))
        hltLabels_->push_back(triggerName);
      else
        hltLabels_->push_back(Form("UnusedL1Tech%d", iT));
    }
  }

  hltTree_->Fill();
}
 
void
mithep::FillerTrigger::FillHlt(edm::Event const& event)
{
  // Fill HLT trigger objects along triggered paths.

  // get HLT trigger information
  edm::Handle<edm::TriggerResults> hTriggerResultsHLT;
  GetProduct(hltResToken_, hTriggerResultsHLT, event);
  auto& triggerResultsHLT = *hTriggerResultsHLT;

  if (verify_)
    assert(triggerResultsHLT.size() == hltConfig_.size());

  // new bitmask
  mithep::BitMask1024 maskhlt;

  // loop over trigger paths
  for (unsigned iPath = 0; iPath != hltConfig_.size(); ++iPath) {
    if (verify_) 
      assert(iPath == hltConfig_.triggerIndex(hltConfig_.triggerName(iPath)));

    if (triggerResultsHLT.accept(iPath))
      maskhlt.SetBit(iPath);
  }

  hltBits_->SetBits(maskhlt);

  if (!hltObjsName_.empty()) {
    // reset trigger objects
    hltObjs_->Reset();
    hltRels_->Reset();

    if (!hltEvtToken_.isUninitialized()) {
      // fill HLT trigger object information from TriggerEvent (case for AOD)
      
      edm::Handle<trigger::TriggerEvent> triggerEventHandle;
      GetProduct(hltEvtToken_, triggerEventHandle, event);
      auto& triggerEvent = *triggerEventHandle;

      // loop over trigger objects and fill them
      trigger::TriggerObjectCollection const& toc = triggerEvent.getObjects();
      VString const& tags = triggerEvent.collectionTags();

      unsigned iL1Extra = 0; // L1 extra has two "blank" tags - identify them with Muon and HFRing

      unsigned iObj = 0;
      for (unsigned iTag = 0; iTag != tags.size(); ++iTag) {
        short tnind = -(iTag + 1);
        std::string const& tag = tags[iTag];

        if (tag.find("hltL1extraParticles") == 0) {
          auto ritr = hltLabelMap_.find(tag);
          if (ritr == hltLabelMap_.end() && tag == "hltL1extraParticles::HLT") {
            if (iL1Extra == 0)
              ritr = hltLabelMap_.find("hltL1extraParticles:Muon:HLT");
            else if (iL1Extra == 1)
              ritr = hltLabelMap_.find("hltL1extraParticles:HFRing:HLT");
            ++iL1Extra;
          }
          if (ritr != hltLabelMap_.end())
            tnind = ritr->second;
        }

        unsigned iNext = triggerEvent.collectionKey(iTag);
        for (; iObj != iNext; ++iObj) {
          auto& tobj = toc[iObj];

          TriggerObjectBase* trigObj = hltObjs_->Allocate();
          new (trigObj) TriggerObjectBase(tobj.id(), 0, tobj.pt(), tobj.eta(), tobj.phi(), tobj.mass());
          trigObj->SetTagInd(tnind);
        }
      }

      // loop over trigger paths
      for (unsigned iPath = 0; iPath != hltConfig_.size(); ++iPath) {
        // modules on this trigger path
        for (std::string const& mLabel : hltConfig_.moduleLabels(iPath)) {
          // check whether the module is packed up in TriggerEvent product
          unsigned filterIndex = triggerEvent.filterIndex(edm::InputTag(mLabel, "", hltProcName_));
          if (filterIndex >= triggerEvent.sizeFilters()) 
            continue;

          std::string mType = hltConfig_.moduleType(mLabel);

          std::string tType = "L1";
          if (mType != "HLTLevel1GTSeed") 
            tType = "L3";

          // find index for module label/type name
          auto riter = hltLabelMap_.find(mLabel);
          assert(riter != hltLabelMap_.end());

          short modIndex = riter->second;

          riter = hltLabelMap_.find(mType);
          assert(riter != hltLabelMap_.end());

          short typeIndex = riter->second;

          // find trigger objects
          trigger::Vids const& vids = triggerEvent.filterIds(filterIndex);
          trigger::Keys const& keys = triggerEvent.filterKeys(filterIndex);

          // loop over trigger objects
          for (unsigned iVid = 0; iVid < vids.size(); ++iVid) {
            auto vid = vids[iVid];
            auto iObj = keys[iVid];

            TriggerObjectRel* trigRel = hltRels_->Allocate();
            new (trigRel) TriggerObjectRel(iPath, vid, iObj, modIndex, typeIndex);
            TriggerObjectBase* trigObj = hltObjs_->At(iObj);

            if (trigObj->Type() == 0)
              trigObj->SetType(vid);
          }
        }
      }
    }
    else {
      // fill HLT trigger object information from TriggerObjectStandAloneCollection (case for MiniAOD)

      edm::Handle<pat::TriggerObjectStandAloneCollection> triggerObjectsHandle;
      GetProduct(triggerObjectsToken_, triggerObjectsHandle, event);
      auto& triggerObjects = *triggerObjectsHandle;

      auto& triggerNames = event.triggerNames(triggerResultsHLT);

      unsigned iL1Extra = 0;
      unsigned iObj = 0;

      for (auto tobj : triggerObjects) { // need to copy by value to call unpackPathNames
        short tnind = -1;

        tobj.unpackPathNames(triggerNames);
        
        std::string const& collectionTag = tobj.collection();
        if (collectionTag.find("hltL1extraParticles") == 0) {
          auto ritr = hltLabelMap_.find(collectionTag);
          if (ritr == hltLabelMap_.end() && collectionTag == "hltL1extraParticles::HLT") {
            if (iL1Extra == 0)
              ritr = hltLabelMap_.find("hltL1extraParticles:Muon:HLT");
            else if (iL1Extra == 1)
              ritr = hltLabelMap_.find("hltL1extraParticles:HFRing:HLT");
            ++iL1Extra;
          }
          if (ritr != hltLabelMap_.end())
            tnind = ritr->second;
        }

        TriggerObjectBase* trigObj = hltObjs_->Allocate();
        new (trigObj) TriggerObjectBase(0, 0, tobj.pt(), tobj.eta(), tobj.phi(), tobj.mass());
        trigObj->SetTagInd(tnind);

        // TriggerObjectStandAlone provides the names of the paths and the filters but does not
        // relate the two - use paths and iterate over path modules as given by HLTConfigProvider.

        // What corresponds to triggerEvent.filterIds(iFilter) is stored in individual pat::TriggerObject
        // as a vector of trigger::TriggerObjectType, which is synched with filterLabels
        // see PhysicsTools/PatAlgos/plugins/PATTriggerProducer.cc
        auto&& objectTypes = tobj.triggerObjectTypes();

        auto fbegin = tobj.filterLabels().begin();
        auto fend = tobj.filterLabels().end();

        for (std::string& path : tobj.pathNames()) {
          auto iPath = hltConfig_.triggerIndex(path);
          
          for (std::string const& mLabel : hltConfig_.moduleLabels(iPath)) {
            if (!tobj.hasFilterLabel(mLabel))
              continue;
            
            std::string mType = hltConfig_.moduleType(mLabel);

            std::string tType = "L1";
            if (mType != "HLTLevel1GTSeed") 
              tType = "L3";

            // find index for module label/type name
            auto riter = hltLabelMap_.find(mLabel);
            assert(riter != hltLabelMap_.end());

            short modIndex = riter->second;

            riter = hltLabelMap_.find(mType);
            assert(riter != hltLabelMap_.end());

            short typeIndex = riter->second;

            auto fitr = std::find(fbegin, fend, mLabel);
            assert(fitr != fend);

            auto vid = objectTypes[fitr - fbegin];

            TriggerObjectRel* trigRel = hltRels_->Allocate();
            new (trigRel) TriggerObjectRel(iPath, vid, iObj, modIndex, typeIndex);
          }
        }

        ++iObj;
      }
    }

  }

  hltObjs_->Trim();
  hltRels_->Trim();
}

void
mithep::FillerTrigger::FillL1t(edm::Event const& event)
{
  //  Fill L1 trigger bits after masking.
  mithep::BitMask128 l1amask;
  mithep::BitMask128 l1tmask;
  mithep::BitMask128 l1abmask;
  mithep::BitMask128 l1tbmask;

  //check size of l1 menu
  int errorCode = 0;
  if (l1GtUtils_.triggerMaskSet(L1GtUtils::AlgorithmTrigger, errorCode).size() > l1amask.Size()) {
    edm::LogError("FillerTrigger") << "L1 config contains too many algos "
                               << l1GtUtils_.triggerMaskSet(L1GtUtils::AlgorithmTrigger, errorCode).size()
                               << " > " << l1amask.Size();
    return;
  }
  //check size of l1 menu
  if (l1GtUtils_.triggerMaskSet(L1GtUtils::TechnicalTrigger, errorCode).size() > l1tmask.Size()) {
    edm::LogError("FillerTrigger") << "L1 config contains too many tech triggers "
                               << l1GtUtils_.triggerMaskSet(L1GtUtils::TechnicalTrigger, errorCode).size()
                               << " > " << l1tmask.Size();
    return;
  }  
  
  std::string triggerName;
  std::string triggerAlias;
  bool decision, beforeMask;
  int prescale, mask;
  for (unsigned iT = 0; iT != 128; ++iT) {
    if (l1GtUtils_.l1TriggerNameFromBit(iT, L1GtUtils::AlgorithmTrigger, triggerAlias, triggerName)) {
      l1GtUtils_.l1Results(event, triggerName, beforeMask, decision, prescale, mask);
      l1amask.SetBit(iT, decision);
      l1abmask.SetBit(iT, beforeMask);
    }

    if (l1GtUtils_.l1TriggerNameFromBit(iT, L1GtUtils::TechnicalTrigger, triggerAlias, triggerName)) {
      l1GtUtils_.l1Results(event, triggerName, beforeMask, decision, prescale, mask);
      l1tmask.SetBit(iT, decision);
      l1tbmask.SetBit(iT, beforeMask);
    }
  }

  l1ABits_->SetBits(l1amask);
  l1TBits_->SetBits(l1tmask);
  l1ABits2_->SetBits(l1abmask);
  l1TBits2_->SetBits(l1tbmask);

  // get L1 trigger readout record information if wanted
  if (l1GTRRToken_.isUninitialized())
    return;

  edm::Handle<L1GlobalTriggerReadoutRecord> gtReadoutRec;
  GetProduct(l1GTRRToken_, gtReadoutRec, event);

  l1AbArr_->Reset();
  auto& fdlVector  = gtReadoutRec->gtFdlVector();
  for (auto& fdlWord : fdlVector) {
    BitMask128 amask;
    BitMask128 tmask;
    unsigned iB = 0;
    // was using finalOR instead of gtDecisionWord - cannot imagine how that made sense (Y.I. 2015.05.23)
    for (auto pass : fdlWord.gtDecisionWord()) {
      if (pass)
        amask.SetBit(iB);
      ++iB;
      if (iB == amask.Size())
        break;
    }
    mithep::L1TriggerMask *aArr = l1AbArr_->Allocate();
    new (aArr) mithep::L1TriggerMask(amask);
    aArr->SetBx(fdlWord.bxInEvent());

    iB = 0;
    for (auto pass : fdlWord.gtTechnicalTriggerWord()) {
      if (pass)
        tmask.SetBit(iB);
      ++iB;
      if (iB == tmask.Size())
        break;
    }
    mithep::L1TriggerMask *tArr = l1TbArr_->Allocate();
    new (tArr) mithep::L1TriggerMask(tmask);
    tArr->SetBx(fdlWord.bxInEvent());
  }
}
