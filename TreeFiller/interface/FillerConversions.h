//--------------------------------------------------------------------------------------------------
// $Id: FillerConversions.h,v 1.2 2008/07/03 07:56:14 loizides Exp $
//
// FillerConversions
//
// Todo
//
// Authors: J. Bendavid
//--------------------------------------------------------------------------------------------------

#ifndef TREEFILLER_FILLERCONVERSIONS_H
#define TREEFILLER_FILLERCONVERSIONS_H

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "MitAna/DataUtil/interface/TreeWriter.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "MitAna/DataTree/interface/Collections.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"

namespace mithep 
{
  class FillerConversions : public BaseFiller
  {  
    public:
      FillerConversions(const edm::ParameterSet &cfg, bool active=1, 
                        const ConversionElectronMap *conversionElectronMap=0);
      ~FillerConversions();

      void                 BookDataBlock(TreeWriter &tws);
      void                 FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
      const ConversionMap *GetConversionMap() const { return conversionMap_; }
  
    private:
      std::string                              edmName_;
      std::string                              mitName_;
      const mithep::ConversionElectronMap*     conversionElectronMap_;
      mithep::ConversionArr                   *conversions_;
      mithep::ConversionMap                   *conversionMap_;
      edm::Handle<reco::ConversionCollection>  conversionProduct_;
  };
}
#endif
