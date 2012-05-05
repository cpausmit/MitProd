//--------------------------------------------------------------------------------------------------
// $Id: FillerSuperClusters.h,v 1.10 2010/06/25 15:18:38 bendavid Exp $
//
// FillerSuperClusters
//
// Implementation of a filler to fill EDM super clusters into our mithep::SuperCluster data
// structure.
//
// Authors: C.Paus, J.Bendavid, S.Xie
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERSUPERCLUSTERS_H
#define MITPROD_TREEFILLER_FILLERSUPERCLUSTERS_H

#include "MitAna/DataTree/interface/SuperClusterCol.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"

class CaloGeometry;
class CaloSubdetectorTopology;
class EcalRecHit;

namespace mithep 
{
  class FillerSuperClusters : public BaseFiller
  {  
    public:
      FillerSuperClusters(const edm::ParameterSet &cfg, const char *name, bool active=1);
      ~FillerSuperClusters();

      void                             BookDataBlock(TreeWriter &tws);
      void 	                       FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
      const mithep::SuperClusterMap   *GetSuperClusterMap() const { return superClusterMap_; }

    protected:
      std::vector<float>               getESHits(double X, double Y, double Z,
						 std::map<DetId, EcalRecHit> rechits_map,
						 const CaloGeometry& geometry,
						 CaloSubdetectorTopology *topology_p, int row);     
      std::vector<float>               getESShape(std::vector<float> ESHits0);

    private:
      std::string                      edmName_;               //edm name of collection
      std::string                      mitName_;               //mit name of collection
      std::string                      basicClusterMapName_;   //name of imp. map wrt basic clus
      std::string                      psClusterMapName_;      //name of imp. map wrt ps clus
      std::string                      caloTowerDetIdMapName_; //name of imp. map wrt caloTowerDetId
      std::string                      superClusterMapName_;   //name of exported map
      std::string                      superClusterIdMapName_; //name of exported id map
      std::string                      caloTowerName_;         //name of calotower collection
      const mithep::BasicClusterMap   *basicClusterMap_;       //map wrt basic clusters
      const mithep::PsClusterMap      *psClusterMap_;          //map wrt preshower clusters
      const mithep::CaloTowerDetIdMap *caloTowerDetIdMap_;    //map wrt calo tower det id's
      mithep::SuperClusterArr         *superClusters_;         //array of super clusters
      mithep::SuperClusterMap         *superClusterMap_;       //map wrt super clusters
      mithep::SuperClusterIdMap       *superClusterIdMap_;     //map of DetIds to superclusters
  };
}
#endif
