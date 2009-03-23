// $Id: TreeService.cc,v 1.12 2009/03/04 07:25:24 loizides Exp $

#include "MitProd/TreeService/interface/TreeService.h"
#include "DataFormats/Provenance/interface/ModuleDescription.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/ActivityRegistry.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/MessageLogger/interface/JobReport.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "MitAna/DataUtil/interface/TreeWriter.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitCommon/OptIO/interface/OptInt.h"

using namespace edm;
using namespace std;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
TreeService::TreeService(const ParameterSet &cfg, ActivityRegistry &ar) : 
  tws_(0),
  zipMode_(1),
  bZipThres_(1),
  gZipThres_(1),
  lzoThres_(1),
  lzmaThres_(1),
  optIOVerbose_(1),
  fDoReset_(cfg.getUntrackedParameter<bool>("doReset",1))
{
  // Constructor.

  if (cfg.exists("treeNames"))
    treeNames_=cfg.getUntrackedParameter<vector<string>   >("treeNames");
  else 
    treeNames_.push_back(Names::gkEvtTreeName);

  if (cfg.exists("fileNames"))
    fileNames_=cfg.getUntrackedParameter<vector<string>   >("fileNames");
  else
    fileNames_.push_back("mit-test");
    
  if (cfg.exists("pathNames"))
    pathNames_=cfg.getUntrackedParameter<vector<string>   >("pathNames");
  else
    pathNames_.push_back(".");

  if (cfg.exists("maxSizes"))
    maxSizes_=cfg.getUntrackedParameter<vector<unsigned> >("maxSizes");
  else 
    maxSizes_.push_back(1024);

  if (cfg.exists("compLevels"))
    compLevels_=cfg.getUntrackedParameter<vector<unsigned> >("compLevels");
  else 
    compLevels_.push_back(9);

  if (cfg.exists("splitLevels"))
    splitLevels_=cfg.getUntrackedParameter<vector<unsigned> >("splitLevels");
  else 
    splitLevels_.push_back(99);

  if (cfg.exists("brSizes"))
    brSizes_=cfg.getUntrackedParameter<vector<unsigned> >("brSizes");
  else 
    brSizes_.push_back(16*1024);

  if (treeNames_.size()!=fileNames_.size()) {
    throw edm::Exception(edm::errors::Configuration, "TreeService::TreeService()\n")
      << "Number of main trees should match number of files " << treeNames_.size() 
      << " " << fileNames_.size() << "\n";
    return;
  }

  if (OptInt::IsActivated()) {
    if (cfg.exists("zipMode"))
      zipMode_=cfg.getUntrackedParameter<unsigned>("zipMode");
    else 
      zipMode_ = 99;

    if (cfg.exists("bZipThres"))
      bZipThres_=cfg.getUntrackedParameter<double>("bZipThres");
    else 
      bZipThres_ = -1.0;

    if (cfg.exists("gZipThres"))
      gZipThres_=cfg.getUntrackedParameter<double>("gZipThres");
    else 
      gZipThres_ = 1.0;

    if (cfg.exists("lzoThres"))
      lzoThres_=cfg.getUntrackedParameter<double>("lzoThres");
    else 
      lzoThres_ = -1.0;

    if (cfg.exists("lzmaThres"))
      lzmaThres_=cfg.getUntrackedParameter<double>("lzmaThres");
    else 
      lzmaThres_ = 0.95;

    if (cfg.exists("optIOVerbose"))
      optIOVerbose_=cfg.getUntrackedParameter<unsigned>("optIOVerbose");
    else
      optIOVerbose_ = 0;

    OptInt::SetZipMode(zipMode_);
    OptInt::SetGzipFraction(gZipThres_);
    OptInt::SetBzipFraction(bZipThres_);
    OptInt::SetLzoFraction(lzoThres_);
    OptInt::SetLzmaFraction(lzmaThres_);
    OptInt::SetVerbose(optIOVerbose_);

  } else {

    if (cfg.exists("zipMode")   || cfg.exists("bZipThres") ||
        cfg.exists("gZipThres") || cfg.exists("lzoThres")  ||
        cfg.exists("lzmaThres")) {
      edm::LogError("TreeService") << "OptIO interface not properly pre-loaded, "
        "ignoring given settings." << std::endl;
    }
  }

  // setup tw array
  tws_.SetOwner(kTRUE);
  tws_.Expand(treeNames_.size());

  // init tree writers 
  for (unsigned int i=0; i<treeNames_.size(); ++i) {

    TreeWriter *t = new TreeWriter(treeNames_.at(i).c_str(),0);
    t->SetPrefix(fileNames_.at(i).c_str());

    if (i<pathNames_.size())
      t->SetBaseURL(pathNames_.at(i).c_str());
    else if (pathNames_.size()>0)
      t->SetBaseURL(pathNames_.at(0).c_str());

    if (i<maxSizes_.size())
      t->SetMaxSize((Long64_t)maxSizes_.at(i)*1024*1024);
    else if (maxSizes_.size()>0)
      t->SetMaxSize((Long64_t)maxSizes_.at(0)*1024*1024);

    if (i<compLevels_.size())
      t->SetCompressLevel(compLevels_.at(i));
    else if (compLevels_.size()>0)
      t->SetCompressLevel(compLevels_.at(0));

    if (i<splitLevels_.size())
      t->SetDefaultSL(splitLevels_.at(i));
    else if (splitLevels_.size()>0)
      t->SetDefaultSL(splitLevels_.at(0));

    if (i<brSizes_.size())
      t->SetDefaultBrSize(brSizes_.at(i));
    else if (brSizes_.size()>0)
      t->SetDefaultBrSize(brSizes_.at(0));

    //t->Print();
    tws_.Add(t);
  }

  // set watchers
  ar.watchPreProcessEvent (this,&TreeService::preEventProcessing);
  ar.watchPostProcessEvent(this,&TreeService::postEventProcessing);
  ar.watchPostBeginJob    (this,&TreeService::postBeginJob);
  ar.watchPostEndJob      (this,&TreeService::postEndJob);
}

//--------------------------------------------------------------------------------------------------
TreeService::~TreeService() 
{
  // Destructor.
}

//--------------------------------------------------------------------------------------------------
TreeWriter *TreeService::get(const char *name) 
{
  // Get TreeWriter by name. If no name given, first one will be returned.

  if (tws_.GetEntries()<=0)
    return 0;

  if (name==0) 
    return dynamic_cast<TreeWriter*>(tws_.At(0));

  TObject *ret = tws_.FindObject(name);

  return dynamic_cast<TreeWriter*>(ret);
}

//--------------------------------------------------------------------------------------------------
void TreeService::postBeginJob()
{
  // Nothing to be done for now
}

//--------------------------------------------------------------------------------------------------
void TreeService::postEndJob()
{
  // Clear all TreeWriter objects.

  tws_.Clear();
}

//--------------------------------------------------------------------------------------------------
void TreeService::postEventProcessing(const Event&, const EventSetup&)
{
  // Loop over all TreeWriter objects before processing of an event.

  for (int i=0; i<tws_.GetEntries(); ++i) {
    TreeWriter *tw=dynamic_cast<TreeWriter*>(tws_.At(i));
    if (tw) {
      if (i<tws_.GetEntries()-1)
        tw->EndEvent();
      else // make sure objects are only reset at the end (if at all)
        tw->EndEvent(fDoReset_);
    }
  }
}

//--------------------------------------------------------------------------------------------------
void TreeService::preEventProcessing(const EventID&, const Timestamp&)
{
  // Loop over all TreeWriter objects after processing of a events.

  for (int i=0; i<tws_.GetEntries(); ++i) {
    TreeWriter *tw=dynamic_cast<TreeWriter*>(tws_.At(i));
    if (tw) {
      if (i==0) // make sure objects are only reset at the beginning (if at all)
        tw->BeginEvent(fDoReset_);
      else
        tw->BeginEvent();
    }
  }
}
