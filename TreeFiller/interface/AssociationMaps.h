//--------------------------------------------------------------------------------------------------
// AssociationMaps
//
// This header file defines the standard types for the AssociationMaps we want to use.
//
// Authors: C.Paus, J.Bendavid
//--------------------------------------------------------------------------------------------------

#ifndef MITPROD_TREEFILLER_ASSOCIATIONMAPS_H
#define MITPROD_TREEFILLER_ASSOCIATIONMAPS_H

#include "DataFormats/Candidate/interface/CandidateFwd.h"
#include "DataFormats/CaloRecHit/interface/CaloCluster.h"
#include "DataFormats/CaloRecHit/interface/CaloClusterFwd.h"
#include "DataFormats/CaloTowers/interface/CaloTowerDetId.h"
#include "DataFormats/EgammaCandidates/interface/Conversion.h"
#include "DataFormats/EgammaCandidates/interface/ConversionFwd.h"
#include "DataFormats/EgammaReco/interface/SuperCluster.h"
#include "DataFormats/EgammaReco/interface/SuperClusterFwd.h"
#include "DataFormats/JetReco/interface/CaloJetCollection.h"
#include "DataFormats/JetReco/interface/JPTJetCollection.h"
#include "DataFormats/JetReco/interface/PFJetCollection.h"
#include "DataFormats/JetReco/interface/TrackJetCollection.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/TauReco/interface/PFTau.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticleFwd.h"
#include "SimDataFormats/Vertex/interface/SimVertexContainer.h"
#include "MitProd/TreeFiller/interface/AssociationMap.h"
#include "MitEdm/DataFormats/interface/Types.h"
#include "MitEdm/DataFormats/interface/BasePartFwd.h"

#include "MitAna/DataTree/interface/DataObjectFwd.h"
#include "MitAna/DataTree/interface/TrackFwd.h"
#include "MitAna/DataTree/interface/BasicClusterFwd.h"
#include "MitAna/DataTree/interface/PsClusterFwd.h"
#include "MitAna/DataTree/interface/SuperClusterFwd.h"
#include "MitAna/DataTree/interface/MCParticleFwd.h"
#include "MitAna/DataTree/interface/CaloTowerFwd.h"
#include "MitAna/DataTree/interface/ElectronFwd.h"
#include "MitAna/DataTree/interface/MuonFwd.h"
#include "MitAna/DataTree/interface/PFTauFwd.h"
#include "MitAna/DataTree/interface/ConversionFwd.h"
#include "MitAna/DataTree/interface/ParticleFwd.h"
#include "MitAna/DataTree/interface/VertexFwd.h"
#include "MitAna/DataTree/interface/BaseVertexFwd.h"
#include "MitAna/DataTree/interface/PFCandidateFwd.h"
#include "MitAna/DataTree/interface/PhotonFwd.h"
#include "MitAna/DataTree/interface/JetFwd.h"
#include "MitAna/DataTree/interface/CaloJetFwd.h"
#include "MitAna/DataTree/interface/JPTJetFwd.h"
#include "MitAna/DataTree/interface/PFJetFwd.h"
#include "MitAna/DataTree/interface/TrackJetFwd.h"
#include "MitAna/DataTree/interface/DecayParticleFwd.h"

namespace mithep
{
  typedef std::pair<int,int> PairIntKey;

  typedef AssociationMap<const mitedm::TrackPtr,            mithep::Track*>         TrackMap;
  typedef AssociationMap<const reco::CaloClusterPtr,        mithep::BasicCluster*>  BasicClusterMap;
  typedef AssociationMap<const reco::CaloClusterPtr,        mithep::PsCluster*>     PsClusterMap;
  typedef AssociationMap<const reco::SuperClusterRef,       mithep::SuperCluster*>  SuperClusterMap;
  typedef AssociationMap<const int,                         mithep::MCParticle*>    GenParticleBarcodeMap;
  typedef AssociationMap<const unsigned int,                mithep::MCParticle*>    SimTrackTidMap;
  typedef AssociationMap<const TrackingParticleRef,         mithep::MCParticle*>    TrackingParticleMap;
  typedef AssociationMap<const edm::Ptr<reco::Track>,       mithep::Electron*>      ConversionElectronMap;
  typedef AssociationMap<const reco::ConversionRef,         mithep::Conversion*>    ConversionMap;
  typedef AssociationMap<const reco::ConversionRef,         mithep::DecayParticle*> ConversionDecayMap;  
  typedef AssociationMap<const mitedm::BasePartPtr,         mithep::Particle*>      BasePartMap;
  typedef AssociationMap<const edm::Ptr<reco::Track>,       mithep::Particle*>      TrackPartMap;
  typedef AssociationMap<const CaloTowerDetId,              mithep::CaloTower*>     CaloTowerMap;
  typedef AssociationMap<const DetId,                       mithep::CaloTower*>     CaloTowerDetIdMap;
  typedef AssociationMap<const int,                         mithep::BaseVertex*>    GenVertexMap;
  typedef AssociationMap<const edm::SimVertexRef,           mithep::BaseVertex*>    SimVertexMap;
  typedef AssociationMap<const mitedm::VertexPtr,           mithep::Vertex*>        VertexMap;
  typedef AssociationMap<const DetId,                       mithep::SuperCluster*>  SuperClusterIdMap;
  typedef AssociationMap<const edm::Ptr<reco::Muon>,        mithep::Muon*>          MuonMap;
  typedef AssociationMap<const edm::Ptr<reco::GsfElectron>, mithep::Electron*>      ElectronMap;
  typedef AssociationMap<const edm::Ptr<reco::GsfElectron>, mithep::Track*>         ElectronTrackMap;
  typedef AssociationMap<const edm::Ptr<reco::Muon>,        mithep::Track*>         MuonTrackMap;
  typedef AssociationMap<const edm::Ptr<reco::Photon>,      mithep::Photon*>        PhotonMap;
  typedef AssociationMap<const edm::Ptr<reco::BaseTau>,     mithep::PFTau*>         PFTauMap;
  typedef AssociationMap<const reco::CandidatePtr,          mithep::PFCandidate*>   PFCandidateMap;
  typedef AssociationMap<const edm::Ptr<reco::Jet>,         mithep::Jet*>           JetMap;
  typedef AssociationMap<const edm::Ptr<reco::Jet>,         mithep::CaloJet*>       CaloJetMap;
  typedef AssociationMap<const edm::Ptr<reco::Jet>,         mithep::JPTJet*>        JPTJetMap;
  typedef AssociationMap<const edm::Ptr<reco::Jet>,         mithep::PFJet*>         PFJetMap;
  typedef AssociationMap<const edm::Ptr<reco::Jet>,         mithep::TrackJet*>      TrackJetMap;
  typedef AssociationMap<const reco::CandidatePtr,          mithep::DataObject*>    CandidateMap;
}
#endif
