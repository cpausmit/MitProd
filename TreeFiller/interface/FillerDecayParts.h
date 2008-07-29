//--------------------------------------------------------------------------------------------------
// $Id: FillerDecayParts.h,v 1.1 2008/07/28 23:13:43 paus Exp $
//
// FillerDecayParts
//
// Imlementation of a filler to fill an mitedm::PartColl into our mithep::PartColl data structure.
//
// Authors: Ch.Paus
//--------------------------------------------------------------------------------------------------

#ifndef TREEFILLER_FILLERDECAYPARTS_H
#define TREEFILLER_FILLERDECAYPARTS_H

#include "MitProd/TreeFiller/interface/BaseFiller.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"
#include "MitAna/DataTree/interface/Collections.h"

namespace mithep 
{
  class FillerDecayParts : public BaseFiller
  {  
  public:
    FillerDecayParts(const edm::ParameterSet &cfg, const char *name, 
		     bool active=1, const mithep::BasePartMap* partMap=0);
    ~FillerDecayParts();
    
    void                  BookDataBlock(TreeWriter &tws);
    void 	          FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
    
  private:
    std::string           edmName_;
    std::string           mitName_;
    const mithep::BasePartMap *partMap_;
    mithep::DecayParticleArr *decays_;
  };
}
#endif
