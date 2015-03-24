//--------------------------------------------------------------------------------------------------
// $Id: FillerMet.h,v 1.6 2010/03/18 20:21:00 bendavid Exp $
//
// FillerMet
//
// Implementation of a filler to fill EDM missing ET objects into our mithep::PileupInfo data structure.
//
// Authors: C.Loizides
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERPILEUPINFO_H
#define MITPROD_TREEFILLER_FILLERPILEUPINFO_H

#include "MitAna/DataTree/interface/PileupInfoFwd.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"

#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"

namespace mithep 
{
  class FillerPileupInfo : public BaseFiller
  {  
    public:
      FillerPileupInfo(const edm::ParameterSet &cfg, edm::ConsumesCollector&, ObjectService*, const char *name, bool active=1);
      ~FillerPileupInfo();

      void                  BookDataBlock(TreeWriter &tws);
      void 	            FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
  
    private:
      edm::EDGetTokenT<std::vector<PileupSummaryInfo> > edmToken_;    //edm name of met collection
      edm::EDGetTokenT<PileupSummaryInfo> edmSingleToken_;    //edm name of met collection
      std::string                 mitName_;    //mit name of PileupInfos
      mithep::PileupInfoArr       *puInfos_;       //array of PileupInfos
  };
}
#endif
