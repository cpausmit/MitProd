// $Id: FillerMCParticles.cc,v 1.4 2008/09/05 23:46:14 bendavid Exp $

#include "MitProd/TreeFiller/interface/FillerMCParticles.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/Common/interface/Handle.h"
#include "SimDataFormats/HepMCProduct/interface/HepMCProduct.h"
#include "SimDataFormats/Track/interface/SimTrack.h"
#include "SimDataFormats/Track/interface/SimTrackContainer.h"
#include "SimDataFormats/Vertex/interface/SimVertex.h"
#include "SimDataFormats/Vertex/interface/SimVertexContainer.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticleFwd.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingVertex.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingVertexContainer.h"
#include "MitAna/DataTree/interface/Names.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerMCParticles::FillerMCParticles(const ParameterSet &cfg, const char *name, bool active) : 
  BaseFiller(cfg, name, active),
  genActive_(Conf().getUntrackedParameter<bool>("genActive",true)),
  simActive_(Conf().getUntrackedParameter<bool>("simActive",true)),
  trackingActive_(Conf().getUntrackedParameter<bool>("trackingActive",false)),
  genEdmName_(Conf().getUntrackedParameter<string>("genEdmName","source")),
  simEdmName_(Conf().getUntrackedParameter<string>("simEdmName","g4SimHits")),
  trackingEdmName_(Conf().getUntrackedParameter<string>("trackingEdmName","mergedtruth:MergedTrackTruth")),
  genMapName_(Conf().getUntrackedParameter<string>("genMapName","GenMap")),
  simMapName_(Conf().getUntrackedParameter<string>("simMapName","SimMap")),
  trackingMapName_(Conf().getUntrackedParameter<string>("trackingMapName","TrackingMap")),
  mitName_(Conf().getUntrackedParameter<string>("mitName",Names::gkMCPartBrn)),
  mcParticles_(new mithep::MCParticleArr(250)),
  genMap_(genActive_?new mithep::GenParticleMap:0),
  simMap_(simActive_?new mithep::SimTrackMap:0),
  trackingMap_(trackingActive_?new mithep::TrackingParticleMap:0)
{
  // Constructor.
}

//--------------------------------------------------------------------------------------------------
FillerMCParticles::~FillerMCParticles()
{
  // Destructor.

  delete mcParticles_;
  delete genMap_;
  delete simMap_;
  delete trackingMap_;
}

//--------------------------------------------------------------------------------------------------
void FillerMCParticles::BookDataBlock(TreeWriter &tws)
{
  // Add branch to tree and publish our maps.

  tws.AddBranch(mitName_.c_str(),&mcParticles_);

  if (genActive_)
    OS()->add(genMap_,genMapName_.c_str());
  if (simActive_)
    OS()->add(simMap_,simMapName_.c_str());
  if (trackingActive_)
    OS()->add(trackingMap_,trackingMapName_.c_str());
}

//--------------------------------------------------------------------------------------------------
void FillerMCParticles::FillDataBlock(const edm::Event      &event, 
                                      const edm::EventSetup &setup)
{
  // Loop over HepMC particle and fill their information.

  mcParticles_->Reset();

  if (genActive_) {
    genMap_->Reset();
  
    Handle<edm::HepMCProduct> hHepMCProduct;
    GetProduct(genEdmName_, hHepMCProduct, event);  
  
    const HepMC::GenEvent &GenEvent = hHepMCProduct->getHepMCData();  
  
    //loop over all hepmc particles and copy their information
    for (HepMC::GenEvent::particle_const_iterator pgen = GenEvent.particles_begin();
        pgen != GenEvent.particles_end(); ++pgen) {
  
      HepMC::GenParticle *mcPart = (*pgen);
      if(!mcPart) continue;
  
      mithep::MCParticle *genParticle = mcParticles_->Allocate();
      new (genParticle) mithep::MCParticle(mcPart->momentum().x(),mcPart->momentum().y(),
                                            mcPart->momentum().z(),mcPart->momentum().e(),
                                            mcPart->pdg_id(),
                                            mcPart->status());
                                            
      genParticle->SetIsGenerated();                                          
      
      genMap_->Add(mcPart->barcode(), genParticle);
    }
    
  }
  
  if (simActive_) {
    simTidMap_.clear();
    simMap_->Reset();

    Handle<edm::SimTrackContainer> hSimTrackProduct;
    GetProduct(simEdmName_, hSimTrackProduct, event);

    const edm::SimTrackContainer simTracks = *(hSimTrackProduct.product());

    // loop through all simParticles
    for (SimTrackContainer::const_iterator iM = simTracks.begin(); 
        iM != simTracks.end(); ++iM) {

      SimTrackRef theRef(hSimTrackProduct, iM-simTracks.begin());
      simTidMap_[iM->trackId()] = theRef; 
      mithep::MCParticle *outSimParticle;
      
      if (genActive_ && (iM->genpartIndex() >= 0) ) {
        outSimParticle = genMap_->GetMit(iM->genpartIndex());
        //printf("Sim px,py,pz,e = %f,%f,%f,%f\nGen px,py,pz,e = %f,%f,%f,%f\n",       iM->momentum().px(),iM->momentum().py(),iM->momentum().pz(),iM->momentum().e(),outSimParticle->Px(),outSimParticle->Py(),outSimParticle->Pz(),outSimParticle->E());
      }
      else {
        outSimParticle = mcParticles_->Allocate();
        new (outSimParticle) mithep::MCParticle(iM->momentum().px(),
                                                iM->momentum().py(),
                                                iM->momentum().pz(),
                                                iM->momentum().e(),
                                                iM->type(), 
                                                -99);
      }
      
      outSimParticle->SetIsSimulated();
      simMap_->Add(theRef, outSimParticle);

    }
  }

  //loop through TrackingParticles and build association map to OAK MCParticles
  if (trackingActive_) {
    trackingMap_->Reset();
  
    Handle<TrackingParticleCollection> hTrackingParticleProduct;
    GetProduct(trackingEdmName_, hTrackingParticleProduct, event);
    
    const TrackingParticleCollection trackingParticles = *(hTrackingParticleProduct.product());
    
    // loop through all TrackingParticles
    for (TrackingParticleCollection::const_iterator iM = trackingParticles.begin();
        iM != trackingParticles.end(); ++iM) {

      if ( simActive_ && iM->g4Tracks().size() ) {
        //printf("Tracking particle has %i SimTracks\n",iM->g4Tracks().size());
//         if (iM->g4Tracks().size()>1) {
//           for (std::vector<SimTrack>::const_iterator iST = iM->g4Tracks().begin();
//             iST != iM->g4Tracks().end(); ++iST) {
//               printf("g4 trackid = %i\n",iST->trackId());
//             }
//         }
        TrackingParticleRef theRef(hTrackingParticleProduct, iM-trackingParticles.begin());
        const SimTrack &theSimTrack = iM->g4Tracks().at(iM->g4Tracks().size()-1);
        //printf("trackId = %i\n",theSimTrack.trackId());
        SimTrackRef theSimTrackRef = simTidMap_[theSimTrack.trackId()];
        mithep::MCParticle *outSimParticle = simMap_->GetMit(theSimTrackRef);
        trackingMap_->Add(theRef, outSimParticle);
      }

    }
  }

  mcParticles_->Trim();
}

//--------------------------------------------------------------------------------------------------
void FillerMCParticles::ResolveLinks(const edm::Event      &event, 
                                     const edm::EventSetup &setup)
{
  // Loop over HepMC particle and resolve their links.

  if (genActive_) {
  
    Handle<edm::HepMCProduct> hHepMCProduct;
    GetProduct(genEdmName_, hHepMCProduct, event);
    
    const HepMC::GenEvent &GenEvent = hHepMCProduct->getHepMCData();  
  
    for (HepMC::GenEvent::particle_const_iterator pgen = GenEvent.particles_begin();
        pgen != GenEvent.particles_end(); ++pgen) {
  
      HepMC::GenParticle *mcPart = (*pgen);
      if(!mcPart) continue;
      
      //check if genpart has a decay vertex
      HepMC::GenVertex *dVertex = mcPart->end_vertex();
      if (!dVertex) continue;
  
      //find corresponding mithep genparticle parent in association table
      mithep::MCParticle *genParent = genMap_->GetMit(mcPart->barcode());

      if (genParent->IsSimulated()) continue;
  
      //set decay vertex
      //division by 10.0 is needed due to HepMC use of mm instead of cm for distance units
      genParent->SetVertex(dVertex->point3d().x()/10.0,
                          dVertex->point3d().y()/10.0,
                          dVertex->point3d().z()/10.0);
  
      //loop through daugthers
      for (HepMC::GenVertex::particles_out_const_iterator pgenD = 
             dVertex->particles_out_const_begin(); 
           pgenD != dVertex->particles_out_const_end(); ++pgenD) {
        HepMC::GenParticle *mcDaughter = (*pgenD);
        mithep::MCParticle *genDaughter = genMap_->GetMit(mcDaughter->barcode());
        genParent->AddDaughter(genDaughter);
        if (!(genDaughter->HasMother()))
          genDaughter->SetMother(genParent);
      }
    }
  }
  
  //loop over SimTracks and resolve links
  if (simActive_) {
    Handle<edm::SimTrackContainer> hSimTrackProduct;
    GetProduct(simEdmName_, hSimTrackProduct, event);

    const edm::SimTrackContainer simTracks = *(hSimTrackProduct.product());
   
    Handle<std::vector<SimVertex> > hSimVertexProduct;
    GetProduct(simEdmName_, hSimVertexProduct, event);

    const edm::SimVertexContainer simVertexes = *(hSimVertexProduct.product());




    // loop through all simParticles
    for (SimTrackContainer::const_iterator iM = simTracks.begin(); 
        iM != simTracks.end(); ++iM) {

      if (iM->vertIndex()>=0) {
        SimTrackRef theRef(hSimTrackProduct, iM-simTracks.begin());
        mithep::MCParticle *simDaughter = simMap_->GetMit(theRef);
        const SimVertex &theVertex = simVertexes.at(iM->vertIndex());
        if (theVertex.parentIndex()>=0) {
          mithep::MCParticle *simParent = simMap_->GetMit(simTidMap_[theVertex.parentIndex()]);
          simParent->SetVertex(theVertex.position().x(),theVertex.position().y(),theVertex.position().z());
          simParent->AddDaughter(simDaughter);
          simDaughter->SetMother(simParent);
        }

      }

    }

  }

//Debug output, will be uncommented and switchable later
//   if (trackingActive_) {
// 
//     Handle<TrackingParticleCollection> hTrackingParticleProduct;
//     GetProduct(trackingEdmName_, hTrackingParticleProduct, event);
//     
//     const TrackingParticleCollection trackingParticles = *(hTrackingParticleProduct.product());  
//   
//     // loop through all simParticles
//     for (TrackingParticleCollection::const_iterator iM = trackingParticles.begin(); 
//         iM != trackingParticles.end(); ++iM) {
// 
//       TrackingParticleRef theRef(hTrackingParticleProduct, iM-trackingParticles.begin());
// 
//       mithep::MCParticle *simParent = trackingMap_->GetMit(theRef);
//       printf("MCParticle pdg = %i, Daughter Pdgs: ", simParent->PdgId());
//       for (UInt_t i=0; i<simParent->NDaughters(); ++i) {
//         printf("%i, ", simParent->Daughter(i)->PdgId());
//       }
//       printf("\n");
// 
//       printf("TrackingParticle pdg = %i, Daughter Pdgs: ", iM->pdgId());
//       for (TrackingVertexRefVector::iterator v= iM->decayVertices().begin(); 
//           v != iM->decayVertices().end(); ++v) {
//         for (TrackingParticleRefVector::iterator pd = v->get()->daughterTracks().begin(); 
//             pd != v->get()->daughterTracks().end(); ++pd) {
//           ;//printf("%i, ", pd->get()->pdgId());
//         }
//       }
//       printf("\n");
//     }
//   }
}
