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
#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/EDGetToken.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "MitAna/DataUtil/interface/TreeWriter.h"
#include <TString.h>

#include <typeinfo>

namespace mithep 
{
  class BranchTable;
  class ObjectService;

  class BaseFiller
  {
    public:
      BaseFiller(const edm::ParameterSet &cfg, ObjectService*, const char *name, bool active=true);
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
      template<typename TYPE>
      edm::EDGetTokenT<TYPE>   GetToken(edm::ConsumesCollector&, std::string const& paramName, std::string const& defVal = "", bool mayConsume = false);
      void                     PrintErrorAndExit(const char *msg) const;
      template <typename TYPE>
      void                     GetProduct(const edm::EDGetTokenT<TYPE>&, edm::Handle<TYPE> &handle,
                                          const edm::Event &event) const;    
      template <typename TYPE>
      bool                     GetProductSafe(const edm::EDGetTokenT<TYPE>&, edm::Handle<TYPE> &handle,
                                              const edm::Event &event) const;    
      ObjectService           *OS() { return os_; }

      const std::string        name_;    //name of this filler
      const std::string        brtname_; //name of branch table (def = BranchTable)
      const edm::ParameterSet  config_;  //parameter set for this filler
      const bool               active_;  //=1 if active
      const bool               verify_;  //=1 if verificatin code is active
      const int                verbose_; //verbosity level (do not introduce more than 0-4 levels)
      BranchTable             *brtable_; //branch dependency table
      ObjectService           *os_;      //object repository passed by the EDAnalyzer
  };
}

template<typename TYPE>
edm::EDGetTokenT<TYPE>
mithep::BaseFiller::GetToken(edm::ConsumesCollector& collector, std::string const& paramName, std::string const& defVal, bool mayConsume)
{
  std::string paramString;
  if(defVal.empty())
    paramString = Conf().getUntrackedParameter<string>(paramName);
  else
    paramString = Conf().getUntrackedParameter<string>(paramName, defVal);

  if(!paramString.empty()){
    edm::InputTag tag(paramString);
    if(mayConsume)
      return collector.mayConsume<TYPE>(tag);
    else
      return collector.consumes<TYPE>(tag);
  }
  else
    return edm::EDGetTokenT<TYPE>();
}

//--------------------------------------------------------------------------------------------------
template <typename TYPE>
inline void mithep::BaseFiller::GetProduct(const edm::EDGetTokenT<TYPE>& token, edm::Handle<TYPE> &handle,
                                           const edm::Event &event) const
{
  // Try to access data collection from EDM file. We check if we really get just one
  // product with the given name. If not we print an error and exit.

  try {
    event.getByToken(token,handle);
    if (!handle.product()) // throws here if handle is not valid
      throw edm::Exception(edm::errors::Configuration, "BaseFiller::GetProduct()\n")
        << "Cannot get " << typeid(TYPE).name() << " for " << Name(); // there should be a better way to get object info..
  } catch (std::exception& e) {
    edm::LogError("BaseFiller") << e.what();
    PrintErrorAndExit(e.what());
  }
}

//--------------------------------------------------------------------------------------------------
template <typename TYPE>
inline bool mithep::BaseFiller::GetProductSafe(const edm::EDGetTokenT<TYPE>& token, edm::Handle<TYPE> &handle,
                                               const edm::Event &event) const
{
  // Try to safely access data collection from EDM file. We check if we really get just one
  // product with the given name. If not, we return false.

  try {
    event.getByToken(token,handle);
    if (!handle.isValid()) 
      return false;
  } catch (...) {
    return false;
  }
  return true;
}
#endif
