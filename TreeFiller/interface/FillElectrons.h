// $Id: FillElectrons.h,v 1.1 2008/06/05 16:07:11 bendavid Exp $

#ifndef TREEFILLER_FILLELECTRONS_H
#define TREEFILLER_FILLELECTRONS_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "MitProd/TreeService/interface/TreeService.h"
#include "MitAna/DataUtil/interface/TreeWriter.h"
#include "MitAna/DataTree/interface/Electron.h"
#include "MitAna/DataTree/interface/Track.h"
#include "MitAna/DataTree/interface/Vector.h"

//--------------------------------------------------------------------------------------------------
//
// FillElectrons
//
// Module copying reconstructed EDM electrons into mithep::Electrons.
//
// Authors: C.Loizides, J.Bendavid
//
//--------------------------------------------------------------------------------------------------

namespace mithep 
{
  class FillElectrons : public edm::EDAnalyzer
  {
    public:
      FillElectrons(const edm::ParameterSet&);
      ~FillElectrons();

      void analyze(const edm::Event&, const edm::EventSetup&);
      void beginJob(edm::EventSetup const&);
      void endJob ();
  
    private:
      mithep::Vector<mithep::Electron> *electrons_;  
      std::string electronSource_;
      std::string electronBranch_;
  };
}
#endif
