// $Id: FillerTracks.cc,v 1.10 2008/07/28 23:13:44 paus Exp $

#include "MitProd/TreeFiller/interface/FillerTracks.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticleFwd.h"
#include "DataFormats/RecoCandidate/interface/TrackAssociation.h"
#include "MitAna/DataTree/interface/Names.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerTracks::FillerTracks(const ParameterSet &cfg, const char *name, 
                           bool active, const SimParticleMap *sm) : 
  BaseFiller(cfg,name,active),
  edmName_(Conf().getUntrackedParameter<string>("edmName","")),
  mitName_(Conf().getUntrackedParameter<string>("mitName","")),
  edmSimAssociationName_(Conf().getUntrackedParameter<string>("edmSimAssociationName","")),
  simMap_(sm),
  tracks_(new mithep::TrackArr(250)), 
  trackMap_(new mithep::TrackMap)
{
  // Constructor
}

//--------------------------------------------------------------------------------------------------
FillerTracks::~FillerTracks()
{
  // Destructor
  delete tracks_;
  delete trackMap_;
}

//--------------------------------------------------------------------------------------------------
void FillerTracks::BookDataBlock(TreeWriter &tws)
{
  // Add tracks branch to tree.
  tws.AddBranch(mitName_.c_str(),&tracks_);
}

//--------------------------------------------------------------------------------------------------
void FillerTracks::FillDataBlock(const edm::Event      &event, 
                                 const edm::EventSetup &setup)
{
  // -----------------------------------------------------------------------------------------------
  // Fill tracks from edm collection into our collection
  // -----------------------------------------------------------------------------------------------
  tracks_  ->Reset();
  trackMap_->Reset();

  Handle<reco::TrackCollection> hTrackProduct;
  GetProduct(edmName_, hTrackProduct, event);  
	
  trackMap_->SetEdmProductId(hTrackProduct.id().id());
  const reco::TrackCollection inTracks = *(hTrackProduct.product());  
  
  // -----------------------------------------------------------------------------------------------
  // for MC SimParticle association (reco->sim mappings)
  // -----------------------------------------------------------------------------------------------
  reco::RecoToSimCollection simAssociation;
  if (simMap_ && !edmSimAssociationName_.empty()) {
    Handle<reco::RecoToSimCollection> simAssociationProduct;
    GetProduct(edmSimAssociationName_, simAssociationProduct, event);  
    simAssociation = *(simAssociationProduct.product());
  }
  
  // -----------------------------------------------------------------------------------------------
  // loop through all tracks and fill the information
  // -----------------------------------------------------------------------------------------------
  for (reco::TrackCollection::const_iterator it = inTracks.begin(); 
       it != inTracks.end(); ++it) {
    mithep::Track *outTrack = tracks_->Allocate();
    // create track and set the core parameters
    new (outTrack) mithep::Track(it->phi(),it->d0(),it->pt(),it->dz(),it->theta());
    outTrack->SetErrors(it->phiError(),it->d0Error(),it->ptError(),it->dzError(),it->thetaError());
    outTrack->SetCharge(it->charge());
	
    // add reference between mithep and edm object
    reco::TrackRef theRef(hTrackProduct, it - inTracks.begin());
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
