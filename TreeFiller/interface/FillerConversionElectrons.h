//--------------------------------------------------------------------------------------------------
// $Id: FillerConversionElectrons.h,v 1.10 2009/03/22 10:00:45 loizides Exp $
//
// FillerConversionElectrons
//
// Implementation of a filler creating mithep::Electron objects from the conversion finder tracks.
// This filler differs significantly from the others to deal with the special way in which
// conversions are stored in the edm.
//
// Authors: J.Bendavid
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERCONVERSIONELECTRONS_H
#define MITPROD_TREEFILLER_FILLERCONVERSIONELECTRONS_H

#include "MitAna/DataTree/interface/ElectronFwd.h"
#include "MitAna/DataTree/interface/TrackFwd.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"

namespace mithep 
{
  class FillerConversionElectrons : public BaseFiller
  {  
    public:
      FillerConversionElectrons(const edm::ParameterSet &cfg, const char *name, bool active=1);
      ~FillerConversionElectrons();

      void                           BookDataBlock(TreeWriter &tws);
      void                           FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
      void                           FillFromTracks(const mithep::TrackCol *tracks, 
                                                    const TrackMap *trackMap);

    private:
      std::string                    mitName_;                //name of Electrons
      std::string                    convInOutTracksName_;    //name of converted in-out track
      std::string                    convOutInTracksName_;    //name of converted out- track
      std::string                    convInOutTrackMapName_;  //name of imported map wrt in-out
      std::string                    convOutInTrackMapName_;  //name of imported map wrt out-in
      std::string                    convElectronMapName_;    //name of exported map wrt electrons
      const mithep::TrackCol        *convInOutTracks_;        //array of converted in-out tracks
      const mithep::TrackCol        *convOutInTracks_;        //array of converted out-in tracks
      const mithep::TrackMap        *convInOutTrackMap_;      //imported map wrt in-out tracks
      const mithep::TrackMap        *convOutInTrackMap_;      //imported map wrt out-in tracks
      mithep::ElectronArr           *convElectrons_;          //array of conversion electrons
      mithep::ConversionElectronMap *convElectronMap_;        //exported map wrt electrons
  };
}
#endif
