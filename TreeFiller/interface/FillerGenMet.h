//--------------------------------------------------------------------------------------------------
// $Id: FillerGenMet.h,v 1.10 2009/06/15 15:00:24 loizides Exp $
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
#include "MitProd/TreeFiller/interface/AssociationMaps.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"

namespace mithep 
{
  class FillerGenMet : public BaseFiller
  {  
    public:
      FillerGenMet(const edm::ParameterSet &cfg, const char *name, bool active=1);
      ~FillerGenMet();

      void                  BookDataBlock(TreeWriter &tws);
      void 	            FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
  
    private:
      std::string           edmName_;    //edm name of met collection
      std::string           mitName_;    //mit name of met collection
      mithep::GenMetArr    *genMets_;    //array of Mets
  };
}
#endif
