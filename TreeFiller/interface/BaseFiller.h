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

#include "tbb/concurrent_unordered_map.h"

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

      std::string const& Name() const    { return name_; }
      bool               Active()  const { return active_; }
      int                Verbose() const { return verbose_; }

      virtual void BookDataBlock(TreeWriter &tws)     = 0;
      virtual void PrepareLinks() {}
      virtual void FillDataBlock(edm::Event const&, edm::EventSetup const&) = 0;
      virtual void FillRunBlock(edm::Run const&, edm::EventSetup const&) {}
      virtual void FillPostRunBlock(edm::Run const&, edm::EventSetup const&) {}
      virtual void ResolveLinks(edm::Event const&, edm::EventSetup const&) {}

    protected:
      void AddBranchDep(std::string const& name, std::string const& dep);
      void PrintErrorAndExit(std::string const& msg) const;
      ObjectService* OS() { return os_; }

      template<class Product, edm::BranchType B = edm::InEvent>
      edm::EDGetTokenT<Product> GetToken(edm::ConsumesCollector&, edm::ParameterSet const&, std::string const& paramName, bool required = true);
      template <class Product, class Source = edm::Event>
      void                      GetProduct(edm::EDGetTokenT<Product> const&, edm::Handle<Product>&, Source const&) const;
      template <class Product, class Source = edm::Event>
      bool                      GetProductSafe(edm::EDGetTokenT<Product> const&, edm::Handle<Product>&, Source const&) const;

      std::string               name_ = "";               //name of this filler
      std::string               brtname_ = "BranchTable"; //name of branch table
      bool                      active_ = true;
      int                       verbose_ = 0; //verbosity level (do not introduce more than 0-4 levels)
      BranchTable*              brtable_ = 0; //branch dependency table
      ObjectService*            os_ = 0;      //object repository passed by the EDAnalyzer
  };

}

template<class Product, edm::BranchType B/* = edm::InEvent*/>
edm::EDGetTokenT<Product>
mithep::BaseFiller::GetToken(edm::ConsumesCollector& collector, edm::ParameterSet const& cfg, std::string const& paramName, bool required/* = true*/)
{
  std::string paramString;

  if (required)
    paramString = cfg.getUntrackedParameter<string>(paramName);
  else
    paramString = cfg.getUntrackedParameter<string>(paramName, "");

  if (paramString.empty()) {
    if (required)
      throw edm::Exception(edm::errors::Configuration, name_ + "::GetToken")
        << "Required product name empty";
    else
      return edm::EDGetTokenT<Product>();
  }
  else {
    edm::InputTag tag(paramString);
    if (required)
      return collector.consumes<Product, B>(tag);
    else
      return collector.mayConsume<Product, B>(tag);
  }
}

//--------------------------------------------------------------------------------------------------
template <class Product, class Source/* = edm::Event*/>
inline
void
mithep::BaseFiller::GetProduct(edm::EDGetTokenT<Product> const& token, edm::Handle<Product>& handle,
                               Source const& source) const
{
  // Try to access data collection from EDM file. We check if we really get just one
  // product with the given name. If not we print an error and exit.

  try {
    source.getByToken(token,handle);
    if (!handle.product()) // throws here if handle is not valid
      throw edm::Exception(edm::errors::Configuration, name_ + "::GetProduct()\n")
        << "Cannot get " << typeid(Product).name() << " for " << Name(); // there should be a better way to get object info..
  } catch (std::exception& e) {
    edm::LogError(name_) << e.what();
    PrintErrorAndExit(e.what());
  }
}

//--------------------------------------------------------------------------------------------------
template <class Product, class Source/* = edm::Event*/>
inline
bool
mithep::BaseFiller::GetProductSafe(edm::EDGetTokenT<Product> const& token, edm::Handle<Product>& handle,
                                   Source const& source) const
{
  // Try to safely access data collection from EDM file. We check if we really get just one
  // product with the given name. If not, we return false.

  try {
    source.getByToken(token, handle);
    if (!handle.isValid()) 
      return false;
  }
  catch (...) {
    return false;
  }

  return true;
}

//--------------------------------------------------------------------------------------------------
// FillerFactory: a trick to register individual fillers as "plugin modules"
//--------------------------------------------------------------------------------------------------

namespace mithep {
  // We call a function that takes a ParameterSet, a ConsumesCollector, an ObjectService, a name string, and a boolean and returns
  // a BaseFiller pointer as FillerFactory
  typedef BaseFiller* (*FillerFactory)(edm::ParameterSet const&, edm::ConsumesCollector&, mithep::ObjectService*, char const*, bool);

  // A singleton class to store information of the filler plugins
  class FillerFactoryStore {
  public:
    // A utility class whose instantiation triggers the registration of a filler plugin
    template<class Filler>
    struct Registration {
      Registration(char const* _name)
      {
        FillerFactoryStore::singleton()->registerFactory(_name,
                                                         [](edm::ParameterSet const& cfg,
                                                            edm::ConsumesCollector& collector,
                                                            mithep::ObjectService* os,
                                                            char const* name, bool active)->BaseFiller*
                                                         {
                                                           return new Filler(cfg, collector, os, name, active);
                                                         });
      }
    };

    // Register a FillerFactory (filler generator function) under a given name
    void registerFactory(char const* _name, FillerFactory _f)
    {
      fillerFactories_[_name] = _f;
    }

    // Retrieve the FillerFactory and instantiate the filler
    BaseFiller* makeFiller(char const* className, edm::ParameterSet const&, edm::ConsumesCollector&, mithep::ObjectService*, char const* name, bool active) const;

    static FillerFactoryStore* singleton();

  private:
    tbb::concurrent_unordered_map<std::string, FillerFactory> fillerFactories_;
  };

}

// A macro that instantiates FillerFactoryStore::Registration for the class FILLER
#define DEFINE_MITHEP_TREEFILLER(FILLER) \
  mithep::FillerFactoryStore::Registration<mithep::FILLER> mithep##FILLER##Registration(#FILLER)

#endif
