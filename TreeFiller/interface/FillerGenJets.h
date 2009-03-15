//--------------------------------------------------------------------------------------------------
// $Id: FillerGenJets.h,v 1.1 2008/09/16 18:17:36 sixie Exp $
//
// FillerGenJets
//
// Implementation of a filler to fill EDM generator-level jets into our 
// mithep::GenJet data structure.
//
// Authors: S.Xie
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERGENJETS_H
#define MITPROD_TREEFILLER_FILLERGENJETS_H

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/JetReco/interface/GenJetCollection.h"
#include "MitAna/DataUtil/interface/TreeWriter.h"
#include "MitAna/DataTree/interface/Collections.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"

namespace mithep 
{
  class FillerGenJets : public BaseFiller
  {  
    public:
      FillerGenJets(const edm::ParameterSet &cfg, const char *name, bool active=1);
      ~FillerGenJets();

      void            BookDataBlock(TreeWriter &tws);
      void 	      FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
  
    private:
      Bool_t              flavorMatchingActive_;            //=true if flavor matching is done
      std::string         edmName_;                         //edm name of genjets collection
      std::string         mitName_;                         //mit name of GenJets 
      std::string         flavorMatchingByReferenceName_;   //name of flavor matching collection
      std::string         flavorMatchingDefinition_;        //name of flavor matching algorithm
      mithep::GenJetArr  *genjets_;                         //array of GenJets
  };
}
#endif
