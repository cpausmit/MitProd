//--------------------------------------------------------------------------------------------------
// $Id: FillerCaloMet.h,v 1.2 2008/07/08 12:38:20 loizides Exp $
//
// FillerCaloMet
//
// Imlementation of a filler to fill EDM missing ET objects into our mithep::Met data structure.
//
// Authors: C.Loizides
//--------------------------------------------------------------------------------------------------

#ifndef TREEFILLER_FILLERCALOMET_H
#define TREEFILLER_FILLERCALOMET_H

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
      std::string           mitName_;    //name of Mets in OAK
      mithep::MetArr       *mets_;       //array of Mets
  };
}
#endif