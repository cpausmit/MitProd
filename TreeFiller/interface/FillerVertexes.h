//--------------------------------------------------------------------------------------------------
// $Id: FillerVertexes.h,v 1.5 2009/03/22 10:00:46 loizides Exp $
//
// FillerVertexes
//
// Implementation of a filler to fill EDM reco:Vertex into our mithep::Vertex data structure.
//
// Authors: J.Bendavid
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERVERTEXES_H
#define MITPROD_TREEFILLER_FILLERVERTEXES_H

#include "MitAna/DataTree/interface/VertexFwd.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"

namespace mithep 
{
  class FillerVertexes : public BaseFiller
  {  
    public:
      FillerVertexes(const edm::ParameterSet &cfg, const char *name, bool active=1);
      ~FillerVertexes();

      void                     BookDataBlock(TreeWriter &tws);
      void                     FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
  
    private:
      std::string              edmName_;        //edm name of Vertex collection
      std::string              mitName_;        //mit name of Vertices
      std::string              vertexMapName_;  //name of exported map
      mithep::VertexArr       *vertexes_;       //array of vertexes
      mithep::VertexMap       *vertexMap_;      //map wrt vertexes
  };
}
#endif
