//--------------------------------------------------------------------------------------------------
// $Id: $
//
// FillerStableParts
//
// Imlementation of a filler to fill an mitedm::PartColl into our mithep::PartColl data structure.
//
// Authors: Ch.Paus
//--------------------------------------------------------------------------------------------------

#ifndef TREEFILLER_FILLERSTABLEPARTS_H
#define TREEFILLER_FILLERSTABLEPARTS_H

#include "MitProd/TreeFiller/interface/BaseFiller.h"
#include "MitAna/DataTree/interface/Collections.h"

namespace mithep 
{
  class FillerStableParts : public BaseFiller
  {  
  public:
    FillerStableParts(const edm::ParameterSet &cfg, const char *name, bool active=1);
    ~FillerStableParts();
    
    void                  BookDataBlock(TreeWriter &tws);
    void 	          FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
    
  private:
    std::string               edmName_;
    std::string               mitName_;
    mithep::StablePartObjArr *stables_;
  };
}
#endif
