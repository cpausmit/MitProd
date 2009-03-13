// $Id: ObjectService.cc,v 1.2 2008/07/30 11:24:43 loizides Exp $

#include "MitProd/ObjectService/interface/ObjectService.h"
#include "DataFormats/Provenance/interface/ModuleDescription.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/ActivityRegistry.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/MessageLogger/interface/JobReport.h"
#include "MitProd/ObjectService/interface/NamedObject.h"

using namespace edm;
using namespace std;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
ObjectService::ObjectService(const ParameterSet &cfg, ActivityRegistry &ar) : 
  obs_(0),
  obsEvt_(0)
{
  // Constructor.

  obs_.SetOwner(kTRUE);
  obsEvt_.SetOwner(kTRUE);

  // set watchers
  ar.watchPreProcessEvent (this,&ObjectService::preEventProcessing);
  ar.watchPostProcessEvent(this,&ObjectService::postEventProcessing);
  ar.watchPostBeginJob    (this,&ObjectService::postBeginJob);
  ar.watchPostEndJob      (this,&ObjectService::postEndJob);
}

//--------------------------------------------------------------------------------------------------
ObjectService::~ObjectService() 
{
  // Destructor.
}

//--------------------------------------------------------------------------------------------------
void ObjectService::postBeginJob()
{
  // Nothing to be done for now
}

//--------------------------------------------------------------------------------------------------
void ObjectService::postEndJob()
{
  // Clear all objects in the hashtables.

  obs_.Delete();
  obsEvt_.Delete();
}

//--------------------------------------------------------------------------------------------------
void ObjectService::postEventProcessing(const Event&, const EventSetup&)
{
  // Remove objects put per event.

  obsEvt_.Clear();
}

//--------------------------------------------------------------------------------------------------
void ObjectService::preEventProcessing(const EventID&, const Timestamp&)
{
  // Nothing to be done for now
}
