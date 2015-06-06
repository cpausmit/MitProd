//--------------------------------------------------------------------------------------------------
// FillerCaloJets
//
// Implementation of a filler to fill EDM calo jets into our mithep::CaloJet data structure.
//
// Authors: C.Loizides
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERCALOJETS_H
#define MITPROD_TREEFILLER_FILLERCALOJETS_H

#include "MitAna/DataTree/interface/CaloJetCol.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"

#include "DataFormats/JetReco/interface/Jet.h"
#include "DataFormats/JetReco/interface/JetID.h"
#include "DataFormats/BTauReco/interface/JetTag.h"
#include "SimDataFormats/JetMatching/interface/JetMatchedPartons.h"

namespace mithep 
{
  class FillerCaloJets : public BaseFiller
  {  
    public:
      FillerCaloJets(const edm::ParameterSet &cfg, edm::ConsumesCollector&, ObjectService*, const char *name, bool active=1);
      ~FillerCaloJets();

      void            BookDataBlock(TreeWriter &tws);
      void 	      FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
  
    private:
      bool            flavorMatchingActive_;            //=true if flavor matching is done  
      bool            bTaggingActive_;                  //=true if bTagging info is filled
      bool            jetToVertexActive_;               //=true if jet to vertex info is done
      bool            jetCorrectionsActive_;            //=true if jet corrections are done
      bool            jetIDActive_;                     //=true if jet id variables are filled
      edm::EDGetTokenT<reco::CaloJetCollection>     edmToken_;                         //edm name of jets collection
      edm::EDGetTokenT<std::vector<double> >     jetToVertexAlphaToken_;            //edm name of jet to vertex alpha coll
      edm::EDGetTokenT<std::vector<double> >     jetToVertexBetaToken_;             //edm name of jet to vertex beta coll
      edm::EDGetTokenT<reco::JetIDValueMap>     jetIDToken_;                       //label of the JetID product
      edm::EDGetTokenT<reco::JetMatchedPartonsCollection>     flavorMatchingByReferenceToken_;   //source of flavor matching
      edm::EDGetTokenT<reco::JetTagCollection>     bJetTagsToken_[mithep::Jet::nBTagAlgos];             //bjet algo discriminant
      std::string     L2JetCorrectorName_;              //label of the L2JetCorrection service
      std::string     L3JetCorrectorName_;              //label of the L3JetCorrection service
      std::string     flavorMatchingDefinition_;        //type of flavor matching
      std::string     mitName_;                         //mit name of jets collection
      std::string     caloTowerMapName_;                //name of imported CaloTowerMap
      std::string     jetMapName_;                      //name of exported CaloJetMap
      const mithep::CaloTowerMap      *caloTowerMap_;   //map wrt CaloTowers
      mithep::CaloJetMap              *jetMap_;         //export map
      mithep::CaloJetArr              *jets_;           //array of Jets
  };
}
#endif
