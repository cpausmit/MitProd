//--------------------------------------------------------------------------------------------------
// $Id: NamedObject.h,v 1.1 2008/07/30 09:04:39 loizides Exp $
//
// NamedObject
//
// Class for storing objects in a THashTable. Objects are not owned and will therefore not
// be deleted. This class is only used internally by the ObjectService. 
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
      NamedObject(const T *ptr) : TNamed(((TObject*)ptr)->GetName(),0), fPtr(ptr) {}
      NamedObject(const T *ptr, const char *n) : TNamed(n,0), fPtr(ptr) {}
      ~NamedObject() { fPtr = 0; }
      const T       *Get() const { return fPtr; }

    private:
      const T       *fPtr; //pointer to object
  };
}
#endif
