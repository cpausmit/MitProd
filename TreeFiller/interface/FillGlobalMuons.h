//--------------------------------------------------------------------------------------------------
// $Id: FillGlobalMuons.h,v 1.4 2008/06/19 16:53:43 loizides Exp $
//
// FillGlobalMuons
//
// Module copying reconstructed EDM muons into mithep::Muons.
//
// Authors: C.Loizides, J.Bendavid, C.Paus
//--------------------------------------------------------------------------------------------------

#ifndef TREEFILLER_FILLGLOBALMUONS_H
#define TREEFILLER_FILLGLOBALMUONS_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "MitAna/DataTree/interface/GlobalMuon.h"
#include "MitAna/DataTree/interface/Track.h"
#include "MitAna/DataTree/interface/Array.h"

namespace mithep 
{
  class FillGlobalMuons : public edm::EDAnalyzer
  {
    public:
      FillGlobalMuons(const edm::ParameterSet&);
      ~FillGlobalMuons();

      void analyze(const edm::Event&, const edm::EventSetup&);
      void beginJob(const edm::EventSetup&);
      void endJob ();
  
    private:
      mithep::Array<mithep::GlobalMuon> *muons_;  
      std::string muonSource_;
      std::string muonBranch_;
  };
}
#endif
