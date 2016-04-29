//--------------------------------------------------------------------------------------------------
// FillerMCVertexes
//
// Implementation of a filler to find the generated vertices into our 
// mithep::BaseVertex data structure.
//
// Authors: C.Loizides, Y.Iiyama
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERMCVERTEXES_H
#define MITPROD_TREEFILLER_FILLERMCVERTEXES_H

#include "MitAna/DataTree/interface/BaseVertexFwd.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"

#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"
#include "SimDataFormats/Vertex/interface/SimVertexContainer.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "DataFormats/PatCandidates/interface/PackedGenParticle.h"

namespace mithep 
{
  class FillerMCVertexes : public BaseFiller
  {
    public:
      FillerMCVertexes(const edm::ParameterSet &cfg, edm::ConsumesCollector&, ObjectService* os, const char *name, bool active=1);
      ~FillerMCVertexes();

      void BookDataBlock(TreeWriter &tws);
      void FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
  
    private:
      enum GenSource {
        kGenParticles,
        kPackedGenParticles,
        kHepMCProduct,
        nGenSources
      };

      bool simActive_;
      unsigned genSource_;
      edm::EDGetTokenT<edm::HepMCProduct> hepMCProdToken_; //edm name of generated particles
      edm::EDGetTokenT<reco::GenParticleCollection> genParticlesToken_; //edm name of generated particles
      edm::EDGetTokenT<pat::PackedGenParticleCollection> packedGenParticlesToken_;
      edm::EDGetTokenT<edm::SimVertexContainer> simVerticesToken_;
      std::string mitName_; //name of MCParticles branch
      std::string genVtxMapName_; //name of GenVertexMap
      std::string simVtxMapName_; //name of SimVertexMap
      mithep::BaseVertexArr *vertexes_; //array of vertexes
      mithep::GenVertexMap *genVtxMap_; //HepMCVertex -> BaseVertex
      mithep::SimVertexMap *simVtxMap_; //SimVertex -> BaseVertex
  };
}
#endif
