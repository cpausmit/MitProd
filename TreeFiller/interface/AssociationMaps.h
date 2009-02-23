//--------------------------------------------------------------------------------------------------
// $Id: AssociationMaps.h,v 1.16 2008/11/04 19:26:33 bendavid Exp $
//
// AssociationMaps
//
// This header file defines the standard types for the AssociationMaps we want to use.
//
// Authors: J.Bendavid
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_ASSOCIATIONMAPS_H
#define MITPROD_TREEFILLER_ASSOCIATIONMAPS_H
 
#include "MitProd/TreeFiller/interface/AssociationMap.h"
#include "MitEdm/DataFormats/interface/Types.h"
#include "MitAna/DataTree/interface/Track.h"
#include "MitAna/DataTree/interface/BasicCluster.h"
#include "MitAna/DataTree/interface/CaloTower.h"
#include "MitAna/DataTree/interface/Types.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/EgammaReco/interface/BasicClusterFwd.h"
#include "DataFormats/EgammaReco/interface/BasicCluster.h"
#include "DataFormats/EgammaReco/interface/SuperClusterFwd.h"
#include "DataFormats/EgammaReco/interface/SuperCluster.h"
#include "DataFormats/CaloTowers/interface/CaloTowerDetId.h"
#include "MitAna/DataTree/interface/MCParticle.h"
#include "MitAna/DataTree/interface/Electron.h"
#include "MitAna/DataTree/interface/Conversion.h"
#include "MitAna/DataTree/interface/Vertex.h"
#include "SimDataFormats/Track/interface/SimTrack.h"
#include "SimDataFormats/Track/interface/SimTrackContainer.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticleFwd.h"
#include "DataFormats/EgammaCandidates/interface/Conversion.h"
#include "DataFormats/EgammaCandidates/interface/ConversionFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "MitEdm/DataFormats/interface/BasePartFwd.h"

namespace mithep
{
  typedef std::pair<int,int> PairIntKey;

  typedef AssociationMap<const mitedm::TrackPtr,            mithep::Track*>        TrackMap;
  typedef AssociationMap<const reco::BasicClusterRef,       mithep::BasicCluster*> BasicClusterMap;
  typedef AssociationMap<const reco::SuperClusterRef,       mithep::SuperCluster*> SuperClusterMap;
  typedef AssociationMap<const edm::Ptr<reco::GenParticle>, mithep::MCParticle*>   AODGenParticleMap;
  typedef AssociationMap<const Int_t,                       mithep::MCParticle*>   GenParticleMap;
  typedef AssociationMap<const SimTrackRef,                 mithep::MCParticle*>   SimTrackMap;
  typedef AssociationMap<const TrackingParticleRef,         mithep::MCParticle*>   TrackingParticleMap;
  typedef AssociationMap<const edm::Ptr<reco::Track>,       mithep::Electron*>     ConversionElectronMap;
  typedef AssociationMap<const reco::ConversionRef,         mithep::Conversion*>   ConversionMap;
  typedef AssociationMap<const mitedm::BasePartPtr,         mithep::Particle*>     BasePartMap;
  typedef AssociationMap<const CaloTowerDetId,              mithep::CaloTower*>    CaloTowerMap;
  typedef AssociationMap<const mitedm::VertexPtr,           mithep::Vertex*>       VertexMap;
  typedef AssociationMap<const DetId,                       mithep::SuperCluster*> SuperClusterIdMap;
}   
#endif
