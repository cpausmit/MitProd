//--------------------------------------------------------------------------------------------------
// $Id: FillerCaloTaus.h,v 1.10 2009/03/15 11:20:40 loizides Exp $
//
// FillerCaloTaus
//
// Implementation of a filler to fill EDM calo taus into our mithep::CaloTau data structure.
//
// Authors: C.Loizides, J.Bendavid
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERCALOTAUS_H
#define MITPROD_TREEFILLER_FILLERCALOTAUS_H

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "MitAna/DataUtil/interface/TreeWriter.h"
#include "MitAna/DataTree/interface/Collections.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"

namespace mithep 
{
  class FillerCaloTaus : public BaseFiller
  {  
    public:
      FillerCaloTaus(const edm::ParameterSet &cfg, const char *name, bool active=1);
      ~FillerCaloTaus();

      void            BookDataBlock(TreeWriter &tws);
      void 	      FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
  
    private:
      std::string     edmName_;                         //edm name of jets collection
      std::string     mitName_;                         //mit name of jets collection
      std::string     trackMapName_;                    //name of imported TrackMap
      std::string     jetMapName_;                      //name of imported CaloJetMap
      std::string     barrelBCMapName_;                 //name of imported barrel BasicCluster Map name
      std::string     endcapBCMapName_;                 //name of imported endcap BasicCluster Map name
      const mithep::TrackMap          *trackMap_;   //map wrt Tracks
      const mithep::CaloJetMap        *jetMap_;     //map wrt calojets
      const mithep::BasicClusterMap   *barrelBCMap_; //map wrt barrel Basic Clusters
      const mithep::BasicClusterMap   *endcapBCMap_; //map wrt endcap Basic Clusters
      mithep::CaloTauArr              *taus_;           //array of Jets
  };
}
#endif
