//--------------------------------------------------------------------------------------------------
// $Id: FillerMet.h,v 1.4 2009/06/15 15:00:25 loizides Exp $
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

namespace mithep 
{
  class FillerMet : public BaseFiller
  {  
    public:
      FillerMet(const edm::ParameterSet &cfg, const char *name, bool active=1);
      ~FillerMet();

      void                  BookDataBlock(TreeWriter &tws, const edm::EventSetup &es);
      void 	            FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
  
    private:
      std::string           edmName_;    //edm name of met collection
      std::string           mitName_;    //mit name of Mets
      mithep::MetArr       *mets_;       //array of Mets
  };
}
#endif
