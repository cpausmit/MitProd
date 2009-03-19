// $Id: FillerTracks.cc,v 1.29 2009/03/15 11:20:41 loizides Exp $

#include "MitProd/TreeFiller/interface/FillerTracks.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/TrackReco/interface/HitPattern.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticleFwd.h"
#include "DataFormats/RecoCandidate/interface/TrackAssociation.h"
#include "TrackingTools/TrackAssociator/interface/TrackDetectorAssociator.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateTransform.h"
#include "MagneticField/Engine/interface/MagneticField.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "MitEdm/DataFormats/interface/Types.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerTracks::FillerTracks(const ParameterSet &cfg, const char *name, bool active) :
  BaseFiller(cfg,name,active),
  ecalAssocActive_(Conf().getUntrackedParameter<bool>("ecalAssocActive",0)),
  edmName_(Conf().getUntrackedParameter<string>("edmName","generalTracks")),
  mitName_(Conf().getUntrackedParameter<string>("mitName",Names::gkTrackBrn)),
  edmSimAssocName_(Conf().getUntrackedParameter<string>("edmSimAssociationName","")),
  trackingMapName_(Conf().getUntrackedParameter<string>("trackingMapName","TrackingMap")),
  barrelSuperClusterIdMapName_(Conf().getUntrackedParameter<string>
                               ("superClusterIdMapName","barrelSuperClusterIdMap")),
  endcapSuperClusterIdMapName_(Conf().getUntrackedParameter<string>
                               ("endcapClusterIdMapName","endcapSuperClusterIdMap")),
  trackMapName_(Conf().getUntrackedParameter<string>("trackMapName",
                                                     Form("%sMapName",mitName_.c_str()))),
  trackingMap_(0),
  tracks_(new mithep::TrackArr(250)), 
  trackMap_(new mithep::TrackMap)
{
  // Constructor.
  
  if (ecalAssocActive_) // initialize track associator configuration if needed
    assocParams_.loadParameters(
      cfg.getUntrackedParameter<ParameterSet>("TrackAssociatorParameters"));
}

//--------------------------------------------------------------------------------------------------
FillerTracks::~FillerTracks()
{
  // Destructor.

  delete tracks_;
  delete trackMap_;
}

//--------------------------------------------------------------------------------------------------
void FillerTracks::BookDataBlock(TreeWriter &tws)
{
  // Add tracks branch to tree, publish and get our objects.

  tws.AddBranch(mitName_,&tracks_);
  OS()->add<TrackArr>(tracks_,mitName_);

  if (!trackingMapName_.empty()) {
    trackingMap_ = OS()->get<TrackingParticleMap>(trackingMapName_);
    if (trackingMap_)
      AddBranchDep(mitName_,trackingMap_->GetBrName());
  }
  if (ecalAssocActive_ && !barrelSuperClusterIdMapName_.empty()) {
    barrelSuperClusterIdMap_ = OS()->get<SuperClusterIdMap>(barrelSuperClusterIdMapName_);
    if (barrelSuperClusterIdMap_)
      AddBranchDep(mitName_,barrelSuperClusterIdMap_->GetBrName());
  }
  if (ecalAssocActive_ && !endcapSuperClusterIdMapName_.empty()) {
    endcapSuperClusterIdMap_ = OS()->get<SuperClusterIdMap>(endcapSuperClusterIdMapName_);
    if (endcapSuperClusterIdMap_)
      AddBranchDep(mitName_,endcapSuperClusterIdMap_->GetBrName());
  }
  if (!trackMapName_.empty()) {
    trackMap_->SetBrName(mitName_);
    OS()->add<TrackMap>(trackMap_,trackMapName_);
  }
}

//--------------------------------------------------------------------------------------------------
void FillerTracks::FillDataBlock(const edm::Event      &event, 
                                 const edm::EventSetup &setup)
{
  // Fill tracks from edm collection into our collection.

  tracks_  ->Delete();
  trackMap_->Reset();

  // initialize handle and get product,
  // this usage allows also to get collections of classes which inherit from reco::Track
  Handle<View<reco::Track> > hTrackProduct;
  GetProduct(edmName_, hTrackProduct, event);  
	
  trackMap_->SetEdmProductId(hTrackProduct.id().id());
  const View<reco::Track> inTracks = *(hTrackProduct.product());  
  
  // for MC SimParticle association (reco->sim mappings)
  reco::RecoToSimCollection simAssociation;
  if (trackingMap_ && !edmSimAssocName_.empty()) {
    Handle<reco::RecoToSimCollection> simAssociationProduct;
    GetProduct(edmSimAssocName_, simAssociationProduct, event);  
    simAssociation = *(simAssociationProduct.product());
    if (verbose_>1)
      printf("SimAssociation Map Size = %i\n",simAssociation.size());
  }
  
  // set up associator for Track-Ecal associations
  TrackDetectorAssociator trackAssociator;
  trackAssociator.useDefaultPropagator();
  edm::ESHandle<MagneticField> bField;
  if (ecalAssocActive_)
    setup.get<IdealMagneticFieldRecord>().get(bField);
  
  // loop through all tracks and fill the information
  for (View<reco::Track>::const_iterator it = inTracks.begin();
         it != inTracks.end(); ++it) {
         
    mithep::Track *outTrack = tracks_->Allocate();
    // create track and set the core parameters
    new (outTrack) mithep::Track(it->qoverp(),it->lambda(),
                                 it->phi(),it->dxy(),it->dsz());
    outTrack->SetErrors(it->qoverpError(),it->lambdaError(),
                        it->phiError(),it->dxyError(),it->dszError());
	
    // fill track quality information
    outTrack->SetChi2(it->chi2());
    outTrack->SetNdof(static_cast<Int_t>(it->ndof()));
    
    //fill hit layer map
    const reco::HitPattern &hits = it->hitPattern();
    for (Int_t i=0; i<hits.numberOfHits(); i++) {
      uint32_t hit = hits.getHitPattern(i);
      if (hits.validHitFilter(hit))
        if (hits.trackerHitFilter(hit))
          outTrack->SetHit(hitReader_.Layer(hit));
                
      if (verbose_>2) {
        if (hits.muonDTHitFilter(hit))
          printf("Muon DT Layer = %i\n", hits.getLayer(hit));
        if (hits.muonCSCHitFilter(hit))
          printf("Muon CSC Layer = %i\n", hits.getLayer(hit));        
        if (hits.muonRPCHitFilter(hit))
          printf("Muon RPC Layer = %i\n", hits.getLayer(hit));
      }
    }

    //make ecal associations
    if (ecalAssocActive_) {
      TrackDetMatchInfo matchInfo;
      //Extra check to allow propagation to work in AOD where no TrackExtra is available
      if (it->extra().isAvailable())
        matchInfo = trackAssociator.associate(event,setup,*it,assocParams_);
      else {
        TrajectoryStateTransform tsTransform;
        FreeTrajectoryState initialState = tsTransform.initialFreeState(*it,&*bField);
        matchInfo = trackAssociator.associate(event, setup, assocParams_, &initialState);
      }
        
      outTrack->SetEtaEcal(matchInfo.trkGlobPosAtEcal.eta());
      outTrack->SetPhiEcal(matchInfo.trkGlobPosAtEcal.phi());

      //fill supercluster link
      if (barrelSuperClusterIdMap_ || endcapSuperClusterIdMap_) {
        mithep::SuperCluster *cluster = 0;
        for (std::vector<const ::CaloTower*>::const_iterator iTower =
               matchInfo.crossedTowers.begin(); 
             iTower<matchInfo.crossedTowers.end() && !cluster; iTower++) {

          for (uint ihit=0; ihit<(*iTower)->constituentsSize() && !cluster; ihit++) {
            DetId hit = (*iTower)->constituent(ihit);
            if (barrelSuperClusterIdMap_ && barrelSuperClusterIdMap_->HasMit(hit))
              cluster = barrelSuperClusterIdMap_->GetMit(hit);
            else if (endcapSuperClusterIdMap_ && endcapSuperClusterIdMap_->HasMit(hit))
              cluster = endcapSuperClusterIdMap_->GetMit(hit);
          }
        }
        if (cluster)
          outTrack->SetSCluster(cluster);
      }
    }
    
    // add reference between mithep and edm object
    if (trackMap_) {
      mitedm::TrackPtr thePtr = inTracks.ptrAt(it - inTracks.begin());
      trackMap_->Add(thePtr, outTrack);
    }

    //do dim associations
    if (trackingMap_ && !edmSimAssocName_.empty()) {
      if (verbose_>1)
        printf("Trying Track-Sim association\n");
      reco::TrackBaseRef theBaseRef = inTracks.refAt(it - inTracks.begin());
      vector<pair<TrackingParticleRef, double> > simRefs;
      Bool_t noSimParticle = 0;
      try {
        simRefs = simAssociation[theBaseRef]; //try to get the sim references if existing
      }
      catch (edm::Exception &ex) {
        noSimParticle = 1;
      }

      if (!noSimParticle) { //loop through sim match candidates
        if (verbose_>1)
          printf("Applying track-sim association\n");
        for (vector<pair<TrackingParticleRef, double> >::const_iterator simRefPair=simRefs.begin(); 
             simRefPair != simRefs.end(); ++simRefPair) 
          if (simRefPair->second > 0.5) // require more than 50% shared hits between reco and sim
            outTrack->SetMCPart(trackingMap_->GetMit(simRefPair->first)); //add reco->sim reference
      }
    }
  }
  tracks_->Trim();
}
