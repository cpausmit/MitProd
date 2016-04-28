#ifndef MITPROD_TREEFILLER_VERTEXMATCH_H
#define MITPROD_TREEFILLER_VERTEXMATCH_H

// Utility function to geometrically match a vertex to a reco::Candidate.
// Optionally can return the offset to the matched vertex.

#include "MitAna/DataTree/interface/BaseVertexFwd.h"
#include "MitCommon/DataFormats/interface/Types.h"

namespace reco {
  class Candidate;
}

namespace mithep {
  mithep::BaseVertex const* VertexMatch(mithep::BaseVertexCol const*, reco::Candidate const&, mithep::ThreeVector* offset = 0);
}

#endif
