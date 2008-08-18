//--------------------------------------------------------------------------------------------------
// $Id: FillerElectrons.h,v 1.5 2008/08/08 11:21:08 sixie Exp $
//
// FillerElectrons
//
// Imlementation of a filler to fill EDM electrons into our mithep::Electron data structure.
//
// Authors: J.Bendavid, S.Xie
//--------------------------------------------------------------------------------------------------

#ifndef TREEFILLER_FILLERELECTRONS_H
#define TREEFILLER_FILLERELECTRONS_H

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrackFwd.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h"
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

      std::string                    edmName_;                   //edm name of electrons collection
      std::string                    mitName_;                   //name of Electrons in OAK
      std::string                    gsfTrackMapName_;           //name of imported map wrt gsf tracks
      std::string                    trackerTrackMapName_;       //name of imported map wrt 
                                                                 //to trk tracks 
      std::string                    barrelEcalRecHitName_;      //name of barrel ecal rechits 
      std::string                    endcapEcalRecHitName_;      //name of endcap ecal rechits
      std::string                    barrelSuperClusterName_;    //name of barrel super clusters 
      std::string                    endcapSuperClusterName_;    //name of endcap super clusters
      std::string                    barrelBasicClusterName_;    //name of barrel basic clusters 
      std::string                    endcapBasicClusterName_;    //name of endcap basic clusters
      std::string                    barrelSuperClusterMapName_; //name of imported map wrt to 
                                                                 //barrel super clusters
      std::string                    endcapSuperClusterMapName_; //name of imported map wrt to 
                                                                 //endcap super cluster
      std::string                    eIDCutBasedName_;           //name of cut based eID algo
      std::string                    eIDCutBasedClassesName_;    //name of cut based classes eID algo
      std::string                    eIDLikelihoodName_;         //name of likelihood eID algo
      std::string                    eIDNeuralNetName_;          //name of neural net eID algo

      mithep::ElectronArr            *electrons_;                //array of Electrons
      const mithep::GsfTrackMap      *gsfTrackMap_;              //map wrt gsf tracks
      const mithep::TrackMap         *trackerTrackMap_;          //map wrt tracker tracks
      const mithep::SuperClusterMap  *barrelSuperClusterMap_;    //map wrt barrel super clusters
      const mithep::SuperClusterMap  *endcapSuperClusterMap_;    //map wrt endcap super clusters 

  };
}
#endif

