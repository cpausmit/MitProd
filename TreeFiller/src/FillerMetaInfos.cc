// $Id: FillerMetaInfos.cc,v 1.35 2009/07/06 13:38:04 loizides Exp $

#include "MitProd/TreeFiller/interface/FillerMetaInfos.h"
#include "FWCore/Framework/interface/TriggerNames.h"
#include "CondFormats/DataRecord/interface/L1GtTriggerMenuRcd.h"
#include "CondFormats/L1TObjects/interface/L1GtTriggerMenuFwd.h"
#include "CondFormats/L1TObjects/interface/L1GtTriggerMenu.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerReadoutRecord.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataTree/interface/EventHeader.h"
#include "MitAna/DataTree/interface/LAHeader.h"
#include "MitAna/DataTree/interface/TriggerMask.h"
#include "MitAna/DataTree/interface/TriggerName.h"
#include "MitAna/DataTree/interface/RunInfo.h"
#include "MitAna/DataTree/interface/TriggerObjectCol.h"
#include "MitProd/ObjectService/interface/ObjectService.h"
#include <TObjectTable.h>
#include <TIterator.h>

using namespace std;
using namespace edm;
using namespace mithep;

bool mithep::FillerMetaInfos::instance_ = 0;

//--------------------------------------------------------------------------------------------------
FillerMetaInfos::FillerMetaInfos(const ParameterSet &cfg, const char *name, bool active) : 
  BaseFiller(cfg,name,(instance_==0||active?1:0)),
  evtName_(Conf().getUntrackedParameter<string>("evtMitName",Names::gkEvtHeaderBrn)),
  runName_(Conf().getUntrackedParameter<string>("runMitName",Names::gkRunInfoBrn)),
  lahName_(Conf().getUntrackedParameter<string>("lahMitName",Names::gkLAHeaderBrn)),
  l1Active_(Conf().getUntrackedParameter<bool>("l1Active",true)),
  l1TableName_(Conf().getUntrackedParameter<string>("l1MitTableName",Names::gkL1TableBrn)),
  l1BitsName_(Conf().getUntrackedParameter<string>("l1MitBitsName",Names::gkL1BitBrn)),
  l1ObjsName_(Conf().getUntrackedParameter<string>("l1MitObjsName",Names::gkL1ObjBrn)),
  hltActive_(Conf().getUntrackedParameter<bool>("hltActive",true)),
  hltResName_(Conf().getUntrackedParameter<string>("hltResEdmName","TriggerResults")),
  hltEvtName_(Conf().getUntrackedParameter<string>("hltEvtEdmName","hltTriggerSummaryAOD")),
  hltTableName_(Conf().getUntrackedParameter<string>("hltTableMitName",Names::gkHltTableBrn)),
  hltLabelName_(Conf().getUntrackedParameter<string>("hltLabelMitName",Names::gkHltLabelBrn)),
  hltBitsName_(Conf().getUntrackedParameter<string>("hltBitsMitName",Names::gkHltBitBrn)),
  hltObjsName_(Conf().getUntrackedParameter<string>("hltObjsMitName",Names::gkHltObjBrn)),
  tws_(0),
  eventHeader_(new EventHeader()),
  evtLAHeader_(new LAHeader()),
  runInfo_(new RunInfo()),
  runTree_(0),
  laTree_(0),
  runEntries_(0),
  l1Entries_(0),
  l1Table_(0),
  l1Tree_(0),
  hltBits_(new TriggerMask),
  hltTable_(new vector<string>),
  hltTabMap_(0),
  hltLabels_(new vector<string>),
  hltLabMap_(0),
  hltObjs_(new TriggerObjectBaseArr),
  hltRels_(new TriggerObjectRelArr),
  hltTree_(0),
  hltEntries_(0),
  fileNum_(0)
{
  // Constructor.

  instance_=1;

  if (Conf().exists("hltProcNames"))
    hltProcNames_ = Conf().getUntrackedParameter<vector<string> >("hltProcNames");
  if (hltProcNames_.size()==0)
    hltProcNames_.push_back("HLT");
}

//--------------------------------------------------------------------------------------------------
FillerMetaInfos::~FillerMetaInfos()
{
  // Destructor.

  delete eventHeader_;
  delete evtLAHeader_;
  delete runInfo_;
  delete l1Table_;
  delete hltTable_;
  delete hltLabels_;
  delete hltObjs_;
  delete hltRels_;
  eventHeader_ = 0;
  evtLAHeader_ = 0;
  runInfo_     = 0;
  l1Table_     = 0;
  hltTable_    = 0;
  hltLabels_   = 0;
  hltObjs_     = 0;
  hltRels_     = 0;
  runTree_     = 0;
  laTree_      = 0;
  hltTree_     = 0;
  l1Tree_      = 0;
}

//--------------------------------------------------------------------------------------------------
void FillerMetaInfos::BookDataBlock(TreeWriter &tws)
{
  // Create run info tree and book our branches.

  // add branches to main tree
  tws.AddBranch(evtName_,&eventHeader_);
  OS()->add<mithep::EventHeader>(eventHeader_,evtName_);
  tws.AddBranch(hltBitsName_,&hltBits_);
  tws.AddBranch(hltObjsName_,&hltObjs_);
  tws.AddBranch(Form("%sRelation",hltObjsName_.c_str()),&hltRels_);
  tws.GetTree()->BranchRef();

  // add branches to run info tree
  tws.AddBranchToTree(Names::gkRunTreeName,runName_,
                      TClass::GetClass(typeid(*runInfo_))->GetName(),&runInfo_);
  tws.SetAutoFill(Names::gkRunTreeName,0);
  runTree_=tws.GetTree(Names::gkRunTreeName);

  // add branches to lookahead tree
  tws.AddBranchToTree(Names::gkLATreeName,Names::gkLAHeaderBrn,
                      TClass::GetClass(typeid(*evtLAHeader_))->GetName(),&runInfo_);
  tws.SetAutoFill(Names::gkLATreeName,0);
  laTree_=tws.GetTree(Names::gkLATreeName);

  // add branches to L1 trigger info tree

  // add branches to HLT trigger info tree
  tws.AddBranchToTree(Names::gkHltTreeName,hltTableName_,
                      TClass::GetClass(typeid(*hltTable_))->GetName(),&hltTable_,32*1024,0);
  tws.AddBranchToTree(Names::gkHltTreeName,hltLabelName_,
                      TClass::GetClass(typeid(*hltLabels_))->GetName(),&hltLabels_,32*1024,0);
  tws.SetAutoFill(Names::gkHltTreeName,0);
  hltTree_=tws.GetTree(Names::gkHltTreeName);

  // store pointer to tree writer 
  tws_ = &tws;
}

//--------------------------------------------------------------------------------------------------
void FillerMetaInfos::FillDataBlock(const edm::Event &event, const edm::EventSetup &setup)
{
  // Fill our data structures.

  // clear map if a new file was opened
  if (tws_->GetFileNumber()!=fileNum_) {
    runmap_.clear();
    fileNum_ = tws_->GetFileNumber();
    runEntries_ = 0;
    l1Entries_  = -1;
    hltEntries_ = -1;
  }

  UInt_t runnum = event.id().run();

  // store look ahead information
  if (runEntries_>0) {
    evtLAHeader_->SetRunNum(runnum);
    laTree_->Fill();
    if(0) { // check for memory leak in usage of ROOT objects
      if(laTree_->GetEntries() % 100==0)
        gObjectTable->Print();
    }
  }

  // fill event header
  eventHeader_->SetEvtNum(event.id().event());
  eventHeader_->SetLumiSec(event.luminosityBlock());
  eventHeader_->SetRunNum(runnum);
  eventHeader_->SetIsMC(!event.isRealData());
  eventHeader_->SetBunchCrossing(event.bunchCrossing());
  eventHeader_->SetExpType(event.experimentType());
  eventHeader_->SetOrbitNumber(event.orbitNumber());
  eventHeader_->SetStoreNumber(event.bunchCrossing());
  eventHeader_->SetTimeStamp(event.time().value());

  // look-up if entry is in map
  map<UInt_t,Int_t>::iterator riter = runmap_.find(runnum);
  if (riter != runmap_.end()) {
    Int_t runentry = riter->second;
    eventHeader_->SetRunEntry(runentry);
    FillHltTrig(event,setup);
    return;
  }

  // fill new run info
  Int_t runentry = runEntries_;
  ++runEntries_;
  eventHeader_->SetRunEntry(runentry);
  runmap_.insert(pair<UInt_t,Int_t>(runnum,runentry));
  runInfo_->SetRunNum(runnum);

  Int_t l1entry = l1Entries_;
  FillL1Info(event,setup);
  FillL1Trig(event,setup);
  if (l1entry < l1Entries_) {
    l1Tree_->Fill();
    runInfo_->SetL1Entry(l1entry);
  } else {
    runInfo_->SetL1Entry(l1entry-1);
  }

  Int_t hltentry = hltEntries_;
  FillHltInfo(event,setup);
  FillHltTrig(event,setup);
  if (hltentry < hltEntries_) {
    runInfo_->SetHltEntry(hltentry);
    hltTree_->Fill();
  } else {
    runInfo_->SetHltEntry(hltentry-1);
  }

  runTree_->Fill();
}

//--------------------------------------------------------------------------------------------------
void FillerMetaInfos::FillHltInfo(const edm::Event &event, const edm::EventSetup &setup)
{
  // Fill HLT trigger table if it changed.

  if (!hltActive_) 
    return;

  // check if we can access the hlt config information
  if (hltProcName_.empty()) {
    string teststr;
    for(unsigned int i=0; i<hltProcNames_.size(); ++i) {
      if (i>0) 
        teststr += ", ";
      teststr += hltProcNames_.at(i);
      edm::LogInfo("FillerMetaInfos") << "Trying to access hlt config using " 
                                      <<  hltProcNames_.at(i) << std::endl;

      if (hltConfig_.init(hltProcNames_.at(i))) {
        hltProcName_ = hltProcNames_.at(i);
        if (hltResName_.find(':')==string::npos)
          hltResName_ += "::";
        else 
          hltResName_ += ":";
        hltResName_ += hltProcName_;
        if (hltEvtName_.find(':')==string::npos)
          hltEvtName_ += "::";
        else 
          hltEvtName_ += ":";
        hltEvtName_ += hltProcName_;
        break;
      } 
    }
    if (hltProcName_.empty()) {
      edm::LogError("FillerMetaInfos") << "Cannot access hlt config using " 
                                       << teststr << std::endl;
      throw edm::Exception(edm::errors::Configuration, "FillerMetaInfos::FillHltInfo()\n")
        << "Cannot access hlt config using " << teststr << std::endl;
      return;
    }
  } else {
    if (!hltConfig_.init(hltProcName_)) {
      edm::LogError("FillerMetaInfos") << "Cannot access hlt config using " 
                                       << hltProcName_ << std::endl;
      throw edm::Exception(edm::errors::Configuration, "FillerMetaInfos::FillHltInfo()\n")
        << "Cannot access hlt config using " << hltProcName_ << std::endl;
      return;
    }
  }

  // check size of menu... < 256
  if (hltConfig_.size()>hltBits_->Size()) {
    edm::LogError("FillerMetaInfos") << "HLT config contains too many paths "
                                     << hltConfig_.size() << " > " 
                                     << hltBits_->Size() << std::endl;
    return;
  }

  vector<string>   *trigtable = new vector<string>;
  map<string,Short_t> *tabmap = new map<string,Short_t>; 
  vector<string>      *labels = new vector<string>;
  map<string,Short_t> *labmap = new map<string,Short_t>; 

  if (0) {
    if (!hltConfig_.tableName().empty())
      trigtable->push_back(hltConfig_.tableName());
    else
      trigtable->push_back("unknown_hlt_config");
  }

  // loop over hlt paths
  for(UInt_t i=0;i<hltConfig_.size();++i) {

    tabmap->insert(pair<string,Short_t>(hltConfig_.triggerName(i),i));
    trigtable->push_back(hltConfig_.triggerName(i));
    const vector<string> &mLabels(hltConfig_.moduleLabels(i));
    for (UInt_t j=0; j<mLabels.size(); ++j) {

      const string& label(mLabels[j]);

      // remove characters which do not work: negation is included as !ModuleName
      // (this might require a change in the underlying CMSSW HLT fwk, so watch out).
      const char *lptr = &label[0];
      if (label[0] == '!') 
        ++lptr;

      map<string,Short_t>::iterator riter = labmap->find(lptr);
      if (riter == labmap->end()) {
        labmap->insert(pair<string,Short_t>(lptr,labels->size()));
        labels->push_back(lptr);
      }

      const string type(hltConfig_.moduleType(lptr));
      riter = labmap->find(type);
      if (riter == labmap->end()) {
        labmap->insert(pair<string,Short_t>(type,labels->size()));
        labels->push_back(type);
      }
    }
  }

  if (hltTable_->size()>=0) {
    // check if existing table is equal to new table:
    //  if so keep it, otherwise store the new one  

    if ((hltTable_->size()==trigtable->size()) && 
        (hltLabels_->size()==labels->size())) {

      bool newEntryFound = false;
      for (UInt_t i=0; i<trigtable->size(); ++i) {
        if (trigtable->at(i) != hltTable_->at(i)) {
          newEntryFound = true;
          break;
        }
      }
      if (!newEntryFound) {
        for (UInt_t i=0; i<labels->size(); ++i) {
        if (labels->at(i) != hltLabels_->at(i)) {
            newEntryFound = true;
            break;
          }
        }
      }

      if (!newEntryFound) {
        if (verbose_>1) 
          cout << "FillHltInfo: Kept previous HLT information" << endl;

        delete trigtable;
        delete labels;
        delete labmap;
        delete tabmap;
        return;
      }
    }
  }

  // new hlt entry
  delete hltTable_;
  delete hltLabels_;
  delete hltTabMap_;
  delete hltLabMap_;
  hltTable_  = trigtable;
  hltLabels_ = labels;
  hltTabMap_ = tabmap;
  hltLabMap_ = labmap;
  hltEntries_++;
}
 
//--------------------------------------------------------------------------------------------------
void FillerMetaInfos::FillHltTrig(const edm::Event &event, const edm::EventSetup &setup)
{
  // Fill HLT trigger objects along triggered paths.

  if (!hltActive_) 
    return;

  // reset trigger objects
  hltObjs_->Reset();
  hltRels_->Reset();

  // get HLT trigger information
  Handle<TriggerResults> triggerResultsHLT;
  GetProduct(hltResName_, triggerResultsHLT, event);

  // get HLT trigger object information
  Handle<trigger::TriggerEvent> triggerEventHLT;
  GetProduct(hltEvtName_, triggerEventHLT, event);

  if (verify_)
    assert(triggerResultsHLT->size()==hltConfig_.size());

  // new bitmask
  BitMask256 mask;

  //map between EDM and our trigger object indices
  std::map<Int_t,Int_t> objmap;       

  // loop over trigger paths
  const UInt_t N = hltConfig_.size();
  for(UInt_t i=0;i<N;++i) {

    const string &name(hltConfig_.triggerName(i));
    const UInt_t tind(hltConfig_.triggerIndex(name.c_str()));
    if (verify_)
      assert(tind==i);
 
    if (verbose_>0)
      cout << "Trigger: path " << name << " [" << i << "]" << endl;

    if (!triggerResultsHLT->accept(tind)) 
      continue;

    // get our trigger bit
    map<string,Short_t>::iterator riter = hltTabMap_->find(name);
    if (riter == hltTabMap_->end()) {
      PrintErrorAndExit(Form("Trigger %s not found\n", name.c_str()));
    }

    UInt_t mytind = riter->second;
    if (verify_) {
      if (mytind!=tind)
        PrintErrorAndExit(Form("Trigger for %s index does not match: %ud %ud\n", 
                               name.c_str(), mytind, tind));
    }

    // set trigger bit
    mask.SetBit(mytind);

    // modules on this trigger path
    const UInt_t M(hltConfig_.size(tind));
    const UInt_t mind(triggerResultsHLT->index(tind));
    assert (mind<M);

    const vector<string> &mLabels(hltConfig_.moduleLabels(tind));
   
    if (verbose_>1) 
      cout << " Last active module - label/type: "
           << mLabels[mind] << "/" << hltConfig_.moduleType(mLabels[mind])
           << " [" << mind << " out of 0-" << (M-1) << " on this path]" << endl;

    // loop over modules on path
    for (UInt_t j=0; j<=mind; ++j) {
      const string &mLabel(mLabels[j]);

      // check whether the module is packed up in TriggerEvent product
      const UInt_t find(triggerEventHLT->filterIndex(InputTag(mLabel,"",hltProcName_)));
      if (find>=triggerEventHLT->sizeFilters()) 
        continue;

      const string mType(hltConfig_.moduleType(mLabel));
      if (verbose_>1)
        cout << " 'L3' filter in slot " << j << " - label/type " 
             << mLabel << "/" << mType << endl;

      // find index for module label/type name
      Short_t modind = -1;
      map<string,Short_t>::iterator riter = hltLabMap_->find(mLabel);
      if (riter != hltLabMap_->end()) {
        modind = riter->second;
      }
      assert(modind!=-1);

      Short_t filind = -1;
      riter = hltLabMap_->find(mType);
      if (riter != hltLabMap_->end()) {
        filind = riter->second;
      }
      assert(filind!=-1);

      // find trigger objects
      const trigger::Vids &vids(triggerEventHLT->filterIds(find));
      const trigger::Keys &keys(triggerEventHLT->filterKeys(find));
      const trigger::size_type nVids(vids.size());
      const trigger::size_type nKeys(keys.size());
      assert(nVids==nKeys);

      if (verbose_>2)
        cout << "   " << nVids  << " accepted 'L3' objects found: " << endl;

      // loop over trigger objects
      const trigger::TriggerObjectCollection &toc(triggerEventHLT->getObjects());
      for (trigger::size_type k=0; k<nVids; ++k) {
        Int_t tocind = keys[k];

        // get trigger object
	const trigger::TriggerObject &tobj(toc[tocind]);
        if (verbose_>2) 
          cout << "   " << k << " " << vids[k] << "/" << keys[k] << ": "
               << tobj.id() << " " << tobj.pt() << " " << tobj.eta() 
               << " " << tobj.phi() << " " << tobj.mass() << endl;

        // look-up if entry is in map
        Int_t objind = -1;
        map<Int_t,Int_t>::iterator riter = objmap.find(tocind);
        if (riter == objmap.end()) { // add new trigger object
          objind = hltObjs_->Entries();
          objmap.insert(pair<Int_t,Int_t>(tocind,objind));
          TriggerObjectBase *trigObj = hltObjs_->Allocate();
          new (trigObj) TriggerObjectBase(tobj.id(),tobj.pt(),tobj.eta(),tobj.phi(),tobj.mass());
        } else { // use existing trigger object
          objind = riter->second;
        }

        TriggerObjectRel *trigRel = hltRels_->Allocate();
        new (trigRel) TriggerObjectRel(mytind,vids[k],objind,modind,filind);
      }
    }
  }
  
  hltBits_->SetBits(mask);
  hltObjs_->Trim();
  hltRels_->Trim();
}

//--------------------------------------------------------------------------------------------------
void FillerMetaInfos::FillL1Info(const edm::Event &event, const edm::EventSetup &setup)
{
  //

}

//--------------------------------------------------------------------------------------------------
void  FillerMetaInfos::FillL1Trig(const edm::Event &event, const edm::EventSetup &setup)
{
  //

  edm::ESHandle<L1GtTriggerMenu> l1GtMenu;
  setup.get<L1GtTriggerMenuRcd>().get(l1GtMenu);
  const AlgorithmMap &algorithmMap = l1GtMenu->gtAlgorithmMap();

  for (CItAlgo itAlgo = algorithmMap.begin(); itAlgo != algorithmMap.end(); itAlgo++) {
    int bitNumber   = (itAlgo->second).algoBitNumber();
    string algoName = (itAlgo->second).algoName();
    //algoBitToAlgo[bitNumber] = &(itAlgo->second);
    cout << " " << bitNumber << algoName << endl;
  }

  edm::Handle<L1GlobalTriggerReadoutRecord> l1Handle;
  GetProduct("gtDigis", l1Handle, event);
  //l1Handle->print(cout);
  l1Handle->printL1Objects(cout);

  const DecisionWord dWord = l1Handle->decisionWord();
  for(int i = 0;i!=128;i++){
    //std::string trig(algorithmMap[i]);
    bool r=dWord.at(i);
    //std::cout << trig << " " << r << std::endl;
  }


}
