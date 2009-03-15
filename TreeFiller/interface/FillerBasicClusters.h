//--------------------------------------------------------------------------------------------------
// $Id: FillerBasicClusters.h,v 1.2 2008/09/10 03:30:22 loizides Exp $
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

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "MitAna/DataUtil/interface/TreeWriter.h"
#include "MitAna/DataTree/interface/Collections.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"

namespace mithep 
{
  class FillerBasicClusters : public BaseFiller
  {  
    public:
      FillerBasicClusters(const edm::ParameterSet &cfg, const char *name, bool active=1);
      ~FillerBasicClusters();

      void                     BookDataBlock(TreeWriter &tws);
      void 	               FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
      const BasicClusterMap    *GetBasicClusterMap() const { return basicClusterMap_; }
  
    private:
      std::string                      edmName_;             //edm name of clusters
      std::string                      mitName_;             //mit name of clusters
      std::string                      basicClusterMapName_; //name of export map
      mithep::BasicClusterArr         *basicClusters_;       //array of basic clusters
      mithep::BasicClusterMap         *basicClusterMap_;     //map wrt basic Clusters
  };
}
#endif
