//--------------------------------------------------------------------------------------------------
// $Id: BaseFiller.h,v 1.1 2008/06/18 13:23:22 paus Exp $
//
// BaseFiller
//
// Base class to define the interface for a filler.
//
// Authors: C.Paus
//--------------------------------------------------------------------------------------------------

#ifndef TREEFILLER_BASEFILLER_H
#define TREEFILLER_BASEFILLER_H

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "MitAna/DataUtil/interface/TreeWriter.h"

namespace mithep 
{
  class BaseFiller
  {
    public:
      BaseFiller(const edm::ParameterSet&, const char *en, const char *mn);
      virtual ~BaseFiller() {}

      virtual bool Active()                                                 { return active_; }
      virtual void BookDataBlock(TreeWriter *tws)                           = 0;
      virtual void FillDataBlock(const edm::Event&, const edm::EventSetup&) = 0;
      virtual void ResolveLinks (const edm::Event&, const edm::EventSetup&) {}

    protected:
      const edm::ParameterSet            &config_;
      std::string                         edmName_;
      std::string                         mitName_;
      const bool                          active_;
  };
}
#endif
