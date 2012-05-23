#include "MitProd/TreeFiller/interface/FillerDCASig.h"
#include "DataFormats/Common/interface/RefToPtr.h"
#include "MitEdm/DataFormats/interface/RefToBaseToPtr.h"
#include "MitAna/DataTree/interface/DCASigCol.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitProd/ObjectService/interface/ObjectService.h"

#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "TrackingTools/Records/interface/TransientRecHitRecord.h"
#include "TrackingTools/TransientTrack/interface/TransientTrack.h"
#include "TrackingTools/PatternTools/interface/Trajectory.h"
#include "TrackingTools/PatternTools/interface/TrajectoryMeasurement.h"
#include "TrackingTools/PatternTools/interface/ClosestApproachInRPhi.h"
#include "TrackingTools/PatternTools/interface/TwoTrackMinimumDistance.h"
#include "RecoVertex/KinematicFitPrimitives/interface/KinematicParticleFactoryFromTransientTrack.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerDCASig::FillerDCASig(const ParameterSet &cfg, const char *name, bool active) :
  BaseFiller            (cfg,name,active),
  edmElectronName_      (Conf().getUntrackedParameter<string>("edmElectronName","gsfElectrons")),
  edmMuonName_          (Conf().getUntrackedParameter<string>("edmMuonName",    "muons")),
  edmTauName_           (Conf().getUntrackedParameter<string>("edmTauName",     "hpsPFTauProducer")),
  mitName_              (Conf().getUntrackedParameter<string>("mitName",        "DCASig")),
  electronMapName_      (Conf().getUntrackedParameter<string>("electronMapName","")),
  muonMapName_          (Conf().getUntrackedParameter<string>("muonMapName",    "")),
  tauMapName_           (Conf().getUntrackedParameter<string>("tauMapName",     "")),
  electronPtMin_        (Conf().getUntrackedParameter<double>("electronPtMin",  5.)),
  muonPtMin_            (Conf().getUntrackedParameter<double>("muonPtMin",      5.)),
  tauPtMin_             (Conf().getUntrackedParameter<double>("tauPtMin",       10.)),
  checkDCARef_          (Conf().getUntrackedParameter<bool>("checkDCARef",      "False")),
  DCASigs_              (new mithep::DCASigArr),
  transientTrackBuilder_(0)
{
}

//--------------------------------------------------------------------------------------------------
FillerDCASig::~FillerDCASig()
{
  // Destructor.
  delete DCASigs_;
}

//--------------------------------------------------------------------------------------------------
void FillerDCASig::BookDataBlock(TreeWriter &tws)
{
  // Add DCA to the tree
  tws.AddBranch(mitName_,&DCASigs_);
  OS()->add<mithep::DCASigArr>(DCASigs_,mitName_);

  //Load lepton maps
  if (!electronMapName_.empty()) {
    electronMap_ = OS()->get<ElectronMap>(electronMapName_);
    if (electronMap_)
      AddBranchDep(mitName_,electronMap_->GetBrName());
    else
      printf("\n FillerDCASig::BookDataBlock Electron map (Name: %s) not found!!\n\n",electronMapName_.data());
  }
  if (!muonMapName_.empty()) {
    muonMap_ = OS()->get<MuonMap>(muonMapName_);
    if (muonMap_)
      AddBranchDep(mitName_,muonMap_->GetBrName());
    else
      printf("\n FillerDCASig::BookDataBlock Muon map (Name: %s) not found!!\n\n",muonMapName_.data());
  }
  if (!tauMapName_.empty()) {
    tauMap_ = OS()->get<PFTauMap>(tauMapName_);
    if (tauMap_)
      AddBranchDep(mitName_,tauMap_->GetBrName());
    else
      printf("\n FillerDCASig::BookDataBlock Tau map (Name: %s) not found!!\n\n",tauMapName_.data());
  }
}
//--------------------------------------------------------------------------------------------------
void FillerDCASig::FillDataBlock(const edm::Event      &event,
                                 const edm::EventSetup &setup)
{
  // Fill conversions data structure and maps.

  DCASigs_->Delete();

  //Transient Track Buildder
  edm::ESHandle<TransientTrackBuilder> builder;
  setup.get<TransientTrackRecord>().get("TransientTrackBuilder",builder);
  transientTrackBuilder_ = builder.product();

  //handle for the electron collection
  Handle<reco::GsfElectronCollection> hElectronProduct;
  GetProduct(edmElectronName_, hElectronProduct, event);
  const reco::GsfElectronCollection inElectrons = *(hElectronProduct.product());

  //handle for the muon collection
  Handle<reco::MuonCollection> hMuonProduct;
  GetProduct(edmMuonName_, hMuonProduct, event);
  const reco::MuonCollection inMuons            = *(hMuonProduct.product());

  // handle for the tau collection
  Handle<reco::PFTauCollection> hTauProduct;
  GetProduct(edmTauName_, hTauProduct, event);
  const reco::PFTauCollection inTaus             = *(hTauProduct.product());

  //Declare some variables
  Double_t lDCA3D      = 0;     //3D x-y   DCA
  Double_t lDCA3DE     = 0;     //3D x-y   DCA Err
  Double_t lDCA2D      = 0;     //2D x-y   DCA
  Double_t lDCA2DE     = 0;     //2D x-y   DCA Err
  Double_t lDCARPhi3D  = 0;     //3D r-phi DCA
  Double_t lDCARPhi3DE = 0;     //3D r-phi DCA Err
  Double_t lDCARPhi2D  = 0;     //2D r-phi DCA
  Double_t lDCARPhi2DE = 0;     //2D r-phi DCA Err

  //Loop through the electrons
  for (reco::GsfElectronCollection::const_iterator iElectron = inElectrons.begin();
       iElectron != inElectrons.end(); ++iElectron) {
    if (iElectron->pt() < electronPtMin_)
      continue;

    //E + Mu Objects
    for (reco::MuonCollection::const_iterator iMuon = inMuons.begin(); iMuon != inMuons.end(); ++iMuon) {
      if (iMuon->pt() < muonPtMin_)
        continue;

      //Declare the Object
      const reco::Track * trackElec  = (const reco::Track*)(iElectron->gsfTrack().get()); // electron Track
      const reco::Track * trackMuon  = (const reco::Track*)(iMuon->innerTrack()  .get());   // muon inner track
      if (trackMuon == NULL || trackElec == NULL || trackElec == trackMuon) continue;
      mithep::DCASig *outDCASig      = DCASigs_->AddNew();
      calculateDCA(lDCA3D    ,lDCA3DE    ,lDCA2D    ,lDCA2DE,
                   lDCARPhi3D,lDCARPhi3DE,lDCARPhi2D,lDCARPhi2DE,
                   trackElec,trackMuon,DCASig::eEMu);

      outDCASig->SetDCA2D    (lDCA2D);       outDCASig->SetDCA2DErr    (lDCA2DE);
      outDCASig->SetDCA3D    (lDCA3D);       outDCASig->SetDCA3DErr    (lDCA3DE);
      outDCASig->SetDCA2DRPhi(lDCARPhi2D);   outDCASig->SetDCA2DRPhiErr(lDCARPhi2DE);
      outDCASig->SetDCA3DRPhi(lDCARPhi3D);   outDCASig->SetDCA3DRPhiErr(lDCARPhi3DE);
      outDCASig->SetType(DCASig::eEMu);

      //Now the references
      edm::Ptr<reco::GsfElectron> pElectronPtr(hElectronProduct, iElectron - inElectrons.begin());
      try {
	outDCASig->SetElectron(electronMap_->GetMit(pElectronPtr));
      }
      catch(...) {
        if (checkDCARef_)  throw edm::Exception(edm::errors::Configuration, "FillerDCASig:FillDataBlock()\n")
          << "Error! Electron unmapped collection " << edmElectronName_ << endl;
      }
      edm::Ptr<reco::Muon>        pMuonPtr(hMuonProduct, iMuon - inMuons.begin());
      try {
	outDCASig->SetMuon     (muonMap_->GetMit(pMuonPtr));
      }
      catch(...) {
        if (checkDCARef_)  throw edm::Exception(edm::errors::Configuration, "FillerDCASig:FillDataBlock()\n")
          << "Error! Muon unmapped collection " << edmMuonName_ << endl;
      }
    }

    //E + Tau objects
    for (reco::PFTauCollection::const_iterator iTau = inTaus.begin(); iTau != inTaus.end(); ++iTau) {
      if (iTau->pt() < tauPtMin_)
        continue;

      //declare the Object
      const reco::Track * trackElec = (const reco::Track*)(iElectron->gsfTrack().get()); // electron Track
      if (iTau->leadPFChargedHadrCand().isNull())
        continue;

      const reco::Track * trackTau = (const reco::Track*)(iTau->leadPFChargedHadrCand()->trackRef().get()); // Tau lead track
      if (trackTau == NULL || trackElec == NULL || trackTau == trackElec)
        continue;

      mithep::DCASig *outDCASig = DCASigs_->AddNew();
      calculateDCA(lDCA3D    ,lDCA3DE    ,lDCA2D    ,lDCA2DE,
                   lDCARPhi3D,lDCARPhi3DE,lDCARPhi2D,lDCARPhi2DE,
                   trackElec,trackTau,DCASig::eETau);

      outDCASig->SetDCA2D    (lDCA2D);       outDCASig->SetDCA2DErr    (lDCA2DE);
      outDCASig->SetDCA3D    (lDCA3D);       outDCASig->SetDCA3DErr    (lDCA3DE);
      outDCASig->SetDCA2DRPhi(lDCARPhi2D);   outDCASig->SetDCA2DRPhiErr(lDCARPhi2DE);
      outDCASig->SetDCA3DRPhi(lDCARPhi3D);   outDCASig->SetDCA3DRPhiErr(lDCARPhi3DE);
      outDCASig->SetType(DCASig::eETau);

      //references
      edm::Ptr<reco::GsfElectron> pElectronPtr(hElectronProduct, iElectron-inElectrons.begin());
      try {
        outDCASig->SetElectron(electronMap_->GetMit(pElectronPtr));
      }
      catch(...) {
        if (checkDCARef_)  throw edm::Exception(edm::errors::Configuration, "FillerDCASig:FillDataBlock()\n")
          << "Error! Electron unmapped collection " << edmElectronName_ << endl;
      }
      edm::Ptr<reco::PFTau> pTauPtr(hTauProduct,iTau - inTaus.begin());
      try {
        outDCASig->SetTau(tauMap_->GetMit(pTauPtr));
      }
      catch(...) {
        if (checkDCARef_)
          throw edm::Exception(edm::errors::Configuration, "FillerDCASig:FillDataBlock()\n")
            << "Error! Tau unmapped collection " << edmTauName_ << endl;
      }
      //std::cout << " ===> Tau Ref : " << &pTauPtr << " -- " << ((tauMap_->GetMit(pTauPtr))) << endl;
    }

    //E + E Objects
    for (reco::GsfElectronCollection::const_iterator iElectron1 = iElectron+1;
         iElectron1 != inElectrons.end(); ++iElectron1) {
      if (iElectron1->pt() < electronPtMin_)
        continue;

      //declare the Object
      const reco::Track * trackElec0 = (const reco::Track*)(iElectron ->gsfTrack().get()); // electron Track
      const reco::Track * trackElec1 = (const reco::Track*)(iElectron1->gsfTrack().get()); // electron Track

      if (trackElec1 == NULL || trackElec0 == NULL || trackElec0 == trackElec1)
        continue;

      mithep::DCASig *outDCASig      = DCASigs_->AddNew();
      calculateDCA(lDCA3D    ,lDCA3DE    ,lDCA2D    ,lDCA2DE,
                   lDCARPhi3D,lDCARPhi3DE,lDCARPhi2D,lDCARPhi2DE,
                   trackElec0,trackElec1,DCASig::eEE);

      outDCASig->SetDCA2D    (lDCA2D);       outDCASig->SetDCA2DErr    (lDCA2DE);
      outDCASig->SetDCA3D    (lDCA3D);       outDCASig->SetDCA3DErr    (lDCA3DE);
      outDCASig->SetDCA2DRPhi(lDCARPhi2D);   outDCASig->SetDCA2DRPhiErr(lDCARPhi2DE);
      outDCASig->SetDCA3DRPhi(lDCARPhi3D);   outDCASig->SetDCA3DRPhiErr(lDCARPhi3DE);
      outDCASig->SetType(DCASig::eEE);

      //references
      edm::Ptr<reco::GsfElectron> pElectronPtr(hElectronProduct, iElectron - inElectrons.begin());
      try {
        outDCASig->SetElectron(electronMap_->GetMit(pElectronPtr));
      }
      catch(...) {
        if (checkDCARef_)
          throw edm::Exception(edm::errors::Configuration, "FillerDCASig:FillDataBlock()\n")
            << "Error! Electron unmapped collection " << edmElectronName_ << endl;
      }
      edm::Ptr<reco::GsfElectron> pElectron1Ptr(hElectronProduct, iElectron1 - inElectrons.begin());
      try {
        outDCASig->SetElectron(electronMap_->GetMit(pElectron1Ptr),true);
      }
      catch(...) {
        if (checkDCARef_)
          throw edm::Exception(edm::errors::Configuration, "FillerDCASig:FillDataBlock()\n")
            << "Error! Electron unmapped collection " << edmElectronName_ << endl;
      }
    }
  }

  // Mu + Tau combination
  for (reco::MuonCollection::const_iterator iMuon = inMuons.begin(); iMuon != inMuons.end();
       ++iMuon) {
    if (iMuon->pt() < muonPtMin_)
      continue;

    for (reco::PFTauCollection::const_iterator iTau = inTaus.begin();
         iTau != inTaus.end(); ++iTau) {
      if (iTau->pt() < tauPtMin_)
        continue;

      const reco::Track * trackMuon  = (const reco::Track*)(iMuon->innerTrack().get());   // muon inner track
      if (iTau->leadPFChargedHadrCand().isNull())
        continue;
      const reco::Track * trackTau = (const reco::Track*)(iTau->leadPFChargedHadrCand()->trackRef().get()); // Tau lead track

      if (trackTau == NULL || trackMuon == NULL || trackTau == trackMuon)
        continue;

      calculateDCA(lDCA3D    ,lDCA3DE    ,lDCA2D    ,lDCA2DE,
                   lDCARPhi3D,lDCARPhi3DE,lDCARPhi2D,lDCARPhi2DE,
                   trackMuon,trackTau,DCASig::eMuTau);

      mithep::DCASig *outDCASig = DCASigs_->AddNew();
      outDCASig->SetDCA2D    (lDCA2D);       outDCASig->SetDCA2DErr    (lDCA2DE);
      outDCASig->SetDCA3D    (lDCA3D);       outDCASig->SetDCA3DErr    (lDCA3DE);
      outDCASig->SetDCA2DRPhi(lDCARPhi2D);   outDCASig->SetDCA2DRPhiErr(lDCARPhi2DE);
      outDCASig->SetDCA3DRPhi(lDCARPhi3D);   outDCASig->SetDCA3DRPhiErr(lDCARPhi3DE);
      outDCASig->SetType(DCASig::eMuTau);

      //references
      edm::Ptr<reco::Muon> pMuonPtr(hMuonProduct, iMuon - inMuons.begin());
      try {
        outDCASig->SetMuon(muonMap_->GetMit(pMuonPtr));
      }
      catch(...) {
        if (checkDCARef_)
          throw edm::Exception(edm::errors::Configuration, "FillerDCASig:FillDataBlock()\n")
            << "Error! Muon unmapped collection " << edmMuonName_ << endl;
      }
      edm::Ptr<reco::PFTau> pTauPtr(hTauProduct, iTau - inTaus.begin());
      try {
        outDCASig->SetTau(tauMap_->GetMit(pTauPtr));
      }
      catch(...) {
        if (checkDCARef_)
          throw edm::Exception(edm::errors::Configuration, "FillerDCASig:FillDataBlock()\n")
            << "Error! Tau unmapped collection " << edmTauName_ << endl;
      }
    }

    //Mu + Mu
    for (reco::MuonCollection::const_iterator iMuon1 = iMuon+1;
         iMuon1 != inMuons.end(); ++iMuon1) {
      if (iMuon1->pt() < muonPtMin_)
        continue;

      const reco::Track * trackMuon0 = (const reco::Track*)(iMuon ->innerTrack().get());   // muon inner track
      const reco::Track * trackMuon1 = (const reco::Track*)(iMuon1->innerTrack().get());   // muon inner track

      if (trackMuon1 == NULL || trackMuon0 == NULL || trackMuon0 == trackMuon1)
        continue;

      calculateDCA(lDCA3D    ,lDCA3DE    ,lDCA2D    ,lDCA2DE,
                   lDCARPhi3D,lDCARPhi3DE,lDCARPhi2D,lDCARPhi2DE,
                   trackMuon0,trackMuon1,DCASig::eMuMu);

      mithep::DCASig *outDCASig = DCASigs_->AddNew();
      outDCASig->SetDCA2D    (lDCA2D);       outDCASig->SetDCA2DErr    (lDCA2DE);
      outDCASig->SetDCA3D    (lDCA3D);       outDCASig->SetDCA3DErr    (lDCA3DE);
      outDCASig->SetDCA2DRPhi(lDCARPhi2D);   outDCASig->SetDCA2DRPhiErr(lDCARPhi2DE);
      outDCASig->SetDCA3DRPhi(lDCARPhi3D);   outDCASig->SetDCA3DRPhiErr(lDCARPhi3DE);
      outDCASig->SetType(DCASig::eMuMu);

      //Now the references
      edm::Ptr<reco::Muon> pMuon0Ptr(hMuonProduct, iMuon - inMuons.begin());
      try {
        outDCASig->SetMuon(muonMap_->GetMit(pMuon0Ptr));
      }
      catch(...) {
        if (checkDCARef_)
          throw edm::Exception(edm::errors::Configuration, "FillerDCASig:FillDataBlock()\n")
            << "Error! Muon unmapped collection " << edmMuonName_ << endl;
      }
      edm::Ptr<reco::Muon> pMuon1Ptr(hMuonProduct, iMuon1 - inMuons.begin());
      try {
        outDCASig->SetMuon(muonMap_->GetMit(pMuon1Ptr),true);
      }
      catch(...) {
        if (checkDCARef_)
          throw edm::Exception(edm::errors::Configuration, "FillerDCASig:FillDataBlock()\n")
            << "Error! Muon unmapped collection " << edmMuonName_ << endl;
      }
    }
  }

  // Tau + Tau combination
  for (reco::PFTauCollection::const_iterator iTau0 = inTaus.begin();
       iTau0 != inTaus.end(); ++iTau0) {
    if (iTau0->pt() < tauPtMin_)
      continue;
    for (reco::PFTauCollection::const_iterator iTau1 = iTau0+1;
         iTau1 != inTaus.end(); ++iTau1) {
      if (iTau0 == iTau1)
        continue;
      if (iTau1->pt() < tauPtMin_)
        continue;

      if (iTau0->leadPFChargedHadrCand().isNull())
        continue;
      if (iTau1->leadPFChargedHadrCand().isNull())
        continue;
      const reco::Track *trackTau0 = (const reco::Track*)(iTau0->leadPFChargedHadrCand()->trackRef().get()); // Tau lead track
      const reco::Track *trackTau1 = (const reco::Track*)(iTau1->leadPFChargedHadrCand()->trackRef().get()); // Tau lead track

      if (trackTau1 == NULL || trackTau0 == NULL || trackTau0 == trackTau1)
        continue;

      calculateDCA(lDCA3D    ,lDCA3DE    ,lDCA2D    ,lDCA2DE,
                   lDCARPhi3D,lDCARPhi3DE,lDCARPhi2D,lDCARPhi2DE,
                   trackTau0,trackTau1,DCASig::eTauTau);

      mithep::DCASig *outDCASig = DCASigs_->AddNew();

      outDCASig->SetDCA2D    (lDCA2D);       outDCASig->SetDCA2DErr    (lDCA2DE);
      outDCASig->SetDCA3D    (lDCA3D);       outDCASig->SetDCA3DErr    (lDCA3DE);
      outDCASig->SetDCA2DRPhi(lDCARPhi2D);   outDCASig->SetDCA2DRPhiErr(lDCARPhi2DE);
      outDCASig->SetDCA3DRPhi(lDCARPhi3D);   outDCASig->SetDCA3DRPhiErr(lDCARPhi3DE);
      outDCASig->SetType(DCASig::eTauTau);

      //references
      edm::Ptr<reco::PFTau> pTau0Ptr(hTauProduct, iTau0 - inTaus.begin());
      try {
	outDCASig->SetTau(tauMap_->GetMit(pTau0Ptr));}
      catch(...) {
        if (checkDCARef_)
	  throw edm::Exception(edm::errors::Configuration, "FillerDCASig:FillDataBlock()\n")
	    << "Error! Tau unmapped collection " << edmTauName_ << endl;
      }
      edm::Ptr<reco::PFTau> pTau1Ptr(hTauProduct, iTau1 - inTaus.begin());
      try {
	outDCASig->SetTau(tauMap_->GetMit(pTau1Ptr),true);
      }
      catch(...) {
        if (checkDCARef_)
	  throw edm::Exception(edm::errors::Configuration, "FillerDCASig:FillDataBlock()\n")
	    << "Error! Tau unmapped collection " << edmTauName_ << endl;
      }
    }
  }
  DCASigs_->Trim();
}

//--------------------------------------------------------------------------------------------------
//Get DCA Significane
//--------------------------------------------------------------------------------------------------
void FillerDCASig::calculateDCA(Double_t &iDCA3D    ,Double_t &iDCA3DE    ,Double_t &iDCA2D    ,
				Double_t &iDCA2DE,
                                Double_t &iDCARPhi3D,Double_t &iDCARPhi3DE,Double_t &iDCARPhi2D,
				Double_t &iDCARPhi2DE,
                                const reco::Track *iTrack1,const reco::Track *iTrack2,
				mithep::DCASig::EDCAType iType) {
  iDCA3D      = -1.0;
  iDCA3DE     = -1.0;
  iDCA2D      = -1.0;
  iDCA2DE     = -1.0;
  iDCARPhi3D  = -1.0;
  iDCARPhi3DE = -1.0;
  iDCARPhi2D  = -1.0;
  iDCARPhi2DE = -1.0;

  if (iTrack1==NULL)
    return;
  if (iTrack2==NULL)
    return;

  ParticleMass pion_mass = 0.139657;
  float pion_sigma       = pion_mass*1.e-6;
  ParticleMass muon_mass = 0.105658;
  float muon_sigma       = muon_mass*1.e-6;
  ParticleMass elec_mass = 0.000511;
  float elec_sigma       = elec_mass*1.e-6;
  float mass1            = elec_mass;
  float mass2            = muon_mass;
  float mass_sigma1      = elec_sigma;
  float mass_sigma2      = muon_sigma;

  if (iType == DCASig::eETau  )   mass1 = pion_sigma;
  if (iType == DCASig::eEE    )   mass2 = elec_sigma;
  if (iType == DCASig::eMuTau ) { mass1 = muon_sigma; mass2 = pion_sigma; }
  if (iType == DCASig::eMuMu  ) { mass1 = muon_sigma; mass2 = muon_sigma; }
  if (iType == DCASig::eTauTau) { mass1 = pion_sigma; mass2 = pion_sigma; }
  if (iType == DCASig::eETau  )   mass_sigma2 = pion_sigma;
  if (iType == DCASig::eEE    )   mass_sigma2 = elec_sigma;
  if (iType == DCASig::eMuTau ) { mass_sigma1 = muon_sigma; mass_sigma2 = pion_sigma; }
  if (iType == DCASig::eMuMu  ) { mass_sigma1 = muon_sigma; mass_sigma2 = muon_sigma; }
  if (iType == DCASig::eTauTau) { mass_sigma1 = pion_sigma; mass_sigma2 = pion_sigma; }

  reco::TransientTrack transientTrack1;
  reco::TransientTrack transientTrack2;

  transientTrack1 = transientTrackBuilder_->build(*iTrack1);
  transientTrack2 = transientTrackBuilder_->build(*iTrack2);
  reco::TransientTrack * trk1Ptr = & transientTrack1;
  reco::TransientTrack * trk2Ptr = & transientTrack2;

  FreeTrajectoryState track1State = trk1Ptr->impactPointTSCP().theState();
  FreeTrajectoryState track2State = trk2Ptr->impactPointTSCP().theState();

  if (trk1Ptr->impactPointTSCP().isValid() &&  trk2Ptr->impactPointTSCP().isValid()) {

    ClosestApproachInRPhi cApp;
    TwoTrackMinimumDistance minDist;

    typedef ROOT::Math::SVector<double, 3> SVector3;
    typedef ROOT::Math::SMatrix<double, 3, 3, ROOT::Math::MatRepSym<double, 3> > SMatrixSym3D;
    cApp.calculate(track1State,track2State);
    minDist.calculate(track1State,track2State);
    if (minDist.status()) {

      minDist.distance();
      std::pair<GlobalPoint,GlobalPoint> pcaLeptons = minDist.points();
      GlobalPoint track1PCA = pcaLeptons.first;
      GlobalPoint track2PCA = pcaLeptons.second;

      //create a KinematicParticleFactory
      KinematicParticleFactoryFromTransientTrack pFactory;

      //initial chi2 and ndf before kinematic fits.
      float chi = 0.;
      float ndf = 0.;
      RefCountedKinematicParticle track1Part = pFactory.particle(transientTrack1,mass1,chi,ndf,mass_sigma1);
      RefCountedKinematicParticle track2Part = pFactory.particle(transientTrack2,mass2,chi,ndf,mass_sigma2);

      SVector3 distanceVector(track1PCA.x()-track2PCA.x(),
                              track1PCA.y()-track2PCA.y(),
                              track1PCA.z()-track2PCA.z());

      iDCA3D = ROOT::Math::Mag(distanceVector);

      std::vector<float> vvv(6);

      vvv[0] = track1Part->stateAtPoint(track1PCA).kinematicParametersError().matrix()(0,0);
      vvv[1] = track1Part->stateAtPoint(track1PCA).kinematicParametersError().matrix()(0,1);
      vvv[2] = track1Part->stateAtPoint(track1PCA).kinematicParametersError().matrix()(1,1);
      vvv[3] = track1Part->stateAtPoint(track1PCA).kinematicParametersError().matrix()(0,2);
      vvv[4] = track1Part->stateAtPoint(track1PCA).kinematicParametersError().matrix()(1,2);
      vvv[5] = track1Part->stateAtPoint(track1PCA).kinematicParametersError().matrix()(2,2);

      SMatrixSym3D track1PCACov(vvv.begin(),vvv.end());

      vvv[0] = track2Part->stateAtPoint(track2PCA).kinematicParametersError().matrix()(0,0);
      vvv[1] = track2Part->stateAtPoint(track2PCA).kinematicParametersError().matrix()(0,1);
      vvv[2] = track2Part->stateAtPoint(track2PCA).kinematicParametersError().matrix()(1,1);
      vvv[3] = track2Part->stateAtPoint(track2PCA).kinematicParametersError().matrix()(0,2);
      vvv[4] = track2Part->stateAtPoint(track2PCA).kinematicParametersError().matrix()(1,2);
      vvv[5] = track2Part->stateAtPoint(track2PCA).kinematicParametersError().matrix()(2,2);

      SMatrixSym3D track2PCACov(vvv.begin(),vvv.end());

      SMatrixSym3D totCov = track1PCACov + track2PCACov;

      if (iDCA3D != 0) iDCA3DE   = sqrt(ROOT::Math::Similarity(totCov, distanceVector))/iDCA3D;
      if (iDCA3D == 0) iDCA3DE   = 0.;

      distanceVector(2) = 0.0;
      iDCA2D    = ROOT::Math::Mag(distanceVector);
      if (iDCA2D != 0) iDCA2DE   = sqrt(ROOT::Math::Similarity(totCov, distanceVector))/iDCA2D;
      if (iDCA2D == 0) iDCA2DE   = 0.;
    }
    if (cApp.status()) {

      cApp.distance();
      std::pair<GlobalPoint,GlobalPoint> pcaLeptons = cApp.points();
      GlobalPoint track1PCA = pcaLeptons.first;
      GlobalPoint track2PCA = pcaLeptons.second;

      //Creating a KinematicParticleFactory
      KinematicParticleFactoryFromTransientTrack pFactory;

      //initial chi2 and ndf before kinematic fits.
      float chi = 0.;
      float ndf = 0.;
      RefCountedKinematicParticle track1Part = pFactory.particle(transientTrack1,mass1,chi,ndf,mass_sigma1);
      RefCountedKinematicParticle track2Part = pFactory.particle(transientTrack2,mass2,chi,ndf,mass_sigma2);

      SVector3 distanceVector(track1PCA.x()-track2PCA.x(),
                              track1PCA.y()-track2PCA.y(),
                              track1PCA.z()-track2PCA.z());
      iDCARPhi3D = ROOT::Math::Mag(distanceVector);

      std::vector<float> vvv(6);

      vvv[0] = track1Part->stateAtPoint(track1PCA).kinematicParametersError().matrix()(0,0);
      vvv[1] = track1Part->stateAtPoint(track1PCA).kinematicParametersError().matrix()(0,1);
      vvv[2] = track1Part->stateAtPoint(track1PCA).kinematicParametersError().matrix()(1,1);
      vvv[3] = track1Part->stateAtPoint(track1PCA).kinematicParametersError().matrix()(0,2);
      vvv[4] = track1Part->stateAtPoint(track1PCA).kinematicParametersError().matrix()(1,2);
      vvv[5] = track1Part->stateAtPoint(track1PCA).kinematicParametersError().matrix()(2,2);

      SMatrixSym3D track1PCACov(vvv.begin(),vvv.end());

      vvv[0] = track2Part->stateAtPoint(track2PCA).kinematicParametersError().matrix()(0,0);
      vvv[1] = track2Part->stateAtPoint(track2PCA).kinematicParametersError().matrix()(0,1);
      vvv[2] = track2Part->stateAtPoint(track2PCA).kinematicParametersError().matrix()(1,1);
      vvv[3] = track2Part->stateAtPoint(track2PCA).kinematicParametersError().matrix()(0,2);
      vvv[4] = track2Part->stateAtPoint(track2PCA).kinematicParametersError().matrix()(1,2);
      vvv[5] = track2Part->stateAtPoint(track2PCA).kinematicParametersError().matrix()(2,2);

      SMatrixSym3D track2PCACov(vvv.begin(),vvv.end());

      SMatrixSym3D totCov = track1PCACov + track2PCACov;

      if (iDCARPhi3D != 0)
	iDCARPhi3DE = sqrt(ROOT::Math::Similarity(totCov, distanceVector))/iDCARPhi3D;
      if (iDCARPhi3D == 0)
	iDCARPhi3DE = 0.;

      distanceVector(2) = 0.0;
      iDCARPhi2D  = ROOT::Math::Mag(distanceVector);
      if (iDCARPhi2D != 0)
	iDCARPhi2DE = sqrt(ROOT::Math::Similarity(totCov, distanceVector))/iDCARPhi2D;
      if (iDCARPhi2D == 0)
	iDCARPhi2DE = 0.;
    }
  }
}

