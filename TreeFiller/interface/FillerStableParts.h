//--------------------------------------------------------------------------------------------------
// $Id: FillerStableParts.h,v 1.12 2009/09/25 08:42:50 loizides Exp $
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

#include "MitEdm/DataFormats/interface/Collections.h"

namespace mithep 
{
  class FillerStableParts : public BaseFiller
  {  
    public:
      FillerStableParts(const edm::ParameterSet &cfg, edm::ConsumesCollector&, ObjectService*, const char *name, bool active=1);
      ~FillerStableParts();
    
      void                                  BookDataBlock(TreeWriter &tws);
      void 	                            FillDataBlock(const edm::Event &e, 
                                                          const edm::EventSetup &es);
    private:
      mithep::Track                        *GetMitTrack(const mitedm::TrackPtr &ptr) const;

      edm::EDGetTokenT<mitedm::StablePartCol> edmToken_;          //edm name of stable parts collection
      std::string                           mitName_;          //mit name of StableParticles
      std::vector<std::string>              trackMapNames_;    //name of import maps wrt tracks
      std::string                           basePartMapName_;  //name of export map wrt stable parts
      std::string                           trackPartMapName_; //name of export map wrt track-stable
      std::vector<const mithep::TrackMap*>  trackMaps_;        //imported maps wrt tracks
      mithep::StableParticleArr            *stables_;          //array of StableParticles
      mithep::BasePartMap                  *basePartMap_;      //map wrt stable parts
      mithep::TrackPartMap                 *trackPartMap_;     //track map wrt stable parts
  };
}
#endif
