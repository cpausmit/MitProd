// $Id: FillerEmbedWeight.cc,v 1.4 2013/05/06 18:27:49 pharris Exp $

#include "MitProd/TreeFiller/interface/FillerEmbedWeight.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataTree/interface/EmbedWeightCol.h"
#include "MitProd/ObjectService/interface/ObjectService.h"
#include "SimDataFormats/GeneratorProducts/interface/GenFilterInfo.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerEmbedWeight::FillerEmbedWeight(const ParameterSet &cfg, const char *name, bool active) : 
  BaseFiller(cfg,name,active),
  edmName_       (Conf().getUntrackedParameter<string>("edmName"       ,"generator_weight")),
  edmSpinnerName_(Conf().getUntrackedParameter<string>("edmSpinnerName","TauSpinnerRec")),
  edmMuEffName_  (Conf().getUntrackedParameter<string>("edmMuEffName"  ,"ZmumuEvtSelEffCorrWeightProducer")),
  edmMuRadName_  (Conf().getUntrackedParameter<string>("edmMuRadName"  ,"muonRadiationCorrWeightProducer")),
  edmKineReweightGenName_ (Conf().getUntrackedParameter<string>("edmKineReweightGenName"  ,"embeddingKineReweightGENembedding")),
  edmKineReweightRecName_ (Conf().getUntrackedParameter<string>("edmKineReweightRecName"  ,"embeddingKineReweightRECembedding")),

  genInfo_(Conf().getUntrackedParameter<bool>  ("useGenInfo","True")),
  recHit_ (Conf().getUntrackedParameter<bool>  ("useRecHit","True")),
  useMuRad_ (Conf().getUntrackedParameter<bool>  ("useMuonRad","True")),
  mitName_(Conf().getUntrackedParameter<string>("mitName",Names::gkEmbedWeightBrn)),
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

  if(!genInfo_) { 
    Handle<double> hEmbedWeight;
    event.getByLabel(edm::InputTag(edmName_,"weight","EmbeddedRECO"),hEmbedWeight);
    inEmbedWeightValue = *(hEmbedWeight.product());  
  } else { 
    edm::Handle<GenFilterInfo> hGenFilterInfo;
    event.getByLabel(edm::InputTag(edmName_, "minVisPtFilter", "EmbeddedRECO"), hGenFilterInfo);
    inEmbedWeightValue = hGenFilterInfo->filterEfficiency();
  }    
  if(recHit_) { 
    Handle<double> hSpinnerWeight;
    event.getByLabel(edm::InputTag(edmSpinnerName_,"TauSpinnerWT","EmbeddedSPIN"),hSpinnerWeight);
    inSpinnerWeightValue = *(hSpinnerWeight.product());  

    Handle<double> hSpinnerWeightFlip;
    event.getByLabel(edm::InputTag(edmSpinnerName_,"TauSpinnerWTFlip","EmbeddedSPIN"),hSpinnerWeightFlip);
    inSpinnerFlipWeightValue = *(hSpinnerWeightFlip.product());  

    Handle<double> hSpinnerWeightMinus;
    event.getByLabel(edm::InputTag(edmSpinnerName_,"TauSpinnerWThplus","EmbeddedSPIN"),hSpinnerWeightMinus);
    inSpinnerMinusWeightValue = *(hSpinnerWeightMinus.product());  

    Handle<double> hSpinnerWeightPlus;
    event.getByLabel(edm::InputTag(edmSpinnerName_,"TauSpinnerWThminus","EmbeddedSPIN"),hSpinnerWeightPlus);
    inSpinnerPlusWeightValue = *(hSpinnerWeightPlus.product());  

    Handle<double> hMuEffWeight;
    event.getByLabel(edm::InputTag(edmMuEffName_,"weight","EmbeddedRECO"),hMuEffWeight);
    inMuEffWeightValue = *(hMuEffWeight.product());  

    Handle<double> hMuEffWeightUp;
    event.getByLabel(edm::InputTag(edmMuEffName_,"weightUp","EmbeddedRECO"),hMuEffWeightUp);
    inMuEffWeightUpValue = *(hMuEffWeightUp.product());  

    Handle<double> hMuEffWeightDown;
    event.getByLabel(edm::InputTag(edmMuEffName_,"weightDown","EmbeddedRECO"),hMuEffWeightDown);
    inMuEffWeightDownValue = *(hMuEffWeightDown.product());  
    
    if(useMuRad_) { 
      Handle<double> hMuRadWeight;
      event.getByLabel(edm::InputTag(edmMuRadName_,"weight","EmbeddedRECO"),hMuRadWeight);
      inMuRadWeightValue = *(hMuRadWeight.product());  
      
      Handle<double> hMuRadWeightUp;
      event.getByLabel(edm::InputTag(edmMuRadName_,"weightUp","EmbeddedRECO"),hMuRadWeightUp);
      inMuRadWeightUpValue = *(hMuRadWeightUp.product());  
      
      Handle<double> hMuRadWeightDown;
      event.getByLabel(edm::InputTag(edmMuRadName_,"weightDown","EmbeddedRECO"),hMuRadWeightDown);
      inMuRadWeightDownValue = *(hMuRadWeightDown.product());  
    }
    Handle<double> hGenTau2PtVsGenTau1PtGen;
    event.getByLabel(edm::InputTag(edmKineReweightGenName_,"genTau2PtVsGenTau1Pt"),hGenTau2PtVsGenTau1PtGen);
    inGenTau2PtVsGenTau1PtGenValue = *(hGenTau2PtVsGenTau1PtGen.product());  

    Handle<double> hGenTau2EtaVsGenTau1EtaGen;
    event.getByLabel(edm::InputTag(edmKineReweightGenName_,"genTau2EtaVsGenTau1Eta"),hGenTau2EtaVsGenTau1EtaGen);
    inGenTau2EtaVsGenTau1EtaGenValue = *(hGenTau2EtaVsGenTau1EtaGen.product());  

    Handle<double> hDiTauMassVsGenDiTauPtGen;
    event.getByLabel(edm::InputTag(edmKineReweightGenName_,"genDiTauMassVsGenDiTauPt"),hDiTauMassVsGenDiTauPtGen);
    inDiTauMassVsGenDiTauPtGenValue = *(hDiTauMassVsGenDiTauPtGen.product());  

    Handle<double> hGenTau2PtVsGenTau1PtRec;
    event.getByLabel(edm::InputTag(edmKineReweightRecName_,"genTau2PtVsGenTau1Pt"),hGenTau2PtVsGenTau1PtRec);
    inGenTau2PtVsGenTau1PtRecValue = *(hGenTau2PtVsGenTau1PtRec.product());  

    Handle<double> hGenTau2EtaVsGenTau1EtaRec;
    event.getByLabel(edm::InputTag(edmKineReweightRecName_,"genTau2EtaVsGenTau1Eta"),hGenTau2EtaVsGenTau1EtaRec);
    inGenTau2EtaVsGenTau1EtaRecValue = *(hGenTau2EtaVsGenTau1EtaRec.product());  

    Handle<double> hDiTauMassVsGenDiTauPtRec;
    event.getByLabel(edm::InputTag(edmKineReweightRecName_,"genDiTauMassVsGenDiTauPt"),hDiTauMassVsGenDiTauPtRec);
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
  
  mithep::EmbedWeight *embedWeight = embedWeight_->Allocate();
  new (embedWeight) mithep::EmbedWeight();

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
