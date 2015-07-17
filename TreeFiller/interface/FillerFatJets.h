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
    void FillSpecific(mithep::Jet&, reco::JetBaseRef const&);
    void FillSpecificSubjet(mithep::XlSubJet&, edm::Ptr<pat::Jet>);
    void PrepareSpecific(edm::Event const&, edm::EventSetup const&) override;
    reco::JetTagCollection const* fBJetTags;
  private:
    void fillPATFatJetVariables(mithep::FatJet&, pat::Jet const&);
    void mithep::FillerFatJets::recalcNsubjettiness(const pat::Jet &, const SVTagInfo &, mithep::FatJet &, std::vector<fastjet::PseudoJet> &);
    double fR0;                                                       //cone size
    std::vector<std::string> fSubjetNames;                            //labels of subjets
    std::vector<edm::InputTag> fSubjetCollectionTags;                 //subjet input tags
    std::vector<edm::Handle<PatJetCollection> > fSubjetCollections;   //vector of vector of pat subjets
    edm::EDGetTokenT<reco::VertexCollection> fPVToken;                //offline primary vertex token
    edm::Handle<reco::VertexCollection> fPVs;                         //offline primary vertices
  };
}

#endif
