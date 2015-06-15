//--------------------------------------------------------------------------------------------------
// FillerPFJets
//
// Implementation of a filler to fill EDM particle flow jets into our mithep::PFJet data structure.
//
// Authors: C.Loizides
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERPFJETS_H
#define MITPROD_TREEFILLER_FILLERPFJETS_H

#include "MitAna/DataTree/interface/PFJetFwd.h"
#include "MitAna/DataTree/interface/Jet.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"

#include "DataFormats/JetReco/interface/PFJetCollection.h"
#include "SimDataFormats/JetMatching/interface/JetMatchedPartons.h"
#include "DataFormats/BTauReco/interface/JetTag.h"

namespace mithep 
{
  template<class JET>
  class FillerPFJets : public BaseFiller
  {  
  public:
    FillerPFJets(edm::ParameterSet const&, edm::ConsumesCollector&, ObjectService*, char const*, bool = true);
    ~FillerPFJets();

    void BookDataBlock(TreeWriter&) override;
    void FillDataBlock(edm::Event const&, edm::EventSetup const&) override;

    typedef std::vector<JET> JetCollection;

  private:
    void initBJetTags(edm::Event const&);
    void setBJetTags(reco::JetBaseRef const&, mithep::PFJet&) const;
    
    bool flavorMatchingActive_; //=true if flavor matching is done  
    bool bTaggingActive_; //=true if bTagging info is filled
    bool jetToVertexActive_; //=true if jet to vertex info is done
    bool jetCorrectionsActive_; //=true if jet corrections are done
    bool fastJetCorrectionsActive_; //=true if fastjet corrections are done
    edm::EDGetTokenT<JetCollection> edmToken_; //edm name of jets collection
    edm::EDGetTokenT<std::vector<double> > jetToVertexAlphaToken_; //edm name of jet to vertex alpha coll
    edm::EDGetTokenT<std::vector<double> > jetToVertexBetaToken_; //edm name of jet to vertex beta coll
    edm::EDGetTokenT<double> rhoToken_; //edm name of Energy per unit area
    edm::EDGetTokenT<reco::JetMatchedPartonsCollection> flavorMatchingByReferenceToken_; //source of flavor matching
    edm::EDGetTokenT<reco::JetTagCollection> bJetTagsToken_[mithep::Jet::nBTagAlgos];
    std::string bJetTagsName_[mithep::Jet::nBTagAlgos];
    reco::JetTagCollection const* bJetTags_[mithep::Jet::nBTagAlgos];
    std::string mitName_; //mit name of jets collection
    std::string L2JetCorrectorName_; //label of the L2JetCorrection service
    std::string L3JetCorrectorName_; //label of the L3JetCorrection service
    std::string flavorMatchingDefinition_; //type of flavor matching
    std::string pfCandMapName_; //name of imported PFCandidateMap
    mithep::PFCandidateMap const* pfCandMap_; //map wrt PFCandidates
    std::string jetMapName_; //name of exported PFJetMap
    mithep::PFJetMap* jetMap_; //export map
    mithep::PFJetArr* jets_; //array of Jets
  };
}
#endif
