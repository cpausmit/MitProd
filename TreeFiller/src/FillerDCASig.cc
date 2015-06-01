#include "MitProd/TreeFiller/interface/FillerDCASig.h"
#include "DataFormats/Common/interface/RefToPtr.h"
#include "MitEdm/DataFormats/interface/RefToBaseToPtr.h"
#include "MitAna/DataTree/interface/DCASigCol.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitProd/ObjectService/interface/ObjectService.h"

#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"
#include "DataFormats/TauReco/interface/PFTau.h"
#include "DataFormats/PatCandidates/interface/Tau.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
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
FillerDCASig::FillerDCASig(const ParameterSet &cfg, edm::ConsumesCollector& collector, ObjectService* os, const char *name, bool active) :
  BaseFiller            (cfg,os,name,active),
  edmElectronToken_     (GetToken<GsfElectronView>(collector, "edmElectronName","gsfElectrons")),
  edmMuonToken_         (GetToken<MuonView>(collector, "edmMuonName",    "muons")),
  edmTauToken_          (GetToken<BaseTauView>(collector, "edmTauName",     "hpsPFTauProducer")),
  edmTauType_           (nTauTypes),
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
  std::string tauType(Conf().getUntrackedParameter<std::string>("edmTauType", "PFTau"));
  if (tauType == "PFTau")
    edmTauType_ = kPFTau;
  else if (tauType == "PATTau")
    edmTauType_ = kPATTau;
  else
    throw edm::Exception(edm::errors::Configuration, "FillerDCASig:Constructor\n")
      << "Error! Invalid tau type";
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
  OS()->add(DCASigs_,mitName_);

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
  Handle<GsfElectronView> hElectronProduct;
  GetProduct(edmElectronToken_, hElectronProduct, event);
  auto& inElectrons = *hElectronProduct;

  //handle for the muon collection
  Handle<MuonView> hMuonProduct;
  GetProduct(edmMuonToken_, hMuonProduct, event);
  auto& inMuons = *hMuonProduct;

  // handle for the tau collection
  Handle<BaseTauView> hTauProduct;
  GetProduct(edmTauToken_, hTauProduct, event);
  auto& inTaus = *hTauProduct;

  if (inTaus.size() != 0) {
    switch (edmTauType_) {
    case kPFTau:
      if (!dynamic_cast<reco::PFTau const*>(&inTaus.at(0)))
        throw edm::Exception(edm::errors::Configuration, "FillerDCASig:FillDataBlock()\n")
          << "Error! Input tau collection is not PF";
      break;
    case kPATTau:
      if (!dynamic_cast<pat::Tau const*>(&inTaus.at(0)))
        throw edm::Exception(edm::errors::Configuration, "FillerDCASig:FillDataBlock()\n")
          << "Error! Input tau collection is not PAT";
      break;
    default:
      break;
    }
  }

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
  unsigned iElectron = -1;
  for (auto&& inElectron : inElectrons) {
    ++iElectron;

    if (inElectron.pt() < electronPtMin_)
      continue;

    if (!inElectron.gsfTrack().isAvailable())
      continue;

    edm::Ptr<reco::GsfElectron> pElectronPtr(hElectronProduct, iElectron);

    auto trackElec = inElectron.gsfTrack().get(); // electron Track

    //E + Mu Objects
    unsigned iMuon = -1;
    for (auto&& inMuon : inMuons) {
      ++iMuon;

      if (inMuon.pt() < muonPtMin_)
        continue;

      if (!inMuon.innerTrack().isAvailable())
        continue;

      //Declare the Object
      auto trackMuon = inMuon.innerTrack().get();   // muon inner track

      if (trackElec == trackMuon) continue;

      edm::Ptr<reco::Muon> pMuonPtr(hMuonProduct, iMuon);

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
      try {
        outDCASig->SetElectron(electronMap_->GetMit(pElectronPtr));
      }
      catch(...) {
        if (checkDCARef_)
          throw edm::Exception(edm::errors::Configuration, "FillerDCASig:FillDataBlock()\n")
            << "Error! Electron unmapped collection";
      }

      try {
        outDCASig->SetMuon(muonMap_->GetMit(pMuonPtr));
      }
      catch(...) {
        if (checkDCARef_)  throw edm::Exception(edm::errors::Configuration, "FillerDCASig:FillDataBlock()\n")
                             << "Error! Muon unmapped collection";
      }
    }

    //E + Tau objects
    unsigned iTau = -1;
    for (auto&& inTau : inTaus) {
      ++iTau;

      if (inTau.pt() < tauPtMin_)
        continue;

      reco::Track const* trackTau = 0;

      if (edmTauType_ == kPFTau) {
        reco::PFTau const& pfTau = static_cast<reco::PFTau const&>(inTau);

        if (pfTau.leadPFChargedHadrCand().isNull())
          continue;
        if (!pfTau.leadPFChargedHadrCand()->trackRef().isAvailable())
          continue;

        trackTau = pfTau.leadPFChargedHadrCand()->trackRef().get(); // Tau lead track

        if (trackTau == trackElec)
          continue;
      }
      else if(edmTauType_ == kPATTau) {
        pat::Tau const& patTau = static_cast<pat::Tau const&>(inTau);

        if (patTau.leadChargedHadrCand().isNull())
          continue;

        pat::PackedCandidate const& leadChargedHadr = static_cast<pat::PackedCandidate const&>(*patTau.leadChargedHadrCand());

        trackTau = &leadChargedHadr.pseudoTrack();

        // no protection against track overlaps here..
      }

      edm::Ptr<reco::BaseTau> pTauPtr(hTauProduct, iTau);

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
      try {
        outDCASig->SetElectron(electronMap_->GetMit(pElectronPtr));
      }
      catch(...) {
        if (checkDCARef_)  throw edm::Exception(edm::errors::Configuration, "FillerDCASig:FillDataBlock()\n")
                             << "Error! Electron unmapped collection";
      }
      try {
        outDCASig->SetTau(tauMap_->GetMit(pTauPtr));
      }
      catch(...) {
        if (checkDCARef_)
          throw edm::Exception(edm::errors::Configuration, "FillerDCASig:FillDataBlock()\n")
            << "Error! Tau unmapped collection";
      }
      //std::cout << " ===> Tau Ref : " << &pTauPtr << " -- " << ((tauMap_->GetMit(pTauPtr))) << endl;
    }

    //E + E Objects
    unsigned iElectron1 = -1;
    for (auto&& inElectron1 : inElectrons) {
      ++iElectron1;

      if (iElectron == iElectron1)
        continue;

      if (inElectron1.pt() < electronPtMin_)
        continue;

      if (!inElectron1.gsfTrack().isAvailable())
        continue;

      edm::Ptr<reco::GsfElectron> pElectron1Ptr(hElectronProduct, iElectron1);

      //declare the Object
      auto trackElec1 = inElectron1.gsfTrack().get(); // electron Track

      mithep::DCASig *outDCASig      = DCASigs_->AddNew();
      calculateDCA(lDCA3D    ,lDCA3DE    ,lDCA2D    ,lDCA2DE,
                   lDCARPhi3D,lDCARPhi3DE,lDCARPhi2D,lDCARPhi2DE,
                   trackElec,trackElec1,DCASig::eEE);

      outDCASig->SetDCA2D    (lDCA2D);       outDCASig->SetDCA2DErr    (lDCA2DE);
      outDCASig->SetDCA3D    (lDCA3D);       outDCASig->SetDCA3DErr    (lDCA3DE);
      outDCASig->SetDCA2DRPhi(lDCARPhi2D);   outDCASig->SetDCA2DRPhiErr(lDCARPhi2DE);
      outDCASig->SetDCA3DRPhi(lDCARPhi3D);   outDCASig->SetDCA3DRPhiErr(lDCARPhi3DE);
      outDCASig->SetType(DCASig::eEE);

      //references
      try {
        outDCASig->SetElectron(electronMap_->GetMit(pElectronPtr));
      }
      catch(...) {
        if (checkDCARef_)
          throw edm::Exception(edm::errors::Configuration, "FillerDCASig:FillDataBlock()\n")
            << "Error! Electron unmapped collection";
      }
      try {
        outDCASig->SetElectron(electronMap_->GetMit(pElectron1Ptr),true);
      }
      catch(...) {
        if (checkDCARef_)
          throw edm::Exception(edm::errors::Configuration, "FillerDCASig:FillDataBlock()\n")
            << "Error! Electron unmapped collection";
      }
    }
  }

  // Mu + Tau combination
  unsigned iMuon = -1;
  for (auto&& inMuon : inMuons) {
    ++iMuon;

    if (inMuon.pt() < muonPtMin_)
      continue;

    if (!inMuon.innerTrack().isAvailable())
      continue;

    edm::Ptr<reco::Muon> pMuonPtr(hMuonProduct, iMuon);

    auto trackMuon = inMuon.innerTrack().get();   // muon inner track

    unsigned iTau = -1;
    for (auto&& inTau : inTaus) {
      ++iTau;

      if (inTau.pt() < tauPtMin_)
        continue;

      reco::Track const* trackTau = 0;

      if (edmTauType_ == kPFTau) {
        reco::PFTau const& pfTau = static_cast<reco::PFTau const&>(inTau);

        if (pfTau.leadPFChargedHadrCand().isNull())
          continue;
        if (!pfTau.leadPFChargedHadrCand()->trackRef().isAvailable())
          continue;

        trackTau = pfTau.leadPFChargedHadrCand()->trackRef().get(); // Tau lead track

        if (trackTau == trackMuon)
          continue;
      }
      else if(edmTauType_ == kPATTau) {
        pat::Tau const& patTau = static_cast<pat::Tau const&>(inTau);

        if (patTau.leadChargedHadrCand().isNull())
          continue;

        pat::PackedCandidate const& leadChargedHadr = static_cast<pat::PackedCandidate const&>(*patTau.leadChargedHadrCand());

        trackTau = &leadChargedHadr.pseudoTrack();

        // no protection against track overlaps here..
      }

      edm::Ptr<reco::BaseTau> pTauPtr(hTauProduct, iTau);

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

      try {
        outDCASig->SetMuon(muonMap_->GetMit(pMuonPtr));
      }
      catch(...) {
        if (checkDCARef_)
          throw edm::Exception(edm::errors::Configuration, "FillerDCASig:FillDataBlock()\n")
            << "Error! Muon unmapped collection";
      }
      try {
        outDCASig->SetTau(tauMap_->GetMit(pTauPtr));
      }
      catch(...) {
        if (checkDCARef_)
          throw edm::Exception(edm::errors::Configuration, "FillerDCASig:FillDataBlock()\n")
            << "Error! Tau unmapped collection";
      }
    }

    //Mu + Mu
    unsigned iMuon1 = -1;
    for (auto&& inMuon1 : inMuons) {
      ++iMuon1;
      
      if (iMuon1 == iMuon)
        continue;

      if (inMuon1.pt() < muonPtMin_)
        continue;

      if (!inMuon1.innerTrack().isAvailable())
        continue;

      edm::Ptr<reco::Muon> pMuon1Ptr(hMuonProduct, iMuon1);

      auto trackMuon = inMuon.innerTrack().get();   // muon inner track
      auto trackMuon1 = inMuon1.innerTrack().get();   // muon inner track

      calculateDCA(lDCA3D    ,lDCA3DE    ,lDCA2D    ,lDCA2DE,
                   lDCARPhi3D,lDCARPhi3DE,lDCARPhi2D,lDCARPhi2DE,
                   trackMuon,trackMuon1,DCASig::eMuMu);

      mithep::DCASig *outDCASig = DCASigs_->AddNew();
      outDCASig->SetDCA2D    (lDCA2D);       outDCASig->SetDCA2DErr    (lDCA2DE);
      outDCASig->SetDCA3D    (lDCA3D);       outDCASig->SetDCA3DErr    (lDCA3DE);
      outDCASig->SetDCA2DRPhi(lDCARPhi2D);   outDCASig->SetDCA2DRPhiErr(lDCARPhi2DE);
      outDCASig->SetDCA3DRPhi(lDCARPhi3D);   outDCASig->SetDCA3DRPhiErr(lDCARPhi3DE);
      outDCASig->SetType(DCASig::eMuMu);

      try {
        outDCASig->SetMuon(muonMap_->GetMit(pMuonPtr));
      }
      catch(...) {
        if (checkDCARef_)
          throw edm::Exception(edm::errors::Configuration, "FillerDCASig:FillDataBlock()\n")
            << "Error! Muon unmapped collection";
      }
      try {
        outDCASig->SetMuon(muonMap_->GetMit(pMuon1Ptr),true);
      }
      catch(...) {
        if (checkDCARef_)
          throw edm::Exception(edm::errors::Configuration, "FillerDCASig:FillDataBlock()\n")
            << "Error! Muon unmapped collection";
      }
    }
  }

  // Tau + Tau combination
  unsigned iTau = -1;
  for (auto&& inTau : inTaus) {
    ++iTau;

    if (inTau.pt() < tauPtMin_)
      continue;

    reco::Track const* trackTau = 0;

    if (edmTauType_ == kPFTau) {
      reco::PFTau const& pfTau = static_cast<reco::PFTau const&>(inTau);

      if (pfTau.leadPFChargedHadrCand().isNull())
        continue;
      if (!pfTau.leadPFChargedHadrCand()->trackRef().isAvailable())
        continue;

      trackTau = pfTau.leadPFChargedHadrCand()->trackRef().get(); // Tau lead track
    }
    else if(edmTauType_ == kPATTau) {
      pat::Tau const& patTau = static_cast<pat::Tau const&>(inTau);

      if (patTau.leadChargedHadrCand().isNull())
        continue;

      pat::PackedCandidate const& leadChargedHadr = static_cast<pat::PackedCandidate const&>(*patTau.leadChargedHadrCand());

      trackTau = &leadChargedHadr.pseudoTrack();
    }

    edm::Ptr<reco::BaseTau> pTauPtr(hTauProduct, iTau);

    unsigned iTau1 = -1;
    for (auto&& inTau1 : inTaus) {
      ++iTau1;

      if (iTau1 == iTau)
        continue;

      if (inTau1.pt() < tauPtMin_)
        continue;

      reco::Track const* trackTau1 = 0;

      if (edmTauType_ == kPFTau) {
        reco::PFTau const& pfTau1 = static_cast<reco::PFTau const&>(inTau1);

        if (pfTau1.leadPFChargedHadrCand().isNull())
          continue;
        if (!pfTau1.leadPFChargedHadrCand()->trackRef().isAvailable())
          continue;

        trackTau1 = pfTau1.leadPFChargedHadrCand()->trackRef().get(); // Tau lead track
      }
      else if(edmTauType_ == kPATTau) {
        pat::Tau const& patTau1 = static_cast<pat::Tau const&>(inTau1);

        if (patTau1.leadChargedHadrCand().isNull())
          continue;

        pat::PackedCandidate const& leadChargedHadr = static_cast<pat::PackedCandidate const&>(*patTau1.leadChargedHadrCand());

        trackTau1 = &leadChargedHadr.pseudoTrack();
      }

      edm::Ptr<reco::BaseTau> pTau1Ptr(hTauProduct, iTau1);

      calculateDCA(lDCA3D    ,lDCA3DE    ,lDCA2D    ,lDCA2DE,
                   lDCARPhi3D,lDCARPhi3DE,lDCARPhi2D,lDCARPhi2DE,
                   trackTau,trackTau1,DCASig::eTauTau);

      mithep::DCASig *outDCASig = DCASigs_->AddNew();

      outDCASig->SetDCA2D    (lDCA2D);       outDCASig->SetDCA2DErr    (lDCA2DE);
      outDCASig->SetDCA3D    (lDCA3D);       outDCASig->SetDCA3DErr    (lDCA3DE);
      outDCASig->SetDCA2DRPhi(lDCARPhi2D);   outDCASig->SetDCA2DRPhiErr(lDCARPhi2DE);
      outDCASig->SetDCA3DRPhi(lDCARPhi3D);   outDCASig->SetDCA3DRPhiErr(lDCARPhi3DE);
      outDCASig->SetType(DCASig::eTauTau);

      //references
      try {
	outDCASig->SetTau(tauMap_->GetMit(pTauPtr));
      }
      catch(...) {
        if (checkDCARef_)
	  throw edm::Exception(edm::errors::Configuration, "FillerDCASig:FillDataBlock()\n")
	    << "Error! Tau unmapped collection";
      }
      try {
	outDCASig->SetTau(tauMap_->GetMit(pTau1Ptr), true);
      }
      catch(...) {
        if (checkDCARef_)
	  throw edm::Exception(edm::errors::Configuration, "FillerDCASig:FillDataBlock()\n")
	    << "Error! Tau unmapped collection";
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

  reco::TransientTrack transientTrack1 = transientTrackBuilder_->build(*iTrack1);
  reco::TransientTrack transientTrack2 = transientTrackBuilder_->build(*iTrack2);

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

