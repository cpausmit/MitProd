//--------------------------------------------------------------------------------------------------
// $Id: FillerMCParticles.h,v 1.4 2008/09/10 03:30:23 loizides Exp $
//
// FillerMCParticles
//
// Imlementation of a filler to fill HepMC particles OR the aod GenParticle candidates (gen)
// AND SimTracks (sim) with association map to TrackingParticles (for hit-based MC truth matching)
// into mithep::MCParticles. Gen and Sim particle information is thus merged into a single 
// collection.
//
// Authors: C.Loizides, J.Bendavid
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERMCPARTICLES_H
#define MITPROD_TREEFILLER_FILLERMCPARTICLES_H

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
      Bool_t                      useAodGen_;   //=true to use aod GenParticles instead of HepMC
      Bool_t                      simActive_;   //=true if simulated particles are filled
      Bool_t                      trackingActive_;   //=true if simulated TrackingParticles are mapped
      std::string                 genEdmName_;  //edm name of generated particles
      std::string                 simEdmName_;  //edm name of simulated particles
      std::string                 trackingEdmName_;  //edm name of simulated TrackingParticles
      std::string                 genMapName_;  //name of exported map wrt generated particles
      std::string                 simMapName_;  //name of exported map wrt simulated particles
      std::string                 trackingMapName_;  //name of exported map wrt simulated TrackingParticles
      std::string                 mitName_;     //name of MCParticles branch in OAK
      mithep::MCParticleArr      *mcParticles_; //array of MCParticles
      mithep::GenParticleBarcodeMap *genMap_;      //map wrt generated particles
      mithep::AODGenParticleMap  *aodGenMap_;      //map wrt generated particles
      mithep::SimTrackTidMap     *simMap_;      //map of SimTracks to G4 track ids
      mithep::TrackingParticleMap *trackingMap_;      //map wrt simulated TrackingParticles
  };
}
#endif
