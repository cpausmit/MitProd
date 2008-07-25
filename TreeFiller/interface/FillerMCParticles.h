//--------------------------------------------------------------------------------------------------
// $Id: FillerMCParticles.h,v 1.6 2008/07/13 08:42:04 loizides Exp $
//
// FillerMCParticles
//
// Imlementation of a filler to fill HepMC particles (gen) AND TrackingParticles (sim)
// into mithep::MCParticles.  Gen and Sim information is thus merged into a single collection
//
// Authors: C.Loizides, J.Bendavid
//--------------------------------------------------------------------------------------------------

#ifndef TREEFILLER_FILLERMCPARTICLES_H
#define TREEFILLER_FILLERMCPARTICLES_H

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "MitAna/DataUtil/interface/TreeWriter.h"
#include "MitAna/DataTree/interface/Collections.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"

namespace mithep 
{
  class FillerMCParticles : public BaseFiller
  {
    public:
      FillerMCParticles(const edm::ParameterSet &cfg, const char *name, bool active=1);
      ~FillerMCParticles();

      void BookDataBlock(TreeWriter &tws);
      void FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
      void ResolveLinks (const edm::Event &e, const edm::EventSetup &es);
      const GenParticleMap *GetGenParticleMap() const;
      const SimParticleMap *GetSimParticleMap() const;
  
    private:
      Bool_t                              genActive_;
      Bool_t                              simActive_;
      std::string                         genEdmName_;
      std::string                         simEdmName_;
      std::string                         mitName_;
      mithep::MCParticleArr              *mcParticles_; 
      mithep::GenParticleMap             *genMap_;
      mithep::SimParticleMap             *simMap_;
  };
}

//--------------------------------------------------------------------------------------------------
inline const mithep::GenParticleMap *mithep::FillerMCParticles::GetGenParticleMap() const
{
  if (genActive_)
    return genMap_;
  else
    return 0;
}

//--------------------------------------------------------------------------------------------------
inline const mithep::SimParticleMap *mithep::FillerMCParticles::GetSimParticleMap() const
{
  if (simActive_)
    return simMap_;
  else
    return 0;
}
#endif
