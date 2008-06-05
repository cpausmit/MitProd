// $Id: FillGenParts.h,v 1.1 2008/05/27 20:30:15 loizides Exp $

#ifndef TREEFILLER_FILLGENPARTS_H
#define TREEFILLER_FILLGENPARTS_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"

#include "MitProd/TreeService/interface/TreeService.h"
#include "MitAna/DataUtil/interface/TreeWriter.h"

#include "MitAna/DataTree/interface/GenParticle.h"
#include "MitAna/DataTree/interface/Vector.h"

//--------------------------------------------------------------------------------------------------
//
// FillGenParts
//
// Module copying HepMC particles into GenParticles.
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
      mithep::Vector<mithep::GenParticle> *genParticles_;  
      std::string mcSource_;
      std::string genPartBrn_;
  };

} /*namespace mithep*/

#endif /*TREEFILLER_FILLGENPARTS_H*/
