//--------------------------------------------------------------------------------------------------
// $Id: FillerMCEventInfo.h,v 1.10 2009/09/25 08:42:50 loizides Exp $
//
// FillerMCEventInfo
//
// Implementation of a filler that stores MC related quantities, like the pdf info, in the
// bambu records.
//
// Authors: C.Loizides
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERMCEVENTINFO_H
#define MITPROD_TREEFILLER_FILLERMCEVENTINFO_H

#include "MitProd/TreeFiller/interface/BaseFiller.h"

#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"

namespace mithep 
{
  class MCEventInfo;

  class FillerMCEventInfo : public BaseFiller
  {
    public:
      FillerMCEventInfo(const edm::ParameterSet &cfg, 
                        edm::ConsumesCollector&,
                        ObjectService*,
                        const char *name="MCEventInfo", bool active=1);
      ~FillerMCEventInfo();

      void                     BookDataBlock(TreeWriter &tws);
      void                     FillDataBlock(const edm::Event &e, const edm::EventSetup &es);

    private:
      std::string              evtName_;              //mit event branch name
      edm::EDGetTokenT<edm::HepMCProduct> genHepMCEvToken_;       //hepmc branch name (if present)
      edm::EDGetTokenT<GenEventInfoProduct> genEvtInfoToken_;       //edm event info name 
      bool                     flavorHistoryActive_;  //=true if flavor history is filled
      edm::EDGetTokenT<unsigned int> flavorHistToken_;       //edm flavor history name
      MCEventInfo             *eventInfo_;            //event info
  };
}
#endif
