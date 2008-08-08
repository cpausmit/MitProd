//--------------------------------------------------------------------------------------------------
// $Id: AssociationMaps.h,v 1.7 2008/07/30 08:39:50 loizides Exp $
//
// AssociationMaps
//
// This header file defines the standard types for the AssociationMaps we want to use.
//
// Authors: J.Bendavid
//--------------------------------------------------------------------------------------------------

#ifndef TREEFILLER_ASSOCIATIONMAPS_H
#define TREEFILLER_ASSOCIATIONMAPS_H
 
#include "MitProd/TreeFiller/interface/AssociationMap.h"
#include "MitAna/DataTree/interface/Track.h"
#include "MitAna/DataTree/interface/BasicCluster.h"
#include "MitAna/DataTree/interface/Types.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/EgammaReco/interface/BasicClusterFwd.h"
#include "DataFormats/EgammaReco/interface/BasicCluster.h"
#include "DataFormats/EgammaReco/interface/SuperClusterFwd.h"
#include "DataFormats/EgammaReco/interface/SuperCluster.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrackFwd.h"
#include "MitAna/DataTree/interface/MCParticle.h"
#include "MitAna/DataTree/interface/Electron.h"
#include "MitAna/DataTree/interface/Conversion.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticleFwd.h"
#include "DataFormats/EgammaCandidates/interface/Conversion.h"
#include "DataFormats/EgammaCandidates/interface/ConversionFwd.h"
#include "MitEdm/DataFormats/interface/BasePartFwd.h"

namespace mithep 
{
  typedef std::pair<int,int> PairIntKey;

  typedef AssociationMap<const reco::TrackRef,        mithep::Track*>        TrackMap;
  typedef AssociationMap<const reco::GsfTrackRef,     mithep::Track*>        GsfTrackMap;
  typedef AssociationMap<const reco::BasicClusterRef, mithep::BasicCluster*> BasicClusterMap;
  typedef AssociationMap<const reco::SuperClusterRef, mithep::SuperCluster*> SuperClusterMap;
  typedef AssociationMap<const Int_t,                 mithep::MCParticle*>   GenParticleMap;
  typedef AssociationMap<const TrackingParticleRef,   mithep::MCParticle*>   SimParticleMap;
  typedef AssociationMap<const reco::TrackRef,        mithep::Electron*>     ConversionElectronMap;
  typedef AssociationMap<const reco::ConversionRef,   mithep::Conversion*>   ConversionMap;
  typedef AssociationMap<const PairIntKey,            mithep::Particle*>     BasePartMap;
}   
#endif
