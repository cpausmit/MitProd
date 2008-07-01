//--------------------------------------------------------------------------------------------------
// $Id: FillerGlobalMuons.h,v 1.3 2008/06/18 19:17:21 loizides Exp $
//
// FillerSimParticles
//
// Imlementation of a filler to fill simulation level particles into our data structure.
//
// Authors: J. Bendavid
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
      FillerSimParticles(const edm::ParameterSet &cfg);
      ~FillerSimParticles();

      void                  BookDataBlock(TreeWriter &tws);
      void                  FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
      void                  ResolveLinks (const edm::Event &e, const edm::EventSetup &es);
      const SimParticleMap *GetSimParticleMap() { return simMap_; }
  
    private:
      std::string				edmName_;
      std::string				edmDataName_;
      std::string				mitName_;
      mithep::Array<mithep::SimParticle>	*simParticles_;
      mithep::SimParticleMap*			simMap_;
      edm::Handle<TrackingParticleCollection>	trackingParticleProduct;
  };
}
#endif
