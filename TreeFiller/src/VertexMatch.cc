#include "MitProd/TreeFiller/interface/VertexMatch.h"

#include "MitAna/DataTree/interface/BaseVertexCol.h"

#include "DataFormats/Candidate/interface/Candidate.h"

mithep::BaseVertex const*
mithep::VertexMatch(mithep::BaseVertexCol const* vertices, reco::Candidate const& candidate, mithep::ThreeVector* offset/* = 0*/)
{
  for (unsigned iV = 0; iV != vertices->GetEntries(); ++iV) {
    auto* vtx = vertices->At(iV);

    double dx = vtx->X() - candidate.vx();
    double dy = vtx->Y() - candidate.vy();
    double dz = vtx->Z() - candidate.vz();

    if (std::abs(dx) < 1.e-6 && std::abs(dy) < 1.e-6 && std::abs(dz) < 1.e-6) {
      if (offset)
        offset->SetCoordinates(dx, dy, dz);

      return vtx;
    }
  }

  return 0;
}
