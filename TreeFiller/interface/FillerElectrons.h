//--------------------------------------------------------------------------------------------------
// $Id: FillerElectrons.h,v 1.3 2008/07/08 12:38:20 loizides Exp $
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
      FillerElectrons(const edm::ParameterSet &cfg, bool active=1);
      ~FillerElectrons();

      void BookDataBlock(TreeWriter &tws);
      void FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
  
    private:
      std::string                  edmName_;              //edm name of electrons collection
      std::string                  mitName_;              //name of Electrons in OAK
      std::string                  gsfTrackMapName_;      //name of imported map wrt gsf tracks
      std::string                  trackerTrackMapName_;  //name of imported map wrt to trk tracks 
      mithep::ElectronArr         *electrons_;            //array of Electrons
      const mithep::GsfTrackMap   *gsfTrackMap_;          //map wrt gsf tracks
      const mithep::TrackMap      *trackerTrackMap_;      //map wrt tracker tracks
  };
}
#endif

