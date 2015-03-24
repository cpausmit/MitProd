// $Id: ObjectService.cc,v 1.2 2008/07/30 11:24:43 loizides Exp $

#include "MitProd/ObjectService/interface/ObjectService.h"
#include "MitProd/ObjectService/interface/NamedObject.h"

using namespace edm;
using namespace std;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
ObjectService::ObjectService() : 
  obs_(0),
  obsEvt_(0)
{
  obs_.SetOwner(kTRUE);
  obsEvt_.SetOwner(kTRUE);
}

//--------------------------------------------------------------------------------------------------
ObjectService::~ObjectService() 
{
}

//--------------------------------------------------------------------------------------------------
void ObjectService::clear()
{
  // Clear all objects in the hashtables.

  obs_.Delete();
  obsEvt_.Delete();
}

//--------------------------------------------------------------------------------------------------
void ObjectService::clearEvt()
{
  // Remove objects put per event.

  obsEvt_.Clear();
}
