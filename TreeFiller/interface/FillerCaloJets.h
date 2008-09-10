//--------------------------------------------------------------------------------------------------
// $Id: FillerCaloJets.h,v 1.6 2008/09/09 12:51:38 sixie Exp $
//
// FillerCaloJets
//
// Imlementation of a filler to fill EDM jets into our mithep::Jet data structure.
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
      std::string           edmName_;                         //edm name of jets collection
      std::string           mitName_;                         //name of Jets in OAK
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
      mithep::JetArr       *jets_;                            //array of Jets
  };
}
#endif
