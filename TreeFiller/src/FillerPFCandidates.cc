#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/EgammaCandidates/interface/Photon.h"
#include "DataFormats/EgammaCandidates/interface/PhotonFwd.h"
#include "DataFormats/Common/interface/RefToPtr.h"
#include "MitAna/DataTree/interface/Muon.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataTree/interface/PFCandidateCol.h"
#include "MitAna/DataTree/interface/Track.h"
#include "MitProd/ObjectService/interface/ObjectService.h"
#include "MitProd/TreeFiller/interface/FillerPFCandidates.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerPFCandidates::FillerPFCandidates(const edm::ParameterSet &cfg, 
                                       edm::ConsumesCollector& collector,
                                       ObjectService* os,
                                       const char *name, bool active) :
  BaseFiller                    (cfg,os,name,active),
  edmToken_(GetToken<PFCollection>(collector, "edmName","particleFlow")),
  edmPfNoPileupToken_(GetToken<PFCollection>(collector, "edmPfNoPileupName", "pfNoElectrons")),
  mitName_                      (Conf().getUntrackedParameter<string>("mitName",
								      Names::gkPFCandidatesBrn)),
  trackerTrackMapNames_         (Conf().exists("trackerTrackMapNames") ? 
				 Conf().getUntrackedParameter<vector<string> >("trackerTrackMapNames") : 
				 vector<string>()),
  gsfTrackMapName_              (Conf().getUntrackedParameter<string>("gsfTrackMapName","")),
  muonMapName_                  (Conf().getUntrackedParameter<string>("muonMapName","")),
  electronMapName_              (Conf().getUntrackedParameter<string>("electronMapName","")),
  photonMapName_                (Conf().getUntrackedParameter<string>("photonMapName","")),
  barrelSuperClusterMapName_    (Conf().getUntrackedParameter<string>("barrelSuperClusterMapName",
								      "")),
  endcapSuperClusterMapName_    (Conf().getUntrackedParameter<string>("endcapSuperClusterMapName",
								      "")),
  pfElectronSuperClusterMapName_(Conf().getUntrackedParameter<string>("pfElectronSuperClusterMapName",
								      "")),
  conversionMapName_            (Conf().getUntrackedParameter<string>("conversionMapName","")),
  pfCandMapName_                (Conf().getUntrackedParameter<string>("pfCandMapName","")),
  pfNoPileupCandMapName_        (Conf().getUntrackedParameter<string>("pfNoPileupCandMapName","")),
  allowMissingTrackRef_         (Conf().getUntrackedParameter<bool>("allowMissingTrackRef",false)),
  allowMissingPhotonRef_        (Conf().getUntrackedParameter<bool>("allowMissingPhotonRef",false)),
  fillPfNoPileup_               (Conf().getUntrackedParameter<bool>("fillPfNoPileup",true)),
  gsfTrackMap_                  (0),
  muonMap_                      (0),
  electronMap_                  (0),
  photonMap_                    (0),
  barrelSuperClusterMap_        (0),
  endcapSuperClusterMap_        (0),
  pfElectronSuperClusterMap_    (0),
  conversionMap_                (0),
  pfCandMap_                    (new mithep::PFCandidateMap),
  pfNoPileupCandMap_            (new mithep::PFCandidateMap),
  pfCands_                      (new mithep::PFCandidateArr(16))
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
  // book all tracker maps
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
  if (!electronMapName_.empty()) {
    electronMap_ = OS()->get<ElectronMap>(electronMapName_);
    if (electronMap_)
      AddBranchDep(mitName_,electronMap_->GetBrName());
  }
  if (!photonMapName_.empty()) {
    photonMap_ = OS()->get<PhotonMap>(photonMapName_);
    if (photonMap_)
      AddBranchDep(mitName_,photonMap_->GetBrName());
  }
  if (!barrelSuperClusterMapName_.empty()) {
    barrelSuperClusterMap_ = OS()->get<SuperClusterMap>(barrelSuperClusterMapName_);
    if (barrelSuperClusterMap_)
      AddBranchDep(mitName_,barrelSuperClusterMap_->GetBrName());
  }
  if (!endcapSuperClusterMapName_.empty()) {
    endcapSuperClusterMap_ = OS()->get<SuperClusterMap>(endcapSuperClusterMapName_);
    if (endcapSuperClusterMap_)
      AddBranchDep(mitName_,endcapSuperClusterMap_->GetBrName());
  }
  if (!pfElectronSuperClusterMapName_.empty()) {
    pfElectronSuperClusterMap_ = OS()->get<SuperClusterMap>(pfElectronSuperClusterMapName_);
    if (pfElectronSuperClusterMap_)
      AddBranchDep(mitName_,pfElectronSuperClusterMap_->GetBrName());
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
  // Fill particle flow candidate information. 

  pfCands_->Delete();
  pfCandMap_->Reset();
  pfNoPileupCandMap_->Reset();

  // get PF Candidates
  //Handle<reco::PFCandidateCollection> hPfCandProduct;
  //GetProduct(edmName_, hPfCandProduct, event);  
  //const reco::PFCandidateCollection &inPfCands = *(hPfCandProduct.product());
  //printf(" Get Particle Flow Candidates\n");
  Handle<PFCollection> hPfCandProduct;
  GetProduct(edmToken_, hPfCandProduct, event);  
  const PFCollection &inPfCands = *(hPfCandProduct.product());

  // get PF NoPileup Candidates
  //CP Handle<reco::PFCandidateCollection> hPfNoPileupCandProduct;
  //CP //Handle<reco::PFCandidateCollection> hPfNoPileupCandProduct;
  //CP GetProduct(edmPfNoPileupName_, hPfNoPileupCandProduct, event);  
  //CP const reco::PFCandidateCollection &inPfNoPileupCands = *(hPfNoPileupCandProduct.product());
  //printf(" Get Particle Flow Candidates - no pileup\n");
  Handle<PFCollection> hPfNoPileupCandProduct;
  GetProduct(edmToken_, hPfNoPileupCandProduct, event);  
  const PFCollection &inPfNoPileupCands = *(hPfNoPileupCandProduct.product());

  //for (reco::PFCandidateCollection::const_iterator iP = inPfCands.begin(); 
  //     iP != inPfCands.end(); ++iP) {
  for (PFCollection::const_iterator iPf = inPfCands.begin(); iPf != inPfCands.end(); ++iPf) {

    // Not nice put well....
    const reco::PFCandidate *iP = &(*(*iPf));

    mithep::PFCandidate *outPfCand = pfCands_->Allocate();
    new (outPfCand) mithep::PFCandidate(iP->px(),iP->py(),iP->pz(),iP->energy());

    // fill standard variables
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
                       
    // fill references to other branches
    if (iP->trackRef().isNonnull()) {
      if (0)  // for debugging
    	printf("track: process = %i, product = %i, algo = %i, highPurity = %i\n",
    	       iP->trackRef().id().processIndex(),iP->trackRef().id().productIndex(),
    	       iP->trackRef()->algo(),iP->trackRef()->quality(reco::TrackBase::highPurity));
      const mithep::Track *thetrack = getMitTrack(refToPtr(iP->trackRef()),allowMissingTrackRef_);
      outPfCand->SetTrackerTrk(thetrack);
    }    
    
    // linking with the GfsTracks
    if (gsfTrackMap_ && iP->gsfTrackRef().isNonnull()) 
      outPfCand->SetGsfTrk(gsfTrackMap_->GetMit(refToPtr(iP->gsfTrackRef())));
    
    // linking with the Muons
    if (muonMap_ && iP->muonRef().isNonnull()) 
      outPfCand->SetMuon(muonMap_->GetMit(refToPtr(iP->muonRef())));
    
    // linking with the Electrons
    if (electronMap_ && iP->gsfElectronRef().isNonnull()) 
      outPfCand->SetElectron(electronMap_->GetMit(refToPtr(iP->gsfElectronRef())));
    
    // linking with the Photons
    if (photonMap_ && iP->photonRef().isNonnull()) 
      try{outPfCand->SetPhoton(photonMap_->GetMit(refToPtr(iP->photonRef())));} 
      catch (...) { 
    	if (!allowMissingPhotonRef_) {
    	  throw edm::Exception(edm::errors::Configuration, "FillerPFCandidates:FillDataBlock()\n")
    	    << "Error! Photon unmapped collection";
    	}
      }
    
    // linking with the SuperClusters
    if (barrelSuperClusterMap_ && endcapSuperClusterMap_ &&
    	pfElectronSuperClusterMap_ && iP->superClusterRef().isNonnull()) {
      if (barrelSuperClusterMap_->HasMit(iP->superClusterRef()))
        outPfCand->SetSCluster(barrelSuperClusterMap_->GetMit(iP->superClusterRef()));
      else if (endcapSuperClusterMap_->HasMit(iP->superClusterRef()))
        outPfCand->SetSCluster(endcapSuperClusterMap_->GetMit(iP->superClusterRef()));
      else if (pfElectronSuperClusterMap_->HasMit(iP->superClusterRef()))
        outPfCand->SetSCluster(pfElectronSuperClusterMap_->GetMit(iP->superClusterRef()));
    }
    
    if (conversionMap_ && iP->conversionRef().isNonnull()) 
      outPfCand->SetConversion(conversionMap_->GetMit(iP->conversionRef()));
    
    // add to exported pf candidate map
    reco::PFCandidatePtr ptr = (*iPf).ptr();
    pfCandMap_->Add(ptr,outPfCand);

    //CP //reco::PFCandidatePtr thePtr(hPfCandProduct,inPfCands[i]);
    //CP //pfCandMap_->Add(thePtr,outPfCand);
    
    // add pf No Pileup map
    bool found = false;
    if (fillPfNoPileup_) { 
      // initially set the candidate to be not part of the NoPilup collection
      outPfCand->SetFlag(mithep::PFCandidate::ePFNoPileup,false);

      // try to find match with the no-pileup map
      //for (reco::PFCandidateCollection::const_iterator iNoPileupP = inPfNoPileupCands.begin();
      //     iNoPileupP != inPfNoPileupCands.end(); ++iNoPileupP) {

      for (PFCollection::const_iterator iPfNp = inPfNoPileupCands.begin();
	   iPfNp != inPfNoPileupCands.end(); ++iPfNp) {
	// Not nice put well....
	const reco::PFCandidate *iPNp = &(*(*iPfNp));
    
    	// this is not the best way to do it, but well I did not come up with a better one yet
    	if (iP->px() == iPNp->px() &&
    	    iP->py() == iPNp->py() &&
    	    iP->pz() == iPNp->pz()   ) {
    
    	  // set the candidate to be part of the NoPilup collection
    	  outPfCand->SetFlag(mithep::PFCandidate::ePFNoPileup,true);
	  // add it to our map

	  reco::PFCandidatePtr ptrNp = (*iPfNp).ptr();
	  pfNoPileupCandMap_->Add(ptrNp,outPfCand);

    	  //CP reco::PFCandidatePtr theNoPileupPtr(hPfNoPileupCandProduct,
    	  //                                    iNoPileupP-inPfNoPileupCands.begin());
    	  //CP pfNoPileupCandMap_->Add(theNoPileupPtr,outPfCand);

    	  if (found)
    	    edm::Exception(edm::errors::Configuration, "FillerPFCandidates:FillDataBlock()\n")
    	      << "PF No Pileup was double linked!! " << endl;
    	  found = true;
    	}
      }
    }
  }

  //printf(" Trim and get out.\n");
  pfCands_->Trim();
}

//--------------------------------------------------------------------------------------------------
void FillerPFCandidates::ResolveLinks(const edm::Event      &event, 
                                      const edm::EventSetup &setup)
{
  // Resolve and fill mother-daughter links.

  //CP Handle<reco::PFCandidateCollection> hPfCandProduct;
  //CP GetProduct(edmName_, hPfCandProduct, event);  
  //CP const reco::PFCandidateCollection &inPfCands = *(hPfCandProduct.product());
  //CP for (reco::PFCandidateCollection::const_iterator iP = inPfCands.begin(); iP != inPfCands.end();
  //CP      ++iP) {
  //CP   reco::PFCandidatePtr thePtr(hPfCandProduct, iP-inPfCands.begin());
  //CP   // CP: I found this code commented, not sure why it was commented, need to check this
  //CP 
  //CP   //mithep::PFCandidate *outPfCand = pfCandMap_->GetMit(thePtr);
  //CP   // fill mother-daughter links
  //CP   //     const reco::CandidatePtr motherCandPtr = iP->sourceCandidatePtr(0);
  //CP   //     const reco::PFCandidatePtr motherPtr(motherCandPtr); 
  //CP   //     if (motherCandPtr.isNonnull()) {
  //CP   //       mithep::PFCandidate *mother = pfCandMap_->GetMit(motherPtr);
  //CP   //       outPfCand->SetMother(mother);
  //CP   //       mother->AddDaughter(outPfCand);
  //CP   //     }
  //CP }

  // get PF Candidates
  //printf(" Get Particle Flow Candidates - ResolveLinks\n");
  Handle<PFCollection> hPfCandProduct;
  GetProduct(edmToken_, hPfCandProduct, event);  
  const PFCollection &inPfCands = *(hPfCandProduct.product());

  // loop through pf candidates and resolve mother-daughter links


  for (PFCollection::const_iterator iPf = inPfCands.begin(); iPf != inPfCands.end(); ++iPf) {
    // Not nice put well....
    //const reco::PFCandidate *iP = &(*(*iPf));
    
    // the FwdPtr is *iPf

    // NOTHING IS HAPPENING ?
  }
}

//--------------------------------------------------------------------------------------------------
const mithep::Track *FillerPFCandidates::getMitTrack(mitedm::TrackPtr ptr, bool allowMissing) const
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
  
  // make sure we got a link if it is required
  if (!mitPart && !allowMissing)
    throw edm::Exception(edm::errors::Configuration, "FillerPFCandidates::FillDataBlock()\n")
    << "Error! MITHEP Object " 
    << "not found in AssociationMaps (" << typeid(*this).name() << ")." << std::endl;
    
  return mitPart;
}
