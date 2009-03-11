//--------------------------------------------------------------------------------------------------
// $Id: FillerMuons.h,v 1.6 2009/03/10 15:56:00 loizides Exp $
//
// FillerMuons
//
// Imlementation of a filler to fill EDM muons into our mithep::Muon data structure.
//
// Authors: J.Bendavid
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERMUONS_H
#define MITPROD_TREEFILLER_FILLERMUONS_H

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
      FillerMuons(const edm::ParameterSet &cfg, const char *name, bool active=1);
      ~FillerMuons();

      void BookDataBlock(TreeWriter &tws);
      void FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
  
    private:
      std::string                 edmName_;               //edm name of muons collection
      std::string                 mitName_;               //name of Muons in OAK
      std::string                 globalTrackMapName_;    //name of imported map wrt global muons
      std::string                 staTrackMapName_;       //name of imported map wrt sta muons
      std::string                 staVtxTrackMapName_;    //name of imported map wrt sta vtx muons
      std::string                 trackerTrackMapName_;   //name of imported map wrt tracker muons
      std::string                 muonMapName_;           //name of exported muon map
      const mithep::TrackMap     *globalTrackMap_;        //map wrt global muons
      const mithep::TrackMap     *standaloneTrackMap_;    //map wrt standalone muons
      const mithep::TrackMap     *standaloneVtxTrackMap_; //map wrt standalone vertex muons
      const mithep::TrackMap     *trackerTrackMap_;       //map wrt tracker track muons
      mithep::MuonMap            *muonMap_;               //exported muon map
      mithep::MuonArr            *muons_;                 //array of Muons
  };
}
#endif
