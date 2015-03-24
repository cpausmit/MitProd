//--------------------------------------------------------------------------------------------------
// $Id: FillerBeamSpot.h,v 1.6 2009/09/25 08:42:50 loizides Exp $
//
// FillerBeamSpot
//
// Implementation of a filler to fill EDM reco:BeamSpot into our mithep::BeamSpot data structure.
//
// Authors: J.Bendavid
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERBEAMSPOT_H
#define MITPROD_TREEFILLER_FILLERBEAMSPOT_H

#include "MitAna/DataTree/interface/BeamSpotFwd.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"

#include "DataFormats/BeamSpot/interface/BeamSpot.h"

namespace mithep 
{
  class FillerBeamSpot : public BaseFiller
  {  
    public:
      FillerBeamSpot(const edm::ParameterSet &cfg, edm::ConsumesCollector&, ObjectService*, const char *name, bool active=1);
      ~FillerBeamSpot();

      void                    BookDataBlock(TreeWriter &tws);
      void                    FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
   
    private:
      edm::EDGetTokenT<reco::BeamSpot> edmToken_;        //edm token for Vertex collection
      std::string             mitName_;        //mit name of vertex branch
      mithep::BeamSpotArr    *beamSpots_;      //the beamspot array
  };
}
#endif
