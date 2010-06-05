// $Id: rootlogon.C,v 1.1.2.1 2010/06/05 01:49:22 paus Exp $
{
  if (gSystem->Getenv("CMSSW_VERSION")) {
    TString str = gSystem->GetMakeSharedLib();
    if (str.Contains("-m32")==0 && str.Contains("-m64")==0) {
      str.ReplaceAll("g++", "g++ -m32");
      gSystem->SetMakeSharedLib(str);
    }
  }
  
  gROOT->Macro("$CMSSW_BASE/src/MitAna/macros/setRootEnv.C+");
  //loadmylib("MitHiggs","HwwMods");
  //loadmylib("MitHiggs","EwkMods");
  //loadmylib("MitHiggs","FakeRateMods");

  // gSystem->AddIncludePath("-I$CMSSW_BASE/src/MitDiMuon/macros");
  // gInterpreter->AddIncludePath(TString(gSystem->Getenv("CMSSW_BASE"))+"/src/MitDiMuon/macros");
  // gROOT->SetMacroPath(TString(gROOT->GetMacroPath())
  //                     +TString(gSystem->Getenv("CMSSW_BASE"))+"/src/MitDiMuon/macros");
  // gROOT->Macro("MitStyle.C+");
  // gSystem->Load("$CMSSW_BASE/lib/slc4_ia32_gcc345/libMitDiMuonTools.so");
  // gSystem->Load("$CMSSW_BASE/lib/slc4_ia32_gcc345/libMitDiMuonMods.so");
  // gSystem->Load("$CMSSW_BASE/lib/slc4_ia32_gcc345/libMitElectronFakesTools.so");
  // gSystem->Load("$CMSSW_BASE/lib/slc4_ia32_gcc345/libMitElectronFakesMods.so");
  // gSystem->Load("$CMSSW_BASE/lib/slc4_ia32_gcc345/libMitHiggsEwkMods.so");
  // gSystem->Load("$CMSSW_BASE/lib/slc4_ia32_gcc345/libMitHiggsFakeRateMods.so");
  // gSystem->Load("$CMSSW_BASE/lib/slc4_ia32_gcc345/libMitHiggsHwwMods.so");
}
