//--------------------------------------------------------------------------------------------------
// $Id: FillerNSVFit.h,v 1.7 2011/04/29 14:18:23 rwolf Exp $
//
// FillerNSVFit
//
// Implementation of a filler to fill nsv fit information for resonances that decay into tau leptons 
// (subsequenctly decaying into electrons, muons or hadrons) from EDM.
//
// Authors: R.Wolf
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERNSVFIT_H
#define MITPROD_TREEFILLER_FILLERNSVFIT_H

#include "MitAna/DataTree/interface/NSVFitFwd.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"

namespace mithep 
{
  class FillerNSVFit : public BaseFiller
  {  
    public:
      FillerNSVFit(const edm::ParameterSet &cfg, const char *name, bool active=1);
      ~FillerNSVFit();

      void                           BookDataBlock(TreeWriter &tws);
      void                           FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
  
    private:
      std::string                    edmName_;        // edm name of NSVFit collection
      std::string                    mitName_;        // mit name of NSVFit collection
      mithep::NSVFitArr              *hypotheses_;     // array of NSVFit hypotheses (can in principle be more than one)
  };
}
#endif
