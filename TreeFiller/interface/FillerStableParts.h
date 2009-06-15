//--------------------------------------------------------------------------------------------------
// $Id: FillerStableParts.h,v 1.7 2009/03/22 10:00:46 loizides Exp $
//
// FillerStableParts
//
// Implementation of a filler to copy the mitedm::StablePart collection into the
// mithep::StableParticle collection.
//
// Authors: C.Paus
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERSTABLEPARTS_H
#define MITPROD_TREEFILLER_FILLERSTABLEPARTS_H

#include "MitAna/DataTree/interface/StableParticleFwd.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"

namespace mithep 
{
  class FillerStableParts : public BaseFiller
  {  
    public:
      FillerStableParts(const edm::ParameterSet &cfg, const char *name, bool active=1);
      ~FillerStableParts();
    
      void                          BookDataBlock(TreeWriter &tws);
      void 	                    FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
      
    private:
      std::string                   edmName_;         //edm name of stable parts collection
      std::string                   mitName_;         //mit name of StableParticles
      std::string                   trackMapName_;    //name of imported map wrt tracks
      std::string                   basePartMapName_; //name of exported map wrt stable parts
      const mithep::TrackMap       *trackMap_;        //imported map wrt tracks
      mithep::StableParticleArr    *stables_;         //array of StableParticles
      mithep::BasePartMap          *basePartMap_;     //map wrt stable parts
  };
}
#endif
