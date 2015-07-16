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
    virtual mithep::Jet* AddNew() override { return static_cast<mithep::PFJetArr*>(jets_)->AddNew(); }
    virtual void FillSpecific(mithep::Jet&, reco::JetBaseRef const&) override;
    void ResolveLinks(edm::Event const&, edm::EventSetup const&) override;

  private:
    void fillPFJetVariables(mithep::PFJet&, reco::PFJet const&);
    virtual void fillPATJetVariables(mithep::PFJet&, pat::Jet const&);
    void initBJetTags(edm::Event const&, reco::JetTagCollection const* [mithep::Jet::nBTagAlgos]) override;
    void setBJetTags(mithep::Jet&, reco::JetBaseRef const&, reco::JetTagCollection const* [mithep::Jet::nBTagAlgos]) const override;
    void initCorrections(edm::Event const&, edm::EventSetup const&) override;
    void setCorrections(mithep::Jet&, reco::Jet const&) override;

    bool fillFromPAT_;

    std::string bJetTagsName_[mithep::Jet::nBTagAlgos];

    std::string pfCandMapName_; //name of imported PFCandidateMap
    mithep::PFCandidateMap const* pfCandMap_; //map wrt PFCandidates
  };
}

#endif
