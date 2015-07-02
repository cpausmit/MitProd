//--------------------------------------------------------------------------------------------------
// FillerJets
//
// Template for fillers of various jet types
//
// Authors: Y.Iiyama
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERJETS_H
#define MITPROD_TREEFILLER_FILLERJETS_H

#include "MitProd/TreeFiller/interface/AssociationMaps.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"
#include "MitProd/ObjectService/interface/ObjectService.h"
#include "MitAna/DataTree/interface/Jet.h"
#include "MitAna/DataCont/interface/Array.h"

#include "DataFormats/JetReco/interface/JetCollection.h"
#include "DataFormats/Common/interface/View.h"
#include "DataFormats/BTauReco/interface/JetTag.h"
#include "SimDataFormats/JetMatching/interface/JetMatchedPartons.h"
#include "JetMETCorrections/Objects/interface/JetCorrector.h"

namespace mithep {

  class FillerJets : public BaseFiller {  
  public:
    FillerJets(edm::ParameterSet const&, edm::ConsumesCollector&, ObjectService*, char const* name, bool active = true);
    ~FillerJets();

    void BookDataBlock(TreeWriter&) override;
    void FillDataBlock(edm::Event const&, edm::EventSetup const&) override;
    void FillPostRunBlock(edm::Run const&, edm::EventSetup const&) override;
    virtual mithep::Jet* AddNew() = 0;
    virtual void BookAdditional(TreeWriter&) {}
    virtual void PrepareSpecific(edm::Event const&, edm::EventSetup const&) {}
    virtual void FillSpecific(mithep::Jet&, reco::JetBaseRef const&) {}
 
  protected:
    enum FlavorMatchingDef {
      kAlgorithmic,
      kPhysics,
      nFlavorMatchingDefs
    };

    virtual void initBJetTags(edm::Event const&, reco::JetTagCollection const* [mithep::Jet::nBTagAlgos]);
    virtual void setBJetTags(mithep::Jet&, reco::JetBaseRef const&, reco::JetTagCollection const* [mithep::Jet::nBTagAlgos]) const;
    virtual void initCorrections(edm::Event const&, edm::EventSetup const&);
    virtual void setCorrections(mithep::Jet&, reco::Jet const&);

    bool flavorMatchingActive_;     //=true if flavor matching is done  
    bool bTaggingActive_;           //=true if bTagging info is filled
    bool jetToVertexActive_;        //=true if jet to vertex info is done
    bool fastJetCorrectionsActive_; //=true if L1 corrections are done
    bool jetCorrectionsActive_;     //=true if L2 & L3 corrections are done

    edm::EDGetTokenT<reco::JetView> edmToken_; //edm name of jets collection
    edm::EDGetTokenT<std::vector<double> > jetToVertexAlphaToken_; //edm name of jet to vertex alpha coll
    edm::EDGetTokenT<std::vector<double> > jetToVertexBetaToken_; //edm name of jet to vertex beta coll
    edm::EDGetTokenT<reco::JetMatchedPartonsCollection> flavorMatchingByReferenceToken_; //source of flavor matching
    edm::EDGetTokenT<reco::JetTagCollection> bJetTagsToken_[mithep::Jet::nBTagAlgos]; //bjet algo discriminant
    edm::EDGetTokenT<double> rhoToken_; //pileup energy density

    std::string mitName_;                         //mit name of jets collection
    std::string L2JetCorrectorName_;              //label of the L2JetCorrection service
    std::string L3JetCorrectorName_;              //label of the L3JetCorrection service
    std::string jetMapName_;                      //name of exported PFJetMap
    FlavorMatchingDef flavorMatchingDefinition_;        //type of flavor matching
    double rho_;
    JetCorrector const* L2Corrector_;
    JetCorrector const* L3Corrector_;

    JetMap*         jetMap_;      //export map
    BaseCollection* jets_;        //array of Jets
  };

}

#endif
