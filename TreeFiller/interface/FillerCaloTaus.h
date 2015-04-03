//--------------------------------------------------------------------------------------------------
// $Id: FillerCaloTaus.h,v 1.4 2009/09/25 08:42:50 loizides Exp $
//
// FillerCaloTaus
//
// Implementation of a filler to fill EDM calo taus into our mithep::CaloTau data structure.
//
// Authors: C.Loizides, J.Bendavid
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERCALOTAUS_H
#define MITPROD_TREEFILLER_FILLERCALOTAUS_H

#include "MitAna/DataTree/interface/CaloTauFwd.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"

#include "DataFormats/TauReco/interface/CaloTauFwd.h"

namespace mithep 
{
  class FillerCaloTaus : public BaseFiller
  {  
    public:
      FillerCaloTaus(const edm::ParameterSet &cfg, edm::ConsumesCollector&, ObjectService*, const char *name, bool active=1);
      ~FillerCaloTaus();

      void                           BookDataBlock(TreeWriter &tws);
      void                           FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
  
    private:
      edm::EDGetTokenT<reco::CaloTauCollection> edmToken_;         //edm name of jets collection
      std::string                    mitName_;         //mit name of jets collection
      std::string                    trackMapName_;    //name of imported TrackMap
      std::string                    jetMapName_;      //name of imported CaloJetMap
      std::string                    barrelBCMapName_; //name of imported barrel cl map name
      std::string                    endcapBCMapName_; //name of imported endcap cl map name
      const mithep::TrackMap        *trackMap_;        //map wrt Tracks
      const mithep::CaloJetMap      *jetMap_;          //map wrt calojets
      const mithep::BasicClusterMap *barrelBCMap_;     //map wrt barrel Basic Clusters
      const mithep::BasicClusterMap *endcapBCMap_;     //map wrt endcap Basic Clusters
      mithep::CaloTauArr            *taus_;            //array of Jets
  };
}
#endif
