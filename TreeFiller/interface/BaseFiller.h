//--------------------------------------------------------------------------------------------------
// BaseFiller
//
// Base class to define the interface for a filler.
//
// Authors: C.Paus
//--------------------------------------------------------------------------------------------------
#ifndef MITPROD_TREEFILLER_BASEFILLER_H
#define MITPROD_TREEFILLER_BASEFILLER_H

#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "MitAna/DataUtil/interface/TreeWriter.h"
#include <TString.h>

namespace mithep 
{
  class BranchTable;
  class ObjectService;

  class BaseFiller
  {
    public:
      BaseFiller(const edm::ParameterSet &cfg, const char *name, bool active=true);
      virtual ~BaseFiller() {}

      bool                     Active()  const { return active_;       }
      void                     AddBranchDep(const char *n, const char *d);
      void                     AddBranchDep(const std::string &n, const char *d)
                                 { AddBranchDep(n.c_str(), d);         }
      void                     AddBranchDep(const std::string &n, const std::string &d)
                                 { AddBranchDep(n.c_str(), d.c_str()); }
      virtual void             BookDataBlock(TreeWriter &tws)     = 0;
      virtual void             FillDataBlock(const edm::Event &e, const edm::EventSetup &es) = 0;
      virtual void             FillRunBlock(edm::Run const &r, edm::EventSetup const &es) {}
      const std::string       &Name()    const { return name_;         }
      virtual void             ResolveLinks(const edm::Event &e, const edm::EventSetup &es)    {}
      int                      Verbose() const { return verbose_;      }
      bool                     Verify()  const { return verify_;       }

    protected:
      const edm::ParameterSet &Conf()    const { return config_;       }
      void                     PrintErrorAndExit(const char *msg) const;
      template <typename TYPE>
      void                     GetProduct(const std::string name, edm::Handle<TYPE> &prod,
                                          const edm::Event &event) const;    
      template <typename TYPE>
      bool                     GetProductSafe(const std::string name, edm::Handle<TYPE> &prod,
                                              const edm::Event &event) const;    
      ObjectService           *OS();

      const std::string        name_;    //name of this filler
      const std::string        brtname_; //name of branch table (def = BranchTable)
      const edm::ParameterSet  config_;  //parameter set for this filler
      const bool               active_;  //=1 if active
      const bool               verify_;  //=1 if verificatin code is active
      const int                verbose_; //verbosity level (do not introduce more than 0-4 levels)
      BranchTable             *brtable_; //branch dependency table
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
        << "Cannot get collection with label " << edmname << " for " << Name() <<  std::endl;
  } catch (...) {
    edm::LogError("BaseFiller") << "Cannot get collection with label "
                                << edmname << " for " << Name() << std::endl;
    PrintErrorAndExit(Form("Cannot get collection with label %s for %s", 
                           edmname.c_str(), name_.c_str()));
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
