//--------------------------------------------------------------------------------------------------
// $Id: BaseFiller.h,v 1.10 2008/07/30 08:39:50 loizides Exp $
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
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/Common/interface/Handle.h"
#include "MitAna/DataUtil/interface/TreeWriter.h"
#include "MitProd/ObjectService/interface/ObjectService.h"
#include "MitProd/TreeFiller/interface/FillMitTree.h"
#include <TString.h>

namespace mithep 
{
  class BaseFiller
  {
    public:
      BaseFiller(const edm::ParameterSet &cfg, const char *name, bool active=true);
      virtual ~BaseFiller() {}

      bool                     Active() const { return active_; }
      virtual void             BookDataBlock(TreeWriter &tws)                                = 0;
      virtual void             FillDataBlock(const edm::Event &e, const edm::EventSetup &es) = 0;
      const std::string       &Name()   const { return name_; }
      virtual void             ResolveLinks(const edm::Event &e, const edm::EventSetup &es)    {}

    protected:
      const edm::ParameterSet &Conf()   const { return config_; }
      void                     PrintErrorAndExit(const char *msg) const;
      template <typename TYPE>
      void                     GetProduct(const std::string name, edm::Handle<TYPE> &prod,
                                          const edm::Event &event) const;    
      template <typename TYPE>
      bool                     GetProductSafe(const std::string name, edm::Handle<TYPE> &prod,
                                              const edm::Event &event) const;    

      ObjectService           *OS() { return FillMitTree::os(); }

      const std::string        name_;    //name of this filler
      const edm::ParameterSet  config_;  //parameter set for this filler
      const bool               active_;  //=1 if active
  };
}

//--------------------------------------------------------------------------------------------------
template <typename TYPE>
inline void mithep::BaseFiller::GetProduct(const std::string edmname, edm::Handle<TYPE> &prod,
                                           const edm::Event &event) const
{
  // Try to access data collection from EDM file. We check if we really get just one
  // product with the given name. If not we print an error and exit.

  try {
    event.getByLabel(edm::InputTag(edmname),prod);
    if (!prod.isValid()) 
      throw edm::Exception(edm::errors::Configuration, "BaseFiller::GetProduct()\n")
        << "Cannot get collection with label " << edmname << std::endl;
  } catch (...) {
    edm::LogError("BaseFiller") << "Cannot get collection with label "
                                << edmname << std::endl;
    PrintErrorAndExit(Form("Cannot get collection with label %s", edmname.c_str()));
  }
}

//--------------------------------------------------------------------------------------------------
template <typename TYPE>
inline bool mithep::BaseFiller::GetProductSafe(const std::string edmname, edm::Handle<TYPE> &prod,
                                               const edm::Event &event) const
{
  // Try to safely access data collection from EDM file. We check if we really get just one
  // product with the given name. If not, we return false.

  try {
    event.getByLabel(edm::InputTag(edmname),prod);
    if (!prod.isValid()) 
      return false;
  } catch (...) {
    return false;
  }
  return true;
}
#endif
