//--------------------------------------------------------------------------------------------------
// $Id: FillerStableParts.h,v 1.1 2008/07/28 23:13:43 paus Exp $
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
#include "MitProd/TreeFiller/interface/AssociationMaps.h"
#include "MitAna/DataTree/interface/Collections.h"

namespace mithep 
{
  class FillerStableParts : public BaseFiller
  {  
  public:
    FillerStableParts(const edm::ParameterSet &cfg, const char *name, bool active=1,
                      const TrackMap *trackMap=0);
    ~FillerStableParts();
    
    void                  BookDataBlock(TreeWriter &tws);
    void 	          FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
    
    const BasePartMap    *GetParticleMap() const { return particleMap_; }
    
  private:
    std::string               edmName_;
    std::string               mitName_;
    mithep::BasePartMap      *particleMap_;
    const mithep::TrackMap   *trackMap_;
    mithep::StableParticleArr *stables_;
  };
}
#endif
