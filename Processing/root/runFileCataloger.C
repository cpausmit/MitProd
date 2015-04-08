#if !defined(__CLING__) || defined(__ROOTCLING__)
#include <TROOT.h>
#include <TSystem.h>
#include "MitAna/DataUtil/interface/Debug.h"
#include "MitAna/Catalog/interface/Catalog.h"
#include "MitAna/Catalog/interface/Dataset.h"
#include "MitAna/TreeMod/interface/AnaFwkMod.h"
//#include "MitAna/TreeMod/interface/HLTMod.h"
#include "MitAna/TreeMod/interface/CatalogingMod.h"
#include "MitAna/TreeMod/interface/Analysis.h"
#endif

using namespace mithep;
const TString slash      = "/";
const TString hadoopDoor = "root://xrootd.cmsaf.mit.edu/";

void catalogFile(const char *dir, const char *file);
void reset();

Analysis      *gAna(0);
//HLTMod        *hMod(0);
CatalogingMod *gMod(0);

//--------------------------------------------------------------------------------------------------
void runFileCataloger(const char *dir = "/mnt/hadoop/cms/store/user/paus/filefi/040/RelValProdZEE_13+CMSSW_7_4_0_pre8-PUpmx50ns_MCRUN2_74_V6-v1+AODSIM/crab_0_150406_153710_0001",
		      const char *file = "bambu-output-file-tmp_000_1_1_rUz.root")
{
  // -----------------------------------------------------------------------------------------------
  // This script runs a full cataloging action on the given directory
  // -----------------------------------------------------------------------------------------------
  gDebugMask        = Debug::kNone;
  gDebugLevel       = 0;
  gErrorIgnoreLevel = kWarning;

  reset();
  catalogFile(dir,file);
  return;
}

//--------------------------------------------------------------------------------------------------
void catalogFile(const char *dir, const char *file)
{
  // set up the modules
  gMod->SetMetaDataString((TString(dir)+slash+TString(file)).Data());
  gMod->SetNFileSet(0);

  // set up analysis
  //hMod->Add(gMod);
  //gAna->SetSuperModule(hMod);

  gAna->SetSuperModule(gMod);
  
  TString fileName = TString(dir) + slash +  + TString(file);
  //printf("Index: %d\n",fileName.Index("castor/cern.ch"));
  if (fileName.Index("castor/cern.ch") != -1)
    fileName = TString("castor:") + fileName;
  if (fileName.Index("mnt/hadoop/cms/store") != -1) {
    fileName.Remove(0,15);
    fileName = hadoopDoor + fileName;
    gMod->SetMetaDataString(fileName.Data());
  }
  
  printf(" Adding: %s\n",fileName.Data());
  gAna->AddFile(fileName);
  gAna->SetUseHLT(0);
  gAna->SetCacheSize(64*1024*1024);

  // run the analysis after successful initialisation
  gAna->Run(false);
}

//--------------------------------------------------------------------------------------------------
void reset()
{
  // reset pointers
  if (gAna)
    delete gAna;
  gAna = new Analysis();

  //if (hMod)
  //  delete hMod;
  //hMod = new HLTMod();
  //hMod->SetPrintTable(kFALSE);

  if (gMod)
    delete gMod;
  gMod = new CatalogingMod();
}
