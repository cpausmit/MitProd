//--------------------------------------------------------------------------------------------------
// $Id: FillerVertexes.h,v 1.2 2008/09/10 03:30:22 loizides Exp $
//
// FillerVertexes
//
// Imlementation of a filler to fill EDM reco:Vertex into our mithep::Vertex data structure.
//
// Authors: J.Bendavid
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERVERTEXES_H
#define MITPROD_TREEFILLER_FILLERVERTEXES_H

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "MitAna/DataUtil/interface/TreeWriter.h"
#include "MitAna/DataTree/interface/Collections.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"

namespace mithep 
{
  class FillerVertexes : public BaseFiller
  {  
    public:
      FillerVertexes(const edm::ParameterSet &cfg, const char *name, bool active=1);
      ~FillerVertexes();

      void                     BookDataBlock(TreeWriter &tws);
      void 	               FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
      const VertexMap         *GetVertexMap() const { return vertexMap_; }
  
    private:
      std::string                           edmName_;
      std::string                           mitName_;
      std::string                           vertexMapName_;        //name of export map
      mithep::VertexArr                    *vertexes_;             //array of vertexes
      mithep::VertexMap                    *vertexMap_;      //map wrt vertexes
  };
}
#endif
