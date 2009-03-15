//--------------------------------------------------------------------------------------------------
// $Id: ObjectService.h,v 1.6 2009/03/13 20:32:25 loizides Exp $
//
// ObjectService 
//
// This service can be used in the config files to provide a simple interface to 
// exchange objects accross fillers and in principle also across modules. There
// are two types of lifetimes: a) over the full run time b) per event. See 
// functions add/addObjEvt and get/getObjEct.
//
// Usage in config files is:
//   service = ObjectService { }
//
// Authors: C.Loizides
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_OBJECTSERVICE_OBJECTSERVICE_H
#define MITPROD_OBJECTSERVICE_OBJECTSERVICE_H

#include <string>
#include "DataFormats/Provenance/interface/EventID.h"
#include "DataFormats/Provenance/interface/Timestamp.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "MitProd/ObjectService/interface/NamedObject.h"
#include "MitProd/ObjectService/interface/NamedObjectOwned.h"
#include <THashTable.h>

namespace edm 
{
  class ActivityRegistry;
  class ParameterSet;
  class ModuleDescription;
}

namespace mithep 
{
  using std::string;

  class ObjectService  {
    public:
      ObjectService(const edm::ParameterSet &cfg, edm::ActivityRegistry &ar);
      ~ObjectService();

      template<class T> bool     add(const T *obj, const char *name);
      template<class T> bool     addObjEvt(T *obj, const char *name);
      template<class T> const T *get(const char *name)             const;
      template<class T> const T *getObjEvt(const char *name)       const;
      template<class T> T       *mod(const char *name)             const;
      template<class T> T       *modObjEvt(const char *name)       const;

      template<class T> bool     add(const T *obj, const string &name)
                                   { return add<T>(obj,name.c_str());       }
      template<class T> bool     addObjEvt(T *obj, const string &name)
                                   { return addObjEvt<T>(obj,name.c_str()); }
      template<class T> const T *get(const string &name)           const
                                   { return get<T>(name.c_str());           }
      template<class T> const T *getObjEvt(const string &name)     const
                                   { return getObjEvt<T>(name.c_str());     }
      template<class T> T       *mod(const string &name)           const
                                   { return mod<T>(name.c_str()); }
      template<class T> T       *modObjEvt(const string &name)     const
                                   { return modObjEvt<T>(name.c_str());     }

    private:
      void       preEventProcessing(const edm::EventID &id, const edm::Timestamp &t);
      void       postEventProcessing(const edm::Event &e, const edm::EventSetup &es);
      void       postBeginJob();
      void       postEndJob();

      THashTable obs_;    //hash table holding the objects
      THashTable obsEvt_; //hash table holding the objects per event
  };
}

//--------------------------------------------------------------------------------------------------
template<class T>
bool mithep::ObjectService::add(const T *obj, const char *name)
{
  // Add object with given name.

  if (obs_.FindObject(name)) {
    edm::LogError("ObjectService") << "Cannot add object with name " << name 
                                   << " since name is already used." 
                                   << std::endl;
    throw edm::Exception(edm::errors::Configuration, "ObjectService::add()\n")
      << "Cannot add object with name " << name 
      << " since name is already used.\n";

    return 0;
  }
      
  NamedObject<T> *no = new NamedObject<T>(obj, name);
  obs_.Add(no);
  return 1;
}

//--------------------------------------------------------------------------------------------------
template<class T>
bool mithep::ObjectService::addObjEvt(T *obj, const char *name)
{
  // Add object with given name for the current event.

  if (obsEvt_.FindObject(name)) {
    edm::LogError("ObjectService") << "Cannot add object with name " << name 
                                   << " to event since name is already used." 
                                   << std::endl;
    throw edm::Exception(edm::errors::Configuration, "ObjectService::addObjEvt()\n")
      << "Cannot add object with name " << name 
      << " to event since name is already used.\n";

    return 0;
  }
      
  NamedObjectOwned<T> *no = new NamedObjectOwned<T>(obj, name);
  obsEvt_.Add(no);
  return 1;
}

//--------------------------------------------------------------------------------------------------
template<class T>
const T *mithep::ObjectService::get(const char *name) const
{
  // Retrieve object with given name.

  TObject *o = obs_.FindObject(name);
  if (!o) 
    return 0;

  const NamedObject<T> *no = static_cast<const NamedObject<T>* >(o);
  if (!no)
    return 0;

  return no->Get();
}

//--------------------------------------------------------------------------------------------------
template<class T>
const T *mithep::ObjectService::getObjEvt(const char *name) const
{
  // Retrieve object for the current event with given name.

  TObject *o = obsEvt_.FindObject(name);
  if (!o) 
    return 0;

  const NamedObjectOwned<T> *no = static_cast<const NamedObjectOwned<T>* >(o);
  if (!no)
    return 0;

  return no->Get();
}

//--------------------------------------------------------------------------------------------------
template<class T>
T *mithep::ObjectService::mod(const char *name) const
{
  // Retrieve object with given name for write access.

  return const_cast<T*>(this->get<T>(name));
}

//--------------------------------------------------------------------------------------------------
template<class T>
T *mithep::ObjectService::modObjEvt(const char *name) const
{
  // Retrieve object for the current event with given name for write access.

  return const_cast<T*>(this->getObjEvt<T>(name));
}
#endif
