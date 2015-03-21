//--------------------------------------------------------------------------------------------------
// $Id: FillerPhotons.h,v 1.21 2012/07/25 03:08:42 paus Exp $
//
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

namespace mithep 
{
  class FillerPhotons : public BaseFiller
  {  
    public:
      FillerPhotons(const edm::ParameterSet &cfg, edm::ConsumesCollector&, const char *name, bool active=1);
      ~FillerPhotons();

      void                             BookDataBlock(TreeWriter &tws);
      void                             FillDataBlock(const edm::Event &e, const edm::EventSetup &es);

    protected:		               
      void                             HERecHitMatcher(const reco::Photon* pho, int zSide,
                                                       double deltaPhiMin, double rhoMin,
                                                       double rhoMax, double rhEnMin,
                                                       ThreeVector &matchRhPos,
						       double &matchedRhEnergy,
						       double &matchedRhTime,
                                                       const HBHERecHitCollection *hbHeRecHitCol,
						       const CaloGeometry *caloGeom); 

    private:		               
      edm::EDGetTokenT<reco::PhotonCollection> edmToken_;                   //edm name: photon collection
      edm::EDGetTokenT<HBHERecHitCollection> HBHERecHitsEdmToken_;        //name: input edm HCAL HE rec hits collection  
      edm::EDGetTokenT<edm::ValueMap<bool> > phIDCutBasedTightToken_;     //name: tight cut phID algo
      edm::EDGetTokenT<edm::ValueMap<bool> > phIDCutBasedLooseToken_;     //name: loose cut phID algo
//      std::string                      EBRecHitsEdmName_;          //name: input edm ECAL EB rec hits collection
//      std::string                      EERecHitsEdmName_;          //name: input edm ECAL EE rec hits collection
//      std::string                      PFCandsEdmName_;            //name: input edm pfCandidates

      std::string                      mitName_;                   //mit name: photon collection
      std::string                      conversionMapName_;         //name: imp. map wrt conv. elecs
      std::string                      oneLegConversionMapName_;   //name: imp. map wrt conv. elecs
      std::string                      barrelSuperClusterMapName_; //name: imp. map wrt barrel sclus
      std::string                      endcapSuperClusterMapName_; //name: imp. map wrt endcap sclus
      std::string                      pfSuperClusterMapName_;     //name: imp. map wrt pflow sclus 
      std::string                      pfClusterMapName_;          //name: imp. map wrt pflow clus       
      std::string                      pfCandMapName_;             //name: imp. map wrt pfcandidates
      std::string                      photonMapName_;             //name: exported photon map
      mithep::PhotonMap                *photonMap_;                //exported photon map
      mithep::PhotonArr                *photons_;                  //array of Photons
      const mithep::ConversionDecayMap *conversionMap_;            //imp. map wrt conver. electrons
      const mithep::ConversionDecayMap *oneLegConversionMap_;      //imp. map wrt conver. electrons
      const mithep::SuperClusterMap    *barrelSuperClusterMap_;    //map wrt barrel super clusters
      const mithep::SuperClusterMap    *endcapSuperClusterMap_;    //map wrt endcap super clusters
      const mithep::SuperClusterMap    *pfSuperClusterMap_;        //map wrt pflow super clusters  
      const mithep::BasicClusterMap    *pfClusterMap_;             //map wrt pflow basic clusters        
      const mithep::PFCandidateMap     *pfCandMap_;                //map wrt pflow candidates
      EGEnergyCorrector                 ecorr_;
  };
}
#endif
