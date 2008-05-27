// $Id:$

#include "MitProd/TreeService/interface/TreeService.h"

#include "DataFormats/Provenance/interface/ModuleDescription.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/ActivityRegistry.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/MessageLogger/interface/JobReport.h"

#include "MitAna/DataUtil/interface/TreeWriter.h"

using namespace edm;
using namespace std;
using namespace mithep;

//-------------------------------------------------------------------------------------------------
TreeService::TreeService(const ParameterSet &cfg, ActivityRegistry &r) 
  : tws_(0),
    treeNames_(cfg.getUntrackedParameter<std::vector<std::string> >("treeNames")),
    fileNames_(cfg.getUntrackedParameter<std::vector<std::string> >("fileNames")),
    pathNames_(cfg.getUntrackedParameter<std::vector<std::string> >("pathNames")),
    maxSizes_(cfg.getUntrackedParameter<std::vector<unsigned> >("maxSizes")),
    compLevels_(cfg.getUntrackedParameter<std::vector<unsigned> >("compLevels")),
    splitLevels_(cfg.getUntrackedParameter<std::vector<unsigned> >("splitLevels")),
    brSizes_(cfg.getUntrackedParameter<std::vector<unsigned> >("brSizes"))
{
  if(treeNames_.size()!=treeNames_.size()) {
    //todo throw !!!
  }

  // setup tw array
  tws_.SetOwner(kTRUE);
  tws_.Expand(treeNames_.size());

  // init tree writers 
  for(unsigned int i=0;i<treeNames_.size();++i) {

    TreeWriter *t = new TreeWriter(treeNames_.at(i).c_str(),1);

    t->SetPrefix(fileNames_.at(i).c_str());
    if(i<pathNames_.size()) t->SetBaseURL(pathNames_.at(i).c_str());
    else if (pathNames_.size()>0) t->SetBaseURL(pathNames_.at(0).c_str());
    if(i<maxSizes_.size()) t->SetMaxSize(maxSizes_.at(i)*1024*1024);
    else if (maxSizes_.size()>0) t->SetMaxSize(maxSizes_.at(0)*1024*1024);
    if(i<compLevels_.size()) t->SetCompressLevel(compLevels_.at(i));
    else if (compLevels_.size()>0) t->SetCompressLevel(compLevels_.at(0));
    if(i<splitLevels_.size()) t->SetDefaultSL(splitLevels_.at(i));
    else if (splitLevels_.size()>0) t->SetDefaultSL(splitLevels_.at(0));
    if(i<brSizes_.size()) t->SetDefaultBrSize(brSizes_.at(i));
    else if (brSizes_.size()>0) t->SetDefaultBrSize(brSizes_.at(0));

    //t->Print();
    tws_.Add(t);
  }

  // set watchers
  r.watchPreProcessEvent(this,&TreeService::preEventProcessing);
  r.watchPostProcessEvent(this,&TreeService::postEventProcessing);
  r.watchPostBeginJob(this,&TreeService::postBeginJob);
  r.watchPostEndJob(this,&TreeService::postEndJob);
}

//-------------------------------------------------------------------------------------------------
TreeService::~TreeService() 
{
}

//-------------------------------------------------------------------------------------------------
TreeWriter* TreeService::get(const char *name) 
{
  if(tws_.GetEntries()<=0) return 0;

  if(name==0) 
    return dynamic_cast<TreeWriter*>(tws_.At(0));

  TObject *ret = tws_.FindObject(name);
  return dynamic_cast<TreeWriter*>(ret);
}

//-------------------------------------------------------------------------------------------------
void TreeService::postBeginJob()
{
  // nothing to be done for now
}

//-------------------------------------------------------------------------------------------------
void TreeService::postEndJob()
{
  tws_.Clear();
}

//-------------------------------------------------------------------------------------------------
void TreeService::postEventProcessing(const Event&, const EventSetup&)
{
  for(int i=0;i<tws_.GetEntries();++i) {
    TreeWriter *tw=dynamic_cast<TreeWriter*>(tws_.At(i));
    if(tw)
      tw->EndEvent();
  }
}

//-------------------------------------------------------------------------------------------------
void TreeService::preEventProcessing(const EventID&, const Timestamp&)
{
  for(int i=0;i<tws_.GetEntries();++i) {
    TreeWriter *tw=dynamic_cast<TreeWriter*>(tws_.At(i));
    if(tw)
      tw->BeginEvent();
  }
}
