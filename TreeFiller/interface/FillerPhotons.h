//--------------------------------------------------------------------------------------------------
// $Id: FillerPhotons.h,v 1.16 2011/10/08 18:54:47 bendavid Exp $
//
// FillerPhotons
//
// Implementation of a filler to fill photons into our data structure,
// Including converted photons.
//
// Authors: J.Bendavid
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERPHOTONS_H
#define MITPROD_TREEFILLER_FILLERPHOTONS_H

#include "MitAna/DataTree/interface/PhotonFwd.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"
#include "HiggsAnalysis/HiggsToGammaGamma/interface/EGEnergyCorrector.h"

namespace mithep 
{
  class FillerPhotons : public BaseFiller
  {  
    public:
      FillerPhotons(const edm::ParameterSet &cfg, const char *name, bool active=1);
      ~FillerPhotons();

      void                    BookDataBlock(TreeWriter &tws);
      void                    FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
  
    private:
      std::string             edmName_;                   //edm name of photon collection
      std::string             mitName_;                   //mit name of photon collection
      std::string             conversionMapName_;         //name of imported map wrt conv electrons
      std::string             barrelSuperClusterMapName_; //name of imp. map wrt barrel sclus
      std::string             endcapSuperClusterMapName_; //name of imp. map wrt endcap sclus 
      std::string             phIDCutBasedTightName_;     //name of tight cut phID algo
      std::string             phIDCutBasedLooseName_;     //name of loose cut phID algo
      bool                    enablePhotonFix_;           //do photonfix energy corrections
      mithep::PhotonArr             *photons_;            //array of Photons
      const mithep::ConversionMap   *conversionMap_;      //imported map wrt conversion electrons
      const mithep::SuperClusterMap *barrelSuperClusterMap_;  //map wrt barrel super clusters
      const mithep::SuperClusterMap *endcapSuperClusterMap_;  //map wrt endcap super clusters 
      EGEnergyCorrector       ecorr_;
  };
}
#endif
