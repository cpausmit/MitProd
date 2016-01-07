//--------------------------------------------------------------------------------------------------
// $Id: FillerPATElectrons.h,v 1.7 2009/09/25 08:42:50 loizides Exp $
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

#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/Common/interface/View.h"

namespace mithep 
{
  class FillerPATElectrons : public BaseFiller
  {  
    public:
      FillerPATElectrons(const edm::ParameterSet &cfg, edm::ConsumesCollector&, ObjectService* os, const char *name, bool active=1);
      ~FillerPATElectrons();

      void                      BookDataBlock(TreeWriter &tws) override;
      void                      PrepareLinks() override;
      void                      FillDataBlock(const edm::Event &e, const edm::EventSetup &es) override;
  
    private:
      edm::EDGetTokenT<edm::View<pat::Electron> > edmToken_;               //edm name of electron collection
      std::string               mitName_;               //mit name of electron collection
      std::string               gsfTrackMapName_;       //name of imported map wrt gsf tracks
      std::string               trackerTrackMapName_;   //name of imported map wrt to trk tracks 
      mithep::ElectronArr      *electrons_;             //electron array
      const mithep::TrackMap   *gsfTrackMap_;           //imported gsf track map
      const mithep::TrackMap   *trackerTrackMap_;       //imported track track map
  };
}
#endif
