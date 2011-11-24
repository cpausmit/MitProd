//--------------------------------------------------------------------------------------------------
// $Id: FillerMCEventInfo.h,v 1.11 2010/03/18 20:21:00 bendavid Exp $
//
// FillerMCEventInfo
//
// Implementation of a filler that stores MC related quantities, like the pdf info, in the
// bambu records.
//
// Authors: C.Loizides
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLEREMBEDWEIGHT_H
#define MITPROD_TREEFILLER_FILLEREMBEDWEIGHT_H
 
#include "MitAna/DataTree/interface/EmbedWeightFwd.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"

namespace mithep 
{
  class FillerEmbedWeight : public BaseFiller
  {
    public:
      FillerEmbedWeight(const edm::ParameterSet &cfg, 
                        const char *name="EmbedWeight", bool active=1);
      ~FillerEmbedWeight();

      void                     BookDataBlock(TreeWriter &tws);
      void                     FillDataBlock(const edm::Event &e, const edm::EventSetup &es);

    private:
      std::string              edmName_;       //edmname
      std::string              mitName_;       //mitname
      EmbedWeightArr           *embedWeight_;          //event info
  };
}
#endif
