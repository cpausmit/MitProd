//--------------------------------------------------------------------------------------------------
// $Id: FillerPATElectrons.h,v 1.2 2008/09/10 03:30:23 loizides Exp $
//
// FillerPATElectrons
//
// Imlementation of a filler to fill PAT electrons into our mithep::Electron data structure.
//
// Authors: S.Xie
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERPATELECTRONS_H
#define MITPROD_TREEFILLER_FILLERPATELECTRONS_H

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrackFwd.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h"
#include "MitAna/DataUtil/interface/TreeWriter.h"
#include "MitAna/DataTree/interface/Collections.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"

namespace mithep 
{
  class FillerPATElectrons : public BaseFiller
  {  
    public:
      FillerPATElectrons(const edm::ParameterSet &cfg, bool active=1,
                         const TrackMap* gsfTrackMap=0, const TrackMap* trackerTrackMap=0);
      ~FillerPATElectrons();

      void BookDataBlock(TreeWriter &tws);
      void FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
  
    private:
      std::string      edmName_;
      std::string      mitName_;
      std::string      gsfTrackMapName_;       //name of imported map wrt gsf tracks
      std::string      trackerTrackMapName_;   //name of imported map wrt to trk tracks 

      mithep::ElectronArr           *electrons_;
      const mithep::TrackMap        *gsfTrackMap_;
      const mithep::TrackMap        *trackerTrackMap_;
  };
}
#endif
