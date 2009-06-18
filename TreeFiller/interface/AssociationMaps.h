//--------------------------------------------------------------------------------------------------
// $Id: AssociationMaps.h,v 1.20 2009/03/20 18:45:32 bendavid Exp $
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
#include "DataFormats/CaloRecHit/interface/CaloClusterFwd.h"
#include "DataFormats/CaloRecHit/interface/CaloCluster.h"
#include "DataFormats/EgammaReco/interface/SuperClusterFwd.h"
#include "DataFormats/EgammaReco/interface/SuperCluster.h"
#include "DataFormats/CaloTowers/interface/CaloTowerDetId.h"
#include "MitAna/DataTree/interface/Muon.h"
#include "MitAna/DataTree/interface/MCParticle.h"
#include "MitAna/DataTree/interface/Electron.h"
#include "MitAna/DataTree/interface/Conversion.h"
#include "MitAna/DataTree/interface/Vertex.h"
#include "MitAna/DataTree/interface/PFCandidate.h"
#include "MitAna/DataTree/interface/CaloJet.h"
#include "MitAna/DataTree/interface/PFJet.h"
#include "SimDataFormats/Track/interface/SimTrack.h"
#include "SimDataFormats/Track/interface/SimTrackContainer.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticleFwd.h"
#include "DataFormats/EgammaCandidates/interface/Conversion.h"
#include "DataFormats/EgammaCandidates/interface/ConversionFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "MitEdm/DataFormats/interface/BasePartFwd.h"
#include "DataFormats/JetReco/interface/CaloJetCollection.h"
#include "DataFormats/JetReco/interface/PFJetCollection.h"

namespace mithep
{
  typedef std::pair<int,int> PairIntKey;

  typedef AssociationMap<const mitedm::TrackPtr,            mithep::Track*>        TrackMap;
  typedef AssociationMap<const reco::CaloClusterPtr,        mithep::BasicCluster*> BasicClusterMap;
  typedef AssociationMap<const reco::SuperClusterRef,       mithep::SuperCluster*> SuperClusterMap;
  typedef AssociationMap<const edm::Ptr<reco::GenParticle>, mithep::MCParticle*>   AODGenParticleMap;
  typedef AssociationMap<const int,                         mithep::MCParticle*>   GenParticleBarcodeMap;
  typedef AssociationMap<const SimTrackRef,                 mithep::MCParticle*>   SimTrackMap;
  typedef AssociationMap<const unsigned int,                mithep::MCParticle*>   SimTrackTidMap;
  typedef AssociationMap<const TrackingParticleRef,         mithep::MCParticle*>   TrackingParticleMap;
  typedef AssociationMap<const edm::Ptr<reco::Track>,       mithep::Electron*>     ConversionElectronMap;
  typedef AssociationMap<const reco::ConversionRef,         mithep::Conversion*>   ConversionMap;
  typedef AssociationMap<const mitedm::BasePartPtr,         mithep::Particle*>     BasePartMap;
  typedef AssociationMap<const edm::Ptr<reco::Track>,       mithep::Particle*>     TrackPartMap;
  typedef AssociationMap<const CaloTowerDetId,              mithep::CaloTower*>    CaloTowerMap;
  typedef AssociationMap<const mitedm::VertexPtr,           mithep::Vertex*>       VertexMap;
  typedef AssociationMap<const DetId,                       mithep::SuperCluster*> SuperClusterIdMap;
  typedef AssociationMap<const edm::Ptr<reco::Muon>,        mithep::Muon*>         MuonMap;
  typedef AssociationMap<const reco::PFCandidatePtr,        mithep::PFCandidate*>  PFCandidateMap;
  typedef AssociationMap<const edm::Ptr<reco::CaloJet>,     mithep::CaloJet*>      CaloJetMap;
  typedef AssociationMap<const edm::Ptr<reco::PFJet>,       mithep::PFJet*>        PFJetMap;
}   
#endif
