// $Id: FillerGsfTracks.cc,v 1.8 2008/07/14 21:01:00 loizides Exp $

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
FillerGsfTracks::FillerGsfTracks(const ParameterSet &cfg, const char *name, 
                                 bool active, const SimParticleMap *sm) : 
  BaseFiller(cfg,name,active),
  edmName_(Conf().getUntrackedParameter<string>("edmName","pixelMatchGsfFit")),
  mitName_(Conf().getUntrackedParameter<string>("mitName","GsfTracks")),
  edmSimAssociationName_(Conf().getUntrackedParameter<string>("edmSimAssociationName",
                                                              "assoc2GsfTracks")),
  simMap_(sm),
  tracks_(new mithep::TrackArr(250)), 
  trackMap_(new mithep::GsfTrackMap)
{
  // Constructor
}

//--------------------------------------------------------------------------------------------------
FillerGsfTracks::~FillerGsfTracks()
{
  // Destructor.

  delete tracks_;
  delete trackMap_;
}

//--------------------------------------------------------------------------------------------------
void FillerGsfTracks::BookDataBlock(TreeWriter &tws)
{
  // Add tracks branch to tree.

  tws.AddBranch(mitName_.c_str(),&tracks_);
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
  for (reco::GsfTrackCollection::const_iterator inTrack = inTracks.begin(); 
       inTrack != inTracks.end(); ++inTrack) {
    
    mithep::Track *outTrack = tracks_->Allocate();
    new (outTrack) mithep::Track(inTrack->phi(),inTrack->d0(),inTrack->pt(),
                                 inTrack->dz(),inTrack->theta());
	
    outTrack->SetErrors(inTrack->phiError(),
                        inTrack->d0Error(),
                        inTrack->ptError(),
                        inTrack->dzError(),
                        inTrack->thetaError());
    outTrack->SetCharge(inTrack->charge());
	
	
    reco::GsfTrackRef theRef(hTrackProduct, inTrack-inTracks.begin());
    trackMap_->Add(theRef, outTrack);
	
    if (simMap_ && !edmSimAssociationName_.empty()) {
      reco::TrackBaseRef theBaseRef(theRef);
      std::vector<std::pair<TrackingParticleRef, double> > simRefs;
      Bool_t noSimParticle=0;
      try {
        simRefs = simAssociation[theBaseRef]; //try to get the sim references if existing
      }
      catch (edm::Exception &ex) {
        noSimParticle=1;
      }
      if (!noSimParticle) { //loop through sim match candidates
        for (vector<pair<TrackingParticleRef, double> >::const_iterator simRefPair=simRefs.begin(); 
             simRefPair != simRefs.end(); ++simRefPair) 
          if ( simRefPair->second > 0.5 ) //require more than 50% shared hits between reco and sim
            outTrack->SetMCPart(simMap_->GetMit(simRefPair->first)); //add reco->sim reference
      }
    }
  }

  tracks_->Trim();
}
