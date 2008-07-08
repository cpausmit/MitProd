//--------------------------------------------------------------------------------------------------
// $Id: FillerElectrons.h,v 1.2 2008/07/01 21:11:47 loizides Exp $
//
// FillerElectrons
//
// Imlementation of a filler to fill EDM electrons into our mithep::Electron data structure.
//
// Authors: J.Bendavid
//--------------------------------------------------------------------------------------------------

#ifndef TREEFILLER_FILLERELECTRONS_H
#define TREEFILLER_FILLERELECTRONS_H

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrackFwd.h"
#include "DataFormats/EgammaCandidates/interface/PixelMatchGsfElectronFwd.h"
#include "MitAna/DataUtil/interface/TreeWriter.h"
#include "MitAna/DataTree/interface/Collections.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"

namespace mithep 
{
  class FillerElectrons : public BaseFiller
  {  
    public:
      FillerElectrons(const edm::ParameterSet &cfg, bool active=1,
                      const GsfTrackMap* gsfTrackMap=0, const TrackMap* trackerTrackMap=0);
      ~FillerElectrons();

      void BookDataBlock(TreeWriter &tws);
      void FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
  
    private:
      std::string                         	          edmName_;
      std::string                         	          mitName_;
      mithep::ElectronArr                                *electrons_;
      const mithep::GsfTrackMap                          *gsfTrackMap_;
      const mithep::TrackMap                             *trackerTrackMap_;
  };
}
#endif
