//--------------------------------------------------------------------------------------------------
// $Id: FillerMuons.h,v 1.13 2010/05/06 17:31:23 bendavid Exp $
//
// FillerMuons
//
// Implementation of a filler to fill EDM muons into our mithep::Muon data structure.
//
// Authors: J.Bendavid
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERMUONS_H
#define MITPROD_TREEFILLER_FILLERMUONS_H

#include "MitAna/DataTree/interface/MuonFwd.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"

namespace mithep 
{
  class FillerMuons : public BaseFiller
  {  
    public:
      FillerMuons(const edm::ParameterSet &cfg, const char *name, bool active=1);
      ~FillerMuons();

      void                        BookDataBlock(TreeWriter &tws);
      void                        FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
  
    private:
      std::string                 edmName_;               //edm name of muons collection
      std::string                 expectedHitsName_;      //edm name of corrected expected hits valuemap
      std::string                 mitName_;               //mit name of Muons
      std::string                 globalTrackMapName_;    //name of imported map wrt global muons
      std::string                 staTrackMapName_;       //name of imported map wrt sta muons
      std::string                 staVtxTrackMapName_;    //name of imported map wrt sta vtx muons
      std::string                 trackerTrackMapName_;   //name of imported map wrt tracker muons
      std::string                 muonMapName_;           //name of exported muon map
      std::string                 pvEdmName_;             //name of primary vertex collection
      std::string                 pvBSEdmName_;           //name of bs-constrained pv collection
      const mithep::TrackMap     *globalTrackMap_;        //map wrt global muons
      const mithep::TrackMap     *standaloneTrackMap_;    //map wrt standalone muons
      const mithep::TrackMap     *standaloneVtxTrackMap_; //map wrt standalone vertex muons
      const mithep::TrackMap     *trackerTrackMap_;       //map wrt tracker track muons
      mithep::MuonMap            *muonMap_;               //exported muon map
      mithep::MuonArr            *muons_;                 //array of Muons
  };
}
#endif
