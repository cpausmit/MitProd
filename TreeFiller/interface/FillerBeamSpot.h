//--------------------------------------------------------------------------------------------------
// $Id: FillerBeamSpot.h,v 1.1 2008/11/12 18:21:28 bendavid Exp $
//
// FillerBeamSpot
//
// Imlementation of a filler to fill EDM reco:Vertex into our mithep::Vertex data structure.
//
// Authors: J.Bendavid
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERBEAMSPOT_H
#define MITPROD_TREEFILLER_FILLERBEAMSPOT_H

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "MitAna/DataTree/interface/Collections.h"
#include "MitAna/DataUtil/interface/TreeWriter.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"

namespace mithep 
{
  class FillerBeamSpot : public BaseFiller
  {  
    public:
      FillerBeamSpot(const edm::ParameterSet &cfg, const char *name, bool active=1);
      ~FillerBeamSpot();

      void BookDataBlock(TreeWriter &tws);
      void FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
  
    private:
      std::string                           edmName_;          //edm name of Vertex collection
      std::string                           mitName_;          //name of Vertices in OAK
      mithep::BeamSpotArr                  *beamSpots_;        //the beamspot array
  };
}
#endif
