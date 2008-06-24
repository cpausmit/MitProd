//--------------------------------------------------------------------------------------------------
// $Id: FillerGlobalMuons.h,v 1.5 2008/06/20 17:52:57 loizides Exp $
//
// FillerGlobalMuons
//
// Imlementation of a filler to fill EDM global muons into mithep::GlobalMuons.
//
// Authors: C.Paus
//--------------------------------------------------------------------------------------------------

#ifndef TREEFILLER_FILLERGLOBALMUONS_H
#define TREEFILLER_FILLERGLOBALMUONS_H

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "MitAna/DataUtil/interface/TreeWriter.h"
#include "MitAna/DataTree/interface/GlobalMuon.h"
#include "MitAna/DataTree/interface/Array.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"

namespace mithep 
{
  class FillerGlobalMuons : public BaseFiller
  {
    public:
      FillerGlobalMuons(const edm::ParameterSet&);
      ~FillerGlobalMuons();

      void BookDataBlock(TreeWriter &tws);
      void FillDataBlock(const edm::Event&, const edm::EventSetup&);
      void ResolveLinks (const edm::Event&, const edm::EventSetup&);
  
    private:
      std::string                         edmName_;
      std::string                         mitName_;
      mithep::Array<mithep::GlobalMuon>  *muons_;  
  };
}
#endif
