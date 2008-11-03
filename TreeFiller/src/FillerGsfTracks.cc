// $Id: FillerGsfTracks.cc,v 1.16 2008/10/16 16:17:17 bendavid Exp $

#include "MitProd/TreeFiller/interface/FillerGsfTracks.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrackFwd.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticleFwd.h"
#include "DataFormats/RecoCandidate/interface/TrackAssociation.h"
#include "TrackingTools/TrackAssociator/interface/TrackDetectorAssociator.h"
#include "MitAna/DataTree/interface/Names.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerGsfTracks::FillerGsfTracks(const ParameterSet &cfg, const char *name, bool active, bool ecalActive) : 
  FillerTracks(cfg,name,active,ecalActive,"pixelMatchGsfFit","GsfTracks"),
  trackMap_(new mithep::GsfTrackMap)
{
  // Constructor.
}

//--------------------------------------------------------------------------------------------------
FillerGsfTracks::~FillerGsfTracks()
{
  // Destructor.

  delete trackMap_;
}

//--------------------------------------------------------------------------------------------------
void FillerGsfTracks::BookDataBlock(TreeWriter &tws)
{
  // Add tracks branch to tree, publish and get our objects.

  tws.AddBranch(mitName_.c_str(),&tracks_);

  trackingMap_ = OS()->get<TrackingParticleMap>(trackingMapName_.c_str());
  barrelSuperClusterIdMap_ = OS()->get<SuperClusterIdMap>(barrelSuperClusterIdMapName_.c_str());
  endcapSuperClusterIdMap_ = OS()->get<SuperClusterIdMap>(endcapSuperClusterIdMapName_.c_str());
  OS()->add<GsfTrackMap>(trackMap_,trackMapName_.c_str());
  OS()->add<TrackArr>(tracks_,mitName_.c_str());
}

//--------------------------------------------------------------------------------------------------
void FillerGsfTracks::FillDataBlock(const edm::Event      &event, 
                                    const edm::EventSetup &setup)
{
  // Fill tracks from edm collection into our collection.

  tracks_->Reset();
  trackMap_->Reset();
  
  Handle<reco::GsfTrackCollection> hTrackProduct;
  GetProduct(edmName_, hTrackProduct, event);

  const reco::GsfTrackCollection inTracks = *(hTrackProduct.product());  
  
  // if we have a Sim Particle association (for monte carlo), initialize the reco->sim mappings
  reco::RecoToSimCollection simAssociation;
  if (trackingMap_ && !edmSimAssocName_.empty()) {
    Handle<reco::RecoToSimCollection> hSimAssociationProduct;
    GetProduct(edmSimAssocName_, hSimAssociationProduct, event);
    simAssociation = *(hSimAssociationProduct.product());
  }
  
  //set up associator for Track-Ecal associations
  TrackDetectorAssociator trackAssociator;
  trackAssociator.useDefaultPropagator();

  // loop through all tracks
  for (reco::GsfTrackCollection::const_iterator it = inTracks.begin(); 
       it != inTracks.end(); ++it) {
    mithep::Track *outTrack = tracks_->Allocate();
    // create track and set the core parameters
    new (outTrack) mithep::Track(it->qoverp(),it->lambda(),it->phi(),it->dxy(),it->dsz());
    outTrack->SetErrors(it->qoverpError(),it->lambdaError(),it->phiError(),it->dxyError(),it->dszError());
	
    //Fill track quality information
    outTrack->SetChi2(it->chi2());
    outTrack->SetNdof(static_cast<Int_t>(it->ndof()));
    
    //Fill hit layer map
    const reco::HitPattern &hits = it->hitPattern();
    for (Int_t i=0; i<hits.numberOfHits(); i++) {
      uint32_t hit = hits.getHitPattern(i);
      if ( hits.validHitFilter(hit) )
        if ( hits.trackerHitFilter(hit) )
          outTrack->SetHit(hitReader_.Layer(hit));
    }
    
    //make ecal associations
    if (ecalAssocActive_) {
      TrackDetMatchInfo matchInfo = trackAssociator.associate(event,setup,
                                                             *it,
                                                              assocParams_);
      outTrack->SetEtaEcal(matchInfo.trkGlobPosAtEcal.eta());
      outTrack->SetPhiEcal(matchInfo.trkGlobPosAtEcal.phi());

      //fill supercluster link
      if (barrelSuperClusterIdMap_ || endcapSuperClusterIdMap_) {
        mithep::SuperCluster *cluster = 0;
        for (std::vector<const ::CaloTower*>::const_iterator iTower = matchInfo.crossedTowers.begin();
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
    reco::GsfTrackRef theRef(hTrackProduct, it - inTracks.begin());
    trackMap_->Add(theRef, outTrack);
	
    if (trackingMap_ && !edmSimAssocName_.empty()) {
      reco::TrackBaseRef theBaseRef(theRef);
      vector<pair<TrackingParticleRef, double> > simRefs;
      Bool_t noSimParticle = 0;
      try {
        simRefs = simAssociation[theBaseRef]; //try to get the sim references if existing
      }
      catch (edm::Exception &ex) {
        noSimParticle = 1;
      }

      if (!noSimParticle) { //loop through sim match candidates
        for (vector<pair<TrackingParticleRef, double> >::const_iterator simRefPair=simRefs.begin(); 
             simRefPair != simRefs.end(); ++simRefPair) 

          if (simRefPair->second > 0.5) // require more than 50% shared hits between reco and sim
            outTrack->SetMCPart(trackingMap_->GetMit(simRefPair->first)); //add reco->sim reference
      }
    }
  }
  tracks_->Trim();
}
