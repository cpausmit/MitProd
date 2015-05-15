//--------------------------------------------------------------------------------------------------
// FillerPhotons
//
// Implementation of a filler to fill photons into our data structure, including converted photons.
//
// Authors: C.Paus, J.Bendavid
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERPHOTONS_H
#define MITPROD_TREEFILLER_FILLERPHOTONS_H

#include "MitAna/DataTree/interface/PhotonFwd.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"

#include "RecoEgamma/EgammaTools/interface/EGEnergyCorrector.h"
#include "DataFormats/HcalRecHit/interface/HcalRecHitCollections.h"
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "DataFormats/EgammaCandidates/interface/PhotonFwd.h"
#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/Common/interface/View.h"

namespace mithep 
{
  class FillerPhotons : public BaseFiller
  {  
    public:
      FillerPhotons(const edm::ParameterSet &cfg, edm::ConsumesCollector&, ObjectService*, const char *name, bool active=1);
      ~FillerPhotons();

      void BookDataBlock(TreeWriter &tws);
      void FillDataBlock(const edm::Event &e, const edm::EventSetup &es);

      typedef edm::View<reco::Photon> PhotonView;

    protected:		               
      void HERecHitMatcher(reco::Photon const& pho, int zSide,
                           double deltaPhiMin, double rhoMin,
                           double rhoMax, double rhEnMin,
                           ThreeVector &matchRhPos,
                           double &matchedRhEnergy,
                           double &matchedRhTime,
                           HBHERecHitCollection const& hbHeRecHitCol,
                           CaloGeometry const& caloGeom); 

    private:		               
      edm::EDGetTokenT<PhotonView>           edmToken_;                   //edm name: photon collection
      edm::EDGetTokenT<HBHERecHitCollection> HBHERecHitsEdmToken_;        //name: input edm HCAL HE rec hits collection  
      edm::EDGetTokenT<edm::ValueMap<bool> > phIDCutBasedTightToken_;     //name: tight cut phID algo
      edm::EDGetTokenT<edm::ValueMap<bool> > phIDCutBasedLooseToken_;     //name: loose cut phID algo

      std::string                      mitName_;                   //mit name: photon collection
      std::string                      conversionMapName_;         //name: imp. map wrt conv. elecs
      std::string                      oneLegConversionMapName_;   //name: imp. map wrt conv. elecs
      std::string                      barrelSuperClusterMapName_; //name: imp. map wrt barrel sclus
      std::string                      endcapSuperClusterMapName_; //name: imp. map wrt endcap sclus
      bool                             checkClusterActive_;
      std::string                      pfEcalBarrelSuperClusterMapName_;     //name: imp. map wrt pflow sclus 
      std::string                      pfEcalEndcapSuperClusterMapName_;     //name: imp. map wrt pflow sclus 
      std::string                      photonMapName_;             //name: exported photon map
      mithep::PhotonMap                *photonMap_;                //exported photon map
      mithep::PhotonArr                *photons_;                  //array of Photons
      const mithep::ConversionDecayMap *conversionMap_;            //imp. map wrt conver. electrons
      const mithep::ConversionDecayMap *oneLegConversionMap_;      //imp. map wrt conver. electrons
      const mithep::SuperClusterMap    *barrelSuperClusterMap_;    //map wrt barrel super clusters
      const mithep::SuperClusterMap    *endcapSuperClusterMap_;    //map wrt endcap super clusters
      const mithep::SuperClusterMap    *pfEcalBarrelSuperClusterMap_;        //map wrt pflow super clusters  
      const mithep::SuperClusterMap    *pfEcalEndcapSuperClusterMap_;        //map wrt pflow super clusters  

      EGEnergyCorrector                 ecorr_;
  };
}
#endif
