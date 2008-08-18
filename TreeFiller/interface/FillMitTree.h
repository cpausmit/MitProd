//--------------------------------------------------------------------------------------------------
// $Id: FillMitTree.h,v 1.7 2008/07/30 16:39:57 loizides Exp $
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
#include "MitProd/ObjectService/interface/ObjectService.h"

namespace mithep 
{
  class BaseFiller;

  class FillMitTree : public edm::EDAnalyzer
  {
    public:
      FillMitTree(const edm::ParameterSet &cfg);
      ~FillMitTree();

      void analyze (const edm::Event &e, const edm::EventSetup &es);
      void beginJob(const edm::EventSetup &es);
      void endJob();

      static ObjectService *os() { return os_; }

    protected:
      bool addActiveFiller(BaseFiller *bf);
      bool configure(const edm::ParameterSet &cfg);

      std::vector<BaseFiller*> fillers_;       //list of active fillers
      bool                     defactive_;     //default activity flag for fillers
      static ObjectService    *os_;            //object service (set in beginJob)
  };
}
#endif