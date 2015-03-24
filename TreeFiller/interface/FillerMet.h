//--------------------------------------------------------------------------------------------------
// $Id: FillerMet.h,v 1.5 2009/09/25 08:42:50 loizides Exp $
//
// FillerMet
//
// Implementation of a filler to fill EDM missing ET objects into our mithep::Met data structure.
//
// Authors: C.Loizides
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERMET_H
#define MITPROD_TREEFILLER_FILLERMET_H

#include "MitAna/DataTree/interface/MetFwd.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"

#include "DataFormats/METReco/interface/METCollection.h"

namespace mithep 
{
  class FillerMet : public BaseFiller
  {  
    public:
      FillerMet(const edm::ParameterSet &cfg, edm::ConsumesCollector&, ObjectService*, const char *name, bool active=1);
      ~FillerMet();

      void                  BookDataBlock(TreeWriter &tws);
      void 	            FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
  
    private:
      edm::EDGetTokenT<reco::METCollection> edmToken_;    //edm name of met collection
      std::string           mitName_;    //mit name of Mets
      mithep::MetArr       *mets_;       //array of Mets
  };
}
#endif
