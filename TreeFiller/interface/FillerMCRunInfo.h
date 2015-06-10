//--------------------------------------------------------------------------------------------------
// FillerMCRunInfo
//
// Fill MC run info from GenRunInfoProduct and LHERunInfoProduct (if the input MC is based on LHE)
//
// Authors: Y.Iiyama
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERMCRUNINFO_H
#define MITPROD_TREEFILLER_FILLERMCRUNINFO_H

#include "MitProd/TreeFiller/interface/BaseFiller.h"

#include "SimDataFormats/GeneratorProducts/interface/GenRunInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/LHERunInfoProduct.h"

namespace mithep 
{
  class MCRunInfo;

  class FillerMCRunInfo : public BaseFiller
  {
  public:
    FillerMCRunInfo(edm::ParameterSet const&, edm::ConsumesCollector&, mithep::ObjectService*, char const* = "MCRunInfo", bool = true);
    ~FillerMCRunInfo();

    void BookDataBlock(mithep::TreeWriter&) override;
    void FillDataBlock(edm::Event const&, edm::EventSetup const&) override {}
    void FillPostRunBlock(edm::Run const&, edm::EventSetup const&) override;

  private:
    void setWeightGroups(std::vector<std::string> const&);

    edm::EDGetTokenT<GenRunInfoProduct> genRunInfoToken_;
    edm::EDGetTokenT<LHERunInfoProduct> lheRunInfoToken_;

    bool const fillFullInfo_;

    mithep::MCRunInfo* mcRunInfo_;
  };
}
#endif
