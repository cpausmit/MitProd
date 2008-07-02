//--------------------------------------------------------------------------------------------------
// $Id: FillerGlobalMuons.h,v 1.3 2008/06/18 19:17:21 loizides Exp $
//
// FillerConversions
//
// Imlementation of a filler to fill tracks into our data structure.
//
// Authors: J. Bendavid
//--------------------------------------------------------------------------------------------------

#ifndef TREEFILLER_FILLERCONVERSIONS_H
#define TREEFILLER_FILLERCONVERSIONS_H

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "MitAna/DataUtil/interface/TreeWriter.h"
#include "DataFormats/Common/interface/Handle.h"
#include "MitAna/DataTree/interface/Track.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "MitAna/DataTree/interface/Collections.h"
#include "MitAna/DataTree/interface/Array.h"
#include "MitProd/TreeService/interface/TreeService.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"
#include "MitProd/TreeFiller/interface/AssociationMap.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"

using namespace std;
using namespace mithep;



namespace mithep 
{
  class FillerConversions : public BaseFiller
  {  
    public:
      FillerConversions(const edm::ParameterSet&, bool active, const ConversionElectronMap* conversionElectronMap=0);
      ~FillerConversions();

      void                                    BookDataBlock(TreeWriter &tws);
      void                                    FillDataBlock(const edm::Event&, const edm::EventSetup&);
      const ConversionMap*                    GetConversionMap() { return conversionMap_; }
  
    private:
      std::string                             edmName_;
      std::string                             edmDataName_;
      std::string                             mitName_;
      const mithep::ConversionElectronMap*    conversionElectronMap_;
      mithep::ConversionArr                  *conversions_;
      mithep::ConversionMap*                  conversionMap_;
      edm::Handle<reco::ConversionCollection> conversionProduct_;
  };
}
#endif
