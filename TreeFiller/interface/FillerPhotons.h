//--------------------------------------------------------------------------------------------------
// $Id: FillerPhotons.h,v 1.20 2012/05/05 16:49:58 paus Exp $
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

namespace mithep 
{
  class FillerPhotons : public BaseFiller
  {  
    public:
      FillerPhotons(const edm::ParameterSet &cfg, const char *name, bool active=1);
      ~FillerPhotons();

      void                             BookDataBlock(TreeWriter &tws);
      void                             FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
  			               
    private:		               
      std::string                      edmName_;                   //edm name: photon collection
      std::string                      mitName_;                   //mit name: photon collection
      std::string                      conversionMapName_;         //name: imp. map wrt conv. elecs
      std::string                      oneLegConversionMapName_;   //name: imp. map wrt conv. elecs
      std::string                      barrelSuperClusterMapName_; //name: imp. map wrt barrel sclus
      std::string                      endcapSuperClusterMapName_; //name: imp. map wrt endcap sclus
      std::string                      pfSuperClusterMapName_;     //name: imp. map wrt pflow sclus 
      std::string                      pfClusterMapName_;          //name: imp. map wrt pflow clus       
      std::string                      pfCandMapName_;             //name: imp. map wrt pfcandidates
      std::string                      photonMapName_;             //name: exported photon map
      std::string                      phIDCutBasedTightName_;     //name: tight cut phID algo
      std::string                      phIDCutBasedLooseName_;     //name: loose cut phID algo
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
