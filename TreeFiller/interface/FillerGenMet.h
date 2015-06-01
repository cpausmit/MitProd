//--------------------------------------------------------------------------------------------------
// FillerGenMet
//
// Implementation of a filler to fill EDM missing ET objects into our mithep::Met data structure.
//
// Authors: C.Loizides
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERGENMET_H
#define MITPROD_TREEFILLER_FILLERGENMET_H

#include "MitAna/DataTree/interface/GenMetFwd.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"

#include "DataFormats/Common/interface/View.h"

namespace reco {
  class MET;
}

namespace mithep 
{
  class FillerGenMet : public BaseFiller
  {  
  public:
    FillerGenMet(edm::ParameterSet const&, edm::ConsumesCollector&, ObjectService*, char const*, bool = true);
    ~FillerGenMet();

    void BookDataBlock(TreeWriter &) override;
    void FillDataBlock(edm::Event const&, edm::EventSetup const&) override;

    typedef edm::View<reco::MET> METView;
  
  private:
    edm::EDGetTokenT<METView> edmToken_;    //edm name of met collection
    std::string               mitName_;     //mit name of met collection
    bool                      fillFromPAT_; //true if filling from miniAOD
    mithep::GenMetArr*        genMets_;     //array of Mets
  };
}
#endif
