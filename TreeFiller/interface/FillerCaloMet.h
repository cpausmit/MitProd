//--------------------------------------------------------------------------------------------------
// $Id: FillerCaloMet.h,v 1.7 2009/03/12 16:00:23 bendavid Exp $
//
// FillerCaloMet
//
// Implementation of a filler to fill EDM missing ET objects into our mithep::Met data structure.
//
// Authors: C.Loizides
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERCALOMET_H
#define MITPROD_TREEFILLER_FILLERCALOMET_H

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/METReco/interface/CaloMETCollection.h"
#include "MitAna/DataUtil/interface/TreeWriter.h"
#include "MitAna/DataTree/interface/Collections.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"

namespace mithep 
{
  class FillerCaloMet : public BaseFiller
  {  
    public:
      FillerCaloMet(const edm::ParameterSet &cfg, const char *name, bool active=1);
      ~FillerCaloMet();

      void            BookDataBlock(TreeWriter &tws);
      void 	      FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
  
    private:
      std::string           edmName_;    //edm name of met collection
      std::string           mitName_;    //mit name of met collection
      mithep::CaloMetArr   *caloMets_;   //array of Mets
  };
}
#endif
