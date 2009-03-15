//--------------------------------------------------------------------------------------------------
// $Id: FillerMet.h,v 1.1 2009/03/12 16:00:23 bendavid Exp $
//
// FillerMet
//
// Implementation of a filler to fill EDM missing ET objects into our mithep::Met data structure.
//
// Authors: C.Loizides
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERMET_H
#define MITPROD_TREEFILLER_FILLERMET_H

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/METReco/interface/METCollection.h"
#include "MitAna/DataUtil/interface/TreeWriter.h"
#include "MitAna/DataTree/interface/Collections.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"

namespace mithep 
{
  class FillerMet : public BaseFiller
  {  
    public:
      FillerMet(const edm::ParameterSet &cfg, const char *name, bool active=1);
      ~FillerMet();

      void            BookDataBlock(TreeWriter &tws);
      void 	      FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
  
    private:
      std::string           edmName_;    //edm name of met collection
      std::string           mitName_;    //mit name of Mets
      mithep::MetArr       *mets_;       //array of Mets
  };
}
#endif
