//--------------------------------------------------------------------------------------------------
// FillerMCEventInfo
//
// Implementation of a filler that stores MC related quantities, like the pdf info, in the
// bambu records.
//
// Authors: C.Loizides, Y.Iiyama
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERMCEVENTINFO_H
#define MITPROD_TREEFILLER_FILLERMCEVENTINFO_H

#include "MitProd/TreeFiller/interface/BaseFiller.h"

#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/LHERunInfoProduct.h"

namespace mithep 
{
  class MCEventInfo;
  class MCRunInfo;

  class FillerMCEventInfo : public BaseFiller {
  public:
    FillerMCEventInfo(edm::ParameterSet const&, edm::ConsumesCollector&, mithep::ObjectService*, char const* = "MCEventInfo", bool = true);
    ~FillerMCEventInfo();

    void BookDataBlock(mithep::TreeWriter&) override;
    void FillDataBlock(edm::Event const&, edm::EventSetup const&) override;
    void FillPostRunBlock(edm::Run const&, edm::EventSetup const&) override;

  private:
    void setWeightGroups(std::vector<std::string> const&);

    std::string evtName_; //mit event branch name
    std::string runName_; //mit event branch name

    edm::EDGetTokenT<edm::HepMCProduct> genHepMCEvToken_; //hepmc branch name (if present)
    edm::EDGetTokenT<GenEventInfoProduct> genEvtInfoToken_; //edm event info name 
    edm::EDGetTokenT<LHEEventProduct> lheEventToken_;
    edm::EDGetTokenT<LHERunInfoProduct> lheRunInfoToken_;
    edm::EDGetTokenT<unsigned> flavorHistToken_;       //edm flavor history name

    bool flavorHistoryActive_;  //=true if flavor history is filled

    mithep::MCEventInfo* eventInfo_;            //event info
    mithep::MCRunInfo* runInfo_;

    std::map<TString, unsigned> weightIds_;
  };
}
#endif
