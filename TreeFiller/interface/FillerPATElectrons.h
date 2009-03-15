//--------------------------------------------------------------------------------------------------
// $Id: FillerPATElectrons.h,v 1.3 2008/11/03 18:11:10 bendavid Exp $
//
// FillerPATElectrons
//
// Implementation of a filler to fill PAT electrons into our mithep::Electron data structure.
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
      FillerPATElectrons(const edm::ParameterSet &cfg, const char *name, bool active=1);
      ~FillerPATElectrons();

      void BookDataBlock(TreeWriter &tws);
      void FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
  
    private:
      std::string               edmName_;               //edm name of electron collection
      std::string               mitName_;               //mit name of electron collection
      std::string               gsfTrackMapName_;       //name of imported map wrt gsf tracks
      std::string               trackerTrackMapName_;   //name of imported map wrt to trk tracks 
      mithep::ElectronArr      *electrons_;             //electron array
      const mithep::TrackMap   *gsfTrackMap_;           //imported gsf track map
      const mithep::TrackMap   *trackerTrackMap_;       //imported track track map
  };
}
#endif
