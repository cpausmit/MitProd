//--------------------------------------------------------------------------------------------------
// $Id: FillerMCVertexes.h,v 1.1 2009/09/25 08:42:42 loizides Exp $
//
// FillerMCVertexes
//
// Implementation of a filler to find the generated main vertex into our 
// mithep::Vertex data structure.
//
// Authors: C.Loizides
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERMCVERTEXES_H
#define MITPROD_TREEFILLER_FILLERMCVERTEXES_H

#include "MitAna/DataTree/interface/VertexFwd.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"

#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"

namespace mithep 
{
  class FillerMCVertexes : public BaseFiller
  {
    public:
      FillerMCVertexes(const edm::ParameterSet &cfg, edm::ConsumesCollector&, const char *name, bool active=1);
      ~FillerMCVertexes();

      void                           BookDataBlock(TreeWriter &tws);
      void                           FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
  
    private:
      bool                           useAodGen_;       //=true if AOD GenParticles to be used
      edm::EDGetTokenT<edm::HepMCProduct> hepMCProdToken_;         //edm name of generated particles
      edm::EDGetTokenT<reco::GenParticleCollection> genParticlesToken_;         //edm name of generated particles
      std::string                    mitName_;         //name of MCParticles branch
      mithep::VertexArr             *vertexes_;        //array of vertexes
  };
}
#endif
