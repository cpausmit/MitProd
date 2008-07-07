//--------------------------------------------------------------------------------------------------
// $Id: FillerCaloJets.h,v 1.4 2008/07/03 07:56:14 loizides Exp $
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
      std::string                           edmName_;
      std::string                           mitName_;
      mithep::JetArr                       *jets_;
      edm::Handle<reco::CaloJetCollection>  jetProduct_;
  };
}
#endif
