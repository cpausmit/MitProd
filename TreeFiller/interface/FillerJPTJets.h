//--------------------------------------------------------------------------------------------------
// FillerJPTJets
//
// Implementation of a filler to fill EDM particle flow jets into our mithep::PFJet data structure.
//
// Authors: C.Loizides
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERJPTJETS_H
#define MITPROD_TREEFILLER_FILLERJPTJETS_H

#include "MitProd/TreeFiller/interface/FillerJets.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"
#include "MitAna/DataTree/interface/JPTJetCol.h"

namespace mithep {

  class FillerJPTJets : public FillerJets {
  public:
    FillerJPTJets(edm::ParameterSet const&, edm::ConsumesCollector&, ObjectService*, char const* name, bool active = true);
    ~FillerJPTJets();

    void PrepareLinks() override;
    mithep::Jet* AddNew() override { return static_cast<mithep::JPTJetArr*>(jets_)->AddNew(); }
    void FillSpecific(mithep::Jet&, reco::JetBaseRef const&) override;
    void ResolveLinks(edm::Event const&, edm::EventSetup const&) override;
  
  private:
    std::string caloJetMapName_;           //name of imported CaloJetMap
    mithep::CaloJetMap const* caloJetMap_; //map wrt CaloJets
  };

}

#endif
