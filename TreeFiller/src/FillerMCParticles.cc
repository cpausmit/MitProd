#include "MitProd/TreeFiller/interface/FillerMCParticles.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Common/interface/RefToPtr.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/METReco/interface/METCollection.h"
#include "SimDataFormats/Track/interface/SimTrack.h"
#include "SimDataFormats/Vertex/interface/SimVertex.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingVertex.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingVertexContainer.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataTree/interface/MCParticleCol.h"
#include "MitAna/DataTree/interface/TrackingParticleCol.h"
#include "MitProd/ObjectService/interface/ObjectService.h"
#include "SimDataFormats/TrackingHit/interface/PSimHit.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerMCParticles::FillerMCParticles(const ParameterSet &cfg, edm::ConsumesCollector& collector, ObjectService* os, const char *name, bool active) : 
  BaseFiller(cfg, os, name, active),
  genActive_(cfg.getUntrackedParameter("genActive", true)),
  simActive_(cfg.getUntrackedParameter("simActive", true)),
  trackingActive_(cfg.getUntrackedParameter("trackingActive", false)),
  fillTracking_(cfg.getUntrackedParameter("fillTracking", false)),
  genSource_(nGenSources),
  hepMCProdToken_(),
  genParticlesToken_(),
  genBarcodesToken_(),
  simTracksToken_(),
  simVerticesToken_(),
  trackingEdmToken_(),
  packedGenParticlesToken_(),
  genMapName_(cfg.getUntrackedParameter<string>("genMapName", "")), //GenMap
  simMapName_(cfg.getUntrackedParameter<string>("simMapName", "")), //SimMap
  trackingMapName_(cfg.getUntrackedParameter<string>("trackingMapName", "")), //TrackingMap
  mitName_(cfg.getUntrackedParameter<string>("mitName", Names::gkMCPartBrn)),
  mitTrackingName_(cfg.getUntrackedParameter<string>("mitTrackingName", Names::gkTrackingParticleBrn)),
  mcParticles_(new mithep::MCParticleArr(250)),
  trackingParticles_(new mithep::TrackingParticleArr(250)),
  genMap_(0),
  aodGenMap_(0),
  packedGenMap_(0),
  simMap_(0),
  trackingMap_(0)
{
  std::string genSourceName(cfg.getUntrackedParameter("genSource", std::string("GenParticleCollection")));
  if (genSourceName == "GenParticleCollection")
    genSource_ = kGenParticles;
  else if (genSourceName == "PackedGenParticleCollection")
    genSource_ = kPackedGenParticles;
  else if (genSourceName == "HepMCProduct")
    genSource_ = kHepMCProduct;
  else
    throw edm::Exception(edm::errors::Configuration, "FillerMCParticles::Constructor")
      << "Parameter genSource must be either one of GenParticleCollection, PackedGenParticleCollection,"
      << " or HepMCProduct.";

  if (genActive_) {
    genMap_ = new mithep::GenParticleBarcodeMap;

    switch (genSource_) {
    case kGenParticles:
      aodGenMap_ = new GenParticleMap;
      genParticlesToken_ = GetToken<reco::GenParticleCollection>(collector, cfg, "genEdmName"); //genParticles
      if (simActive_)
        genBarcodesToken_ = GetToken<std::vector<int> >(collector, cfg, "genEdmName"); //genParticles
      break;

    case kPackedGenParticles:
      packedGenMap_ = new PackedGenParticleMap;
      packedGenParticlesToken_ = GetToken<pat::PackedGenParticleCollection>(collector, cfg, "genEdmName"); //packedGenParticles
      break;

    case kHepMCProduct:
      hepMCProdToken_ = GetToken<edm::HepMCProduct>(collector, cfg, "genEdmName"); //genParticles
      break;

    default:
      break;
    }
  }

  if (simActive_) {
    simMap_ = new mithep::SimTrackTidMap;
    simTracksToken_ = GetToken<edm::SimTrackContainer>(collector, cfg, "simEdmName"); //g4SimHits
    simVerticesToken_ = GetToken<std::vector<SimVertex> >(collector, cfg, "simEdmName"); //g4SimHits
  }

  if (trackingActive_) {
    trackingMap_ = new mithep::TrackingParticleMap;
    trackingEdmToken_ = GetToken<TrackingParticleCollection>(collector, cfg, "trackingEdmName"); //mergedtruth:MergedTrackTruth
  }
}

//--------------------------------------------------------------------------------------------------
FillerMCParticles::~FillerMCParticles()
{
  // Destructor.

  delete mcParticles_;
  delete trackingParticles_;
  delete genMap_;
  delete aodGenMap_;
  delete packedGenMap_;
  delete simMap_;
  delete trackingMap_;
}

//--------------------------------------------------------------------------------------------------
void FillerMCParticles::BookDataBlock(TreeWriter &tws)
{
  // Add branch to tree and publish our maps.

  Int_t brsize = tws.GetDefaultBrSize();
  if (brsize<128*1024)
    brsize=128*1024;

  tws.AddBranch(mitName_,&mcParticles_,brsize);
  OS()->add<mithep::MCParticleArr>(mcParticles_,mitName_);

  if (trackingActive_ && fillTracking_) {
    tws.AddBranch(mitTrackingName_,&trackingParticles_,brsize);
    OS()->add<mithep::TrackingParticleArr>(trackingParticles_,mitTrackingName_);
  }
  
  if (genActive_ && !genMapName_.empty()) {
    genMap_->SetBrName(mitName_);
    OS()->add(genMap_,genMapName_);
  }
  if (simActive_ && !simMapName_.empty()) {
    simMap_->SetBrName(mitName_);
    OS()->add(simMap_,simMapName_);
  }
  if (trackingActive_ && !trackingMapName_.empty()) {
    trackingMap_->SetBrName(mitName_);
    OS()->add(trackingMap_,trackingMapName_);
  }
}

//--------------------------------------------------------------------------------------------------
void FillerMCParticles::FillDataBlock(const edm::Event      &event, 
                                      const edm::EventSetup &setup)
{
  // Loop over generated or simulated particles and fill their information.

  mcParticles_->Delete();
  
  if (trackingActive_ && fillTracking_) {
    trackingParticles_->Delete();
  }

  if (genActive_) {
    genMap_->Reset();
  
    if (genSource_ == kGenParticles) {
      aodGenMap_->Reset();
  
      Handle<reco::GenParticleCollection> hGenPProduct;
      GetProduct(genParticlesToken_, hGenPProduct, event);  
      reco::GenParticleCollection const& genParticles = *hGenPProduct;
    
      // element by element aligned collection of hepmc barcodes associated to
      // the genparticles
      std::vector<int> const* genBarcodes = 0;
      if (simActive_) {
        Handle<std::vector<int> > hGenBarcodes;
        GetProduct(genBarcodesToken_, hGenBarcodes, event);
        genBarcodes = hGenBarcodes.product();
      }
 
      // loop over all genparticles and copy their information
      unsigned iPart = 0;
      for (auto&& inPart : genParticles) {
        mithep::MCParticle *mcPart = mcParticles_->AddNew();

        mcPart->SetPtEtaPhiM(inPart.pt(), inPart.eta(), inPart.phi(), inPart.mass());
        mcPart->SetPdgId(inPart.pdgId());
        mcPart->SetStatus(inPart.status());
        mcPart->SetIsGenerated();

        // need to keep an eye for updates
        for (unsigned iF = 0; iF != reco::GenStatusFlags::kIsLastCopyBeforeFSR + 1; ++iF)
          mcPart->SetStatusFlag(iF, inPart.statusFlags().flags_[iF]);
      
        // add hepmc barcode association, needed to merge in sim particles
        if (simActive_) {
          int genBarcode = genBarcodes->at(iPart);
          genMap_->Add(genBarcode, mcPart);
        }

        reco::GenParticleRef ref(hGenPProduct, iPart);
        aodGenMap_->Add(ref, mcPart);
        
        ++iPart;
      }
    }
    else if (genSource_ == kPackedGenParticles) {
      packedGenMap_->Reset();
  
      Handle<pat::PackedGenParticleCollection> hGenPProduct;
      GetProduct(packedGenParticlesToken_, hGenPProduct, event);  
      pat::PackedGenParticleCollection const& genParticles = *hGenPProduct;
  
      // loop over all genparticles and copy their information
      unsigned iPart = 0;
      for (auto&& inPart : genParticles) {
        mithep::MCParticle *mcPart = mcParticles_->AddNew();

        mcPart->SetPtEtaPhiM(inPart.pt(), inPart.eta(), inPart.phi(), inPart.mass());
        mcPart->SetPdgId(inPart.pdgId());
        mcPart->SetStatus(inPart.status());
        mcPart->SetIsGenerated();

        // need to keep an eye for updates
        for (unsigned iF = 0; iF != reco::GenStatusFlags::kIsLastCopyBeforeFSR + 1; ++iF)
          mcPart->SetStatusFlag(iF, inPart.statusFlags().flags_[iF]);
      
        pat::PackedGenParticleRef ref(hGenPProduct, iPart);
        packedGenMap_->Add(ref, mcPart);

        ++iPart;
      }
    }
    else if (genSource_ == kHepMCProduct) {
      Handle<edm::HepMCProduct> hHepMCProduct;
      GetProduct(hepMCProdToken_, hHepMCProduct, event);  
      const HepMC::GenEvent &GenEvent = hHepMCProduct->getHepMCData();  

      // loop over all hepmc particles and copy their information
      for (HepMC::GenEvent::particle_const_iterator pgen = GenEvent.particles_begin();
           pgen != GenEvent.particles_end(); ++pgen) {
  
        HepMC::GenParticle *genParticle = (*pgen);
        if(!genParticle) 
          continue;
  
        mithep::MCParticle *mcPart = mcParticles_->AddNew();

        mcPart->SetPtEtaPhiM(genParticle->momentum().perp(), genParticle->momentum().eta(), genParticle->momentum().phi(), genParticle->momentum().m());
        mcPart->SetPdgId(genParticle->pdg_id());
        mcPart->SetStatus(genParticle->status());
        mcPart->SetIsGenerated();

        genMap_->Add(genParticle->barcode(), mcPart);
      }
    }
  }

  if (simActive_) {
    simMap_->Reset();

    Handle<edm::SimTrackContainer> hSimTrackProduct;
    GetProduct(simTracksToken_, hSimTrackProduct, event);

    edm::SimTrackContainer const& simTracks = *hSimTrackProduct;

    // loop through all simParticles
    for (auto&& simTrack : simTracks) {
      mithep::MCParticle *outSimParticle = 0;
      
      if (genActive_ && (simTrack.genpartIndex() >= 0) ) {
        outSimParticle = genMap_->GetMit(simTrack.genpartIndex());
        if (verbose_>1) {
          printf("Sim px,py,pz,e = %f,%f,%f,%f\nGen px,py,pz,e = %f,%f,%f,%f\n",       
                 simTrack.momentum().px(),simTrack.momentum().py(),
                 simTrack.momentum().pz(),simTrack.momentum().e(),
                 outSimParticle->Px(),outSimParticle->Py(),
                 outSimParticle->Pz(),outSimParticle->E());
        }
      } else {
        outSimParticle = mcParticles_->AddNew();

        outSimParticle->SetPtEtaPhiM(simTrack.momentum().pt(), simTrack.momentum().eta(), simTrack.momentum().phi(), simTrack.momentum().mass());
        outSimParticle->SetPdgId(simTrack.type());
        outSimParticle->SetStatus(-99);
      }
      
      outSimParticle->SetIsSimulated();
      simMap_->Add(simTrack.trackId(), outSimParticle);
    }
  }

  // loop through TrackingParticles and build association map to MCParticles

  if (trackingActive_) {
    trackingMap_->Reset();
  
    Handle<TrackingParticleCollection> hTrackingParticleProduct;
    GetProduct(trackingEdmToken_, hTrackingParticleProduct, event);

    TrackingParticleCollection const& trackingParticles = *hTrackingParticleProduct;
    
    // loop through all TrackingParticles
    if (simActive_) {
      unsigned iPart = -1;
      for (auto&& inPart : trackingParticles) {
        ++iPart;

        if (inPart.g4Tracks().size() == 0)
          continue;

        TrackingParticleRef theRef(hTrackingParticleProduct, iPart);

        const SimTrack &theSimTrack = inPart.g4Tracks().back();
        mithep::MCParticle *outSimParticle = simMap_->GetMit(theSimTrack.trackId());
        trackingMap_->Add(theRef, outSimParticle);
        
        if (fillTracking_) {
          mithep::TrackingParticle *outTracking = trackingParticles_->AddNew();
          
          // fill associated sim tracks
          // ?? shouldn't we be adding inPart.trackId()?? (Y.I. 18/03/2015)
          for (std::vector<SimTrack>::const_iterator iST = inPart.g4Tracks().begin();
               iST != inPart.g4Tracks().end(); ++iST) {

            outTracking->AddMCPart(simMap_->GetMit(theSimTrack.trackId()));
          }

          // Below commented out on 18/03/2015
          // TrackingParticle no longer (>= 6_2_X) holds information on the underlying sim hits
          // due to speed and memory consumption concerns.
          
          // fill hit information
          //YI reco::HitPattern hitPattern;
          
          //CP THIS NEEDS TO WORK --> pSimHits disappeared from TrackingParticle?!
          //CP --> turns out this is obsolete!! Will delete soon ;-)

          //CP // loop through sim hits and construct intermediate reco::HitPattern
          //CP int nHits = 0;
          //CP for (std::vector<PSimHit>::const_iterator iSimHit = inPart.pSimHit_begin();
          //CP       iSimHit != inPart.pSimHit_end(); ++iSimHit) {
          //CP   unsigned int detectorIdIndex = iSimHit->detUnitId();
          //CP   DetId detectorId = DetId(detectorIdIndex);
          //CP   InvalidTrackingRecHit tHit(detectorId,TrackingRecHit::valid);          
          //CP   hitPattern.set(tHit,nHits);
          //CP   ++nHits;
          //CP }

          //YI // construct hit layer bitmask as used in bambu
          //YI BitMask48 hits;
          //YI // search for all good crossed layers (with or without hit)
          //YI for (Int_t hi=0; hi<hitPattern.numberOfHits(); hi++) {
          //YI   uint32_t hit = hitPattern.getHitPattern(hi);
          //YI   if (hitPattern.getHitType(hit)<=1) {
          //YI     if (hitPattern.trackerHitFilter(hit)) {
          //YI       hits.SetBit(hitReader_.Layer(hit));
          //YI     }
          //YI   }
          //YI }
          
          // outTracking->SetHits(hits); 
          
          // if (verbose_>1) {
          //   printf("Hit Layer Mask: ");
          //   for (Int_t biti=47; biti >= 0; --biti) {
          //         printf("%i",hits.TestBit(biti));
          //   }
          //   printf("\n");
          // }
          
        }
        
        if (verbose_ > 1) {
          printf("trackId = %i\n",theSimTrack.trackId());
          printf("Tracking particle has %i SimTracks\n",(int)inPart.g4Tracks().size());
          if (inPart.g4Tracks().size()>1) {
            for (std::vector<SimTrack>::const_iterator iST = inPart.g4Tracks().begin();
                 iST != inPart.g4Tracks().end(); ++iST) {
              printf("g4 trackid = %i\n",iST->trackId());
            }
          }
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

  if (genActive_) {
    if (genSource_ == kGenParticles) {
      Handle<reco::GenParticleCollection> hGenPProduct;
      GetProduct(genParticlesToken_, hGenPProduct, event);  
  
      reco::GenParticleCollection const& genParticles = *hGenPProduct;

      // loop over all genparticles and copy their information
      unsigned iPart = 0;
      for (auto&& inPart : genParticles) {
        unsigned nDaughters = inPart.numberOfDaughters();
        if (nDaughters == 0) {
          ++iPart;
          continue;
        }

        reco::GenParticleRef ref(hGenPProduct, iPart);
        MCParticle *mcMother = aodGenMap_->GetMit(ref);

        // set mother decay vertex
        reco::Candidate const* genDaughter = inPart.daughter(0);
        mcMother->SetVertex(genDaughter->vx(), genDaughter->vy(), genDaughter->vz());

        for (unsigned iD = 0; iD < nDaughters; ++iD) {
          MCParticle *mcDaughter = aodGenMap_->GetMit(inPart.daughterRef(iD));
          // set mother-daughter links
          mcMother->AddDaughter(mcDaughter);
          if (!mcDaughter->HasMother())
            mcDaughter->SetMother(mcMother);
        }

        ++iPart;
      }
    }
    else if (genSource_ == kHepMCProduct) {
      Handle<edm::HepMCProduct> hHepMCProduct;
      GetProduct(hepMCProdToken_, hHepMCProduct, event);
    
      const HepMC::GenEvent &GenEvent = hHepMCProduct->getHepMCData();  
  
      for (HepMC::GenEvent::particle_const_iterator pgen = GenEvent.particles_begin();
           pgen != GenEvent.particles_end(); ++pgen) {
  
        HepMC::GenParticle *mcPart = (*pgen);
        if(!mcPart) 
          continue;
      
        // check if genpart has a decay vertex
        HepMC::GenVertex *dVertex = mcPart->end_vertex();
        if (!dVertex) 
          continue;

        // find corresponding mithep genparticle parent in association table
        mithep::MCParticle *genParent = genMap_->GetMit(mcPart->barcode());
  
        // set decay vertex
        // division by 10.0 is needed due to HepMC use of mm instead of cm for distance units
        genParent->SetVertex(dVertex->point3d().x()/10.0,
                             dVertex->point3d().y()/10.0,
                             dVertex->point3d().z()/10.0);
  
        // loop through daugthers
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
  }
  
  // loop over SimTracks and resolve links
  if (simActive_) {
    Handle<edm::SimTrackContainer> hSimTrackProduct;
    GetProduct(simTracksToken_, hSimTrackProduct, event);

    edm::SimTrackContainer const& simTracks = *hSimTrackProduct;

    Handle<std::vector<SimVertex> > hSimVertexProduct;
    GetProduct(simVerticesToken_, hSimVertexProduct, event);

    edm::SimVertexContainer const& simVertexes = *hSimVertexProduct;

    // loop through all simParticles
    for (auto&& simTrack : simTracks) {
      if (simTrack.vertIndex() >= 0) {
        mithep::MCParticle *simDaughter = simMap_->GetMit(simTrack.trackId());
        const SimVertex &theVertex = simVertexes.at(simTrack.vertIndex());
        if (theVertex.parentIndex() >= 0) {
          mithep::MCParticle *simParent = simMap_->GetMit(theVertex.parentIndex());
          simParent->SetVertex(theVertex.position().x(),
                               theVertex.position().y(),
                               theVertex.position().z());
          //make sure we don't double count the decay tree
          if (!simParent->HasDaughter(simDaughter)) {
            simParent->AddDaughter(simDaughter);
          }
          if (!simDaughter->HasMother()) {
            simDaughter->SetMother(simParent);
          }
        }
      }
    }
  }

  if (verbose_ > 1 && trackingActive_) {
    Handle<TrackingParticleCollection> hTrackingParticleProduct;
    GetProduct(trackingEdmToken_, hTrackingParticleProduct, event);

    TrackingParticleCollection const& trackingParticles = *hTrackingParticleProduct;

    // loop through all simParticles
    unsigned iPart = 0;
    for (auto&& inPart : trackingParticles) {
      TrackingParticleRef theRef(hTrackingParticleProduct, iPart);
      ++iPart;
      
      mithep::MCParticle *simParent = trackingMap_->GetMit(theRef);
      printf("MCParticle pdg = %i, Daughter Pdgs: ", simParent->PdgId());
      for (UInt_t i=0; i<simParent->NDaughters(); ++i) {
        printf("%i, ", simParent->Daughter(i)->PdgId());
      }
      printf("\n");
      
      printf("TrackingParticle pdg = %i, Daughter Pdgs: ", inPart.pdgId());
      for (TrackingVertexRefVector::iterator v= inPart.decayVertices().begin(); 
           v != inPart.decayVertices().end(); ++v) {
        for (TrackingParticleRefVector::iterator pd = v->get()->daughterTracks().begin(); 
             pd != v->get()->daughterTracks().end(); ++pd) {
          printf("%i, ", pd->get()->pdgId());
        }
      }
      printf("\n");
    }
  }
}

DEFINE_MITHEP_TREEFILLER(FillerMCParticles);
