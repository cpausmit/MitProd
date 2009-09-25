//--------------------------------------------------------------------------------------------------
// $Id: FillerSuperClusters.h,v 1.6 2009/06/15 15:00:25 loizides Exp $
//
// FillerSuperClusters
//
// Implementation of a filler to fill EDM super clusters into our mithep::SuperCluster 
// data structure.
//
// Authors: S.Xie
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERSUPERCLUSTERS_H
#define MITPROD_TREEFILLER_FILLERSUPERCLUSTERS_H

#include "MitAna/DataTree/interface/SuperClusterCol.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"

namespace mithep 
{
  class FillerSuperClusters : public BaseFiller
  {  
    public:
      FillerSuperClusters(const edm::ParameterSet &cfg, const char *name, bool active=1);
      ~FillerSuperClusters();

      void                            BookDataBlock(TreeWriter &tws, const edm::EventSetup &es);
      void 	                      FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
      const mithep::SuperClusterMap  *GetSuperClusterMap() const { return superClusterMap_; }

    private:
      std::string                     edmName_;               //edm name of collection
      std::string                     mitName_;               //mit name of collection
      std::string                     basicClusterMapName_;   //name of imp. map wrt basic clus
      std::string                     superClusterMapName_;   //name of exported map
      std::string                     superClusterIdMapName_; //name of exported id map
      const mithep::BasicClusterMap  *basicClusterMap_;       //map wrt basic clusters
      mithep::SuperClusterArr        *superClusters_;         //array of super clusters
      mithep::SuperClusterMap        *superClusterMap_;       //map wrt super clusters
      mithep::SuperClusterIdMap      *superClusterIdMap_;     //map of DetIds to superclusters
  };
}
#endif
