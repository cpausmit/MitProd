//--------------------------------------------------------------------------------------------------
// FillerPFJets
//
// Implementation of a filler to fill EDM particle flow jets into our mithep::PFJet data structure.
//
// Authors: C.Loizides, Y.Iiyama
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERPFJETS_H
#define MITPROD_TREEFILLER_FILLERPFJETS_H

#include "MitProd/TreeFiller/interface/FillerJets.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"
#include "MitAna/DataTree/interface/PFJetCol.h"

namespace reco {
  class PFJet;
}
namespace pat {
  class Jet;
}

namespace mithep {

  class FillerPFJets : public FillerJets {
  public:
    FillerPFJets(edm::ParameterSet const&, edm::ConsumesCollector&, ObjectService*, char const*, bool = true);
    ~FillerPFJets();

    void PrepareLinks() override;
    mithep::Jet* AddNew() override { return static_cast<mithep::PFJetArr*>(jets_)->AddNew(); }
    void FillSpecific(mithep::Jet&, reco::JetBaseRef const&) override;
    void ResolveLinks(edm::Event const&, edm::EventSetup const&) override;

  protected:
    void fillPFJetVariables(mithep::PFJet&, reco::PFJet const&);
    bool fillFromPAT_;
    void fillPATJetVariables(mithep::PFJet&, pat::Jet const&);
    void initBJetTags(edm::Event const&, reco::JetTagCollection const* [mithep::Jet::nBTagAlgos]) override;
    void setBJetTags(mithep::Jet&, reco::JetBaseRef const&, reco::JetTagCollection const* [mithep::Jet::nBTagAlgos]) const override;
    void initCorrections(edm::Event const&, edm::EventSetup const&) override;
    void setCorrections(mithep::Jet&, reco::Jet const&) override;

    std::string bJetTagsName_[mithep::Jet::nBTagAlgos];

    std::string pfCandMapName_; //name of imported PFCandidateMap
    mithep::PFCandidateMap const* pfCandMap_; //map wrt PFCandidates
  };
}

#endif
