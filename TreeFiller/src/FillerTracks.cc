// $Id: FillerTracks.cc,v 1.3 2008/07/02 13:30:09 bendavid Exp $

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
  BaseFiller(cfg, name, active),
  edmName_(Conf().getUntrackedParameter<string>("edmName","")),
  mitName_(Conf().getUntrackedParameter<string>("mitName","")),
  edmSimAssociationName_(Conf().getUntrackedParameter<string>("edmSimAssociationName","")),
  simMap_(sm),
  tracks_(new mithep::TrackArr), 
  trackMap_(new mithep::TrackMap)
{
  // Constructor.
}

//--------------------------------------------------------------------------------------------------
FillerTracks::~FillerTracks()
{
  // Destructor.

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
  // Fill tracks from edm collection into our collection.

  tracks_->Reset();
  trackMap_->Reset();
  
  // get the tracks collection
  try {
    event.getByLabel(edm::InputTag(edmName_),trackProduct_);
  } catch (cms::Exception& ex) {
    edm::LogError("FillerTracks") << "Error! Cannot get collection with label " 
                                  << edmName_ << endl;
    throw edm::Exception(edm::errors::Configuration, "FillerTracks:FillDataBlock()\n")
      << "Error! Cannot get collection with label " << edmName_ << endl;
  }
	
  trackMap_->SetEdmProductId(trackProduct_.id().id());
  const reco::TrackCollection inTracks = *(trackProduct_.product());  
  
  // if we have a Sim Particle association (for monte carlo), initialize the reco->sim mappings
  reco::RecoToSimCollection simAssociation;
  if (simMap_ && !edmSimAssociationName_.empty()) {
    Handle<reco::RecoToSimCollection> simAssociationProduct;
    try {
      event.getByLabel(edm::InputTag(edmSimAssociationName_), simAssociationProduct);
    }
    catch (cms::Exception& ex) {
      edm::LogError("FillerTracks") << "Error! Cannot get collection with label " 
                                    << edmSimAssociationName_ << endl;
      throw edm::Exception(edm::errors::Configuration, "FillerTracks:FillDataBlock()\n")
        << "Error! Cannot get collection with label " << edmSimAssociationName_ << endl;
    }
    simAssociation = *(simAssociationProduct.product());
  }
  
  // loop through all tracks
  for (reco::TrackCollection::const_iterator inTrack = inTracks.begin(); 
       inTrack != inTracks.end(); ++inTrack) {
    
    mithep::Track* outTrack = tracks_->Allocate();
    new (outTrack) mithep::Track(inTrack->phi(),
                                 inTrack->d0(),
                                 inTrack->pt(),
                                 inTrack->dz(),
                                 inTrack->theta());
	
    outTrack->SetErrors(inTrack->phiError(),
                        inTrack->d0Error(),
                        inTrack->ptError(),
                        inTrack->dzError(),
                        inTrack->thetaError());

    outTrack->SetCharge(inTrack->charge());
	
    // add reference between mithep and edm object
    reco::TrackRef theRef(trackProduct_, inTrack-inTracks.begin());
    trackMap_->Add(theRef, outTrack);
	
    if (simMap_ && !edmSimAssociationName_.empty()) {
      reco::TrackBaseRef theBaseRef(theRef);
      vector<pair<TrackingParticleRef, double> > simRefs;
      Bool_t noSimParticle=0;
      try {
        simRefs = simAssociation[theBaseRef]; //try to get the sim references if existing
      }
      catch (edm::Exception& ex) {
        noSimParticle=1;
      }
      if (!noSimParticle) { //loop through sim match candidates
        for (vector<pair<TrackingParticleRef, double> >::const_iterator simRefPair=simRefs.begin(); 
             simRefPair != simRefs.end(); ++simRefPair) 

          if ( simRefPair->second > 0.5 ) // require more than 50% shared hits between reco and sim
            outTrack->SetSimParticle(simMap_->GetMit(simRefPair->first)); //add reco->sim reference
      }
    }
  }

  tracks_->Trim();
}
