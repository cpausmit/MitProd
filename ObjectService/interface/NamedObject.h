//--------------------------------------------------------------------------------------------------
// $Id: ObjectService.h,v 1.7 2008/07/03 08:25:12 loizides Exp $
//
// ObjectService 
      // Class for storing event objects:
      // It assumes ownership of the object.
      // It is needed to force the THashTable to call Hash on a TNamed
      // to allow lookup using the hash value given the object's name
      // (as opposed to its pointer value, as is done for TObject's).
//
//
// Authors: C.Loizides
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_NAMEDOBJECT_H
#define MITPROD_NAMEDOBJECT_H

#include <TNamed.h>

namespace mithep 
{
  template<class T>
  class NamedObject : public TNamed
  {
    public:
      NamedObject(T *ptr) : TNamed(((TObject*)ptr)->GetName(),0), fPtr(ptr) {}
      NamedObject(T *ptr, const char *n) : TNamed(n,0), fPtr(ptr) {}
      ~NamedObject() { delete fPtr; }
      T             *Get()       { return fPtr; }
      const T       *Get() const { return fPtr; }

    private:
      T             *fPtr; //pointer to object
  };
}
#endif
