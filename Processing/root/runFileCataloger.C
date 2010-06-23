// $Id: runFileCataloger.C,v 1.1.2.1 2010/06/05 01:49:22 paus Exp $

#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TROOT.h>
#include <TSystem.h>
#include "MitAna/DataUtil/interface/Debug.h"
#include "MitAna/Catalog/interface/Catalog.h"
#include "MitAna/Catalog/interface/Dataset.h"
#include "MitAna/TreeMod/interface/CatalogingMod.h"
#include "MitAna/TreeMod/interface/Analysis.h"
#endif

using namespace mithep;
const TString slash = "/";

void catalogFile(const char *dir, const char *file);
void reset();

Analysis      *gAna(0);
CatalogingMod *gMod(0);

//--------------------------------------------------------------------------------------------------
void runFileCataloger(const char *dir = "/castor/cern.ch/user/p/paus/filler/004/s8-qcddj_15_20-id9",
		      const char *file = "s8-qcddj_15_20-id9_000_1.root")
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
  gMod->SetNFileSet      (0);

  // set up analysis
  gAna->SetSuperModule(gMod);
  
  TString fileName = TString(dir) + slash +  + TString(file);
  printf("Index: %d\n",fileName.Index("castor/cern.ch"));
  if (fileName.Index("castor/cern.ch") != -1)
    fileName = TString("castor:") + fileName;
  if (fileName.Index("pnfs/cmsaf.mit.edu") != -1) {
    fileName = TString("dcap://t2srv0005.cmsaf.mit.edu/") + fileName;
    gMod->SetMetaDataString(fileName.Data());
  }
  
  printf(" Adding: %s\n",fileName.Data());
  gAna->AddFile(fileName);
  gAna->SetUseHLT(0);

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

  if (gMod)
    delete gMod;
  gMod = new CatalogingMod();
}