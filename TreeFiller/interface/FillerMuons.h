//--------------------------------------------------------------------------------------------------
// $Id: FillerMuons.h,v 1.2 2008/07/01 21:11:47 loizides Exp $
//
// FillerMuons
//
// Imlementation of a filler to fill EDM muons into our mithep::Muon data structure.
//
// Authors: J.Bendavid
//--------------------------------------------------------------------------------------------------

#ifndef TREEFILLER_FILLERMUONS_H
#define TREEFILLER_FILLERMUONS_H

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "MitAna/DataUtil/interface/TreeWriter.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "MitAna/DataTree/interface/Collections.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"

namespace mithep 
{
  class FillerMuons : public BaseFiller
  {  
    public:
      FillerMuons(const edm::ParameterSet &cfg, bool active=1,
                  const TrackMap *globalMap=0, const TrackMap *stdMap=0, 
                  const TrackMap *stdVtxMap=0, const TrackMap *trackerMap=0);
      ~FillerMuons();

      void BookDataBlock(TreeWriter &tws);
      void FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
  
    private:
      std::string                          edmName_;
      std::string                          mitName_;
      mithep::MuonArr                     *muons_;
      const mithep::TrackMap              *globalTrackMap_;
      const mithep::TrackMap              *standaloneTrackMap_;
      const mithep::TrackMap              *standaloneVtxTrackMap_;
      const mithep::TrackMap              *trackerTrackMap_;
  };
}
#endif
