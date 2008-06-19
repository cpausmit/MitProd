//--------------------------------------------------------------------------------------------------
// $Id: FillGenParts.h,v 1.5 2008/06/18 19:18:06 loizides Exp $
//
// FillGenParts
//
// Module copying HepMC particles into mithep::GenParticles.
//
// Authors: C.Loizides
//
//--------------------------------------------------------------------------------------------------

#ifndef TREEFILLER_FILLGENPARTS_H
#define TREEFILLER_FILLGENPARTS_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "MitProd/TreeService/interface/TreeService.h"
#include "MitAna/DataUtil/interface/TreeWriter.h"
#include "MitAna/DataTree/interface/GenParticle.h"
#include "MitAna/DataTree/interface/Array.h"

namespace mithep 
{
  class FillGenParts : public edm::EDAnalyzer
  {
    public:
      FillGenParts(const edm::ParameterSet&);
      ~FillGenParts();

      void analyze(const edm::Event&, const edm::EventSetup&);
      void beginJob(const edm::EventSetup&);
      void endJob ();
  
    private:
      mithep::Array<mithep::GenParticle> *genParticles_;  
      std::string mcSource_;
      std::string genPartBrn_;
  };
}
#endif
