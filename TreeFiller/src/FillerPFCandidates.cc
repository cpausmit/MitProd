// $Id: FillerPFCandidates.cc,v 1.4 2009/09/25 08:42:51 loizides Exp $

#include "MitProd/TreeFiller/interface/FillerPFCandidates.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/Common/interface/RefToPtr.h"
#include "MitAna/DataTree/interface/Muon.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataTree/interface/PFCandidateCol.h"
#include "MitAna/DataTree/interface/Track.h"
#include "MitProd/ObjectService/interface/ObjectService.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerPFCandidates::FillerPFCandidates(const edm::ParameterSet &cfg, 
                                       const char *name, bool active) :
  BaseFiller(cfg,name,active),
  edmName_(Conf().getUntrackedParameter<string>("edmName","particleFlow")),
  mitName_(Conf().getUntrackedParameter<string>("mitName",Names::gkPFCandidatesBrn)),
  trackerTrackMapName_(Conf().getUntrackedParameter<string>("trackerTrackMapName","")),
  gsfTrackMapName_(Conf().getUntrackedParameter<string>("gsfTrackMapName","")),
  muonMapName_(Conf().getUntrackedParameter<string>("muonMapName","")),
  conversionMapName_(Conf().getUntrackedParameter<string>("conversionMapName","")),
  pfCandMapName_(Conf().getUntrackedParameter<string>("pfCandMapName","")),
  trackerTrackMap_(0),
  gsfTrackMap_(0),
  muonMap_(0),
  conversionMap_(0),
  pfCandMap_(new mithep::PFCandidateMap),
  pfCands_(new mithep::PFCandidateArr(16))
{
  // Constructor.
}

//--------------------------------------------------------------------------------------------------
FillerPFCandidates::~FillerPFCandidates() 
{
  // Destructor.

  delete pfCands_;
  delete pfCandMap_;
}

//--------------------------------------------------------------------------------------------------
void FillerPFCandidates::BookDataBlock(TreeWriter &tws) 
{ 
  // Add particle-flow candidate branch to tree and get pointers to maps.

  tws.AddBranch(mitName_,&pfCands_);
  OS()->add<mithep::PFCandidateArr>(pfCands_,mitName_);

  if (!trackerTrackMapName_.empty()) {
    trackerTrackMap_ = OS()->get<TrackMap>(trackerTrackMapName_);
    if (trackerTrackMap_)
      AddBranchDep(mitName_,trackerTrackMap_->GetBrName());
  }
  if (!gsfTrackMapName_.empty()) {
    gsfTrackMap_ = OS()->get<TrackMap>(gsfTrackMapName_);
    if (gsfTrackMap_)
      AddBranchDep(mitName_,gsfTrackMap_->GetBrName());
  }
  if (!muonMapName_.empty()) {
    muonMap_ = OS()->get<MuonMap>(muonMapName_);
    if (muonMap_)
      AddBranchDep(mitName_,muonMap_->GetBrName());
  }
  if (!conversionMapName_.empty()) {
    conversionMap_ = OS()->get<ConversionMap>(conversionMapName_);
    if (conversionMap_)
      AddBranchDep(mitName_,conversionMap_->GetBrName());
  }
  if (!pfCandMapName_.empty()) {
    pfCandMap_->SetBrName(mitName_);
    OS()->add<PFCandidateMap>(pfCandMap_,pfCandMapName_);
  }
}

//--------------------------------------------------------------------------------------------------
void FillerPFCandidates::FillDataBlock(const edm::Event      &event, 
                                       const edm::EventSetup &setup)
{
  // Fill muon information. 

  pfCands_->Delete();
  pfCandMap_->Reset();
  
  Handle<reco::PFCandidateCollection> hPfCandProduct;
  GetProduct(edmName_, hPfCandProduct, event);  
  const reco::PFCandidateCollection &inPfCands = *(hPfCandProduct.product());

  for (reco::PFCandidateCollection::const_iterator iP = inPfCands.begin(); 
       iP != inPfCands.end(); ++iP) {
    mithep::PFCandidate *outPfCand = pfCands_->Allocate();
    new (outPfCand) mithep::PFCandidate(iP->px(),iP->py(),iP->pz(),iP->energy());

    // fill variables
    outPfCand->SetCharge(iP->charge());
    outPfCand->SetEECal(iP->ecalEnergy());
    outPfCand->SetEHCal(iP->hcalEnergy());
    outPfCand->SetEECalRaw(iP->rawEcalEnergy());
    outPfCand->SetEHCalRaw(iP->rawHcalEnergy());
    outPfCand->SetEPS1(iP->pS1Energy());
    outPfCand->SetEPS2(iP->pS2Energy());
    outPfCand->SetPError(iP->deltaP());
    outPfCand->SetMvaEPi(iP->mva_e_pi());
    outPfCand->SetMvaEMu(iP->mva_e_mu());
    outPfCand->SetMvaPiMu(iP->mva_pi_mu());
    outPfCand->SetMvaGamma(iP->mva_nothing_gamma());
    outPfCand->SetMvaNeutralH(iP->mva_nothing_nh());
    outPfCand->SetMvaGammaNeutralH(iP->mva_gamma_nh());
    outPfCand->SetEtaECal(iP->positionAtECALEntrance().eta());
    outPfCand->SetPhiECal(iP->positionAtECALEntrance().eta());

    // fill source vertex
    outPfCand->SetVertex(iP->vertex().x(),iP->vertex().y(),iP->vertex().z());

    // fill pf type enum
    if (iP->particleId()==reco::PFCandidate::X)
      outPfCand->SetPFType(mithep::PFCandidate::eX);
    else if (iP->particleId()==reco::PFCandidate::h)
      outPfCand->SetPFType(mithep::PFCandidate::eHadron);
    else if (iP->particleId()==reco::PFCandidate::e)
      outPfCand->SetPFType(mithep::PFCandidate::eElectron);
    else if (iP->particleId()==reco::PFCandidate::mu)
      outPfCand->SetPFType(mithep::PFCandidate::eMuon);
    else if (iP->particleId()==reco::PFCandidate::gamma)
      outPfCand->SetPFType(mithep::PFCandidate::eGamma);
    else if (iP->particleId()==reco::PFCandidate::h0)
      outPfCand->SetPFType(mithep::PFCandidate::eNeutralHadron);
    else if (iP->particleId()==reco::PFCandidate::h_HF)
      outPfCand->SetPFType(mithep::PFCandidate::eHadronHF);
    else if (iP->particleId()==reco::PFCandidate::egamma_HF)
      outPfCand->SetPFType(mithep::PFCandidate::eEGammaHF);

    // fill pf flags bitmask
    outPfCand->SetFlag(mithep::PFCandidate::eNormal, 
                       iP->flag(reco::PFCandidate::NORMAL));
    outPfCand->SetFlag(mithep::PFCandidate::eEMPhiSModules, 
                       iP->flag(reco::PFCandidate::E_PHI_SMODULES));
    outPfCand->SetFlag(mithep::PFCandidate::eEMEta0, 
                       iP->flag(reco::PFCandidate::E_ETA_0));
    outPfCand->SetFlag(mithep::PFCandidate::eEMEtaModules, 
                       iP->flag(reco::PFCandidate::E_ETA_MODULES));
    outPfCand->SetFlag(mithep::PFCandidate::eEMBarrelEndcap, 
                       iP->flag(reco::PFCandidate::E_BARREL_ENDCAP));
    outPfCand->SetFlag(mithep::PFCandidate::eEMPreshowerEdge, 
                       iP->flag(reco::PFCandidate::E_PRESHOWER_EDGE));
    outPfCand->SetFlag(mithep::PFCandidate::eEMPreshower, 
                       iP->flag(reco::PFCandidate::E_PRESHOWER));
    outPfCand->SetFlag(mithep::PFCandidate::eEMEndCapEdge, 
                       iP->flag(reco::PFCandidate::E_ENDCAP_EDGE));
    outPfCand->SetFlag(mithep::PFCandidate::eHEta0, 
                       iP->flag(reco::PFCandidate::H_ETA_0));
    outPfCand->SetFlag(mithep::PFCandidate::eHBarrelEndcap, 
                       iP->flag(reco::PFCandidate::H_BARREL_ENDCAP));
    outPfCand->SetFlag(mithep::PFCandidate::eHEndcapVFCal, 
                       iP->flag(reco::PFCandidate::H_ENDCAP_VFCAL));
    outPfCand->SetFlag(mithep::PFCandidate::eHVFCalEdge, 
                       iP->flag(reco::PFCandidate::H_VFCAL_EDGE));
    outPfCand->SetFlag(mithep::PFCandidate::eToNuclInt, 
                       iP->flag(reco::PFCandidate::T_TO_NUCLINT));
    outPfCand->SetFlag(mithep::PFCandidate::eFromNuclInt, 
                       iP->flag(reco::PFCandidate::T_FROM_NUCLINT));
    outPfCand->SetFlag(mithep::PFCandidate::eFromV0, 
                       iP->flag(reco::PFCandidate::T_FROM_V0));
    outPfCand->SetFlag(mithep::PFCandidate::eFromGammaConv, 
                       iP->flag(reco::PFCandidate::T_FROM_GAMMACONV));
    outPfCand->SetFlag(mithep::PFCandidate::eToConversion, 
                       iP->flag(reco::PFCandidate::GAMMA_TO_GAMMACONV));

    // fill references to other branches
    if (trackerTrackMap_ && iP->trackRef().isNonnull()) 
      outPfCand->SetTrackerTrk(trackerTrackMap_->GetMit(refToPtr(iP->trackRef())));    
    if (gsfTrackMap_ && iP->gsfTrackRef().isNonnull()) 
      outPfCand->SetGsfTrk(gsfTrackMap_->GetMit(refToPtr(iP->gsfTrackRef())));
    if (muonMap_ && iP->muonRef().isNonnull()) 
      outPfCand->SetMuon(muonMap_->GetMit(refToPtr(iP->muonRef())));
    if (conversionMap_ && iP->conversionRef().isNonnull()) 
      outPfCand->SetConversion(conversionMap_->GetMit(iP->conversionRef()));

    // add to exported pf candidate map
    reco::PFCandidatePtr thePtr(hPfCandProduct, iP - inPfCands.begin());
    pfCandMap_->Add(thePtr, outPfCand);
    
  }
  pfCands_->Trim();
}

//--------------------------------------------------------------------------------------------------
void FillerPFCandidates::ResolveLinks(const edm::Event      &event, 
                                      const edm::EventSetup &setup)
{
  // Resolve and fill mother-daughter links.

  Handle<reco::PFCandidateCollection> hPfCandProduct;
  GetProduct(edmName_, hPfCandProduct, event);  
  const reco::PFCandidateCollection &inPfCands = *(hPfCandProduct.product());

  // loop through pf candidates and resolve mother-daughter links
  for (reco::PFCandidateCollection::const_iterator iP = inPfCands.begin(); 
       iP != inPfCands.end(); ++iP) {
    
    reco::PFCandidatePtr thePtr(hPfCandProduct, iP - inPfCands.begin());
    mithep::PFCandidate *outPfCand = pfCandMap_->GetMit(thePtr);

    // fill mother-daughter links
    const reco::CandidatePtr motherCandPtr = iP->sourceCandidatePtr(0);
    const reco::PFCandidatePtr motherPtr(motherCandPtr); 
    if (motherCandPtr.isNonnull()) {
      mithep::PFCandidate *mother = pfCandMap_->GetMit(motherPtr);
      outPfCand->SetMother(mother);
      mother->AddDaughter(outPfCand);
    }
  }
}
