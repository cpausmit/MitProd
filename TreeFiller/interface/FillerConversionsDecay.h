//--------------------------------------------------------------------------------------------------
// $Id: FillerConversionsDecay.h,v 1.14 2010/03/18 20:21:00 bendavid Exp $
//
// FillerConversionsDecay
//
// Implementation of a filler creating mithep::DecayParticle objects from the reco::Conversion objects
//
// Authors: J.Bendavid
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERCONVERSIONSDECAY_H
#define MITPROD_TREEFILLER_FILLERCONVERSIONS_H

#include "MitAna/DataTree/interface/ConversionFwd.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"
#include "MitEdm/DataFormats/interface/BasePartFwd.h"
#include "MitAna/DataTree/interface/StableDataFwd.h"
#include "MitAna/DataTree/interface/DecayDataFwd.h"
#include "MitAna/DataTree/interface/DecayParticleFwd.h"

namespace mithep 
{
  class FillerConversionsDecay : public BaseFiller
  {  
    public:
      FillerConversionsDecay(const edm::ParameterSet &cfg, const char *name, bool active=1);
      ~FillerConversionsDecay();

      void                                     BookDataBlock(TreeWriter &tws);
      void                                     FillDataBlock(const edm::Event &e, 
                                                             const edm::EventSetup &es);
  
    private:
      mithep::Particle                        *GetMitParticle(edm::Ptr<reco::Track> ptr) const;
    
      std::string                              edmName_;             //name of edm conversions
      std::string                              mitName_;             //mit name of Conversions
      std::string                              stableDataName_;   //= mitName_ + "_StableDatas"  
      std::string                              convElectronMapName_; //name of imported electrons map
      std::vector<std::string>                 stablePartMapNames_;  //name imp maps wrt stable parts
      std::string                              conversionMapName_;   //name of exported conv map
      std::vector<const mithep::TrackPartMap*> stablePartMaps_;      //maps wrt stable parts
      mithep::DecayParticleArr                 *decays_;           //array of DecayParticles
      mithep::StableDataArr                    *stableData_;       //array of StableDatas
      mithep::ConversionDecayMap               *conversionMap_;       //exported map wrt Conversions
  };
}
#endif
