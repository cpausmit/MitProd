//--------------------------------------------------------------------------------------------------
// $Id: FillerMCParticles.h,v 1.2 2008/07/31 12:34:04 loizides Exp $
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

#include <map>
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "MitAna/DataUtil/interface/TreeWriter.h"
#include "MitAna/DataTree/interface/Collections.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"
#include "SimDataFormats/Track/interface/SimTrackContainer.h"

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
      Bool_t                      trackingActive_;   //=true if simulated TrackingParticles are mapped
      std::string                 genEdmName_;  //edm name of generated particles
      std::string                 simEdmName_;  //edm name of simulated particles
      std::string                 trackingEdmName_;  //edm name of simulated TrackingParticles
      std::string                 genMapName_;  //name of exported map wrt generated particles
      std::string                 simMapName_;  //name of exported map wrt simulated particles
      std::string                 trackingMapName_;  //name of exported map wrt simulated TrackingParticles
      std::string                 mitName_;     //name of MCParticles branch in OAK
      std::map<unsigned int, SimTrackRef> simTidMap_; //map of SimTracks to G4 track ids
      mithep::MCParticleArr      *mcParticles_; //array of MCParticles
      mithep::GenParticleMap     *genMap_;      //map wrt generated particles
      mithep::SimTrackMap        *simMap_;      //map wrt simulated particles
      mithep::TrackingParticleMap *trackingMap_;      //map wrt simulated TrackingParticles
  };
}
#endif
