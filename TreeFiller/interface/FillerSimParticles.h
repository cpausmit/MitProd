//--------------------------------------------------------------------------------------------------
// $Id: FillerSimParticles.h,v 1.6 2008/07/07 16:14:01 loizides Exp $
//
// FillerSimParticles
//
// Imlementation of a filler to fill simulation level particles into our data structure.
//
// Authors: J.Bendavid
//--------------------------------------------------------------------------------------------------

#ifndef TREEFILLER_FILLERSIMPARTICLES_H
#define TREEFILLER_FILLERSIMPARTICLES_H

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticleFwd.h"
#include "MitAna/DataUtil/interface/TreeWriter.h"
#include "MitAna/DataTree/interface/Collections.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"

namespace mithep 
{
  class FillerSimParticles : public BaseFiller
  {  
    public:
      FillerSimParticles(const edm::ParameterSet &cfg, bool active=1, 
                         const GenParticleMap *genMap=0);
      ~FillerSimParticles();

      void                  BookDataBlock(TreeWriter &tws);
      void                  FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
      void                  ResolveLinks (const edm::Event &e, const edm::EventSetup &es);
      const SimParticleMap *GetSimParticleMap() const { return simMap_; }
  
    private:
      std::string                               edmName_;
      std::string                               mitName_;
      const mithep::GenParticleMap             *genMap_;
      mithep::Array<mithep::SimParticle>       *simParticles_;
      mithep::SimParticleMap                   *simMap_;
  };
}
#endif
