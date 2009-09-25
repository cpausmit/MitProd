//--------------------------------------------------------------------------------------------------
// $Id: FillerMCVertexes.h,v 1.8 2009/06/15 15:00:25 loizides Exp $
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

namespace mithep 
{
  class FillerMCVertexes : public BaseFiller
  {
    public:
      FillerMCVertexes(const edm::ParameterSet &cfg, const char *name, bool active=1);
      ~FillerMCVertexes();

      void                           BookDataBlock(TreeWriter &tws, const edm::EventSetup &es);
      void                           FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
  
    private:
      bool                           useAodGen_;       //=true if AOD GenParticles to be used
      std::string                    edmName_;         //edm name of generated particles
      std::string                    mitName_;         //name of MCParticles branch
      mithep::VertexArr             *vertexes_;        //array of vertexes
  };
}
#endif
