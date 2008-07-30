//--------------------------------------------------------------------------------------------------
// $Id: NamedObject.h,v 1.2 2008/07/30 11:24:43 loizides Exp $
//
// NamedObjectOwned
//
// Class for storing objects in a THashTable. Objects are owned and will therefore be
// deleted. This class is only used internally by the ObjectService. 
//
// Authors: C.Loizides
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_NAMEDOBJECTOWNED_H
#define MITPROD_NAMEDOBJECTOWNED_H

#include <TNamed.h>

namespace mithep 
{
  template<class T>
  class NamedObjectOwned : public TNamed
  {
    public:
      NamedObjectOwned(T *ptr) : TNamed(((TObject*)ptr)->GetName(),0), fPtr(ptr) {}
      NamedObjectOwned(T *ptr, const char *n) : TNamed(n,0), fPtr(ptr) {}
      ~NamedObjectOwned() { delete fPtr; }
      const T *Get() const { return fPtr; }

    private:
      T       *fPtr; //pointer to object
  };
}
#endif
