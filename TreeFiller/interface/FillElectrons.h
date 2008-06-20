//--------------------------------------------------------------------------------------------------
// $Id: FillElectrons.h,v 1.6 2008/06/19 16:53:43 loizides Exp $
//
// FillElectrons
//
// Module copying HepMC particles into Electrons.
//
// Authors: C.Loizides, J.Bendavid
//--------------------------------------------------------------------------------------------------

#ifndef TREEFILLER_FILLELECTRONS_H
#define TREEFILLER_FILLELECTRONS_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "MitAna/DataTree/interface/Electron.h"
#include "MitAna/DataTree/interface/Track.h"
#include "MitAna/DataTree/interface/Array.h"

namespace mithep 
{
  class FillElectrons : public edm::EDAnalyzer
  {
    public:
      FillElectrons(const edm::ParameterSet&);
      ~FillElectrons();

      void analyze(const edm::Event&, const edm::EventSetup&);
      void beginJob(const edm::EventSetup&);
      void endJob ();
  
    private:
      mithep::Array<mithep::Electron> *electrons_;  
      std::string electronSource_;
      std::string electronBranch_;
  };
}
#endif
