// $Id: FillerMetaInfos.cc,v 1.49 2009/11/26 21:41:51 loizides Exp $

#include "MitProd/TreeFiller/interface/FillerMetaInfos.h"
#include "CondFormats/DataRecord/interface/L1GtTriggerMenuRcd.h"
#include "CondFormats/L1TObjects/interface/L1GtTriggerMenu.h"
#include "CondFormats/L1TObjects/interface/L1GtTriggerMenuFwd.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerRecord.h"
#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerReadoutRecord.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/TriggerNames.h"
#include "MitAna/DataTree/interface/EventHeader.h"
#include "MitAna/DataTree/interface/LAHeader.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataTree/interface/L1TriggerMaskCol.h"
#include "MitAna/DataTree/interface/RunInfo.h"
#include "MitAna/DataTree/interface/TriggerMask.h"
#include "MitAna/DataTree/interface/TriggerObjectBase.h"
#include "MitAna/DataTree/interface/TriggerObjectBaseCol.h"
#include "MitAna/DataTree/interface/TriggerObjectRelCol.h"
#include "MitProd/ObjectService/interface/ObjectService.h"
#include <TObjectTable.h>
#include <TIterator.h>

using namespace std;
using namespace edm;
using namespace mithep;

Int_t mithep::FillerMetaInfos::instance_ = 0;

//--------------------------------------------------------------------------------------------------
FillerMetaInfos::FillerMetaInfos(const ParameterSet &cfg, const char *name, bool active) : 
  BaseFiller(cfg,name,(instance_==0||active?1:0)),
  evtName_(Conf().getUntrackedParameter<string>("evtMitName",Names::gkEvtHeaderBrn)),
  runTreeName_(Conf().getUntrackedParameter<string>("runTreeMitName",Names::gkRunTreeName)),
  lahTreeName_(Conf().getUntrackedParameter<string>("lahTreeMitName",Names::gkLATreeName)),
  hltTreeName_(Conf().getUntrackedParameter<string>("hltTreeMitName",
                                                Form("%s%s",Names::gkHltTreeName,Istr()))),
  hltActive_(Conf().getUntrackedParameter<bool>("hltActive",true)),
  hltResName_(Conf().getUntrackedParameter<string>("hltResEdmName","TriggerResults")),
  hltEvtName_(Conf().getUntrackedParameter<string>("hltEvtEdmName","hltTriggerSummaryAOD")),
  hltTableName_(Conf().getUntrackedParameter<string>("hltTableMitName",Names::gkHltTableBrn)),
  hltLabelName_(Conf().getUntrackedParameter<string>("hltLabelMitName",Names::gkHltLabelBrn)),
  hltBitsName_(Conf().getUntrackedParameter<string>("hltBitsMitName",
                                                    Form("%s%s",Names::gkHltBitBrn,Istr()))),
  hltObjsName_(Conf().getUntrackedParameter<string>("hltObjsMitName",
                                                    Form("%s%s",Names::gkHltObjBrn,Istr()))),
  l1Active_(Conf().getUntrackedParameter<bool>("l1Active",true)),
  l1GTRecName_(Conf().getUntrackedParameter<string>("l1GtRecordEdmName","l1GtRecord")),
  l1GTRRName_(Conf().getUntrackedParameter<string>("l1GtReadRecEdmName","")),
  l1TBitsName_(Conf().getUntrackedParameter<string>("l1TechBitsMitName",
                                                    Form("%s%s",Names::gkL1TechBitsBrn,Istr()))),
  l1ABitsName_(Conf().getUntrackedParameter<string>("l1AlgoBitsMitName",
                                                    Form("%s%s",Names::gkL1AlgoBitsBrn,Istr()))),
  tws_(0),
  eventHeader_(new EventHeader()),
  evtLAHeader_(new LAHeader()),
  runInfo_(new RunInfo()),
  runTree_(0),
  laTree_(0),
  runEntries_(0),
  hltBits_(new TriggerMask),
  hltTable_(new vector<string>),
  hltTabMap_(0),
  hltLabels_(new vector<string>),
  hltLabMap_(0),
  hltObjs_(new TriggerObjectBaseArr),
  hltRels_(new TriggerObjectRelArr),
  hltTree_(0),
  hltEntries_(0),
  fileNum_(0),
  l1TBits_(new L1TriggerMask),
  l1ABits_(new L1TriggerMask),
  l1TBits2_(new L1TriggerMask),
  l1ABits2_(new L1TriggerMask),
  l1AbArr_(new L1TriggerMaskArr),
  l1TbArr_(new L1TriggerMaskArr)
{
  // Constructor.

  ++instance_;

  if (Conf().exists("hltProcNames"))
    hltProcNames_ = Conf().getUntrackedParameter<vector<string> >("hltProcNames");
  if (hltProcNames_.size()==0)
    hltProcNames_.push_back("HLT");

  if (l1Active_ && !hltActive_)
    PrintErrorAndExit("L1Active set _without_ hltActive set as well is not supported");
}

//--------------------------------------------------------------------------------------------------
FillerMetaInfos::~FillerMetaInfos()
{
  // Destructor.

  delete eventHeader_;
  delete evtLAHeader_;
  delete runInfo_;
  delete hltBits_;
  delete hltTable_;
  delete hltLabels_;
  delete hltObjs_;
  delete hltRels_;
  delete l1TBits_;
  delete l1ABits_;
  delete l1AbArr_;
  delete l1TbArr_;
  eventHeader_ = 0;
  evtLAHeader_ = 0;
  runInfo_     = 0;
  hltTable_    = 0;
  hltLabels_   = 0;
  hltObjs_     = 0;
  hltRels_     = 0;
  runTree_     = 0;
  laTree_      = 0;
  hltTree_     = 0;
}

//--------------------------------------------------------------------------------------------------
void FillerMetaInfos::BookDataBlock(TreeWriter &tws, const edm::EventSetup &es)
{
  // Create run info tree and book our branches.

  // add branches to main tree
  if (OS()->get<mithep::EventHeader>(evtName_)==0) {
    tws.AddBranch(evtName_,&eventHeader_);
    OS()->add<mithep::EventHeader>(eventHeader_,evtName_);
  }

  // add branches to run info tree
  runTree_=tws.GetTree(runTreeName_);
  if (runTree_==0) {
    tws.AddBranchToTree(runTreeName_,Names::gkRunInfoBrn,
                        TClass::GetClass(typeid(*runInfo_))->GetName(),&runInfo_);
    tws.SetAutoFill(runTreeName_,0);
    runTree_=tws.GetTree(runTreeName_);
  }

  // add branches to lookahead tree
  laTree_=tws.GetTree(lahTreeName_);
  if (laTree_==0) {
    tws.AddBranchToTree(lahTreeName_,Names::gkLAHeaderBrn,
                        TClass::GetClass(typeid(*evtLAHeader_))->GetName(),&runInfo_);
    tws.SetAutoFill(lahTreeName_,0);
    laTree_=tws.GetTree(lahTreeName_);
  }

  // add HLT branches to main tree and to HLT trigger info tree
  if (hltActive_) {
    tws.AddBranch(hltBitsName_,&hltBits_);
    tws.AddBranch(hltObjsName_,&hltObjs_);
    tws.AddBranch(Form("%sRelation",hltObjsName_.c_str()),&hltRels_);

    tws.AddBranchToTree(hltTreeName_,hltTableName_,
                        TClass::GetClass(typeid(*hltTable_))->GetName(),&hltTable_,32*1024,0);
    tws.AddBranchToTree(hltTreeName_,hltLabelName_,
                        TClass::GetClass(typeid(*hltLabels_))->GetName(),&hltLabels_,32*1024,0);
    tws.SetAutoFill(hltTreeName_,0);
    hltTree_=tws.GetTree(hltTreeName_);
  }

  if (l1Active_) {
    tws.AddBranch(l1TBitsName_,&l1TBits_);
    tws.AddBranch(l1ABitsName_,&l1ABits_);
    tws.AddBranch(Form("%sBeforeMask",l1TBitsName_.c_str()),&l1TBits2_);
    tws.AddBranch(Form("%sBeforeMask",l1ABitsName_.c_str()),&l1ABits2_);
    if (l1GTRRName_.size()) {
      tws.AddBranch(Form("%sBxs",l1TBitsName_.c_str()),&l1TbArr_);
      tws.AddBranch(Form("%sBxs",l1ABitsName_.c_str()),&l1AbArr_);
    }
  }

  // store pointer to tree writer 
  tws_ = &tws;
}

//--------------------------------------------------------------------------------------------------
void FillerMetaInfos::FillBitAMask(BitMask128 &bits, const DecisionWord &dw)
{
  // Fill bit mask.

  size_t dws = dw.size();
  if (dws>128) 
    dws = 128;    
  for (size_t i=0; i<dws;++i) {
    if (dw.at(i))
      bits.SetBit(i);
  }
}

//--------------------------------------------------------------------------------------------------
void FillerMetaInfos::FillBitTMask(BitMask128 &bits, const TechnicalTriggerWord &tw)
{
  // Fill bit mask.

  size_t tws = tw.size();
  if (tws>128) 
    tws = 128;    
  for (size_t i=0; i<tws;++i) {
    if (tw.at(i))
      bits.SetBit(i);
  }
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
    FillL1Trig(event,setup);
    return;
  }

  // fill new run info
  Int_t runentry = runEntries_;
  ++runEntries_;
  eventHeader_->SetRunEntry(runentry);
  runmap_.insert(pair<UInt_t,Int_t>(runnum,runentry));
  runInfo_->SetRunNum(runnum);

  Int_t hltentry = hltEntries_;
  FillHltInfo(event,setup);
  FillHltTrig(event,setup);
  FillL1Trig(event,setup);
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
    for(UInt_t i=0; i<hltProcNames_.size(); ++i) {
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

  // get HLT trigger object information to be able to access the tag information
  Handle<trigger::TriggerEvent> triggerEventHLT;
  GetProduct(hltEvtName_, triggerEventHLT, event);
  const std::vector<std::string> &tags(triggerEventHLT->collectionTags());
  for(UInt_t i=0,c=0; i<tags.size(); ++i) {
    string tag(tags.at(i));
    if (tag.compare(0,19,"hltL1extraParticles")!=0)
      continue;
    if (tag == "hltL1extraParticles::HLT") {
      if (c==0)
        tag = "hltL1extraParticles:Muon:HLT";
      else if (c==1)
        tag = "hltL1extraParticles:HFRing:HLT";
      else
        tag = Form("hltL1extraParticles:Unknown%d:HLT", c);
      ++c;
    }
    map<string,Short_t>::iterator riter = labmap->find(tag);
    if (riter == labmap->end()) {
      labmap->insert(pair<string,Short_t>(tag,labels->size()));
      labels->push_back(tag);
    }
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

  if (l1Active_) {
    edm::ESHandle<L1GtTriggerMenu> menuRcd;
    setup.get<L1GtTriggerMenuRcd>().get(menuRcd) ;
    const L1GtTriggerMenu* menu = menuRcd.product();

    // get L1 algo names
    size_t counter = 0;
    labels->push_back("xxx-L1AlgoNames-xxx");
    for (CItAlgo algo = menu->gtAlgorithmMap().begin(); 
         algo!=menu->gtAlgorithmMap().end(); ++algo) {
      int algBitNumber = algo->second.algoBitNumber();
      if (algBitNumber!=counter) {
        labels->push_back(Form("UnusedL1Algo%d",counter));
      } else {
        labels->push_back(algo->second.algoName());
      }
      ++counter;
    }
    if (counter>=l1ABits_->Size()) {
      edm::LogError("FillerMetaInfos") << "Cannot store more than 128 L1 algo bits, but got " 
                                       << counter << std::endl;
    }

    // get L1 tech names
    counter = 0;
    labels->push_back("xxx-L1TechNames-xxx");
    for (CItAlgo algo = menu->gtTechnicalTriggerMap().begin(); 
         algo!=menu->gtTechnicalTriggerMap().end(); ++algo) {
      int algBitNumber = algo->second.algoBitNumber();
      if (algBitNumber!=counter) {
        labels->push_back(Form("UnusedL1Tech%d",counter));
      } else {
        labels->push_back(algo->second.algoName());
      }
      ++counter;
    }
    if (counter>=l1TBits_->Size()) {
      edm::LogError("FillerMetaInfos") << "Cannot store more than 128 L1 technical bits, but got " 
                                       << counter << std::endl;
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
  BitMask256 maskhlt;

  // loop over trigger objects and fill them
  const trigger::TriggerObjectCollection &toc(triggerEventHLT->getObjects());
  const std::vector<std::string> &tags(triggerEventHLT->collectionTags());
  for(UInt_t i=0,iprev=0,c=0; i<tags.size(); ++i) {

    Short_t tnind = -(i+1);
    string tag(tags.at(i));
    if (tag.compare(0,19,"hltL1extraParticles")==0) {
      if (tag == "hltL1extraParticles::HLT") {
        if (c==0)
          tag = "hltL1extraParticles:Muon:HLT";
        else if (c==1)
          tag = "hltL1extraParticles:HFRing:HLT";
        else
          tag = Form("hltL1extraParticles:Unknown%d:HLT", c-1);
        ++c;
      }
      map<string,Short_t>::iterator riter = hltLabMap_->find(tag);
      if (riter != hltLabMap_->end()) {
        tnind = riter->second;
      }
    }

    UInt_t inext(triggerEventHLT->collectionKey(i));
    if (verbose_>2)
      cout << i << " " << tag << " with " << inext-iprev << " objects " << endl;
    for (UInt_t k=iprev; k<inext; ++k) {
      const trigger::TriggerObject &tobj = toc[k];
      TriggerObjectBase *trigObj = hltObjs_->Allocate();
      new (trigObj) TriggerObjectBase(tobj.id(),0,tobj.pt(),tobj.eta(),tobj.phi(),tobj.mass());
      trigObj->SetTagInd(tnind);
      if (verbose_>4)
        cout << "   " << k << " " << tobj.id() << " " << tobj.pt() << " " << tobj.eta() 
             << " " << tobj.phi() << " " << tobj.mass() << endl;
    }
    iprev = inext;
  }

  // loop over trigger paths
  const UInt_t N = hltConfig_.size();
  for(UInt_t i=0;i<N;++i) {

    const string &name(hltConfig_.triggerName(i));
    const UInt_t tind(hltConfig_.triggerIndex(name));
    if (verify_)
      assert(tind==i);
 
    if (verbose_>0)
      cout << "Trigger: path " << name << " [" << i << "]" << endl;

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

      string tType = "L1";
      if (mType != "HLTLevel1GTSeed") 
        tType = "L3";

      if (verbose_>1)
        cout << " '" << tType << "' filter in slot " << j 
             << " - label/type " << mLabel << "/" << mType << endl;

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
        cout << "   " << nVids  << " accepted '" << tType << "' objects found: " << endl;

      // loop over trigger objects
      const trigger::TriggerObjectCollection &toc(triggerEventHLT->getObjects());
      for (trigger::size_type k=0; k<nVids; ++k) {
        Int_t tocind = keys[k];

	const trigger::TriggerObject &tobj(toc[tocind]);

        if (verbose_>2) 
          cout << "   " << k << " " << vids[k] << "/" << keys[k] << ": "
               << tobj.id() << " " << tobj.pt() << " " << tobj.eta() 
               << " " << tobj.phi() << " " << tobj.mass() << endl;

        TriggerObjectRel *trigRel = hltRels_->Allocate();
        new (trigRel) TriggerObjectRel(mytind,vids[k],tocind,modind,filind);
        TriggerObjectBase *trigObj = hltObjs_->At(tocind);
        if (trigObj->Type()==0) {
          trigObj->SetType(vids[k]);
        } else {
          if (trigObj->Type()!=vids[k]) {
            if (verbose_>3) 
              cout << "   -> Have already type " << trigObj->Type() 
                   << " ignored new type " << vids[k] << endl;
          }
        }
      }
    }

    if (triggerResultsHLT->accept(tind))
      maskhlt.SetBit(mytind);
  }
  
  hltBits_->SetBits(maskhlt);
  hltObjs_->Trim();
  hltRels_->Trim();
}

//--------------------------------------------------------------------------------------------------
void FillerMetaInfos::FillL1Trig(const edm::Event &event, const edm::EventSetup &setup)
{
  //  Fill L1 trigger bits after masking.

  if (!l1Active_) 
    return;

  // get L1 trigger record information
  Handle<L1GlobalTriggerRecord> gtRecord;
  GetProduct(l1GTRecName_, gtRecord, event);

  // deal with algo bits
  BitMask128 l1amask;
  DecisionWord dw(gtRecord->decisionWord());
  FillBitAMask(l1amask, dw);
  l1ABits_->SetBits(l1amask);

  // deal with tech bits
  BitMask128 l1tmask;
  TechnicalTriggerWord tw(gtRecord->technicalTriggerWord());
  FillBitTMask(l1tmask, tw);
  l1TBits_->SetBits(l1tmask);

  BitMask128 l1abmask;
  DecisionWord dwb(gtRecord->decisionWordBeforeMask());
  FillBitAMask(l1abmask, dwb);
  l1ABits2_->SetBits(l1abmask);

  // deal with tech bits
  BitMask128 l1tbmask;
  TechnicalTriggerWord twb(gtRecord->technicalTriggerWordBeforeMask());
  FillBitTMask(l1tbmask, twb);
  l1TBits2_->SetBits(l1tbmask);

  // get L1 trigger readout record information if wanted
  if (l1GTRRName_.size()==0)
    return;

  Handle<L1GlobalTriggerReadoutRecord> gtReadoutRec;
  GetProduct(l1GTRRName_, gtReadoutRec, event);

  l1AbArr_->Reset();
  const std::vector<L1GtFdlWord> &m_gtFdlWord(gtReadoutRec->gtFdlVector());
  for (std::vector<L1GtFdlWord>::const_iterator itBx = m_gtFdlWord.begin();
       itBx != m_gtFdlWord.end(); ++itBx) {
    BitMask128 amask;
    DecisionWord dw((*itBx).finalOR());
    FillBitAMask(amask,dw);
    mithep::L1TriggerMask *nm = l1AbArr_->Allocate();
    new (nm) mithep::L1TriggerMask(amask);
    nm->SetBx((*itBx).bxInEvent());
  }

  l1TbArr_->Reset();
  for (std::vector<L1GtFdlWord>::const_iterator itBx = m_gtFdlWord.begin();
       itBx != m_gtFdlWord.end(); ++itBx) {
    BitMask128 amask;
    TechnicalTriggerWord dw((*itBx).gtTechnicalTriggerWord());
    FillBitTMask(amask,dw);
    mithep::L1TriggerMask *nm = l1TbArr_->Allocate();
    new (nm) mithep::L1TriggerMask(amask);
    nm->SetBx((*itBx).bxInEvent());
  }
}

//--------------------------------------------------------------------------------------------------
const char *FillerMetaInfos::Istr() const 
{
  // Return instance if instance > 0.

  if (instance_==0)
    return "";

  return Form("_%d",instance_);
}
