// $Id: FillerMCParticles.cc,v 1.9 2009/03/15 11:20:41 loizides Exp $

#include "MitProd/TreeFiller/interface/FillerMCParticles.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Common/interface/RefToPtr.h"
#include "SimDataFormats/HepMCProduct/interface/HepMCProduct.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
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
  useAodGen_(Conf().getUntrackedParameter<bool>("useAodGen",true)),
  simActive_(Conf().getUntrackedParameter<bool>("simActive",true)),
  trackingActive_(Conf().getUntrackedParameter<bool>("trackingActive",false)),
  genEdmName_(Conf().getUntrackedParameter<string>("genEdmName","source")),
  simEdmName_(Conf().getUntrackedParameter<string>("simEdmName","g4SimHits")),
  trackingEdmName_(Conf().getUntrackedParameter<string>("trackingEdmName",
                                                        "mergedtruth:MergedTrackTruth")),
  genMapName_(Conf().getUntrackedParameter<string>("genMapName","GenMap")),
  simMapName_(Conf().getUntrackedParameter<string>("simMapName","SimMap")),
  trackingMapName_(Conf().getUntrackedParameter<string>("trackingMapName","TrackingMap")),
  mitName_(Conf().getUntrackedParameter<string>("mitName",Names::gkMCPartBrn)),
  mcParticles_(new mithep::MCParticleArr(250)),
  genMap_(genActive_?new mithep::GenParticleBarcodeMap:0),
  aodGenMap_((genActive_&&useAodGen_)?new mithep::AODGenParticleMap:0),
  simMap_(simActive_?new mithep::SimTrackTidMap:0),
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
  delete aodGenMap_;
  delete simMap_;
  delete trackingMap_;
}

//--------------------------------------------------------------------------------------------------
void FillerMCParticles::BookDataBlock(TreeWriter &tws)
{
  // Add branch to tree and publish our maps.

  tws.AddBranch(mitName_,&mcParticles_);
  OS()->add<mithep::MCParticleArr>(mcParticles_,mitName_);

  if (genActive_ && !genMapName_.empty()) {
    genMap_->SetBrName(mitName_);
    if (genMap_)
      OS()->add(genMap_,genMapName_);
  }
  if (simActive_ && !simMapName_.empty()) {
    simMap_->SetBrName(mitName_);
    if (simMap_)
      OS()->add(simMap_,simMapName_);
  }
  if (trackingActive_ && !trackingMapName_.empty()) {
    trackingMap_->SetBrName(mitName_);
    if (trackingMap_)
      OS()->add(trackingMap_,trackingMapName_);
  }
}

//--------------------------------------------------------------------------------------------------
void FillerMCParticles::FillDataBlock(const edm::Event      &event, 
                                      const edm::EventSetup &setup)
{
  // Loop over generated or simulated particles and fill their information.

  mcParticles_->Delete();

  if (genActive_)
    genMap_->Reset();
  
  if (genActive_ && !useAodGen_) {
    Handle<edm::HepMCProduct> hHepMCProduct;
    GetProduct(genEdmName_, hHepMCProduct, event);  
  
    const HepMC::GenEvent &GenEvent = hHepMCProduct->getHepMCData();  
  
    // loop over all hepmc particles and copy their information
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
  
  if (genActive_ && useAodGen_) {
 
    aodGenMap_->Reset();
  
    Handle<reco::GenParticleCollection> hGenPProduct;
    GetProduct(genEdmName_, hGenPProduct, event);  
    
    // element by element aligned collection of hepmc barcodes associated to
    // the genparticles
    Handle<std::vector<int> > genBarcodes;
    if (simActive_)
      GetProduct(genEdmName_, genBarcodes, event);  
  
    const reco::GenParticleCollection genParticles = *(hGenPProduct.product());  
  
    //loop over all genparticles and copy their information
    for (reco::GenParticleCollection::const_iterator pgen = genParticles.begin();
        pgen != genParticles.end(); ++pgen) {
        
      mithep::MCParticle *mcPart = mcParticles_->Allocate();
      new (mcPart) mithep::MCParticle(pgen->px(),pgen->py(),
                                            pgen->pz(),pgen->energy(),
                                            pgen->pdgId(),
                                            pgen->status());
                                            
      mcPart->SetIsGenerated();                                          
      
      // add hepmc barcode association, needed to merge in sim particles
      if (simActive_) {
        int genBarcode = genBarcodes->at(pgen - genParticles.begin());
        genMap_->Add(genBarcode, mcPart);
      }
      
      edm::Ptr<reco::GenParticle> thePtr(hGenPProduct, pgen - genParticles.begin());
      aodGenMap_->Add(thePtr, mcPart);
    }
  }
  
  if (simActive_) {
    simMap_->Reset();

    Handle<edm::SimTrackContainer> hSimTrackProduct;
    GetProduct(simEdmName_, hSimTrackProduct, event);

    const edm::SimTrackContainer simTracks = *(hSimTrackProduct.product());

    // loop through all simParticles
    for (SimTrackContainer::const_iterator iM = simTracks.begin(); 
        iM != simTracks.end(); ++iM) {

      mithep::MCParticle *outSimParticle = 0;
      
      if (genActive_ && (iM->genpartIndex() >= 0) ) {
        outSimParticle = genMap_->GetMit(iM->genpartIndex());
        if (verbose_>1) {
          printf("Sim px,py,pz,e = %f,%f,%f,%f\nGen px,py,pz,e = %f,%f,%f,%f\n",       
                 iM->momentum().px(),iM->momentum().py(),
                 iM->momentum().pz(),iM->momentum().e(),
                 outSimParticle->Px(),outSimParticle->Py(),
                 outSimParticle->Pz(),outSimParticle->E());
        }
      } else {
        outSimParticle = mcParticles_->Allocate();
        new (outSimParticle) mithep::MCParticle(iM->momentum().px(),
                                                iM->momentum().py(),
                                                iM->momentum().pz(),
                                                iM->momentum().e(),
                                                iM->type(), -99);
      }
      
      outSimParticle->SetIsSimulated();
      simMap_->Add(iM->trackId(), outSimParticle);
    }
  }

  // loop through TrackingParticles and build association map to MCParticles
  if (trackingActive_) {
    trackingMap_->Reset();
  
    Handle<TrackingParticleCollection> hTrackingParticleProduct;
    GetProduct(trackingEdmName_, hTrackingParticleProduct, event);
    
    const TrackingParticleCollection trackingParticles = *(hTrackingParticleProduct.product());
    
    // loop through all TrackingParticles
    for (TrackingParticleCollection::const_iterator iM = trackingParticles.begin();
        iM != trackingParticles.end(); ++iM) {

      if ( simActive_ && iM->g4Tracks().size() ) {
        if (verbose_>1) {
          printf("Tracking particle has %i SimTracks\n",iM->g4Tracks().size());
          if (iM->g4Tracks().size()>1) {
            for (std::vector<SimTrack>::const_iterator iST = iM->g4Tracks().begin();
                 iST != iM->g4Tracks().end(); ++iST) {
              printf("g4 trackid = %i\n",iST->trackId());
            }
          }
          TrackingParticleRef theRef(hTrackingParticleProduct, iM-trackingParticles.begin());
          const SimTrack &theSimTrack = iM->g4Tracks().at(iM->g4Tracks().size()-1);
          printf("trackId = %i\n",theSimTrack.trackId());
          mithep::MCParticle *outSimParticle = simMap_->GetMit(theSimTrack.trackId());
          trackingMap_->Add(theRef, outSimParticle);
        }
      }
    }
  }

  mcParticles_->Trim();
}

//--------------------------------------------------------------------------------------------------
void FillerMCParticles::ResolveLinks(const edm::Event      &event, 
                                     const edm::EventSetup &setup)
{
  // Loop over generated and simulated particles and resolve their links.

  if (genActive_ && !useAodGen_) {
  
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
  
  //loop over aod GenParticle candidates and resolve their links
  
  if (genActive_ && useAodGen_) {
  
    Handle<reco::GenParticleCollection> hGenPProduct;
    GetProduct(genEdmName_, hGenPProduct, event);  
  
    const reco::GenParticleCollection genParticles = *(hGenPProduct.product());  
  
    //loop over all genparticles and copy their information
    for (reco::GenParticleCollection::const_iterator pgen = genParticles.begin();
        pgen != genParticles.end(); ++pgen) {
  
      int nDaughters = pgen->numberOfDaughters();
      
      if (nDaughters>0) {
      
        edm::Ptr<reco::GenParticle> thePtr(hGenPProduct, pgen - genParticles.begin());
        MCParticle *mcMother = aodGenMap_->GetMit(thePtr);
        for (int i=0; i<nDaughters; ++i) {
          const reco::Candidate *genDaughter = pgen->daughter(i);
          MCParticle *mcDaughter = aodGenMap_->GetMit(refToPtr(pgen->daughterRef(i)));
          //set mother decay vertex
          if (i==0)
            mcMother->SetVertex(genDaughter->vx(),genDaughter->vy(),genDaughter->vz());
          
          //set mother-daughter links
          mcMother->AddDaughter(mcDaughter);
          if (!mcDaughter->HasMother())
            mcDaughter->SetMother(mcMother);
          
        }
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
        mithep::MCParticle *simDaughter = simMap_->GetMit(iM->trackId());
        const SimVertex &theVertex = simVertexes.at(iM->vertIndex());
        if (theVertex.parentIndex()>=0) {
          mithep::MCParticle *simParent = simMap_->GetMit(theVertex.parentIndex());
          simParent->SetVertex(theVertex.position().x(),
                               theVertex.position().y(),
                               theVertex.position().z());
          //make sure we don't double count the decay tree
          if ( !simParent->HasDaughter(simDaughter) ) {
            simParent->AddDaughter(simDaughter);
          }
          if ( !simDaughter->HasMother() ) {
            simDaughter->SetMother(simParent);
          }
        }
      }
    }
  }

  if (verbose_>1 && trackingActive_) {
    Handle<TrackingParticleCollection> hTrackingParticleProduct;
    GetProduct(trackingEdmName_, hTrackingParticleProduct, event);
    
    const TrackingParticleCollection trackingParticles = *(hTrackingParticleProduct.product());  
   
    // loop through all simParticles
    for (TrackingParticleCollection::const_iterator iM = trackingParticles.begin(); 
         iM != trackingParticles.end(); ++iM) {
      
      TrackingParticleRef theRef(hTrackingParticleProduct, iM-trackingParticles.begin());
      
      mithep::MCParticle *simParent = trackingMap_->GetMit(theRef);
      printf("MCParticle pdg = %i, Daughter Pdgs: ", simParent->PdgId());
      for (UInt_t i=0; i<simParent->NDaughters(); ++i) {
        printf("%i, ", simParent->Daughter(i)->PdgId());
      }
      printf("\n");
      
      printf("TrackingParticle pdg = %i, Daughter Pdgs: ", iM->pdgId());
      for (TrackingVertexRefVector::iterator v= iM->decayVertices().begin(); 
           v != iM->decayVertices().end(); ++v) {
        for (TrackingParticleRefVector::iterator pd = v->get()->daughterTracks().begin(); 
             pd != v->get()->daughterTracks().end(); ++pd) {
          printf("%i, ", pd->get()->pdgId());
        }
      }
      printf("\n");
    }
  }
}
