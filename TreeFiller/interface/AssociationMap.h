//--------------------------------------------------------------------------------------------------
// $Id: AssociationMap.h,v 1.1 2008/07/01 14:39:28 loizides Exp $
//
// Association Map
//
// Wrapper for std::map, used to resolve links during tree filling.
// This class needs work! CL.
//
// Authors: J.Bendavid, C.Loizides
//--------------------------------------------------------------------------------------------------

#ifndef TREEFILLER_ASSOCIATIONMAP_H
#define TREEFILLER_ASSOCIATIONMAP_H

#include <map>
#include <TObject.h>

namespace mithep
{
  template <class EdmClass, class MitClass> class AssociationMap : public TObject
  { 
    typedef std::map<EdmClass, MitClass> fwdMapType;
    typedef std::map<MitClass, EdmClass> revMapType;
    
    public:
      AssociationMap() : edmProductId_(0) {}
      ~AssociationMap() {}
      
      void       Add(EdmClass edmObj, MitClass mitObj);
      EdmClass   GetEdmRef(MitClass mitObj) const;
      Int_t      GetEdmProductId()          const { return edmProductId_; }
      Int_t      GetEntries()               const { return fwdMap_.size(); }
      MitClass   GetMit(EdmClass edmObj)    const;
      void       Reset()                          { fwdMap_.clear(); revMap_.clear(); }
      void       SetEdmProductId(Int_t id)        { edmProductId_ = id; }

    protected:
      fwdMapType fwdMap_;       //map between edm ref and mit ptr 
      revMapType revMap_;       //map between mit ptr and edm ref
      Int_t      edmProductId_; //product id for consistency check
  };
}

//--------------------------------------------------------------------------------------------------
template <class EdmClass, class MitClass>
inline void mithep::AssociationMap<EdmClass,MitClass>::Add(EdmClass edmObj, MitClass mitObj) 
{
  fwdMap_[edmObj]=mitObj;
  revMap_.insert(std::pair<MitClass, EdmClass>(mitObj,edmObj));
}

//--------------------------------------------------------------------------------------------------
template <class EdmClass, class MitClass>
inline MitClass mithep::AssociationMap<EdmClass,MitClass>::GetMit(EdmClass edmObj) const
{
  typename fwdMapType::const_iterator iter = fwdMap_.find(edmObj);

  if (iter != fwdMap_.end())
    return iter->second;
  else return 0;
}
      
//--------------------------------------------------------------------------------------------------
template <class EdmClass, class MitClass>
inline EdmClass mithep::AssociationMap<EdmClass,MitClass>::GetEdmRef(MitClass mitObj) const
{
  typename revMapType::const_iterator iter = revMap_.find(mitObj);
  if (iter != revMap_.end())
    return iter->second;
  else return 0;
}
#endif
