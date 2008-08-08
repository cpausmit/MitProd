//--------------------------------------------------------------------------------------------------
// $Id: $
//
// FillerSuperClusters
//
// Imlementation of a filler to fill EDM super clusters into our mithep::SuperCluster data structure.
//
// Authors: Si Xie
//--------------------------------------------------------------------------------------------------

#ifndef TREEFILLER_FILLERSUPERCLUSTERS_H
#define TREEFILLER_FILLERSUPERCLUSTERS_H

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "MitAna/DataUtil/interface/TreeWriter.h"
#include "DataFormats/EgammaReco/interface/SuperClusterFwd.h"
#include "MitAna/DataTree/interface/Collections.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"

namespace mithep 
{
  class FillerSuperClusters : public BaseFiller
  {  
    public:
      FillerSuperClusters(const edm::ParameterSet &cfg, const char *name, bool active=1);
      ~FillerSuperClusters();

      void            BookDataBlock(TreeWriter &tws);
      void 	      FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
      
      const mithep::SuperClusterMap    *GetSuperClusterMap() const { return superClusterMap_; }

    private:
      std::string                           edmName_;
      std::string                           mitName_;

      std::string                           basicClusterMapName_;          //name of imported map wrt basic clusters
      std::string                           superClusterMapName_;          //name of export map

      const mithep::BasicClusterMap         *basicClusterMap_;             //map wrt basic clusters

      mithep::SuperClusterArr               *superClusters_;               //array of super clusters
      mithep::SuperClusterMap               *superClusterMap_;             //map wrt super clusters
      
      
  };
}
#endif
 
