//--------------------------------------------------------------------------------------------------
// $Id: FillerVertexes.h,v 1.1 2008/09/30 13:03:32 bendavid Exp $
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

      void BookDataBlock(TreeWriter &tws);
      void FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
  
    private:
      std::string                           edmName_;        //edm name of Vertex collection
      std::string                           mitName_;        //name of Vertices in OAK
      std::string                           vertexMapName_;  //name of exported map
      mithep::VertexArr                    *vertexes_;       //array of vertexes
      mithep::VertexMap                    *vertexMap_;      //map wrt vertexes
  };
}
#endif
