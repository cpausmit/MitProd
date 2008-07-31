//--------------------------------------------------------------------------------------------------
// $Id: FillerMCParticles.h,v 1.1 2008/07/25 11:33:57 bendavid Exp $
//
// FillerMCParticles
//
// Imlementation of a filler to fill HepMC particles (gen) AND TrackingParticles (sim)
// into mithep::MCParticles. Gen and Sim particle information is thus merged into a single 
// collection.
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
  
    private:
      Bool_t                      genActive_;   //=true if generated particles are filled
      Bool_t                      simActive_;   //=true if simulated particles are filled
      std::string                 genEdmName_;  //edm name of generated particles
      std::string                 simEdmName_;  //edm name of simulated particles
      std::string                 genMapName_;  //name of exported map wrt generated particles
      std::string                 simMapName_;  //name of exported map wrt simulated particles
      std::string                 mitName_;     //name of MCParticles branch in OAK
      mithep::MCParticleArr      *mcParticles_; //array of MCParticles
      mithep::GenParticleMap     *genMap_;      //map wrt generated particles
      mithep::SimParticleMap     *simMap_;      //map wrt simulated particles
  };
}
#endif
