//--------------------------------------------------------------------------------------------------
// $Id: FillerDecayParts.h,v 1.10 2008/10/23 15:43:15 loizides Exp $
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
#include "MitProd/TreeFiller/interface/HitPatternReader.h"
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
    
      std::string                               edmName_;          //edm name of collection
      std::string                               mitName_;          //mit name of collection
      std::string                               stableDataName_;   //= mitName_ + "_StableDatas"
      std::string                               decayDataName_;    //= mitName_ + "_DecayDatas"
      std::string                               vertexMapName_;    //name map wrt primary vertexes
      std::vector<std::string>                  basePartMapNames_; //name imp maps wrt stable parts
      std::vector<const mithep::BasePartMap*>   basePartMaps_;     //maps wrt stable parts
      const mithep::VertexMap                  *vertexMap_;        //map wrt primary vertexes
      mithep::DecayParticleArr                 *decays_;           //array of DecayParticles
      mithep::StableDataArr                    *stableData_;       //array of StableDatas
      mithep::DecayDataArr                     *decayData_;        //array of DecayDatas
      HitPatternReader                          hitReader_;        //hit pattern reader
  };
}
#endif
