//--------------------------------------------------------------------------------------------------
// $Id: FillerPATElectrons.h,v 1.6 2009/06/15 15:00:25 loizides Exp $
//
// FillerPATElectrons
//
// Implementation of a filler to fill PAT electrons into our mithep::Electron data structure.
//
// Authors: S.Xie
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERPATELECTRONS_H
#define MITPROD_TREEFILLER_FILLERPATELECTRONS_H

#include "MitAna/DataTree/interface/ElectronFwd.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"

namespace mithep 
{
  class FillerPATElectrons : public BaseFiller
  {  
    public:
      FillerPATElectrons(const edm::ParameterSet &cfg, const char *name, bool active=1);
      ~FillerPATElectrons();

      void                      BookDataBlock(TreeWriter &tws, const edm::EventSetup &es);
      void                      FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
  
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
