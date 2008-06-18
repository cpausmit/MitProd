//--------------------------------------------------------------------------------------------------
// $Id: FillerGlobalMuons.h,v 1.1 2008/06/18 13:23:22 paus Exp $
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
#include "MitAna/DataTree/interface/Array.h"
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
      mithep::Array<mithep::GlobalMuon>  *muons_;  
      const std::string                   edmName_;
      const std::string                   mitName_;
      const bool                          active_;
  };
}
#endif
