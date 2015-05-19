//--------------------------------------------------------------------------------------------------
// FillerMCParticles
//
// Imlementation of a filler to fill HepMC particles or the AOD GenParticle candidates (gen)
// and SimTracks (sim) with association map to TrackingParticles (for hit-based MC truth matching)
// into mithep::MCParticles. Generated and simulated particle information is thus merged into a 
// single collection.
//
// Authors: C.Loizides, J.Bendavid
//--------------------------------------------------------------------------------------------------
#ifndef MITPROD_TREEFILLER_FILLERMCPARTICLES_H
#define MITPROD_TREEFILLER_FILLERMCPARTICLES_H

#include "MitAna/DataTree/interface/MCParticleFwd.h"
#include "MitAna/DataTree/interface/TrackingParticleFwd.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"
#include "MitProd/TreeFiller/interface/HitPatternReader.h"

#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "SimDataFormats/Vertex/interface/SimVertexContainer.h"
#include "SimDataFormats/Track/interface/SimTrackContainer.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticleFwd.h"
#include "DataFormats/PatCandidates/interface/PackedGenParticle.h"

namespace mithep 
{
  class FillerMCParticles : public BaseFiller
  {
    public:
      FillerMCParticles(const edm::ParameterSet &cfg, edm::ConsumesCollector&, ObjectService*, const char *name, bool active=1);
      ~FillerMCParticles();

      void                           BookDataBlock(TreeWriter &tws);
      void                           FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
      void                           ResolveLinks (const edm::Event &e, const edm::EventSetup &es);
  
    private:
      typedef AssociationMap<const reco::GenParticleRef, mithep::MCParticle*> GenParticleMap;
      typedef AssociationMap<const pat::PackedGenParticleRef, mithep::MCParticle*> PackedGenParticleMap;

      enum GenSource {
        kGenParticles,
        kPackedGenParticles,
        kHepMCProduct,
        nGenSources
      };

      bool                           genActive_;       //=true if generated particles are filled
      bool                           simActive_;       //=true if simulated particles are filled
      bool                           trackingActive_;  //=true if TrackingParticles are mapped
      bool                           fillTracking_;    //=true if detailed sim hit info is filled
      unsigned                       genSource_;       //=switch for gen particles source
      edm::EDGetTokenT<edm::HepMCProduct> hepMCProdToken_; //edm token of generated particles
      edm::EDGetTokenT<reco::GenParticleCollection> genParticlesToken_; //edm token of generated particles
      edm::EDGetTokenT<std::vector<int> > genBarcodesToken_; //edm token of generated particles
      edm::EDGetTokenT<edm::SimTrackContainer> simTracksToken_;
      edm::EDGetTokenT<std::vector<SimVertex> > simVerticesToken_;
      edm::EDGetTokenT<TrackingParticleCollection> trackingEdmToken_; //edm token of simulated TrackingParticles
      edm::EDGetTokenT<pat::PackedGenParticleCollection> packedGenParticlesToken_;
      std::string                    genMapName_;      //name of exp map wrt generated particles
      std::string                    simMapName_;      //name of exp map wrt simulated particles
      std::string                    trackingMapName_; //name of exp map wrt TrackingParticles
      std::string                    mitName_;         //name of MCParticles branch
      std::string                    mitTrackingName_; //name of TrackingParticles branch
      mithep::MCParticleArr         *mcParticles_;     //array of MCParticles
      mithep::TrackingParticleArr   *trackingParticles_; //array of TrackingParticles
      mithep::GenParticleBarcodeMap *genMap_;          //map wrt generated particles
      GenParticleMap                *aodGenMap_;       //map wrt generated particles
      PackedGenParticleMap          *packedGenMap_;    //map wrt generated particles
      mithep::SimTrackTidMap        *simMap_;          //map of SimTracks to G4 track ids
      mithep::TrackingParticleMap   *trackingMap_;     //map wrt TrackingParticles
      mithep::HitPatternReader       hitReader_;       //hit pattern reader
  };
}
#endif
