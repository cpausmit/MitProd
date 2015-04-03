// $Id: runSimpleFileCataloger.C,v 1.2 2012/03/29 23:41:59 paus Exp $

#if !defined(__CLING__) || defined(__ROOTCLING__)
#include <TROOT.h>
#include <TSystem.h>
#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TBasket.h>
#include <TCanvas.h>
#include <TPie.h>
#endif

const TString slash      = "/";
const TString hadoopDoor = "root://xrootd.cmsaf.mit.edu/";

void analyzeBranchSizes(const char *dir, const char *file);

//--------------------------------------------------------------------------------------------------
void runSizeAnalyzer(const char *dir  = "/mnt/hadoop/cmsprod/filefi/025/r11b-pho-j16-v1",
		     const char *file = "FA6DDCFE-1D43-E111-8E71-002590200900.root")
{
  // -----------------------------------------------------------------------------------------------
  // This script runs a full cataloging action on the given directory
  // -----------------------------------------------------------------------------------------------
  analyzeBranchSizes(dir,file);
  return;
}

//--------------------------------------------------------------------------------------------------
Long64_t RawBytes(TBranch* branch, UInt_t recursion=0) 
{
  Long64_t   totBytes    = branch->GetTotBytes();
  TObjArray *subBranches = branch->GetListOfBranches();
  for (Int_t i=0; i<subBranches->GetEntries(); ++i) {
    TBranch *subBranch = (TBranch*)subBranches->At(i);
    totBytes += RawBytes(subBranch, recursion+1);
  }
  return totBytes;
}

//--------------------------------------------------------------------------------------------------
Long64_t ZipBytes(TBranch* branch, Double_t ratio, UInt_t recursion=0) 
{
  //TBasket* currentBasket = branch->GetBasket(branch->GetReadBasket());
  //currentBasket->SetWriteMode();
  //branch->WriteBasket(currentBasket,0);

  Long64_t totBytes = branch->GetZipBytes();
  TObjArray *subBranches = branch->GetListOfBranches();
  for (Int_t i=0; i<subBranches->GetEntries(); ++i) {
    TBranch *subBranch = (TBranch*) subBranches->At(i);
    totBytes += ZipBytes(subBranch,ratio,recursion+1);
  }
  return totBytes;
}

//--------------------------------------------------------------------------------------------------
void analyzeBranchSizes(const char *dir, const char *file)
{
  TString fileName = TString(dir) + slash +  + TString(file);
  if (fileName.Index("castor/cern.ch") != -1)
    fileName = TString("castor:") + fileName;
  if (fileName.Index("mnt/hadoop/cms/store") != -1) {
    fileName.Remove(0,15);
    fileName = hadoopDoor + fileName;
  }
  
  TFile* f       = TFile::Open(fileName.Data());
  TTree* tree    = (TTree*) f->FindObjectAny("Events");
  printf("XX-CATALOG-XX %s %lld\n",fileName.Data(),tree->GetEntries());

  Long64_t evtRawSize = tree->GetTotBytes();
  Long64_t evtZipSize = tree->GetZipBytes();

  Double_t evtComp    = (Double_t) evtRawSize/evtZipSize;

  TObjArray *evtBranches = tree->GetListOfBranches();
  UInt_t     nBranches   = evtBranches->GetEntries();

  Long64_t   rawTotal    = 0;
  Long64_t   zipTotal    = 0;

  printf("\nSizes in bytes\n");
 printf(  "===============\n");
  for (UInt_t i=0; i<nBranches; ++i) {
    TBranch* branch        = (TBranch*) evtBranches->At(i);
    Long64_t branchSizeZip = ZipBytes(branch, evtComp);
    Long64_t branchSizeRaw = RawBytes(branch);
    
    printf("%-40s Size = %10lld %10lld\n",branch->GetName(),branchSizeRaw,branchSizeZip);

    rawTotal += branchSizeRaw;
    zipTotal += branchSizeZip;

  }

 printf("\nSizes relative to total in percent\n");
 printf(  "==================================\n");
  for (UInt_t i=0; i<nBranches; ++i) {
    TBranch* branch        = (TBranch*) evtBranches->At(i);
    Long64_t branchSizeZip = ZipBytes(branch, evtComp);
    Long64_t branchSizeRaw = RawBytes(branch);
    
    printf("%-40s Size = %10f %10f\n",branch->GetName(),
	   100*double(branchSizeRaw)/double(rawTotal),
	   100*double(branchSizeZip)/double(zipTotal));
  }


  TTree* allTree = (TTree*) f->FindObjectAny("AllEvents");
  if (allTree)
    printf("XX-CATALOG-XX %s %lld %lld\n",fileName.Data(),tree->GetEntries(),allTree->GetEntries());
  else
    printf("\n WARNING -- All event tree not found in this file.\n\n");
}
