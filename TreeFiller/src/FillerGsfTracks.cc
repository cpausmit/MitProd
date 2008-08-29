// $Id: FillerGsfTracks.cc,v 1.12 2008/08/28 22:21:01 loizides Exp $

#include "MitProd/TreeFiller/interface/FillerGsfTracks.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrackFwd.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticleFwd.h"
#include "DataFormats/RecoCandidate/interface/TrackAssociation.h"
#include "MitAna/DataTree/interface/Names.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerGsfTracks::FillerGsfTracks(const ParameterSet &cfg, const char *name, bool active) : 
  FillerTracks(cfg,name,active,"pixelMatchGsfFit","GsfTracks"),
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

  simMap_ = OS()->get<SimParticleMap>(simMapName_.c_str());
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
  if (simMap_ && !edmSimAssociationName_.empty()) {
    Handle<reco::RecoToSimCollection> hSimAssociationProduct;
    GetProduct(edmSimAssociationName_, hSimAssociationProduct, event);
    simAssociation = *(hSimAssociationProduct.product());
  }
  
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
          outTrack->SetHit(layerMap_[hit]);
    }
    
    
    // add reference between mithep and edm object
    reco::GsfTrackRef theRef(hTrackProduct, it - inTracks.begin());
    trackMap_->Add(theRef, outTrack);
	
    if (simMap_ && !edmSimAssociationName_.empty()) {
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
            outTrack->SetMCPart(simMap_->GetMit(simRefPair->first)); //add reco->sim reference
      }
    }
  }
  tracks_->Trim();
}
