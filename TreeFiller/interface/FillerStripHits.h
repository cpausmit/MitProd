//--------------------------------------------------------------------------------------------------
// $Id: FillerStripHits.h,v 1.1 2009/09/25 08:42:27 loizides Exp $
//
// FillerStripHits
//
// Implementation of a filler to fill strip hits into our mithep::StripHit data structure.
//
// Authors: C.Loizides
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERSTRIPHITS_H
#define MITPROD_TREEFILLER_FILLERSTRIPHITS_H

#include "MitAna/DataTree/interface/StripHitFwd.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"

class TrackerGeometry;

namespace mithep 
{
  class FillerStripHits : public BaseFiller
  {  
    public:
      FillerStripHits(const edm::ParameterSet &cfg, const char *name, bool active=1);
      ~FillerStripHits();

      void                                BookDataBlock(TreeWriter &tws, 
                                                        const edm::EventSetup &es);
      void                                FillDataBlock(const edm::Event &e, 
                                                        const edm::EventSetup &es);

    protected:

      std::string                         edmName_;    //edm name of pixel hits col
      std::string                         mitName_;    //mit name of pixel hits col
      mithep::StripHitArr                *shits_;      //array of strip hits
  };
}
#endif
