// $Id: runFileCataloger.C,v 1.4 2011/10/19 10:43:12 paus Exp $

#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TROOT.h>
#include <TSystem.h>
#endif

const TString slash      = "/";
const TString dCacheDoor = "dcap://t2srv0012.cmsaf.mit.edu/";
//const TString dCacheDoor = "dcap://t2srv0005.cmsaf.mit.edu/";
const TString hadoopDoor = "root://xrootd.cmsaf.mit.edu/";

void catalogFile(const char *dir, const char *file);
void reset();

//--------------------------------------------------------------------------------------------------
void runSimpleFileCataloger(const char *dir  = "/mnt/hadoop/cms/store/user/paus/fabstoec/Summer11Private/TTH_HToGG_M-120_TuneZ2_7TeV-pythia6/Summer11-PU32_8CM_START42_V13C-v4/GEN-SIM-RECO",
			    const char *file = "")
{
  // -----------------------------------------------------------------------------------------------
  // This script runs a full cataloging action on the given directory
  // -----------------------------------------------------------------------------------------------
  reset();
  catalogFile(dir,file);
  return;
}

//--------------------------------------------------------------------------------------------------
void catalogFile(const char *dir, const char *file)
{
  TString fileName = TString(dir) + slash +  + TString(file);
  //printf("Index: %d\n",fileName.Index("castor/cern.ch"));
  if (fileName.Index("castor/cern.ch") != -1)
    fileName = TString("castor:") + fileName;
  if (fileName.Index("pnfs/cmsaf.mit.edu") != -1) {
    fileName = dCacheDoor + fileName;
  }
  if (fileName.Index("mnt/hadoop/cms/store") != -1) {
    fileName.Remove(0,15);
    fileName = hadoopDoor + fileName;
  }
  
  TFile* f    = TFile::Open(fileName.Data());
  TTree* tree = (TTree*) f->FindObjectAny("Events");
  printf("XX-CATALOG-XX %s %d\n",fileName.Data(),tree->GetEntries());
}

//--------------------------------------------------------------------------------------------------
void reset()
{
}



void runSimpleFileCataloger(TString dir = "root://xroot.cmsaf.mit.edu/store/user/paus/fabstoec/Summer11Private//TTH_HToGG_M-120_TuneZ2_7TeV-pythia6/Summer11-PU32_8CM_START42_V13C-v4/GEN-SIM-RECO",
		 TString fileName="SUM11-HGGPRIV-0540-4890-0018.root") {
 
  TString fullName = dir+TString("/") +fileName;
  TFile*  file     = TFile::Open(fullName.Data());
  TTree*  tree     = (TTree*) file->FindObjectAny("Events");
  std::cout << tree->GetEntries() << std::endl;
}
