//--------------------------------------------------------------------------------------------------
// $Id: FillerCaloJets.h,v 1.4 2008/08/31 10:13:31 sixie Exp $
//
// FillerCaloJets
//
// Imlementation of a filler to fill EDM jets into our mithep::Jet data structure.
//
// Authors: C.Loizides
//--------------------------------------------------------------------------------------------------

#ifndef TREEFILLER_FILLERCALOJETS_H
#define TREEFILLER_FILLERCALOJETS_H

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
      std::string           edmName_;                         //edm name of jets collection
      std::string           mitName_;                         //name of Jets in OAK
      std::string           flavorMatchingByReferenceName_;   //edm name of jets collection
      std::string           flavorMatchingDefinition_;        //name of Jets in OAK
      std::string           caloTowerMapName_;                //name of imported CaloTowerMap
      const mithep::CaloTowerMap *caloTowerMap_;              //map wrt CaloTowers
      mithep::JetArr       *jets_;                            //array of Jets

  };
}
#endif
