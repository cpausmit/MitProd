//--------------------------------------------------------------------------------------------------
// FillerFatJets
//
// Implementation of a filler to fill EDM particle flow jets into our mithep::PFJet data structure.
//
// Authors: C.Loizides, Y.Iiyama
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERFATJETS_H
#define MITPROD_TREEFILLER_FILLERFATJETS_H

#include "MitProd/TreeFiller/interface/FillerJets.h"
#include "MitProd/TreeFiller/interface/FillerPFJets.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"
#include "MitAna/DataTree/interface/FatJetCol.h"

namespace reco {
  class FatJet;
}
namespace pat {
  class Jet;
}

typedef std::vector<pat::Jet> PatJetCollection;
typedef std::vector<edm::Ptr<pat::Jet> > PatJetPtrCollection;

namespace mithep {

  class FillerFatJets : public FillerPFJets {
  public:
    FillerFatJets(edm::ParameterSet const&, edm::ConsumesCollector&, ObjectService*, char const*, bool = true);
    ~FillerFatJets();

    mithep::Jet* AddNew() override { return static_cast<mithep::FatJetArr*>(jets_)->AddNew(); }
    void FillSpecific(mithep::Jet&, reco::JetBaseRef const&) override;

  private:
    void fillPATFatJetVariables(mithep::FatJet&, pat::Jet const&);
    void GetSetupInfo(edm::EventSetup const&) override;

    double fR0;
    std::vector<std::string> fSubjetNames;
    std::vector<edm::InputTag> fSubjetCollectionTags;
    std::vector<edm::Handle<PatJetCollection> > fSubjetCollections;



  };
}

#endif
