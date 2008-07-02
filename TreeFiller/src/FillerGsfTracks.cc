// $Id: FillerGsfTracks.cc,v 1.2 2008/07/01 21:11:47 loizides Exp $

#include "MitProd/TreeFiller/interface/FillerGsfTracks.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrackFwd.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticleFwd.h"
#include "DataFormats/RecoCandidate/interface/TrackAssociation.h"

#include "MitAna/DataTree/interface/Track.h"
#include "MitAna/DataTree/interface/Names.h"

using namespace std;
using namespace edm;
using namespace mithep;

//-------------------------------------------------------------------------------------------------
FillerGsfTracks::FillerGsfTracks(const ParameterSet &cfg, const char *name, 
                                 bool active, const SimParticleMap *sm) : 
  BaseFiller(cfg, name, active),
  edmName_(Conf().getUntrackedParameter<string>("edmName","pixelMatchGsfFit")),
  mitName_(Conf().getUntrackedParameter<string>("mitName","GsfTracks")),
  edmSimAssociationName_(Conf().getUntrackedParameter<string>("edmSimAssociationName","assoc2GsfTracks")),
  simMap_(sm),
  tracks_(new mithep::Array<mithep::Track>), trackMap_(new mithep::GsfTrackMap)
{
  // Constructor
}

//-------------------------------------------------------------------------------------------------
FillerGsfTracks::~FillerGsfTracks()
{
  // Destructor.

  delete trackMap_;
}

//-------------------------------------------------------------------------------------------------
void FillerGsfTracks::BookDataBlock(TreeWriter &tws)
{
  // Add tracks branch to tree.

  tws.AddBranch(mitName_.c_str(),&tracks_);
}

//-------------------------------------------------------------------------------------------------
void FillerGsfTracks::FillDataBlock(const edm::Event      &event, 
                                    const edm::EventSetup &setup)
{
  // Fill tracks from edm collection into our collection.

  tracks_->Reset();
  trackMap_->Reset();
  
  try {
    event.getByLabel(edm::InputTag(edmName_),trackProduct_);
  } catch (cms::Exception& ex) {
    edm::LogError("FillerGsfTracks") << "Error! Cannot get collection with label " 
                                     << edmName_ << endl;
    throw edm::Exception(edm::errors::Configuration, "FillerGsfTracks:FillDataBlock()\n")
      << "Error! Cannot get collection with label " << edmName_ << endl;
  }
  
  // if we have a Sim Particle association (for monte carlo), initialize the reco->sim mappings
  reco::RecoToSimCollection simAssociation;
  if (simMap_ && edmSimAssociationName_!="") {
    Handle<reco::RecoToSimCollection> simAssociationProduct;
    try {
      event.getByLabel(edmSimAssociationName_, simAssociationProduct);
    }
    catch (cms::Exception& ex) {
      edm::LogError("FillerGsfTracks") << "Error! Cannot get collection with label " 
                                       << edmSimAssociationName_ << endl;
      throw edm::Exception(edm::errors::Configuration, "FillerGsfTracks:FillDataBlock()\n")
        << "Error! Cannot get collection with label " << edmSimAssociationName_ << endl;
    }
    simAssociation = *(simAssociationProduct.product());
  }
  
  const reco::GsfTrackCollection inTracks = *(trackProduct_.product());  
  
  // loop through all tracks
  for (reco::GsfTrackCollection::const_iterator inTrack = inTracks.begin(); 
       inTrack != inTracks.end(); ++inTrack) {
    
    mithep::Track* outTrack = tracks_->Allocate();
    new (outTrack) mithep::Track(inTrack->phi(),inTrack->d0(),inTrack->pt(),
                                 inTrack->dz(),inTrack->theta());
	
    outTrack->SetErrors(inTrack->phiError(),
                        inTrack->d0Error(),
                        inTrack->ptError(),
                        inTrack->dzError(),
                        inTrack->thetaError());
    outTrack->SetCharge(inTrack->charge());
	
	
    reco::GsfTrackRef theRef(trackProduct_, inTrack-inTracks.begin());
    trackMap_->Add(theRef, outTrack);
	
    if (simMap_ && edmSimAssociationName_!="") {
      reco::TrackBaseRef theBaseRef(theRef);
      std::vector<std::pair<TrackingParticleRef, double> > simRefs;
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
          if ( simRefPair->second > 0.5 ) //require more than 50% shared hits between reco and sim
            outTrack->SetSimParticle(simMap_->GetMit(simRefPair->first)); //add reco->sim reference
      }
    }
  }

  tracks_->Trim();
}


