//--------------------------------------------------------------------------------------------------
// $Id: FillerGenJets.h,v 1.5 2009/09/25 08:42:50 loizides Exp $
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

#include "MitAna/DataTree/interface/GenJetFwd.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"

#include "DataFormats/JetReco/interface/GenJetCollection.h"
#include "SimDataFormats/JetMatching/interface/JetMatchedPartons.h"

namespace mithep 
{
  class FillerGenJets : public BaseFiller
  {  
    public:
      FillerGenJets(const edm::ParameterSet &cfg, edm::ConsumesCollector&, ObjectService*, const char *name, bool active=1);
      ~FillerGenJets();

      void                BookDataBlock(TreeWriter &tws);
      void 	          FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
  
    private:
      Bool_t              flavorMatchingActive_;            //=true if flavor matching is done
      edm::EDGetTokenT<reco::GenJetCollection> edmToken_;                         //edm name of genjets collection
      edm::EDGetTokenT<reco::JetMatchedPartonsCollection> flavorMatchingByReferenceToken_;   //name of flavor matching collection
      std::string         mitName_;                         //mit name of GenJets 
      std::string         flavorMatchingDefinition_;        //name of flavor matching algorithm
      mithep::GenJetArr  *genjets_;                         //array of GenJets
  };
}
#endif
