//--------------------------------------------------------------------------------------------------
// $Id: FillMuons.h,v 1.6 2008/06/20 17:52:57 loizides Exp $
//
// Association Map
//
// wrapper for std::map, used to resolve links during tree filling
//
// Authors: J.Bendavid
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
      
      void Add(EdmClass edmObj, MitClass mitObj) {
        fwdMap[edmObj]=mitObj;
	//revMap[mitObj]=edmObj;
	revMap.insert(std::pair<MitClass, EdmClass>(mitObj,edmObj));
      }
      
      MitClass GetMit(EdmClass edmObj) const {
	//MitClass iter = fwdMap.find(edmObj);
	//fwdMapType::iterator iter;
	//fwdIter = fwdMap.find(edmObj);
	//if ( iter != fwdMap.end() )
	//	return iter->second;
	//else return 0;
	return fwdMap.find(edmObj)->second;
      }
      
       EdmClass GetEdmRef(MitClass mitObj) const {
//         //EdmClass iter = revMap.find(mitObj);
// // 	revMapType::iterator iter = revMap.find(mitObj);
// // 	if ( iter != revMap.end() )
// // 		return iter->second;
// // 	else return 0;
 	return revMap.find(mitObj)->second;
       }
      
      Int_t GetEntries() { return fwdMap.size(); }
      
      void Reset() {
      	fwdMap.clear();
	revMap.clear();
      }
      
	Int_t GetEdmProductId() const { return edmProductId_; }
	void  SetEdmProductId(Int_t id) { edmProductId_ = id; }

    protected:
	fwdMapType fwdMap;
	revMapType revMap;
	Int_t edmProductId_;
      
  };
}
#endif
