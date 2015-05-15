//--------------------------------------------------------------------------------------------------
// AssociationMap
//
// Wrapper for std::map, used to resolve links during tree filling.
//
// Authors: C.Paus, J.Bendavid, C.Loizides
//--------------------------------------------------------------------------------------------------
#ifndef MITPROD_TREEFILLER_ASSOCIATIONMAP_H
#define MITPROD_TREEFILLER_ASSOCIATIONMAP_H

#include <map>
#include <TObject.h>
#include "FWCore/MessageLogger/interface/MessageLogger.h"

namespace mithep
{
  template <class EdmClass, class MitClass> class AssociationMap : public TObject
  { 
  public:
    typedef std::map<EdmClass, MitClass> fwdMapType;
    typedef std::map<MitClass, EdmClass> revMapType;
    
    AssociationMap() : edmProductId_(0) {}
    ~AssociationMap() {}
    
    void              Add(EdmClass edmObj, MitClass mitObj);
    EdmClass          GetEdm(MitClass mitObj, bool throwOnFail = kTRUE) const;
    Int_t             GetEdmProductId()          const { return edmProductId_;  }
    Int_t             GetEntries()               const { return fwdMap_.size(); }
    MitClass          GetMit(EdmClass edmObj, bool throwOnFail = kTRUE) const;
    const char       *GetBrName()                const { return brname_.c_str();           }
    bool              HasMit(EdmClass edmObj)    const;
    void              Reset()                          { fwdMap_.clear(); revMap_.clear(); }
    void              SetEdmProductId(Int_t id)        { edmProductId_ = id;               }
    void              SetBrName(const std::string &n)  { brname_ = n;                      }
    void              SetBrName(const char *n)         { brname_ = n;                      }
    const fwdMapType &FwdMap()                   const { return fwdMap_;                   }
    const revMapType &RevMap()                   const { return revMap_;                   }
    
  protected:
    fwdMapType   fwdMap_;       //map between edm ref and mit ptr 
    revMapType   revMap_;       //map between mit ptr and edm ref
    Int_t        edmProductId_; //product id for consistency check
    std::string  brname_;       //branch name of MIT objects 
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
inline MitClass mithep::AssociationMap<EdmClass,MitClass>::GetMit(EdmClass edmObj, bool throwOnFail/* = kTRUE*/) const
{
  typename fwdMapType::const_iterator iter = fwdMap_.find(edmObj);

  if (iter != fwdMap_.end())
    return iter->second;
  else if (throwOnFail)
    throw edm::Exception(edm::errors::Configuration, "AssociationMap::GetMit()\n")
      << "Error! EDM Object (" << typeid(edmObj).name() 
      << ") not found in AssociationMap (" << typeid(*this).name() << ")." << std::endl;
  else
    return static_cast<MitClass>(0); // Relies on MitClass being a pointer type
}
      
//--------------------------------------------------------------------------------------------------
template <class EdmClass, class MitClass>
inline EdmClass mithep::AssociationMap<EdmClass,MitClass>::GetEdm(MitClass mitObj, bool throwOnFail/* = kTRUE*/) const
{
  typename revMapType::const_iterator iter = revMap_.find(mitObj);

  if (iter != revMap_.end())
    return iter->second;
  else if (throwOnFail)
    throw edm::Exception(edm::errors::Configuration, "AssociationMap::GetEdm()\n")
      << "Error! MITHEP Object (" << typeid(mitObj).name() 
      << ") not found in AssociationMap (" << typeid(*this).name() << ")." << std::endl;
  else
    return EdmClass();
}

//--------------------------------------------------------------------------------------------------
template <class EdmClass, class MitClass>
inline bool mithep::AssociationMap<EdmClass,MitClass>::HasMit(EdmClass edmObj) const
{
  typename fwdMapType::const_iterator iter = fwdMap_.find(edmObj);

  if (iter != fwdMap_.end())
    return true;
  else 
    return false;
}
#endif
