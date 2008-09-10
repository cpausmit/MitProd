// $Id: FillerMetaInfos.cc,v 1.13 2008/08/18 11:16:13 sixie Exp $

#include "MitProd/TreeFiller/interface/FillerMetaInfos.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Framework/interface/TriggerNames.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataTree/interface/EventHeader.h"
#include "MitAna/DataTree/interface/LAHeader.h"
#include "MitAna/DataTree/interface/TriggerName.h"
#include "MitAna/DataTree/interface/RunInfo.h"
#include <TObjectTable.h>
#include <THashTable.h>
#include <TIterator.h>

using namespace std;
using namespace edm;
using namespace mithep;

bool mithep::FillerMetaInfos::instance_ = 0;

//--------------------------------------------------------------------------------------------------
FillerMetaInfos::FillerMetaInfos(const ParameterSet &cfg, bool active) : 
  BaseFiller(cfg,"MetaInfos",(instance_==0||active?1:0)),
  hltActive_(Conf().getUntrackedParameter<bool>("hltActive",true)),
  l1Active_(Conf().getUntrackedParameter<bool>("l1Active",true)),
  evtName_(Conf().getUntrackedParameter<string>("evtName",Names::gkEvtHeaderBrn)),
  runName_(Conf().getUntrackedParameter<string>("runName",Names::gkRunInfoBrn)),
  lahName_(Conf().getUntrackedParameter<string>("lahName",Names::gkLAHeaderBrn)),
  hltName_(Conf().getUntrackedParameter<string>("hltName","TriggerResults")),
  l1tName_(Conf().getUntrackedParameter<string>("l1tName","todo")),
  tws_(0),
  eventHeader_(new EventHeader()),
  evtLAHeader_(new LAHeader()),
  runInfo_(new RunInfo()),
  runTree_(0),
  laTree_(0),
  runEntries_(0),
  fileNum_(0)
{
  // Constructor.

  instance_=1;
}

//--------------------------------------------------------------------------------------------------
FillerMetaInfos::~FillerMetaInfos()
{
  // Destructor.

  delete eventHeader_;
  delete evtLAHeader_;
  delete runInfo_;
  eventHeader_ = 0;
  evtLAHeader_ = 0;
  runInfo_     = 0;
  runTree_     = 0;
  laTree_      = 0;
}

//--------------------------------------------------------------------------------------------------
void FillerMetaInfos::BookDataBlock(TreeWriter &tws)
{
  // Create run info tre and book our branches.

  // add branches to main tree
  tws.AddBranch(evtName_.c_str(),"mithep::EventHeader",&eventHeader_);

  // add branches to run info tree
  tws.AddBranchToTree(Names::gkRunTreeName,runName_.c_str(),"mithep::RunInfo",&runInfo_);
  tws.SetAutoFill(Names::gkRunTreeName,0);
  runTree_=tws.GetTree(Names::gkRunTreeName);

  // add branches to lookahead tree
  tws.AddBranchToTree(Names::gkLATreeName,Names::gkLAHeaderBrn,"mithep::LAHeader",&evtLAHeader_);
  tws.SetAutoFill(Names::gkLATreeName,0);
  laTree_=tws.GetTree(Names::gkLATreeName);

  // store pointer to tree writer 
  tws_ = &tws;
}

//--------------------------------------------------------------------------------------------------
void FillerMetaInfos::FillDataBlock(const edm::Event &event, 
                                    const edm::EventSetup &setup)
{
  // Fill our data structures.

  // clear map if a new file was opened
  if (tws_->GetFileNumber()!=fileNum_) {
    runmap_.clear();
    fileNum_ = tws_->GetFileNumber();
    runEntries_ = 0;
  }

  UInt_t runnum = event.id().run();

  // store look ahead information
  if (runEntries_>0) {
    evtLAHeader_->SetRunNum(runnum);
    laTree_->Fill();
    if(0) {
      if(laTree_->GetEntries() % 100==0)
        gObjectTable->Print();
    }
  }

  // fill event header
  eventHeader_->SetEvtNum(event.id().event());
  eventHeader_->SetLumiSec(event.luminosityBlock());
  eventHeader_->SetRunNum(runnum);

  // look-up if entry is in map
  map<UInt_t,Int_t>::iterator riter = runmap_.find(runnum);
  if (riter != runmap_.end()) {
    Int_t runentry = riter->second;
    eventHeader_->SetRunEntry(runentry);

//      FillHltTrigger(event,setup);
    return;
  }

  // fill new run info
  Int_t runentry = runEntries_;
  eventHeader_->SetRunEntry(runentry);
  runmap_.insert(pair<UInt_t,Int_t>(runnum,runentry));

  runInfo_->SetRunNum(runnum);
  FillHltInfo(event,setup);
//FillHltTrigger(event,setup);

  runTree_->Fill();

  ++runEntries_;
}

//--------------------------------------------------------------------------------------------------
void FillerMetaInfos::FillHltInfo(const edm::Event &event, 
                                  const edm::EventSetup &setup)
{
  //

  if (!hltActive_) return;

  using namespace reco;
  using namespace trigger;

  ParameterSet ps;
  if (!event.getProcessParameterSet("HLT",ps))
  {
    // todo error...
    return;
  } 

  typedef std::vector<std::string> vstring;

  Handle<TriggerResults> triggerResultsHLT;
  GetProduct(hltName_, triggerResultsHLT, event);

  THashTable trigtable(1000,0);
  THashTable modtable(1000,0);
  THashTable testtable(1000,0);

  TriggerNames triggerNames(*(triggerResultsHLT.product())); 
  for(UInt_t i=0;i<triggerNames.size();++i) {

    TriggerName *trig = new TriggerName(triggerNames.triggerName(i).c_str(),i);
    trigtable.Add(trig);

    if (!ps.exists(triggerNames.triggerName(i))) {
      cout << "Error " << triggerNames.triggerName(i) << " not found" << endl;
    } else {
      vstring path(ps.getParameter<vstring>(triggerNames.triggerName(i)));
      for(UInt_t j=0;j<path.size();++j) {
        TriggerName *mod = new TriggerName(path.at(j).c_str(),j);
        if(!modtable.FindObject(mod->Name()))
          modtable.Add(mod);
        TriggerName *mod2 = new TriggerName(path.at(j).c_str(),i);
        testtable.Add(mod2);
      }
    }
  }

  trigtable.Rehash(trigtable.GetSize());
  modtable.Rehash(modtable.GetSize());
  testtable.Rehash(modtable.GetSize());
  cout << "------------- " << testtable.AverageCollisions() << endl;
  TIterator *iter = modtable.MakeIterator();
  while( TriggerName *t = dynamic_cast<TriggerName*>(iter->Next()) ) {
    cout << t->Name() 
         << ": " << modtable.Collisions(t->Name()) 
         << ": " << testtable.Collisions(t->Name()) << endl;
  }
  
#if 0
  Handle<TriggerEvent> handle;
  GetProduct("hltTriggerSummaryAOD", handle, event);

  Handle<TriggerResults> triggerResultsHLT;
  GetProduct(hltName_, triggerResultsHLT, event);

  //This gives names of trigger paths and the accept bit.
  TriggerNames triggerNames(*(triggerResultsHLT.product())); 
  for(UInt_t i=0;i<triggerNames.size();++i) {
//    cout << i << " " << triggerNames.triggerName(i) << " " << triggerResultsHLT->accept(i) << " " 
//         << triggerResultsHLT->index(i) << endl;
  }


   if (1) {
     cout << "Used Processname: " << handle->usedProcessName() << endl;
     const size_type nC(handle->sizeCollections());
     cout << "Number of packed Collections: " << nC << endl;
     cout << "The Collections: #, tag, 1-past-end index" << endl;
     for (size_type iC=0; iC!=nC; ++iC) {
       cout << iC << " "
	    << handle->collectionTag(iC).encode() << " "
	    << handle->collectionKey(iC) << endl;
     }
     const size_type nO(handle->sizeObjects());
     cout << "Number of TriggerObjects: " << nO << endl;
     cout << "The TriggerObjects: #, id, pt, eta, phi, mass" << endl;
     const TriggerObjectCollection& TOC(handle->getObjects());
     for (size_type iO=0; iO!=nO; ++iO) {
       const TriggerObject& TO(TOC[iO]);
       cout << iO << " " << TO.id() << " " << TO.pt() << " " << TO.eta() << " " << TO.phi() << " " << TO.mass() << endl;
     }
     const size_type nF(handle->sizeFilters());
     cout << "Number of TriggerFilters: " << nF << endl;
     cout << "The Filters: #, tag, #ids/#keys, the id/key pairs" << endl;
     for (size_type iF=0; iF!=nF; ++iF) {
       const Vids& VIDS (handle->filterIds(iF));
       const Keys& KEYS(handle->filterKeys(iF));
       const size_type nI(VIDS.size());
       const size_type nK(KEYS.size());
       cout << iF << " " << handle->filterTag(iF).encode()
	    << " " << nI << "/" << nK
	    << " the pairs: ";
       const size_type n(max(nI,nK));
       for (size_type i=0; i!=n; ++i) {
	 cout << " " << VIDS[i] << "/" << KEYS[i];
       }
       cout << endl;
       assert (nI==nK);
     }
   } else {
     cout << "Handle invalid! Check InputTag provided." << endl;
   }
   cout << endl;

#endif


}
