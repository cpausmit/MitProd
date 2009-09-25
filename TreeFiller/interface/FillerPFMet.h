//--------------------------------------------------------------------------------------------------
// $Id: FillerPFMet.h,v 1.4 2009/06/15 15:00:25 loizides Exp $
//
// FillerPFMet
//
// Implementation of a filler to fill EDM missing ET objects into our mithep::PFMet data structure.
//
// Authors: C.Loizides
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERPFMET_H
#define MITPROD_TREEFILLER_FILLERPFMET_H

#include "MitAna/DataTree/interface/PFMetFwd.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"

namespace mithep 
{
  class FillerPFMet : public BaseFiller
  {  
    public:
      FillerPFMet(const edm::ParameterSet &cfg, const char *name, bool active=1);
      ~FillerPFMet();

      void                  BookDataBlock(TreeWriter &tws, const edm::EventSetup &es);
      void                  FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
  
    private:
      std::string           edmName_;    //edm name of met collection
      std::string           mitName_;    //mit name of met collection
      mithep::PFMetArr     *pfMets_;     //array of Mets
  };
}
#endif
