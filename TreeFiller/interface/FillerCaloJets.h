//--------------------------------------------------------------------------------------------------
// $Id: FillerCaloJets.h,v 1.9 2009/03/11 18:14:55 bendavid Exp $
//
// FillerCaloJets
//
// Implementation of a filler to fill EDM calo jets into our mithep::CaloJet data structure.
//
// Authors: C.Loizides
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERCALOJETS_H
#define MITPROD_TREEFILLER_FILLERCALOJETS_H

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/JetReco/interface/CaloJetCollection.h"
#include "MitAna/DataUtil/interface/TreeWriter.h"
#include "MitAna/DataTree/interface/Collections.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"

namespace mithep 
{
  class FillerCaloJets : public BaseFiller
  {  
    public:
      FillerCaloJets(const edm::ParameterSet &cfg, const char *name, bool active=1);
      ~FillerCaloJets();

      void            BookDataBlock(TreeWriter &tws);
      void 	      FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
  
    private:
      bool            flavorMatchingActive_;            //=true if flavor matching is done  
      bool            bTaggingActive_;                  //=true if bTagging info is filled
      bool            jetToVertexActive_;               //=true if jet to vertex info is done
      bool            jetCorrectionsActive_;            //=true if jet corrections are done
      std::string     edmName_;                         //edm name of jets collection
      std::string     mitName_;                         //mit name of jets collection
      std::string     jetToVertexAlphaName_;            //edm name of jet to vertex alpha coll
      std::string     jetToVertexBetaName_;             //edm name of jet to vertex beta coll
      std::string     L2JetCorrectorName_;              //label of the L2JetCorrection service
      std::string     L3JetCorrectorName_;              //label of the L3JetCorrection service
      std::string     flavorMatchingByReferenceName_;   //source of flavor matching
      std::string     flavorMatchingDefinition_;        //type of flavor matching
      std::string     jetProbabilityBJetTagsName_;             //bjet algo discriminant
      std::string     jetBProbabilityBJetTagsName_;            //bjet algo discriminant
      std::string     simpleSecondaryVertexBJetTagsName_;      //bjet algo discriminant
      std::string     combinedSecondaryVertexBJetTagsName_;    //bjet algo discriminant
      std::string     combinedSecondaryVertexMVABJetTagsName_; //bjet algo discriminant
      std::string     impactParameterMVABJetTagsName_;         //bjet algo discriminant
      std::string     trackCountingHighEffBJetTagsName_;       //bjet algo discriminant
      std::string     trackCountingHighPurBJetTagsName_;       //bjet algo discriminant
      std::string     softMuonBJetTagsName_;                   //bjet algo discriminant        
      std::string     softMuonNoIPBJetTagsName_;               //bjet algo discriminant
      std::string     softElectronBJetTagsName_;               //bjet algo discriminant
      std::string     caloTowerMapName_;                //name of imported CaloTowerMap
      const mithep::CaloTowerMap      *caloTowerMap_;   //map wrt CaloTowers
      mithep::CaloJetArr              *jets_;           //array of Jets
  };
}
#endif
