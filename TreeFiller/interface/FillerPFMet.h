//--------------------------------------------------------------------------------------------------
// FillerPFMet
//
// Implementation of a filler to fill EDM missing ET objects into our mithep::PFMet data structure.
//
// Authors: C.Loizides, Y.Iiyama
//--------------------------------------------------------------------------------------------------

// reverse header protection - this file should be included only by FillerPFMet.cc
#ifdef MITPROD_TREEFILLER_FILLERPFMET

#include "MitAna/DataTree/interface/PFMetFwd.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"

namespace mithep 
{
  template<class MET>
  class FillerPFMet : public BaseFiller {  
  public:
    FillerPFMet(edm::ParameterSet const&, edm::ConsumesCollector&, ObjectService*, char const* name, bool active = true);
    ~FillerPFMet();

    void BookDataBlock(TreeWriter&) override;
    void FillDataBlock(edm::Event const&, edm::EventSetup const&) override;

    typedef std::vector<MET> MetCollection;
  
  private:
    mithep::PFMet* FillBasicData(MET const*);

    edm::EDGetTokenT<MetCollection> edmToken_;       //edm name of met collection
    edm::EDGetTokenT<MET>           edmSingleToken_; //edm name of met collection
    std::string                     mitName_;        //mit name of met collection
    mithep::PFMetArr*               pfMets_;         //array of Mets
  };
}
#endif
