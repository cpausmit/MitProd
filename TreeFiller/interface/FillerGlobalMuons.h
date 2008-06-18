//--------------------------------------------------------------------------------------------------
// $Id: $
//
// FillerGlobalMuons
//
// Imlementation of a filler to fill global muons into our data structure.
//
// Authors: C.Paus
//--------------------------------------------------------------------------------------------------
#ifndef TREEFILLER_FILLERGLOBALMUONS_H
#define TREEFILLER_FILLERGLOBALMUONS_H

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "MitAna/DataUtil/interface/TreeWriter.h"
#include "MitAna/DataTree/interface/GlobalMuon.h"
#include "MitAna/DataTree/interface/Muon.h"
#include "MitAna/DataTree/interface/Track.h"
#include "MitAna/DataTree/interface/Vector.h"
#include "MitProd/TreeService/interface/TreeService.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"

namespace mithep 
{
  class FillerGlobalMuons : public BaseFiller
  {
    public:
      FillerGlobalMuons(const edm::ParameterSet&);
      ~FillerGlobalMuons();

      bool Active       () { return active_; }
      void BookDataBlock(TreeWriter *tws);
      void FillDataBlock(const edm::Event&, const edm::EventSetup&);
      void ResolveLinks (const edm::Event&, const edm::EventSetup&);
  
    private:
      mithep::Vector<mithep::GlobalMuon> *muons_;  
      const std::string                   edmName_;
      const std::string                   mitName_;
      const bool                          active_;
  };
}
#endif
