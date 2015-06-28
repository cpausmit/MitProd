//--------------------------------------------------------------------------------------------------
// FillerPFJets
//
// Implementation of a filler to fill EDM particle flow jets into our mithep::PFJet data structure.
//
// Authors: C.Loizides, Y.Iiyama
//--------------------------------------------------------------------------------------------------

// reverse header protection - this file should be included only by FillerPFJets.cc
#ifndef FILLERPFJETS_INSTANCE
#warning This header is protected from accidental inclusions.
#abort // there is no such preprocessor - will therefore abort.
#endif

#ifndef MITPROD_TREEFILLER_FILLERPFJETS_H
#define MITPROD_TREEFILLER_FILLERPFJETS_H

#include "MitProd/TreeFiller/interface/FillerJets.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"
#include "MitAna/DataTree/interface/PFJetCol.h"

#include "DataFormats/JetReco/interface/PFJet.h"

namespace mithep {

  template<class PFJET>
  class FillerPFJets : public FillerJets<mithep::PFJet> {
  public:
    FillerPFJets(edm::ParameterSet const&, edm::ConsumesCollector&, ObjectService*, char const*, bool = true);
    ~FillerPFJets();

    void PrepareLinks() override;
    void PrepareSpecific(edm::Event const&, edm::EventSetup const&);
    void FillSpecific(mithep::PFJet&, reco::JetBaseRef const&) override;
    void ResolveLinks(edm::Event const&, edm::EventSetup const&) override;

  private:
    void initBJetTags(edm::Event const&, reco::JetTagCollection const* [mithep::Jet::nBTagAlgos]) override;
    void setBJetTags(mithep::Jet&, reco::JetBaseRef const&, reco::JetTagCollection const* [mithep::Jet::nBTagAlgos]) const override;
    
    bool fastJetCorrectionsActive_; //=true if fastjet corrections are done
    edm::EDGetTokenT<double> rhoToken_; //edm name of Energy per unit area
    double rho_;

    std::string bJetTagsName_[mithep::Jet::nBTagAlgos];

    std::string pfCandMapName_; //name of imported PFCandidateMap
    mithep::PFCandidateMap const* pfCandMap_; //map wrt PFCandidates
  };
}

#endif
