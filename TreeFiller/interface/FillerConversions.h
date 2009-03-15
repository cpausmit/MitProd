//--------------------------------------------------------------------------------------------------
// $Id: FillerConversions.h,v 1.7 2009/03/10 15:56:00 loizides Exp $
//
// FillerConversions
//
// Implementation of a filler creating mithep::Conversion objects from the reconstructed
// conversion electrons.
//
// Authors: J.Bendavid
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERCONVERSIONS_H
#define MITPROD_TREEFILLER_FILLERCONVERSIONS_H

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
      FillerConversions(const edm::ParameterSet &cfg, const char *name, bool active=1);
      ~FillerConversions();

      void                 BookDataBlock(TreeWriter &tws);
      void                 FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
  
    private:
      std::string                            edmName_;             //name of edm conversions
      std::string                            mitName_;             //mit name of Conversions
      std::string                            convElectronMapName_; //name of imported electrons map
      std::string                            conversionMapName_;   //name of exported conv map
      const mithep::ConversionElectronMap   *convElectronMap_;     //imported map wrt conv electrons
      mithep::ConversionArr                 *conversions_;         //array of Conversions
      mithep::ConversionMap                 *conversionMap_;       //exported map wrt Conversions
  };
}
#endif
