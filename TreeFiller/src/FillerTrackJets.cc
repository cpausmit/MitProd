#define FILLERJETS_INSTANCE

#include "MitProd/TreeFiller/interface/FillerTrackJets.h"

#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/JetReco/interface/TrackJet.h"
#include "DataFormats/Common/interface/RefToPtr.h"

mithep::FillerTrackJets::FillerTrackJets(edm::ParameterSet const& cfg, edm::ConsumesCollector& collector, mithep::ObjectService* os, char const* name, bool active/* = true*/) : 
  FillerJets<mithep::TrackJet>(cfg, collector, os, name, active),
  trackMapName_(cfg.getUntrackedParameter<string>("trackMapName","trackMapName")),
  vertexMapName_(cfg.getUntrackedParameter<string>("vertexMapName","")),
  trackMap_(0),
  vertexMap_(0)
{
}

mithep::FillerTrackJets::~FillerTrackJets()
{
}

void
mithep::FillerTrackJets::PrepareLinks()
{
  if (!trackMapName_.empty()) {
    trackMap_ = OS()->get<TrackMap>(trackMapName_);
    if (trackMap_)
      AddBranchDep(mitName_,trackMap_->GetBrName());
  }
  if (!vertexMapName_.empty()) {
    vertexMap_ = OS()->get<VertexMap>(vertexMapName_);
    if (vertexMap_)
      AddBranchDep(mitName_,vertexMap_->GetBrName());
  }
}

void
mithep::FillerTrackJets::ResolveLinks(edm::Event const& event, edm::EventSetup const&)
{
  if (!trackMap_ && !vertexMap_)
    return;

  for (auto& mapElem : jetMap_->FwdMap()) {
    auto&& jPtr = mapElem.first;
    auto& inJet = static_cast<reco::TrackJet const&>(*jPtr);
    auto& outJet = *mapElem.second;

    //fill primary vertex reference
    if (vertexMap_ && inJet.primaryVertex().isNonnull())
      outJet.SetVertex(vertexMap_->GetMit(refToPtr(inJet.primaryVertex())));

    // add track refs
    if (trackMap_) {
      for (uint i=0; i < inJet.numberOfTracks(); ++i) {
        auto&& trkPtr = inJet.track(i);
        outJet.AddTrack(trackMap_->GetMit(trkPtr));
      }
    }
  }
}

DEFINE_MITHEP_TREEFILLER(FillerTrackJets);
