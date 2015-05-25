//--------------------------------------------------------------------------------------------------
// FillMitTree
//
// This is the one Edm Module which we use to perform all our actions to properly fill the MitTree.
// The general function is that this module handles a list of generic fillers, which are called in
// the appropriate places of the Edm Module. We do make sure that first the information is
// completely stored and later the necessary links will get resolved.
//
// Authors: C.Paus
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLMITTREE_H
#define MITPROD_TREEFILLER_FILLMITTREE_H

#include "FWCore/Framework/interface/EDAnalyzer.h"

namespace mithep 
{
  class BranchTable;
  class BaseFiller;
  class TreeWriter;
  class ObjectService;
  class LAHeader;

  class FillMitTree : public edm::EDAnalyzer
  {
  public:
    FillMitTree(edm::ParameterSet const&);
    ~FillMitTree();

    void beginRun(edm::Run const&, edm::EventSetup const&) override;
    void endRun(edm::Run const&, edm::EventSetup const&) override;
    void analyze(edm::Event const&, edm::EventSetup const&) override;
    void beginJob() override;
    void endJob() override;

    ObjectService *os() { return os_; }

  protected:
    bool addActiveFiller(BaseFiller*);
    bool configure(edm::ParameterSet const&);
    bool configureTreeWriter(edm::ParameterSet const&);

    std::vector<BaseFiller*> fillers_;       //list of active fillers
    bool                     defactive_;     //default activity flag for fillers
    BranchTable             *brtable_;       //branch dependency table
    LAHeader                *evtLAHeader_;  //look-ahead event header
    int                      acfnumber_;     //keep track of active file number
    TreeWriter              *tws_;           //pointer to tree writer
    ObjectService           *os_;            //object service (set in beginJob)
  };
}
#endif
