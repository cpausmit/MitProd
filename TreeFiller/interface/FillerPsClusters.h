//---------------------------------------------------------------------------------------------------
// $Id: FillerPsClusters.h,v 1.8 2010/03/24 15:40:57 sixie Exp $
//
// FillerPsClusters
//
// Implementation of a filler to fill EDM preshower clusters into the
// mithep::PsCluster data structure.
//
// Authors: C.Paus, S.Xie, J.Bendavid
//---------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_FILLERPSCLUSTERS_H
#define MITPROD_TREEFILLER_FILLERPSCLUSTERS_H

#include "MitAna/DataTree/interface/PsClusterFwd.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"
#include "MitProd/TreeFiller/interface/BaseFiller.h"

#include "DataFormats/EgammaReco/interface/PreshowerClusterFwd.h"

namespace mithep 
{
  class FillerPsClusters : public BaseFiller
  {  
    public:
      FillerPsClusters(const edm::ParameterSet &cfg, edm::ConsumesCollector&, const char *name, bool active=1);
      ~FillerPsClusters();

      void                             BookDataBlock(TreeWriter &tws);
      void 	                       FillDataBlock(const edm::Event &e, const edm::EventSetup &es);
  
    private:
      edm::EDGetTokenT<reco::PreshowerClusterCollection> edmToken_;          //edm name of clusters
      std::string                      mitName_;          //mit name of clusters
      std::string                      psClusterMapName_; //name of export map
      mithep::PsClusterArr            *psClusters_;       //array of basic clusters
      mithep::PsClusterMap            *psClusterMap_;     //map wrt basic Clusters
  };
}
#endif
