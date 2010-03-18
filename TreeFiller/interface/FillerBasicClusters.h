//--------------------------------------------------------------------------------------------------
// $Id: FillerBasicClusters.h,v 1.6 2009/09/25 08:42:50 loizides Exp $
//
// FillerBasicClusters
//
// Implementation of a filler to fill EDM basic clusters into the
// mithep::BasicCluster data structure.
//
// Authors: S.Xie
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERBASICCLUSTERS_H
#define MITPROD_TREEFILLER_FILLERBASICCLUSTERS_H

#include "MitAna/DataTree/interface/BasicClusterFwd.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"

namespace mithep 
{
  class FillerBasicClusters : public BaseFiller
  {  
    public:
      FillerBasicClusters(const edm::ParameterSet &cfg, const char *name, bool active=1);
      ~FillerBasicClusters();

      void                             BookDataBlock(TreeWriter &tws);
      void 	                       FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
  
    private:
      std::string                      edmName_;             //edm name of clusters
      std::string                      mitName_;             //mit name of clusters
      std::string                      basicClusterMapName_; //name of export map
      mithep::BasicClusterArr         *basicClusters_;       //array of basic clusters
      mithep::BasicClusterMap         *basicClusterMap_;     //map wrt basic Clusters
  };
}
#endif
