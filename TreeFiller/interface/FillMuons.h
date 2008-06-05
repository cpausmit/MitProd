// $Id$

#ifndef TREEFILLER_FILLMUONS_H
#define TREEFILLER_FILLMUONS_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"

#include "MitProd/TreeService/interface/TreeService.h"
#include "MitAna/DataUtil/interface/TreeWriter.h"

#include "MitAna/DataTree/interface/Muon.h"
#include "MitAna/DataTree/interface/Track.h"
#include "MitAna/DataTree/interface/Vector.h"
#include <TClonesArray.h>

//--------------------------------------------------------------------------------------------------
//
// FillMuons
//
// Module copying HepMC particles into Muons.
//
// Authors: C.Loizides, J. Bendavid
//
//--------------------------------------------------------------------------------------------------

namespace mithep 
{
  class FillMuons : public edm::EDAnalyzer
  {
    public:
      FillMuons(const edm::ParameterSet&);
      ~FillMuons();

      void analyze(const edm::Event&, const edm::EventSetup&);
      void beginJob(edm::EventSetup const&);
      void endJob ();
  
    private:
      mithep::Vector<mithep::Muon> *muons_;  
      std::string muonSource_;
      std::string muonBranch_;
  };

} /*namespace mithep*/

#endif
