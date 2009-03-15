//--------------------------------------------------------------------------------------------------
// $Id: FillerPhotons.h,v 1.9 2009/03/10 15:56:00 loizides Exp $
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

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "MitAna/DataUtil/interface/TreeWriter.h"
#include "MitAna/DataTree/interface/Collections.h"
#include "DataFormats/EgammaCandidates/interface/PhotonFwd.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"

namespace mithep 
{
  class FillerPhotons : public BaseFiller
  {  
    public:
      FillerPhotons(const edm::ParameterSet &cfg, const char *name, bool active=1);
      ~FillerPhotons();

      void BookDataBlock(TreeWriter &tws);
      void FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
  
    private:
      std::string             edmName_;                   //edm name of photon collection
      std::string             mitName_;                   //mit name of photon collection
      std::string             photonIDName_;              //name of Photon ID collection
      std::string             conversionMapName_;         //name of imported map wrt conv electrons
      std::string             barrelSuperClusterMapName_; //name of imp. map wrt barrel sclus
      std::string             endcapSuperClusterMapName_; //name of imp. map wrt endcap sclus 
      mithep::PhotonArr             *photons_;            //array of Photons
      const mithep::ConversionMap   *conversionMap_;      //imported map wrt conversion electrons
      const mithep::SuperClusterMap *barrelSuperClusterMap_;  //map wrt barrel super clusters
      const mithep::SuperClusterMap *endcapSuperClusterMap_;  //map wrt endcap super clusters 
  };
}
#endif
