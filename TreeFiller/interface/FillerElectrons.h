//--------------------------------------------------------------------------------------------------
// $Id: FillerElectrons.h,v 1.17 2009/06/15 15:00:24 loizides Exp $
//
// FillerElectrons
//
// Implementation of a filler to fill EDM electrons into our mithep::Electron data structure.
//
// Authors: J.Bendavid, S.Xie
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERELECTRONS_H
#define MITPROD_TREEFILLER_FILLERELECTRONS_H

#include "MitAna/DataTree/interface/ElectronFwd.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"

namespace mithep 
{
  class FillerElectrons : public BaseFiller
  {  
    public:
      FillerElectrons(const edm::ParameterSet &cfg, const char *name, bool active=1);
      ~FillerElectrons();

      void                           BookDataBlock(TreeWriter &tws);
      void                           FillDataBlock(const edm::Event &e, 
                                                   const edm::EventSetup &es);
  
    private:
      std::string                    edmName_;                   //edm name of electrons collection
      std::string                    mitName_;                   //mit name of Electrons collection
      std::string                    gsfTrackMapName_;           //name of imported map wrt gsf trks
      std::string                    trackerTrackMapName_;       //name of imported map wrt trk trks
      std::string                    barrelEcalRecHitName_;      //name of barrel ecal rechits 
      std::string                    endcapEcalRecHitName_;      //name of endcap ecal rechits
      std::string                    barrelSuperClusterName_;    //name of barrel super clusters 
      std::string                    endcapSuperClusterName_;    //name of endcap super clusters
      std::string                    barrelBasicClusterName_;    //name of barrel basic clusters 
      std::string                    endcapBasicClusterName_;    //name of endcap basic clusters
      std::string                    barrelSuperClusterMapName_; //name of imp. map wrt barrel sclus
      std::string                    endcapSuperClusterMapName_; //name of imp. map wrt endcap sclus
      std::string                    pfSuperClusterMapName_;     //name of imp. map wrt pflow sclus
      std::string                    eIDCutBasedTightName_;      //name of tight cut eID algo
      std::string                    eIDCutBasedLooseName_;      //name of loose cut eID algo
      std::string                    isoTrackColName_;           //name of iso track collection
      std::string                    isoCaloTowerColName_;       //name of iso CaloTower collection
      std::string                    ecalJurassicIsoName_;       //name of Ecal Jurassic isolation
      std::string                    hcalJurassicIsoName_;       //name of Hcal Jurassic isolation
      std::string                    trackerIsoName_;            //name of tracker isolation
      std::string                    gsfTrackAssocName_;         //name of gsf track association map
      mithep::ElectronArr           *electrons_;                 //array of Electrons
      const mithep::TrackMap        *gsfTrackMap_;               //map wrt gsf tracks
      const mithep::TrackMap        *trackerTrackMap_;           //map wrt tracker tracks
      const mithep::SuperClusterMap *barrelSuperClusterMap_;     //map wrt barrel super clusters
      const mithep::SuperClusterMap *endcapSuperClusterMap_;     //map wrt endcap super clusters
      const mithep::SuperClusterMap *pfSuperClusterMap_;         //map wrt pflow super clusters 
  };
}
#endif
