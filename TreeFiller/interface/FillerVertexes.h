//--------------------------------------------------------------------------------------------------
// $Id: FillerVertexes.h,v 1.7 2009/09/25 08:42:50 loizides Exp $
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

      void                     BookDataBlock(TreeWriter &tws, const edm::EventSetup &es);
      void                     FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
  
    private:
      std::string              edmName_;        //edm name of Vertex collection
      std::string              mitName_;        //mit name of Vertices
      std::string              trackMapName_;   //name of imported map wrt trk trks
      std::string              vertexMapName_;  //name of exported map
      mithep::VertexArr       *vertexes_;       //array of vertexes
      mithep::VertexMap       *vertexMap_;      //map wrt vertexes
      const mithep::TrackMap  *trackMap_; //map wrt tracker tracks
  };
}
#endif
