//--------------------------------------------------------------------------------------------------
// $Id: FillerPATMuons.h,v 1.1 2008/08/12 10:13:46 sixie Exp $
//
// FillerPATMuons
//
// Imlementation of a filler to fill EDM muons into our mithep::Muon data structure.
//
// Authors: J.Bendavid
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERPATMUONS_H
#define MITPROD_TREEFILLER_FILLERPATMUONS_H

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "MitAna/DataUtil/interface/TreeWriter.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "MitAna/DataTree/interface/Collections.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"

namespace mithep 
{
  class FillerPATMuons : public BaseFiller
  {  
    public:
      FillerPATMuons(const edm::ParameterSet &cfg, bool active=1,
                  const TrackMap *globalMap=0, const TrackMap *stdMap=0, 
                  const TrackMap *stdVtxMap=0, const TrackMap *trackerMap=0);
      ~FillerPATMuons();

      void BookDataBlock(TreeWriter &tws);
      void FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
  
    private:
      std::string                 edmName_;
      std::string                 mitName_;
      std::string                 globalTrackMapName_;    //name of imported map wrt global muons
      std::string                 staTrackMapName_;       //name of imported map wrt sta muons
      std::string                 staVtxTrackMapName_;    //name of imported map wrt sta vtx muons
      std::string                 trackerTrackMapName_;   //name of imported map wrt tracker muons

      const mithep::TrackMap     *globalTrackMap_;
      const mithep::TrackMap     *standaloneTrackMap_;
      const mithep::TrackMap     *standaloneVtxTrackMap_;
      const mithep::TrackMap     *trackerTrackMap_;
      mithep::MuonArr            *muons_;
  };
}
#endif
