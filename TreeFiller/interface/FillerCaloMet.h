//--------------------------------------------------------------------------------------------------
// $Id: FillerCaloMet.h,v 1.11 2009/09/25 08:42:50 loizides Exp $
//
// FillerCaloMet
//
// Implementation of a filler to fill EDM missing ET objects into our mithep::Met data structure.
//
// Authors: C.Loizides
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERCALOMET_H
#define MITPROD_TREEFILLER_FILLERCALOMET_H

#include "MitAna/DataTree/interface/CaloMetFwd.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"

#include "DataFormats/METReco/interface/CaloMETCollection.h"

namespace mithep 
{
  class FillerCaloMet : public BaseFiller
  {  
    public:
      FillerCaloMet(const edm::ParameterSet &cfg, edm::ConsumesCollector&, const char *name, bool active=1);
      ~FillerCaloMet();

      void                  BookDataBlock(TreeWriter &tws);
      void 	            FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
  
    private:
      edm::EDGetTokenT<reco::CaloMETCollection>           edmToken_;    //edm name of met collection
      std::string           mitName_;    //mit name of met collection
      mithep::CaloMetArr   *caloMets_;   //array of Mets
  };
}
#endif
