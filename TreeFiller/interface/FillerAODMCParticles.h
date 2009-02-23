//--------------------------------------------------------------------------------------------------
// $Id: FillerAODMCParticles.h,v 1.4 2008/09/10 03:30:23 loizides Exp $
//
// FillerAODMCParticles
//
// Imlementation of a filler to fill HepMC particles (gen) AND TrackingParticles (sim)
// into mithep::MCParticles. Gen and Sim particle information is thus merged into a single 
// collection.
//
// Authors: C.Loizides, J.Bendavid
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERAODMCPARTICLES_H
#define MITPROD_TREEFILLER_FILLERAODMCPARTICLES_H

#include <map>
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "MitAna/DataUtil/interface/TreeWriter.h"
#include "MitAna/DataTree/interface/Collections.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"

namespace mithep 
{
  class FillerAODMCParticles : public BaseFiller
  {
    public:
      FillerAODMCParticles(const edm::ParameterSet &cfg, const char *name, bool active=1);
      ~FillerAODMCParticles();

      void BookDataBlock(TreeWriter &tws);
      void FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
      void ResolveLinks (const edm::Event &e, const edm::EventSetup &es);
  
    private:
      std::string                 genEdmName_;  //edm name of generated particles
      std::string                 genMapName_;  //name of exported map wrt generated particles
      std::string                 mitName_;     //name of MCParticles branch in OAK
      mithep::MCParticleArr      *mcParticles_; //array of MCParticles
      mithep::AODGenParticleMap  *genMap_;      //map wrt generated particles
  };
}
#endif
