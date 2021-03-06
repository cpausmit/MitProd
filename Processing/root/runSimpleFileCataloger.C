// $Id: runSimpleFileCataloger.C,v 1.2 2012/03/29 23:41:59 paus Exp $

#if !defined(__CLING__) || defined(__ROOTCLING__)
#include <TROOT.h>
#include <TSystem.h>
#endif

const TString slash      = "/";
const TString hadoopDoor = "root://xrootd.cmsaf.mit.edu/";

void catalogFile(const char *dir, const char *file);
void reset();

//--------------------------------------------------------------------------------------------------
void runSimpleFileCataloger(const char *dir  = "/mnt/hadoop/cms/store/user/paus/filefi/044/GJets_DR-0p4_HT-200To400_TuneCUETP8M1_13TeV-madgraphMLM-pythia8+RunIISpring16DR80-PUSpring16_80X_mcRun2_asymptotic_2016_v3-v1+AODSIM/crab_0_160517-163911",
			    const char *file = "22E85412-3F12-E611-8BFA-008CFAF0682E_tmp.root")
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
  TTree *tree = 0, *allTree = 0;

  TString fileName = TString(dir) + slash +  + TString(file);
  if      (fileName.Index("/mnt/hadoop/cms/store") != -1) {
    fileName.Remove(0,15);
    fileName = hadoopDoor + fileName;
  }
  else if (fileName.Index("/cms/store") != -1) {
    fileName.Remove(0,4);
    fileName = hadoopDoor + fileName;
  }
  
  printf("\n Opening: %s\n\n",fileName.Data());
  TFile* f       = TFile::Open(fileName.Data());

  tree = (TTree*) f->FindObjectAny("Delphes");
  if (tree) {
    printf("0000 %s %lld %lld\n",fileName.Data(),tree->GetEntries(),tree->GetEntries());
    return;
  }

  tree    = (TTree*) f->FindObjectAny("events");
  allTree  = (TTree*) f->FindObjectAny("all");
  if (tree && allTree) {
    printf("XX-CATALOG-XX 0000 %s %lld %lld %d %d %d %d\n",
	   fileName.Data(),tree->GetEntries(),allTree->GetEntries(),1,1,1,1);
    return;
  }

  if (tree) {
    printf("XX-CATALOG-XX 0000 %s %lld %lld %d %d %d %d\n",
	   fileName.Data(),tree->GetEntries(),tree->GetEntries(),1,1,1,1);
    return;
  }

  tree    = (TTree*) f->FindObjectAny("Events");
  if (tree)
    printf("XX-CATALOG-XX 0000 %s %lld %lld %d %d %d %d\n",
	   fileName.Data(),tree->GetEntries(),tree->GetEntries(),1,1,1,1);

  allTree = (TTree*) f->FindObjectAny("AllEvents");
  if (tree && allTree)
    printf("XX-CATALOG-XX 0000 %s %lld %lld %d %d %d %d\n",
	   fileName.Data(),tree->GetEntries(),allTree->GetEntries(),1,1,1,1);
}

//--------------------------------------------------------------------------------------------------
void reset()
{
}
