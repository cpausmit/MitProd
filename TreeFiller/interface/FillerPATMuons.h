//--------------------------------------------------------------------------------------------------
// $Id: FillerPATMuons.h,v 1.5 2009/06/15 15:00:25 loizides Exp $
//
// FillerPATMuons
//
// Implementation of a filler to fill EDM muons into our mithep::Muon data structure.
//
// Authors: J.Bendavid
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERPATMUONS_H
#define MITPROD_TREEFILLER_FILLERPATMUONS_H

#include "MitAna/DataTree/interface/MuonFwd.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"

namespace mithep 
{
  class FillerPATMuons : public BaseFiller
  {  
    public:
      FillerPATMuons(const edm::ParameterSet &cfg, const char *name, bool active=1);
      ~FillerPATMuons();

      void                        BookDataBlock(TreeWriter &tws, const edm::EventSetup &es);
      void                        FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
  
    private:
      std::string                 edmName_;               //edm name of muon collection
      std::string                 mitName_;               //mit name of muon collection
      std::string                 globalTrackMapName_;    //name of imported map wrt global muons
      std::string                 staTrackMapName_;       //name of imported map wrt sta muons
      std::string                 staVtxTrackMapName_;    //name of imported map wrt sta vtx muons
      std::string                 trackerTrackMapName_;   //name of imported map wrt tracker muons
      const mithep::TrackMap     *globalTrackMap_;        //map wrt global muons
      const mithep::TrackMap     *standaloneTrackMap_;    //map wrt standalone muons
      const mithep::TrackMap     *standaloneVtxTrackMap_; //map wrt standalone vertex muons
      const mithep::TrackMap     *trackerTrackMap_;       //map wrt tracker track muons
      mithep::MuonArr            *muons_;                 //array of Muons
  };
}
#endif
