// $Id:$

#ifndef TREEFILLER_FILLGENPARTS_H
#define TREEFILLER_FILLGENPARTS_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"

#include "MitProd/TreeService/interface/TreeService.h"
#include "MitAna/DataUtil/interface/TreeWriter.h"

#include "MitAna/DataTree/interface/Particle.h"
#include "MitAna/DataTree/interface/Vector.h"
#include <TClonesArray.h>

//--------------------------------------------------------------------------------------------------
//
// FillGenParts
//
// Module copying HepMC particles into Particles.
//
// Authors: C.Loizides
//
//--------------------------------------------------------------------------------------------------

namespace mithep 
{
  class FillGenParts : public edm::EDAnalyzer
  {
    public:
      FillGenParts(const edm::ParameterSet&);
      ~FillGenParts();

      void analyze(const edm::Event&, const edm::EventSetup&);
      void beginJob(edm::EventSetup const&);
      void endJob ();
  
    private:
      mithep::Vector<mithep::Particle> *genParticles_;  
      std::string anaMCSource_;
  };

} /*namespace mithep*/

#endif /*TREEFILLER_FILLGENPARTS_H*/
