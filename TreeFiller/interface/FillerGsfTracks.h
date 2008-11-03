//--------------------------------------------------------------------------------------------------
// $Id: FillerGsfTracks.h,v 1.9 2008/09/10 03:30:23 loizides Exp $
//
// FillerGsfTracks
//
// Imlementation of a filler to fill EDM gsf tracks into our mithep::Track data structure.
//
// Authors: J.Bendavid
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERGSFTRACKS_H
#define MITPROD_TREEFILLER_FILLERGSFTRACKS_H

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrackFwd.h"
#include "MitProd/TreeFiller/interface/FillerTracks.h"

namespace mithep 
{
  class FillerGsfTracks : public FillerTracks
  {  
    public:
      FillerGsfTracks(const edm::ParameterSet &cfg, const char *name, bool active=1, bool ecalActive=0);
      ~FillerGsfTracks();

      void BookDataBlock(TreeWriter &tws);
      void FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
      void InitLayerMap();
  
    private:
      mithep::GsfTrackMap             *trackMap_;              //map wrt tracks
  };
}
#endif
