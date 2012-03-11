// $Id: FillerPFCandidates.cc,v 1.14 2011/10/10 20:57:40 bendavid Exp $

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
  edmPfNoPileupName_(Conf().getUntrackedParameter<string>("edmName","pfNoElectrons")),
  mitName_(Conf().getUntrackedParameter<string>("mitName",Names::gkPFCandidatesBrn)),
  trackerTrackMapNames_(Conf().exists("trackerTrackMapNames") ? 
                    Conf().getUntrackedParameter<vector<string> >("trackerTrackMapNames") : 
                    vector<string>()),
  gsfTrackMapName_(Conf().getUntrackedParameter<string>("gsfTrackMapName","")),
  muonMapName_(Conf().getUntrackedParameter<string>("muonMapName","")),
  conversionMapName_(Conf().getUntrackedParameter<string>("conversionMapName","")),
  pfCandMapName_(Conf().getUntrackedParameter<string>("pfCandMapName","")),
  pfNoPileupCandMapName_(Conf().getUntrackedParameter<string>("pfNoPileupMapName","")),
  allowMissingTrackRef_(Conf().getUntrackedParameter<bool>("allowMissingTrackRef",false)),
  fillPfNoPileup_(Conf().getUntrackedParameter<bool>("fillPfNoPileup",true)),
  gsfTrackMap_(0),
  muonMap_(0),
  conversionMap_(0),
  pfCandMap_(new mithep::PFCandidateMap),
  pfNoPileupCandMap_(new mithep::PFCandidateMap),
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
  delete pfNoPileupCandMap_;
}

//--------------------------------------------------------------------------------------------------
void FillerPFCandidates::BookDataBlock(TreeWriter &tws) 
{ 
  // Add particle-flow candidate branch to tree and get pointers to maps.

  tws.AddBranch(mitName_,&pfCands_);
  OS()->add<mithep::PFCandidateArr>(pfCands_,mitName_);

//   if (!trackerTrackMapName_.empty()) {
//     trackerTrackMap_ = OS()->get<TrackMap>(trackerTrackMapName_);
//     if (trackerTrackMap_)
//       AddBranchDep(mitName_,trackerTrackMap_->GetBrName());
//   }

  for (std::vector<std::string>::const_iterator bmapName = trackerTrackMapNames_.begin();
        bmapName!=trackerTrackMapNames_.end(); ++bmapName) {
    if (!bmapName->empty()) {
      const TrackMap *map = OS()->get<TrackMap>(*bmapName);
      if (map) {
        trackerTrackMaps_.push_back(map);
        AddBranchDep(mitName_,map->GetBrName());
      }
    }
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
  if (!pfNoPileupCandMapName_.empty()) {
    pfNoPileupCandMap_->SetBrName(mitName_);
    OS()->add<PFCandidateMap>(pfNoPileupCandMap_,pfNoPileupCandMapName_);
  }
}

//--------------------------------------------------------------------------------------------------
void FillerPFCandidates::FillDataBlock(const edm::Event      &event, 
                                       const edm::EventSetup &setup)
{
  // Fill muon information. 

  pfCands_->Delete();
  pfCandMap_->Reset();
  pfNoPileupCandMap_->Reset();

  Handle<reco::PFCandidateCollection> hPfCandProduct;
  GetProduct(edmName_, hPfCandProduct, event);  
  const reco::PFCandidateCollection &inPfCands = *(hPfCandProduct.product());

  Handle<reco::PFCandidateCollection> hPfNoPileupCandProduct;
  GetProduct(edmPfNoPileupName_, hPfNoPileupCandProduct, event);  
  const reco::PFCandidateCollection &inPfNoPileupCands = *(hPfNoPileupCandProduct.product());
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
    outPfCand->SetPhiECal(iP->positionAtECALEntrance().phi());

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
    outPfCand->SetFlag(mithep::PFCandidate::eToDispVtx, 
                       iP->flag(reco::PFCandidate::T_TO_DISP));
    outPfCand->SetFlag(mithep::PFCandidate::eFromDispVtx, 
                       iP->flag(reco::PFCandidate::T_FROM_DISP));
    outPfCand->SetFlag(mithep::PFCandidate::eFromV0, 
                       iP->flag(reco::PFCandidate::T_FROM_V0));
    outPfCand->SetFlag(mithep::PFCandidate::eFromGammaConv, 
                       iP->flag(reco::PFCandidate::T_FROM_GAMMACONV));
    outPfCand->SetFlag(mithep::PFCandidate::eToConversion, 
                       iP->flag(reco::PFCandidate::GAMMA_TO_GAMMACONV));

    //printf("pf type = %i\n",iP->particleId());
                       
    // fill references to other branches
    if (iP->trackRef().isNonnull()) {
      //printf("track: process = %i, product = %i, algo = %i, highPurity = %i\n",iP->trackRef().id().processIndex(),iP->trackRef().id().productIndex(),iP->trackRef()->algo(),iP->trackRef()->quality(reco::TrackBase::highPurity));
      const mithep::Track *thetrack = getMitTrack(refToPtr(iP->trackRef()),allowMissingTrackRef_);
      outPfCand->SetTrackerTrk(thetrack);
    }    
    if (gsfTrackMap_ && iP->gsfTrackRef().isNonnull()) 
      outPfCand->SetGsfTrk(gsfTrackMap_->GetMit(refToPtr(iP->gsfTrackRef())));
    if (muonMap_ && iP->muonRef().isNonnull()) 
      outPfCand->SetMuon(muonMap_->GetMit(refToPtr(iP->muonRef())));
    if (conversionMap_ && iP->conversionRef().isNonnull()) 
      outPfCand->SetConversion(conversionMap_->GetMit(iP->conversionRef()));

    // add to exported pf candidate map
    reco::PFCandidatePtr thePtr(hPfCandProduct, iP - inPfCands.begin());
    pfCandMap_->Add(thePtr, outPfCand);
    
    //add pf No Pileup map
    //===> Do I do a loop?
    bool found = false;
    if(fillPfNoPileup_) { 
      outPfCand->SetFlag(mithep::PFCandidate::ePFNoPileup,false);
      for (reco::PFCandidateCollection::const_iterator iNoPileupP = inPfNoPileupCands.begin();
	   iNoPileupP != inPfNoPileupCands.end(); ++iNoPileupP) {
	if(iP->px() == iNoPileupP->px() && iP->py() == iNoPileupP->py() && iNoPileupP->pz() == iP->pz()) { 
	  reco::PFCandidatePtr theNoPileupPtr(hPfNoPileupCandProduct, iNoPileupP - inPfNoPileupCands.begin());
	  pfNoPileupCandMap_->Add(theNoPileupPtr, outPfCand);
	  outPfCand->SetFlag(mithep::PFCandidate::ePFNoPileup,true);
	  if(found) edm::Exception(edm::errors::Configuration, "FillerPFCandidates:FillDataBlock()\n")
	    << "Error! PF No Pileup double linked " << endl;
	  found = true;
	}
      }
    }
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
//     const reco::CandidatePtr motherCandPtr = iP->sourceCandidatePtr(0);
//     const reco::PFCandidatePtr motherPtr(motherCandPtr); 
//     if (motherCandPtr.isNonnull()) {
//       mithep::PFCandidate *mother = pfCandMap_->GetMit(motherPtr);
//       outPfCand->SetMother(mother);
//       mother->AddDaughter(outPfCand);
//     }
  }
}

//--------------------------------------------------------------------------------------------------
const mithep::Track *FillerPFCandidates::getMitTrack(mitedm::TrackPtr ptr, bool allowmissing) const
{
  // Return our particle referenced by the edm pointer.

  mithep::Track *mitPart = 0;
  for (std::vector<const mithep::TrackMap*>::const_iterator bmap = trackerTrackMaps_.begin();
        bmap!=trackerTrackMaps_.end(); ++bmap) {
    const mithep::TrackMap *theMap = *bmap;
    if (theMap->HasMit(ptr)) {
      mitPart = theMap->GetMit(ptr);
      return mitPart;
    }
  }
  
  if (!mitPart && !allowmissing)
    throw edm::Exception(edm::errors::Configuration, "FillerPFCandidates::FillDataBlock()\n")
    << "Error! MITHEP Object " 
    << "not found in AssociationMaps (" << typeid(*this).name() << ")." << std::endl;
    
  return mitPart;
}
