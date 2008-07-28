//--------------------------------------------------------------------------------------------------
// $Id: $
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
#include "MitAna/DataTree/interface/BasePart.h"
#include "MitAna/DataTree/interface/Collections.h"

namespace mithep 
{
  class FillerDecayParts : public BaseFiller
  {  
  public:
    FillerDecayParts(const edm::ParameterSet &cfg, const char *name, 
		     bool active=1);
    ~FillerDecayParts();
    
    void                  BookDataBlock(TreeWriter &tws);
    void 	          FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
    
  private:
    std::string           edmName_;
    std::string           mitName_;
    mithep::DecayPartObjArr *decays_;
  };
}
#endif
