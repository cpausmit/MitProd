//--------------------------------------------------------------------------------------------------
// $Id: FillerCaloJets.h,v 1.8 2008/09/16 18:20:46 sixie Exp $
//
// FillerCaloJets
//
// Imlementation of a filler to fill EDM jets into our mithep::CaloJet data structure.
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
      Bool_t                flavorMatchingActive_;            //=true if flavor matching is done  
      Bool_t                bTaggingActive_;                  //=true if bTagging info is filled
      Bool_t                jetToVertexActive_;               //=true if jet to vertex info is done
      Bool_t                jetCorrectionsActive_;            //=true if jet corrections are done
      std::string           edmName_;                         //edm name of jets collection
      std::string           mitName_;                         //name of Jets in OAK
      std::string           jetToVertexAlphaName_;            //edm name of jet to vertex alpha coll
      std::string           jetToVertexBetaName_;             //edm name of jet to vertex beta coll
      std::string           L2JetCorrectorName_;              //label of the L2JetCorrection service
      std::string           L3JetCorrectorName_;              //label of the L3JetCorrection service
      std::string           flavorMatchingByReferenceName_;   //edm name of jets collection
      std::string           flavorMatchingDefinition_;        //name of Jets in OAK
      std::string           jetProbabilityBJetTagsName_;      //names for various bJet Tagging
      std::string           jetBProbabilityBJetTagsName_;     //algorithms
      std::string           simpleSecondaryVertexBJetTagsName_;
      std::string           combinedSecondaryVertexBJetTagsName_;
      std::string           combinedSecondaryVertexMVABJetTagsName_;
      std::string           impactParameterMVABJetTagsName_;
      std::string           trackCountingHighEffBJetTagsName_;
      std::string           trackCountingHighPurBJetTagsName_;
      std::string           softMuonBJetTagsName_;
      std::string           softMuonNoIPBJetTagsName_;
      std::string           softElectronBJetTagsName_;
      std::string           caloTowerMapName_;                //name of imported CaloTowerMap

      const mithep::CaloTowerMap *caloTowerMap_;              //map wrt CaloTowers
      mithep::CaloJetArr   *jets_;                            //array of Jets
  };
}
#endif
