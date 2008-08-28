// $Id: FillerGsfTracks.cc,v 1.11 2008/07/31 13:39:58 bendavid Exp $

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
  simMapName_(Conf().getUntrackedParameter<string>("simMapName","SimMap")),
  trackMapName_(Conf().getUntrackedParameter<string>("trackMapName",
                                                     Form("%sMapName",mitName_.c_str()))),
  simMap_(0),
  tracks_(new mithep::TrackArr(250)), 
  trackMap_(new mithep::GsfTrackMap)
{
  // Constructor.
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
  // Add tracks branch to tree, publish and get our objects.

  tws.AddBranch(mitName_.c_str(),&tracks_);
  InitLayerMap();

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
    outTrack->SetNdof(it->ndof());
    
    //Fill hit layer map
    // Format is known to have changed slightly in 21x
    // Also due to reco::HitPattern limitation in 20x currently we do not distinguish stereo hits
    // from regular hits, and therefore the stereo information is merged into
    // the normal layer hits (this also means that stereo hits are not treated as
    // seperate hits when counting the number of hits).  This will be fixed in 21x
    const reco::HitPattern &hits = it->hitPattern();
    for (Int_t i=0; i<hits.numberOfHits(); i++) {
      uint32_t hit = hits.getHitPattern(i);
      if ( hits.validHitFilter(hit) )
        if ( hits.trackerHitFilter(hit) )
          outTrack->SetHit(layerMap_[hit]);
                
//       if ( hits.muonDTHitFilter(hit) )
//         printf("Muon DT Layer = %i\n", hits.getLayer(hit));
//       if ( hits.muonCSCHitFilter(hit) )
//         printf("Muon CSC Layer = %i\n", hits.getLayer(hit));        
//       if ( hits.muonRPCHitFilter(hit) )
//         printf("Muon RPC Layer = %i\n", hits.getLayer(hit));
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
//     printf("inTrack : px=%5f,py=%5f,pz=%5f\n",it->px(),it->py(),it->pz());
//     printf("outTrack: px=%5f,py=%5f,pz=%5f\n",outTrack->Px(),outTrack->Py(),outTrack->Pz());
//     printf("inTrack : p=%5f,pt=%5f,theta=%5f,phi=%5f,dz=%5f\n",it->p(),it->pt(),
//             it->theta(),it->phi(),it->dz());
//     printf("outTrack: p=%5f,pt=%5f,theta=%5f,phi=%5f,dz=%5f\n",outTrack->P(),outTrack->Pt()
//             ,outTrack->Theta(),outTrack->Phi(),outTrack->Z0());
  }

  tracks_->Trim();
}

//--------------------------------------------------------------------------------------------------
void FillerGsfTracks::InitLayerMap()
{
  // Initialize mapping between hit layer format in reco::HitPattern and the one used in
  // mithep::Track. Note in 21x stereo layers are treated separatelely.
  
  layerMap_[1160] = mithep::Track::PXB1;
  layerMap_[1168] = mithep::Track::PXB2;
  layerMap_[1176] = mithep::Track::PXB3;
  layerMap_[1288] = mithep::Track::PXF1;
  layerMap_[1296] = mithep::Track::PXF2;
  layerMap_[1416] = mithep::Track::TIB1;
  layerMap_[1420] = mithep::Track::TIB1S;
  layerMap_[1424] = mithep::Track::TIB2;
  layerMap_[1428] = mithep::Track::TIB2S;
  layerMap_[1432] = mithep::Track::TIB3;
  layerMap_[1440] = mithep::Track::TIB4;
  layerMap_[1544] = mithep::Track::TID1;
  layerMap_[1548] = mithep::Track::TID1S;
  layerMap_[1552] = mithep::Track::TID2;
  layerMap_[1556] = mithep::Track::TID2S;
  layerMap_[1560] = mithep::Track::TID3;
  layerMap_[1672] = mithep::Track::TOB1;
  layerMap_[1676] = mithep::Track::TOB1S;
  layerMap_[1680] = mithep::Track::TOB2;
  layerMap_[1684] = mithep::Track::TOB2S;
  layerMap_[1688] = mithep::Track::TOB3;
  layerMap_[1696] = mithep::Track::TOB4;
  layerMap_[1704] = mithep::Track::TOB5;
  layerMap_[1712] = mithep::Track::TOB6;
  layerMap_[1800] = mithep::Track::TEC1;
  layerMap_[1804] = mithep::Track::TEC1S;
  layerMap_[1808] = mithep::Track::TEC2;
  layerMap_[1812] = mithep::Track::TEC2S;
  layerMap_[1816] = mithep::Track::TEC3;
  layerMap_[1824] = mithep::Track::TEC4;
  layerMap_[1832] = mithep::Track::TEC5;
  layerMap_[1836] = mithep::Track::TEC5S;
  layerMap_[1840] = mithep::Track::TEC6;
  layerMap_[1848] = mithep::Track::TEC7;
  layerMap_[1856] = mithep::Track::TEC8;
  layerMap_[1864] = mithep::Track::TEC9;
}
