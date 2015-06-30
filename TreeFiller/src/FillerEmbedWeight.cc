#include "MitProd/TreeFiller/interface/FillerEmbedWeight.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataTree/interface/EmbedWeightCol.h"
#include "MitProd/ObjectService/interface/ObjectService.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerEmbedWeight::FillerEmbedWeight(const ParameterSet &cfg, edm::ConsumesCollector& collector, ObjectService* os, const char *name, bool active) : 
  BaseFiller(cfg,os,name,active),
  genInfo_(cfg.getUntrackedParameter<bool>  ("useGenInfo", true)),
  recHit_ (cfg.getUntrackedParameter<bool>  ("useRecHit", true)),
  useMuRad_ (cfg.getUntrackedParameter<bool>  ("useMuonRad", true)),
  genWeightToken_(GetToken<double>(collector, cfg, "genWeightName", !genInfo_)), //generator_weight:weight:EmbeddedRECO
  genInfoToken_(GetToken<GenFilterInfo>(collector, cfg, "genInfoName", genInfo_)), //generator_weight:minVisPtFilter:EmbeddedRECO
  spinnerWeightToken_(GetToken<double>(collector, cfg, "spinnerWeightName", recHit_)), //TauSpinnerRec:TauSpinnerWT:EmbeddedSPIN
  spinnerWeightFlipToken_(GetToken<double>(collector, cfg, "spinnerWeightFlipName", recHit_)), //TauSpinnerRec:TauSpinnerWTFlip:EmbeddedSPIN
  spinnerWeightMinusToken_(GetToken<double>(collector, cfg, "spinnerWeightMinusName", recHit_)), //TauSpinnerRec:TauSpinnerWThplus:EmbeddedSPIN
  spinnerWeightPlusToken_(GetToken<double>(collector, cfg, "spinnerWeightPlusName", recHit_)), //TauSpinnerRec:TauSpinnerWThminus:EmbeddedSPIN
  muEffWeightToken_(GetToken<double>(collector, cfg, "muEffWeightName", recHit_)), //ZmumuEvtSelEffCorrWeightProducer:weight:EmbeddedRECO
  muEffWeightUpToken_(GetToken<double>(collector, cfg, "muEffWeightUpName", recHit_)), //ZmumuEvtSelEffCorrWeightProducer:weightUp:EmbeddedRECO
  muEffWeightDownToken_(GetToken<double>(collector, cfg, "muEffWeightDownName", recHit_)), //ZmumuEvtSelEffCorrWeightProducer:weightDown:EmbeddedRECO
  muRadWeightToken_(GetToken<double>(collector, cfg, "muRadWeightName", recHit_ && useMuRad_)), //muonRadiationCorrWeightProducer:weight:EmbeddedRECO
  muRadWeightUpToken_(GetToken<double>(collector, cfg, "muRadWeightUpName", recHit_ && useMuRad_)), //muonRadiationCorrWeightProducer:weightUp:EmbeddedRECO
  muRadWeightDownToken_(GetToken<double>(collector, cfg, "muRadWeightDownName", recHit_ && useMuRad_)), //muonRadiationCorrWeightProducer:weightDown:EmbeddedRECO
  genTau2PtVsGenTau1PtGenToken_(GetToken<double>(collector, cfg, "genTau2PtVsGenTau1PtGenName", recHit_)), //embeddingKineReweightGENembedding:genTau2PtVsGenTau1Pt
  genTau2EtaVsGenTau1EtaGenToken_(GetToken<double>(collector, cfg, "genTau2EtaVsGenTau1EtaGenName", recHit_)), //embeddingKineReweightGENembedding:genTau2EtaVsGenTau1Eta
  diTauMassVsGenDiTauPtGenToken_(GetToken<double>(collector, cfg, "diTauMassVsGenDiTauPtGenName", recHit_)), //embeddingKineReweightGENembedding:genDiTauMassVsGenDiTauPt
  genTau2PtVsGenTau1PtRecToken_(GetToken<double>(collector, cfg, "genTau2PtVsGenTau1PtRecName", recHit_)), //embeddingKineReweightRECembedding:genTau2PtVsGenTau1Pt
  genTau2EtaVsGenTau1EtaRecToken_(GetToken<double>(collector, cfg, "genTau2EtaVsGenTau1EtaRecName", recHit_)), //embeddingKineReweightRECembedding:genTau2EtaVsGenTau1Eta
  diTauMassVsGenDiTauPtRecToken_(GetToken<double>(collector, cfg, "diTauMassVsGenDiTauPtRecName", recHit_)),  //embeddingKineReweightRECembedding:genDiTauMassVsGenDiTauPt
  mitName_(cfg.getUntrackedParameter<string>("mitName",Names::gkEmbedWeightBrn)),
  embedWeight_(new mithep::EmbedWeightArr)
{
  // Constructor.
}

//--------------------------------------------------------------------------------------------------
FillerEmbedWeight::~FillerEmbedWeight()
{
  // Destructor.

   delete embedWeight_;
}

//--------------------------------------------------------------------------------------------------
void FillerEmbedWeight::BookDataBlock(TreeWriter &tws)
{
  // Add mets branch to tree.

  tws.AddBranch(mitName_,&embedWeight_);
  OS()->add<mithep::EmbedWeightArr>(embedWeight_,mitName_);
}

//--------------------------------------------------------------------------------------------------
void FillerEmbedWeight::FillDataBlock(const edm::Event      &event, 
				      const edm::EventSetup &setup)  
{
  // Fill missing energy from edm collection into our collection.
  embedWeight_->Delete();
  
  double inEmbedWeightValue              = 1;
  double inSpinnerWeightValue            = 1;
  double inSpinnerFlipWeightValue        = 1;
  double inSpinnerPlusWeightValue        = 1;
  double inSpinnerMinusWeightValue       = 1;
  double inMuEffWeightValue              = 1;
  double inMuEffWeightUpValue            = 1;
  double inMuEffWeightDownValue          = 1;
  double inMuRadWeightValue              = 1;
  double inMuRadWeightUpValue            = 1;
  double inMuRadWeightDownValue          = 1;
  double inGenTau2PtVsGenTau1PtGenValue  = 1;
  double inGenTau2EtaVsGenTau1EtaGenValue = 1;
  double inDiTauMassVsGenDiTauPtGenValue = 1;
  double inGenTau2PtVsGenTau1PtRecValue  = 1;
  double inGenTau2EtaVsGenTau1EtaRecValue= 1;
  double inDiTauMassVsGenDiTauPtRecValue = 1;

  if(genInfo_) { 
    Handle<GenFilterInfo> hGenFilterInfo;
    GetProduct(genInfoToken_, hGenFilterInfo, event);
    inEmbedWeightValue = hGenFilterInfo->filterEfficiency();
  } else { 
    Handle<double> hEmbedWeight;
    GetProduct(genWeightToken_, hEmbedWeight, event);
    inEmbedWeightValue = *(hEmbedWeight.product());  
  }    
  if(recHit_) { 
    Handle<double> hSpinnerWeight;
    GetProduct(spinnerWeightToken_, hSpinnerWeight, event);
    inSpinnerWeightValue = *(hSpinnerWeight.product());  

    Handle<double> hSpinnerWeightFlip;
    GetProduct(spinnerWeightFlipToken_, hSpinnerWeightFlip, event);
    inSpinnerFlipWeightValue = *(hSpinnerWeightFlip.product());  

    Handle<double> hSpinnerWeightMinus;
    GetProduct(spinnerWeightMinusToken_, hSpinnerWeightMinus, event);
    inSpinnerMinusWeightValue = *(hSpinnerWeightMinus.product());  

    Handle<double> hSpinnerWeightPlus;
    GetProduct(spinnerWeightPlusToken_, hSpinnerWeightPlus, event);
    inSpinnerPlusWeightValue = *(hSpinnerWeightPlus.product());  

    Handle<double> hMuEffWeight;
    GetProduct(muEffWeightToken_, hMuEffWeight, event);
    inMuEffWeightValue = *(hMuEffWeight.product());  

    Handle<double> hMuEffWeightUp;
    GetProduct(muEffWeightUpToken_, hMuEffWeightUp, event);
    inMuEffWeightUpValue = *(hMuEffWeightUp.product());  

    Handle<double> hMuEffWeightDown;
    GetProduct(muEffWeightDownToken_, hMuEffWeightDown, event);
    inMuEffWeightDownValue = *(hMuEffWeightDown.product());  
    
    if(useMuRad_) { 
      Handle<double> hMuRadWeight;
      GetProduct(muRadWeightToken_, hMuRadWeight, event);
      inMuRadWeightValue = *(hMuRadWeight.product());  
      
      Handle<double> hMuRadWeightUp;
      GetProduct(muRadWeightUpToken_, hMuRadWeightUp, event);
      inMuRadWeightUpValue = *(hMuRadWeightUp.product());  
      
      Handle<double> hMuRadWeightDown;
      GetProduct(muRadWeightDownToken_, hMuRadWeightDown, event);
      inMuRadWeightDownValue = *(hMuRadWeightDown.product());  
    }
    Handle<double> hGenTau2PtVsGenTau1PtGen;
    GetProduct(genTau2PtVsGenTau1PtGenToken_, hGenTau2PtVsGenTau1PtGen, event);
    inGenTau2PtVsGenTau1PtGenValue = *(hGenTau2PtVsGenTau1PtGen.product());  

    Handle<double> hGenTau2EtaVsGenTau1EtaGen;
    GetProduct(genTau2EtaVsGenTau1EtaGenToken_, hGenTau2EtaVsGenTau1EtaGen, event);
    inGenTau2EtaVsGenTau1EtaGenValue = *(hGenTau2EtaVsGenTau1EtaGen.product());  

    Handle<double> hDiTauMassVsGenDiTauPtGen;
    GetProduct(diTauMassVsGenDiTauPtGenToken_, hDiTauMassVsGenDiTauPtGen, event);
    inDiTauMassVsGenDiTauPtGenValue = *(hDiTauMassVsGenDiTauPtGen.product());  

    Handle<double> hGenTau2PtVsGenTau1PtRec;
    GetProduct(genTau2PtVsGenTau1PtRecToken_, hGenTau2PtVsGenTau1PtRec, event);
    inGenTau2PtVsGenTau1PtRecValue = *(hGenTau2PtVsGenTau1PtRec.product());  

    Handle<double> hGenTau2EtaVsGenTau1EtaRec;
    GetProduct(genTau2EtaVsGenTau1EtaRecToken_, hGenTau2EtaVsGenTau1EtaRec, event);
    inGenTau2EtaVsGenTau1EtaRecValue = *(hGenTau2EtaVsGenTau1EtaRec.product());  

    Handle<double> hDiTauMassVsGenDiTauPtRec;
    GetProduct(diTauMassVsGenDiTauPtRecToken_, hDiTauMassVsGenDiTauPtRec, event);
    inDiTauMassVsGenDiTauPtRecValue = *(hDiTauMassVsGenDiTauPtRec.product());  
  }
  const double inEmbedWeight        = inEmbedWeightValue;
  const double inSpinnerWeight      = inSpinnerWeightValue;
  const double inSpinnerFlipWeight  = inSpinnerFlipWeightValue;
  const double inSpinnerPlusWeight  = inSpinnerPlusWeightValue;
  const double inSpinnerMinusWeight = inSpinnerMinusWeightValue;
  const double inMuEffWeight        = inMuEffWeightValue;
  const double inMuEffWeightUp      = inMuEffWeightUpValue;
  const double inMuEffWeightDown    = inMuEffWeightDownValue;
  const double inMuRadWeight        = inMuRadWeightValue;
  const double inMuRadWeightUp      = inMuRadWeightUpValue;
  const double inMuRadWeightDown    = inMuRadWeightDownValue;
  const double inGenTau2PtVsGenTau1PtGen        = inGenTau2PtVsGenTau1PtGenValue;
  const double inGenTau2EtaVsGenTau1EtaGen      = inGenTau2EtaVsGenTau1EtaGenValue;
  const double inDiTauMassVsGenDiTauPtGen       = inDiTauMassVsGenDiTauPtGenValue;
  const double inGenTau2PtVsGenTau1PtRec        = inGenTau2PtVsGenTau1PtRecValue;
  const double inGenTau2EtaVsGenTau1EtaRec      = inGenTau2EtaVsGenTau1EtaRecValue;
  const double inDiTauMassVsGenDiTauPtRec       = inDiTauMassVsGenDiTauPtRecValue;
  
  mithep::EmbedWeight *embedWeight = embedWeight_->AddNew();

  embedWeight->SetGenWeight                (inEmbedWeight);
  embedWeight->SetSpinnerWeight            (inSpinnerWeight);
  embedWeight->SetSpinnerFlipWeight        (inSpinnerFlipWeight);
  embedWeight->SetSpinnerPlusWeight        (inSpinnerPlusWeight);
  embedWeight->SetSpinnerMinusWeight       (inSpinnerMinusWeight);
  embedWeight->SetMuEffWeight              (inMuEffWeight);
  embedWeight->SetMuEffWeightUp            (inMuEffWeightUp);
  embedWeight->SetMuEffWeightDown          (inMuEffWeightDown);
  embedWeight->SetMuRadWeight              (inMuRadWeight);
  embedWeight->SetMuRadWeightUp            (inMuRadWeightUp);
  embedWeight->SetMuRadWeightDown          (inMuRadWeightDown);
  embedWeight->SetGenTau2VsGenTau1PtGen    (inGenTau2PtVsGenTau1PtGen);
  embedWeight->SetGenTau2VsGenTau1EtaGen   (inGenTau2EtaVsGenTau1EtaGen);
  embedWeight->SetDiTauMassVsGenDiTauPtGen (inDiTauMassVsGenDiTauPtGen);
  embedWeight->SetGenTau2VsGenTau1PtRec    (inGenTau2PtVsGenTau1PtRec);
  embedWeight->SetGenTau2VsGenTau1EtaRec   (inGenTau2EtaVsGenTau1EtaRec);
  embedWeight->SetDiTauMassVsGenDiTauPtRec (inDiTauMassVsGenDiTauPtRec);
  if(recHit_) embedWeight->SetWeight();
  embedWeight_->Trim();
}

DEFINE_MITHEP_TREEFILLER(FillerEmbedWeight);
