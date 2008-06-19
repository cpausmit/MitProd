//--------------------------------------------------------------------------------------------------
// $Id: FillerMetaInfos.h,v 1.3 2008/06/18 19:17:21 loizides Exp $
//
// FillerMetaInfos
//
// Authors: C.Loizides
//--------------------------------------------------------------------------------------------------

#ifndef TREEFILLER_FILLERMETAINFOS_H
#define TREEFILLER_FILLERMETAINFOS_H

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
  class FillerMetaInfos : public BaseFiller
  {
    public:
      FillerMetaInfos(const edm::ParameterSet&);
      ~FillerMetaInfos();

      void BookDataBlock(TreeWriter *tws);
      void FillDataBlock(const edm::Event&, const edm::EventSetup&);
      void ResolveLinks (const edm::Event&, const edm::EventSetup&);
  
    private:

  };
}
#endif
