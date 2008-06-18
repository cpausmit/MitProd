//--------------------------------------------------------------------------------------------------
// $Id: FillMitTree.h,v 1.1 2008/06/18 13:23:22 paus Exp $
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

#ifndef TREEFILLER_FILLMITTREE_H
#define TREEFILLER_FILLMITTREE_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"

namespace mithep 
{
  class FillMitTree : public edm::EDAnalyzer
  {
    public:
      FillMitTree(const edm::ParameterSet&);
      ~FillMitTree();

      void analyze (const edm::Event&, const edm::EventSetup&);
      void beginJob(edm::EventSetup const&);
      void endJob  ();
  
    private:
      std::vector<BaseFiller*>  fillers_;       // list of our fillers and potential fillers
  };
}
#endif
