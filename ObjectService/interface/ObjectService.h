//--------------------------------------------------------------------------------------------------
// $Id: ObjectService.h,v 1.7 2008/07/03 08:25:12 loizides Exp $
//
// ObjectService 
//
// This service can be used in the config files to provide access to the TreeWriter,
// for example:
//
//   service = ObjectService { 
//
//   }
//
// Authors: C.Loizides
//
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_OBJECTSERVICE_H
#define MITPROD_OBJECTSERVICE_H

#include "DataFormats/Provenance/interface/EventID.h"
#include "DataFormats/Provenance/interface/Timestamp.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"

#include <TObjArray.h>

namespace edm 
{
  class ActivityRegistry;
  class ParameterSet;
  class ModuleDescription;
}

namespace mithep 
{
  class ObjectService  {
    public:
      ObjectService(const edm::ParameterSet &cfg, edm::ActivityRegistry &ar);
      ~ObjectService();

//      TreeWriter *get(const char *name=0);

    private:
#if 0
      void preEventProcessing(const edm::EventID &id, const edm::Timestamp &t);
      void postEventProcessing(const edm::Event& e, const edm::EventSetup& es);
      void postBeginJob();
      void postEndJob();
#endif
      TObjArray obs_; //array holding the hash table

      // parameters for service
      std::vector<std::string> hashNames_;   //hash names
  };
}
#endif
