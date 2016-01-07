#include "MitProd/TreeFiller/interface/FillerVertexes.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataTree/interface/VertexCol.h"
#include "MitProd/ObjectService/interface/ObjectService.h"
#include "MitEdm/DataFormats/interface/RefToBaseToPtr.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerVertexes::FillerVertexes(const ParameterSet &cfg, edm::ConsumesCollector& collector, ObjectService* os, const char *name, bool active) : 
  BaseFiller(cfg,os,name,active),
  trkAssocByPacked_(cfg.getUntrackedParameter<bool>("trkAssocByPacked", false)),
  edmToken_(GetToken<reco::VertexCollection>(collector, cfg, "edmName")), //offlinePrimaryVertices
  mitName_(cfg.getUntrackedParameter<string>("mitName","PrimaryVertexes")),
  trackMapName_(cfg.getUntrackedParameter<string>("trackMapName","")),
  vertexMapName_(cfg.getUntrackedParameter<string>("vertexMapName","VertexMap")),
  vertexes_(new mithep::VertexArr(100)),
  vertexMap_(new mithep::VertexMap),
  trackMap_(0),
  pfTrackMap_(0)
{
}

//--------------------------------------------------------------------------------------------------
FillerVertexes::~FillerVertexes()
{
  delete vertexes_;
  delete vertexMap_;
}

//--------------------------------------------------------------------------------------------------
void FillerVertexes::BookDataBlock(TreeWriter &tws)
{
  // Add Vertex branch and the VertexMap to tree.

  tws.AddBranch(mitName_,&vertexes_);
  OS()->add<VertexArr>(vertexes_,mitName_);

  if (!vertexMapName_.empty()) {
    vertexMap_->SetBrName(mitName_);
    OS()->add<VertexMap>(vertexMap_,vertexMapName_);
  }
}

void FillerVertexes::PrepareLinks()
{
  if (!trackMapName_.empty()) {
    if (trkAssocByPacked_) {
      pfTrackMap_ = OS()->get<CandidateMap>(trackMapName_);
      AddBranchDep(mitName_, pfTrackMap_->GetBrName());
    }
    else {
      trackMap_ = OS()->get<TrackMap>(trackMapName_);
      AddBranchDep(mitName_,trackMap_->GetBrName());
    }
  }
}

//--------------------------------------------------------------------------------------------------
void FillerVertexes::FillDataBlock(const edm::Event      &event, 
                                   const edm::EventSetup &setup)
{
  // Fill the Vertex branch.

  vertexes_->Delete();
  vertexMap_->Reset();

  Handle<reco::VertexCollection> hVertexProduct;
  GetProduct(edmToken_, hVertexProduct, event);
  vertexMap_->SetEdmProductId(hVertexProduct.id().id());
  reco::VertexCollection const& inVertexes = *hVertexProduct;

  // loop through all vertexes
  for (reco::VertexCollection::const_iterator inV = inVertexes.begin(); 
       inV != inVertexes.end(); ++inV) {

    mithep::Vertex *outVertex = vertexes_->Allocate();
    new (outVertex) mithep::Vertex(inV->x(), inV->y(), inV->z(),
                                   inV->xError(), inV->yError(), inV->zError());
    outVertex->SetChi2(inV->chi2());
    outVertex->SetIsValid(inV->isValid());
    outVertex->SetNdof(inV->ndof());
    outVertex->SetNTracksFit(inV->tracksSize());

    //add vertex to the map
    mitedm::VertexPtr thePtr(hVertexProduct, inV-inVertexes.begin());
    vertexMap_->Add(thePtr, outVertex);
  }
  vertexes_->Trim();
}

void FillerVertexes::ResolveLinks(const edm::Event& event, const edm::EventSetup&)
{
  for (auto&& mapElem : vertexMap_->FwdMap()) {
    auto& inVertex = *mapElem.first;
    auto* outVertex = mapElem.second;

    //fill tracks associated to the vertex
    if (trackMap_) {
      for (auto&& iTrack = inVertex.tracks_begin(); iTrack != inVertex.tracks_end(); ++iTrack)
        outVertex->AddTrack(trackMap_->GetMit(mitedm::refToBaseToPtr(*iTrack)), inVertex.trackWeight(*iTrack));
    }
    else if (pfTrackMap_) {
      for (auto&& pfTrackElem : pfTrackMap_->FwdMap()) {
        pat::PackedCandidate const& cand = static_cast<pat::PackedCandidate const&>(*pfTrackElem.first);

        auto&& vtxRef = cand.vertexRef();
        // should always be non-null, but just in case
        if (vtxRef.isNull() || vtxRef.get() != &inVertex)
          continue;

        double weight = -1.;
        switch (cand.pvAssociationQuality()) {
        case pat::PackedCandidate::CompatibilityBTag:
        case pat::PackedCandidate::CompatibilityDz:
          weight = 0.;
          break;
        case pat::PackedCandidate::UsedInFitLoose:
          weight = 0.5;
          break;
        case pat::PackedCandidate::UsedInFitTight:
          weight = 1.;
          break;
        default:
          break;
        }

        mithep::Track const* track = static_cast<mithep::Track const*>(pfTrackElem.second);
        outVertex->AddTrack(track, weight);
      }
    }
  }
}


DEFINE_MITHEP_TREEFILLER(FillerVertexes);
