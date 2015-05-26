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
    FillerPhotons(edm::ParameterSet const&, edm::ConsumesCollector&, mithep::ObjectService*, char const*, bool = true);
    ~FillerPhotons();

    void BookDataBlock(mithep::TreeWriter &) override;
    void FillDataBlock(edm::Event const&, edm::EventSetup const&) override;

    typedef edm::View<reco::Photon> PhotonView;

  protected:		               
    void HERecHitMatcher(reco::Photon const&, int zSide,
                         double deltaPhiMin, double rhoMin,
                         double rhoMax, double rhEnMin,
                         ThreeVector&,
                         double& matchedRhEnergy,
                         double& matchedRhTime,
                         HBHERecHitCollection const&,
                         CaloGeometry const&); 

  private:		               
    edm::EDGetTokenT<PhotonView>           edmToken_;                   //edm name: photon collection
    edm::EDGetTokenT<HBHERecHitCollection> HBHERecHitsEdmToken_;        //name: input edm HCAL HE rec hits collection  
    edm::EDGetTokenT<edm::ValueMap<bool> > phIDCutBasedTightToken_;     //name: tight cut phID algo
    edm::EDGetTokenT<edm::ValueMap<bool> > phIDCutBasedLooseToken_;     //name: loose cut phID algo
    std::string                            phIDCutBasedTightName_;
    std::string                            phIDCutBasedLooseName_;

    std::string                       mitName_;                   //mit name: photon collection
    std::string                       conversionMapName_;         //name: imp. map wrt conv. elecs
    std::string                       oneLegConversionMapName_;   //name: imp. map wrt conv. elecs
    std::string                       barrelSuperClusterMapName_; //name: imp. map wrt barrel sclus
    std::string                       endcapSuperClusterMapName_; //name: imp. map wrt endcap sclus
    bool                              checkClusterActive_;
    bool                              fillFromPAT_;               //true when filling from PAT (e.g. MiniAOD)
    std::string                       pfEcalBarrelSuperClusterMapName_;     //name: imp. map wrt pflow sclus 
    std::string                       pfEcalEndcapSuperClusterMapName_;     //name: imp. map wrt pflow sclus 
    std::string                       photonMapName_;             //name: exported photon map
    std::string                       photonPFMapName_;           //name of exported PF->photon map
    mithep::PhotonMap*                photonMap_;                //exported photon map
    mithep::CandidateMap*             photonPFMap_;              //exported PF->photon map
    mithep::PhotonArr*                photons_;                  //array of Photons
    mithep::ConversionDecayMap const* conversionMap_;            //imp. map wrt conver. electrons
    mithep::ConversionDecayMap const* oneLegConversionMap_;      //imp. map wrt conver. electrons
    mithep::SuperClusterMap const*    barrelSuperClusterMap_;    //map wrt barrel super clusters
    mithep::SuperClusterMap const*    endcapSuperClusterMap_;    //map wrt endcap super clusters
    mithep::SuperClusterMap const*    pfEcalBarrelSuperClusterMap_;        //map wrt pflow super clusters  
    mithep::SuperClusterMap const*    pfEcalEndcapSuperClusterMap_;        //map wrt pflow super clusters  

    EGEnergyCorrector                 ecorr_;
  };
}
#endif
