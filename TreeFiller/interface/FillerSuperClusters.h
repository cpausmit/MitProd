//--------------------------------------------------------------------------------------------------
// $Id: FillerSuperClusters.h,v 1.12 2013/05/15 14:00:29 ksung Exp $
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

#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "DataFormats/EgammaReco/interface/SuperClusterFwd.h"
#include "DataFormats/CaloTowers/interface/CaloTowerFwd.h"

class CaloGeometry;
class CaloSubdetectorTopology;
class EcalRecHit;

namespace mithep 
{
  class FillerSuperClusters : public BaseFiller
  {  
    public:
      FillerSuperClusters(const edm::ParameterSet &cfg, edm::ConsumesCollector&, ObjectService*, const char *name, bool active=1);
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
      void                             SCTimeSpanCalculator(const reco::SuperCluster* scl, 
                                                            double xtalEnergyThr, double seedTime, 
                                                            double& SCLeadTimeSpan,
							    double& SCSubLeadTimeSpan, 
                                                            const EcalRecHitCollection* ebRecHitCol,
							    const EcalRecHitCollection* eeRecHitCol); 

    private:
      edm::EDGetTokenT<reco::SuperClusterCollection> edmToken_;               //edm name of collection
      edm::EDGetTokenT<CaloTowerCollection> caloTowerToken_;         //name of calotower collection
      edm::EDGetTokenT<EcalRecHitCollection> ebRecHitsToken_;         //name of EB rechit collection
      edm::EDGetTokenT<EcalRecHitCollection> eeRecHitsToken_;         //name of EE rechit collection
      edm::EDGetTokenT<EcalRecHitCollection> esRecHitsToken_;         //name of ES rechit collection
      std::string                      mitName_;               //mit name of collection
      std::string                      basicClusterMapName_;   //name of imp. map wrt basic clus
      std::string                      psClusterMapName_;      //name of imp. map wrt ps clus
      std::string                      psXClusterMapName_;     //name of imp. map wrt ps x clus
      std::string                      psYClusterMapName_;     //name of imp. map wrt ps y clus
      std::string                      caloTowerDetIdMapName_; //name of imp. map wrt caloTowerDetId
      std::string                      superClusterMapName_;   //name of exported map
      std::string                      superClusterIdMapName_; //name of exported id map
      const mithep::BasicClusterMap   *basicClusterMap_;       //map wrt basic clusters
      const mithep::PsClusterMap      *psClusterMap_;          //map wrt preshower clusters
      const mithep::PsClusterMap      *psXClusterMap_;         //map wrt preshower x clusters
      const mithep::PsClusterMap      *psYClusterMap_;         //map wrt preshower y clusters      
      const mithep::CaloTowerDetIdMap *caloTowerDetIdMap_;     //map wrt calo tower det id's
      mithep::SuperClusterArr         *superClusters_;         //array of super clusters
      mithep::SuperClusterMap         *superClusterMap_;       //map wrt super clusters
      mithep::SuperClusterIdMap       *superClusterIdMap_;     //map of DetIds to superclusters
  };
}
#endif
