//--------------------------------------------------------------------------------------------------
// $Id: FillerPixelHits.h,v 1.1 2009/09/25 08:42:27 loizides Exp $
//
// FillerPixelHits
//
// Implementation of a filler to fill pixel hits into our mithep::PixelHit data structure.
//
// Authors: C.Loizides
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERPIXELHITS_H
#define MITPROD_TREEFILLER_FILLERPIXELHITS_H

#include "MitAna/DataTree/interface/PixelHitFwd.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"

class TrackerGeometry;

namespace mithep 
{
  class FillerPixelHits : public BaseFiller
  {  
    public:
      FillerPixelHits(const edm::ParameterSet &cfg, const char *name, bool active=1);
      ~FillerPixelHits();

      void                                BookDataBlock(TreeWriter &tws, 
                                                        const edm::EventSetup &es);
      void                                FillDataBlock(const edm::Event &e, 
                                                        const edm::EventSetup &es);

    protected:

      std::string                         edmName_;    //edm name of pixel hits col
      std::string                         mitName_;    //mit name of pixel hits col
      mithep::PixelHitArr                *phits_;      //array of pixel hits
  };
}
#endif
