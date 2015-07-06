//--------------------------------------------------------------------------------------------------
// FillerCaloJets
//
// Implementation of a filler to fill EDM calo jets into our mithep::CaloJet data structure.
//
// Authors: C.Loizides
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERCALOJETS_H
#define MITPROD_TREEFILLER_FILLERCALOJETS_H

#include "MitProd/TreeFiller/interface/FillerJets.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"
#include "MitAna/DataTree/interface/CaloJetCol.h"

#include "DataFormats/JetReco/interface/JetID.h"

namespace mithep {

  class FillerCaloJets : public FillerJets {
  public:
    FillerCaloJets(edm::ParameterSet const&, edm::ConsumesCollector&, ObjectService*, char const* name, bool active = true);
    ~FillerCaloJets();

    void PrepareLinks() override;
    mithep::Jet* AddNew() override { return static_cast<mithep::CaloJetArr*>(jets_)->AddNew(); }
    void PrepareSpecific(edm::Event const&, edm::EventSetup const&) override;
    void FillSpecific(mithep::Jet&, reco::JetBaseRef const&) override;
    void ResolveLinks(edm::Event const&, edm::EventSetup const&) override;

  private:
    bool jetIDActive_;                                 //=true if jet id variables are filled
    edm::EDGetTokenT<reco::JetIDValueMap> jetIDToken_; //label of the JetID product
    reco::JetIDValueMap const* jetIDMap_;

    std::string caloTowerMapName_;                     //name of imported CaloTowerMap
    mithep::CaloTowerMap const* caloTowerMap_;         //map wrt CaloTowers
  };
}

#endif
