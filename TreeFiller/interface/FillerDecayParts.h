//--------------------------------------------------------------------------------------------------
// $Id: FillerDecayParts.h,v 1.14 2009/09/25 08:42:50 loizides Exp $
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

#include "MitEdm/DataFormats/interface/BasePartFwd.h"
#include "MitAna/DataTree/interface/StableDataFwd.h"
#include "MitAna/DataTree/interface/DecayDataFwd.h"
#include "MitAna/DataTree/interface/DecayParticleFwd.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"
#include "MitProd/TreeFiller/interface/HitPatternReader.h"

#include "MitEdm/DataFormats/interface/Collections.h"

namespace mithep 
{
  class Partice;

  class FillerDecayParts : public BaseFiller
  {  
    public:
    FillerDecayParts(const edm::ParameterSet &cfg, edm::ConsumesCollector&, ObjectService*, const char *name, bool active=1);
      ~FillerDecayParts();
    
      void                                      BookDataBlock(TreeWriter &tws) override;
      void                                      PrepareLinks() override;
      void                                      FillDataBlock(const edm::Event &e,
                                                              const edm::EventSetup &es) override;
    
    private:
      mithep::Particle                         *getMitParticle(mitedm::BasePartPtr ptr) const;
    
      edm::EDGetTokenT<mitedm::DecayPartCol>    edmToken_;          //edm name of collection
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
      mithep::HitPatternReader                  hitReader_;        //hit pattern reader
  };
}
#endif
