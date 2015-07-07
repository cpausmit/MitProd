//--------------------------------------------------------------------------------------------------
// FillerTrackJets
//
// Implementation of a filler to fill EDM track jets into our mithep::TrackJet data structure.
//
// Authors: C.Loizides
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERTRACKJETS_H
#define MITPROD_TREEFILLER_FILLERTRACKJETS_H

#include "MitProd/TreeFiller/interface/FillerJets.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"
#include "MitAna/DataTree/interface/TrackJetCol.h"

namespace mithep {

  class FillerTrackJets : public FillerJets {  
  public:
    FillerTrackJets(edm::ParameterSet const&, edm::ConsumesCollector&, ObjectService*, char const* name, bool active = true);
    ~FillerTrackJets();

    void PrepareLinks() override;
    mithep::Jet* AddNew() override { return static_cast<mithep::TrackJetArr*>(jets_)->AddNew(); }
    void ResolveLinks(edm::Event const&, edm::EventSetup const&) override;

  private:
    std::string trackMapName_;           //name of imported TrackMap
    std::string vertexMapName_;          //name map wrt primary vertexes
    mithep::TrackMap  const* trackMap_;  //map wrt PFCandidates
    mithep::VertexMap const* vertexMap_; //map wrt primary vertexes
  };
}
#endif
