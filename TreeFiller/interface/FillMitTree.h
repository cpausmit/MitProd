//--------------------------------------------------------------------------------------------------
// $Id: FillMitTree.h,v 1.17 2009/12/12 22:33:46 bendavid Exp $
//
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

  class FillMitTree : public edm::EDAnalyzer
  {
    public:
      FillMitTree(const edm::ParameterSet &cfg);
      ~FillMitTree();

      void                     beginRun(edm::Run const &, edm::EventSetup const&);
      void                     analyze(const edm::Event &e, const edm::EventSetup &es);
      void                     beginJob();
      void                     endJob();

      static ObjectService    *os() { return os_; }

    protected:
      bool                     addActiveFiller(BaseFiller *bf);
      bool                     configure(const edm::ParameterSet &cfg);
      bool                     configureTreeWriter(const edm::ParameterSet &cfg);

      std::vector<BaseFiller*> fillers_;       //list of active fillers
      bool                     defactive_;     //default activity flag for fillers
      BranchTable             *brtable_;       //branch dependency table
      int                      acfnumber_;     //keep track of active file number
      TreeWriter              *tws_;           //pointer to tree writer
      static ObjectService    *os_;            //object service (set in beginJob)
  };
}
#endif
