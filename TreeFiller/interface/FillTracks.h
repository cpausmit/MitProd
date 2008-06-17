// $Id: FillTracks.h,v 1.2 2008/06/11 12:50:17 loizides Exp $

#ifndef TREEFILLER_FILLTRACKS_H
#define TREEFILLER_FILLTRACKS_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "MitProd/TreeService/interface/TreeService.h"
#include "MitAna/DataUtil/interface/TreeWriter.h"
#include "MitAna/DataTree/interface/Track.h"
#include "MitAna/DataTree/interface/Array.h"

//--------------------------------------------------------------------------------------------------
//
// FillTracks
//
// Module copying general EDM tracks into mithep::Tracks.
//
// Authors: C.Loizides, J.Bendavid
//
//--------------------------------------------------------------------------------------------------

namespace mithep 
{
  class FillTracks : public edm::EDAnalyzer
  {
    public:
      FillTracks(const edm::ParameterSet&);
      ~FillTracks();

      void analyze(const edm::Event&, const edm::EventSetup&);
      void beginJob(edm::EventSetup const&);
      void endJob ();
  
    private:
      mithep::Array<mithep::Track> *tracks_;  
      std::string trackSource_;
      std::string trackBranch_;
  };
}
#endif
