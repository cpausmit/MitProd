//--------------------------------------------------------------------------------------------------
// $Id: BaseFiller.h,v 1.3 2008/06/19 16:53:43 loizides Exp $
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
      BaseFiller(const edm::ParameterSet&, const char *name, bool active=true);
      virtual ~BaseFiller() {}

      bool                     Active() const { return active_; }
      virtual void             BookDataBlock(TreeWriter &tws)                           = 0;
      virtual void             FillDataBlock(const edm::Event&, const edm::EventSetup&) = 0;
      const std::string       &Name()   const { return name_; }
      virtual void             ResolveLinks(const edm::Event&, const edm::EventSetup&)  {}

    protected:
      const std::string        name_;
      const edm::ParameterSet  config_;
      const bool               active_;

      const edm::ParameterSet &Conf()   const { return config_; }
  };
}
#endif
