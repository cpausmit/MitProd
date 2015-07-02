//--------------------------------------------------------------------------------------------------
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

#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"

namespace mithep 
{
  class FillerVertexes : public BaseFiller
  {  
    public:
      FillerVertexes(const edm::ParameterSet &cfg, edm::ConsumesCollector&, ObjectService*, const char *name, bool active=1);
      ~FillerVertexes();

      void BookDataBlock(TreeWriter &tws);
      void PrepareLinks();
      void FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
      void ResolveLinks(const edm::Event &e, const edm::EventSetup &es);
  
    private:
      bool                     trkAssocByPacked_; // associate to tracks extracted from pat::PackedCandidates
      edm::EDGetTokenT<reco::VertexCollection> edmToken_;        //edm name of Vertex collection
      std::string              mitName_;        //mit name of Vertices
      std::string              trackMapName_;   //name of imported map wrt trk trks
      std::string              vertexMapName_;  //name of exported map
      mithep::VertexArr       *vertexes_;       //array of vertexes
      mithep::VertexMap       *vertexMap_;      //map wrt vertexes
      const mithep::TrackMap  *trackMap_; //map wrt tracker tracks
      const mithep::CandidateMap *pfTrackMap_; //pat packed cand -> mithep::Track
  };
}
#endif
