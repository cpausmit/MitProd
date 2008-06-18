//--------------------------------------------------------------------------------------------------
// $Id: $
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
      BaseFiller(const edm::ParameterSet&);
      virtual ~BaseFiller() {}

      virtual void  BookDataBlock(TreeWriter *tws)                           = 0;
      virtual void  FillDataBlock(const edm::Event&, const edm::EventSetup&) = 0;
      virtual void  ResolveLinks (const edm::Event&, const edm::EventSetup&) {}

    protected:
      const edm::ParameterSet *config_;
      std::string              edmName_;
      std::string              mitName_;
  };
}
#endif
