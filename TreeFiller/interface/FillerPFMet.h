//--------------------------------------------------------------------------------------------------
// $Id: FillerPFMet.h,v 1.6 2010/03/18 20:21:00 bendavid Exp $
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

#include "DataFormats/METReco/interface/PFMETCollection.h"
#include "DataFormats/METReco/interface/PFMET.h"

namespace mithep 
{
  class FillerPFMet : public BaseFiller
  {  
    public:
      FillerPFMet(const edm::ParameterSet &cfg, edm::ConsumesCollector& collector, const char *name, bool active=1);
      ~FillerPFMet();

      void                  BookDataBlock(TreeWriter &tws);
      void                  FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
  
    private:
      edm::EDGetTokenT<reco::PFMETCollection> edmToken_;    //edm name of met collection
      edm::EDGetTokenT<reco::PFMET> edmSingleToken_;    //edm name of met collection
      std::string           mitName_;    //mit name of met collection
      mithep::PFMetArr     *pfMets_;     //array of Mets
  };
}
#endif
