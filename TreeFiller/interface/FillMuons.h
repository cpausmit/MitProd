//--------------------------------------------------------------------------------------------------
// $Id: FillMuons.h,v 1.5 2008/06/19 16:53:43 loizides Exp $
//
// FillMuons
//
// Module copying reconstructed EDM muons into mithep::Muons.
//
// Authors: C.Loizides, J.Bendavid, C.Paus
//--------------------------------------------------------------------------------------------------

#ifndef TREEFILLER_FILLMUONS_H
#define TREEFILLER_FILLMUONS_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "MitAna/DataTree/interface/Muon.h"
#include "MitAna/DataTree/interface/Track.h"
#include "MitAna/DataTree/interface/Array.h"

namespace mithep 
{
  class FillMuons : public edm::EDAnalyzer
  {
    public:
      FillMuons(const edm::ParameterSet&);
      ~FillMuons();

      void analyze(const edm::Event&, const edm::EventSetup&);
      void beginJob(const edm::EventSetup&);
      void endJob();
  
    private:
      mithep::Array<mithep::Muon> *muons_;  
      std::string muonSource_;
      std::string muonBranch_;
  };
}
#endif
