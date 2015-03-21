//--------------------------------------------------------------------------------------------------
// $Id: FillerGenMet.h,v 1.3 2009/09/25 08:42:50 loizides Exp $
//
// FillerGenMet
//
// Implementation of a filler to fill EDM missing ET objects into our mithep::Met data structure.
//
// Authors: C.Loizides
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERGENMET_H
#define MITPROD_TREEFILLER_FILLERGENMET_H

#include "MitAna/DataTree/interface/GenMetFwd.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"

#include "DataFormats/METReco/interface/GenMETCollection.h"

namespace mithep 
{
  class FillerGenMet : public BaseFiller
  {  
    public:
      FillerGenMet(const edm::ParameterSet &cfg, edm::ConsumesCollector&, const char *name, bool active=1);
      ~FillerGenMet();

      void                  BookDataBlock(TreeWriter &tws);
      void 	            FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
  
    private:
      edm::EDGetTokenT<reco::GenMETCollection> edmToken_;    //edm name of met collection
      std::string           mitName_;    //mit name of met collection
      mithep::GenMetArr    *genMets_;    //array of Mets
  };
}
#endif
