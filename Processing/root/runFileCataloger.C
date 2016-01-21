#if !defined(__CLING__) || defined(__ROOTCLING__)
#include <TROOT.h>
#include <TSystem.h>
#include "MitAna/DataUtil/interface/Debug.h"
#include "MitAna/Catalog/interface/Catalog.h"
#include "MitAna/Catalog/interface/Dataset.h"
#include "MitAna/TreeMod/interface/AnaFwkMod.h"
#include "MitAna/TreeMod/interface/CatalogingMod.h"
#include "MitAna/TreeMod/interface/Analysis.h"
#endif

using namespace mithep;
const TString slash      = "/";
const TString hadoopDoor = "root://xrootd.cmsaf.mit.edu/";

void catalogFile(const char *dir, const char *file);
void reset();

Analysis      *gAna(0);
CatalogingMod *gMod(0);

//--------------------------------------------------------------------------------------------------
void runFileCataloger(const char *dir = "/mnt/hadoop/cms/store/user/paus/filefi/040/RelValProdZEE_13+CMSSW_7_4_0_pre8-PUpmx50ns_MCRUN2_74_V6-v1+AODSIM",
		      const char *file = "EED02C28-0EC0-E411-8A45-002618943919.root")
{
  // -----------------------------------------------------------------------------------------------
  // This script runs a full cataloging action on the given directory/file
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
  // setting up the cataloging details

  // define the file name
  TString fileName = TString(dir) + slash +  + TString(file);

  // set up the modules
  gMod->SetMetaDataString(fileName.Data());
  gMod->SetNFileSet(0);
  
  // make sure we find out what the 'real' file location is
  if (fileName.Index("castor/cern.ch") != -1)
    fileName = TString("castor:") + fileName;
  if (fileName.Index("mnt/hadoop/cms/store") != -1) {
    fileName.Remove(0,15);
    fileName = hadoopDoor + fileName;
    gMod->SetMetaDataString(fileName.Data());
  }
  
  // adding the file to the analysis
  printf(" Adding: %s\n",fileName.Data());
  gAna->AddFile(fileName);
  gAna->SetUseHLT(0);                                              // this is crucial, no HLT please

  // run the analysis after successful initialisation
  gAna->Run(false);
}

//--------------------------------------------------------------------------------------------------
void reset()
{
  // reset pointers

  // declare the analysis
  if (gAna)
    delete gAna;
  gAna = new Analysis();

  // make the cataloging module
  if (gMod)
    delete gMod;
  gMod = new CatalogingMod();

  // declare the super module
  gAna->SetSuperModule(gMod);
}
