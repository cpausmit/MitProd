//--------------------------------------------------------------------------------------------------
// $Id: FillerBeamSpot.h,v 1.5 2009/06/15 15:00:24 loizides Exp $
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

namespace mithep 
{
  class FillerBeamSpot : public BaseFiller
  {  
    public:
      FillerBeamSpot(const edm::ParameterSet &cfg, const char *name, bool active=1);
      ~FillerBeamSpot();

      void                    BookDataBlock(TreeWriter &tws, const edm::EventSetup &es);
      void                    FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
   
    private:
      std::string             edmName_;        //edm name of Vertex collection
      std::string             mitName_;        //mit name of vertex branch
      mithep::BeamSpotArr    *beamSpots_;      //the beamspot array
  };
}
#endif
