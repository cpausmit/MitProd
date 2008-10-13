//--------------------------------------------------------------------------------------------------
// $Id: FillerDecayParts.h,v 1.7 2008/09/30 13:03:15 bendavid Exp $
//
// FillerDecayParts
//
// Imlementation of a filler to copy the mitedm::DecayPart collection into the
// mithep::DecayParticle collection.
//
// Authors: C.Paus
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERDECAYPARTS_H
#define MITPROD_TREEFILLER_FILLERDECAYPARTS_H

#include "MitProd/TreeFiller/interface/BaseFiller.h"
#include "MitProd/TreeFiller/interface/FillerTracks.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"
#include "MitAna/DataTree/interface/Particle.h"
#include "MitAna/DataTree/interface/Collections.h"
#include "MitEdm/DataFormats/interface/BasePartFwd.h"

namespace mithep 
{
  class FillerDecayParts : public BaseFiller
  {  
    public:
      FillerDecayParts(const edm::ParameterSet &cfg, const char *name, bool active=1);
      ~FillerDecayParts();
    
      void BookDataBlock(TreeWriter &tws);
      void FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
    
    private:
      mithep::Particle *getMitParticle(mitedm::BasePartPtr ptr) const;
    
      std::string                               edmName_;         //edm name of decay parts collection
      std::string                               mitName_;         //name of DecayParticles in OAK
      std::string                               vertexMapName_;   //name of imported map wrt primary vertexes
      std::vector<std::string>                  basePartMapNames_; //name of imported maps wrt stable parts
      std::vector<const mithep::BasePartMap*>   basePartMaps_;     //maps wrt stable parts
      const mithep::VertexMap                  *vertexMap_;        //map wrt primary vertexes
      mithep::DecayParticleArr                 *decays_;          //array of DecayParticles
      mithep::StableDataArr                    *stableData_;      //array of StableDatas
      mithep::DecayDataArr                     *decayData_;       //array of DecayDatas
      std::map<uint32_t,mithep::Track::EHitLayer>  layerMap_;        //hitlayer bitmap map
  };
}
#endif
