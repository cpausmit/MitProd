//--------------------------------------------------------------------------------------------------
// $Id: AssociationMaps.h,v 1.4 2008/07/07 16:14:01 loizides Exp $
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
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrackFwd.h"
#include "MitAna/DataTree/interface/MCParticle.h"
#include "MitAna/DataTree/interface/Electron.h"
#include "MitAna/DataTree/interface/Conversion.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticleFwd.h"
#include "DataFormats/EgammaCandidates/interface/Conversion.h"
#include "DataFormats/EgammaCandidates/interface/ConversionFwd.h"

namespace mithep 
{
  typedef AssociationMap<const reco::TrackRef,       mithep::Track*>        TrackMap;
  typedef AssociationMap<const reco::GsfTrackRef,    mithep::Track*>        GsfTrackMap;
  typedef AssociationMap<const Int_t,                mithep::MCParticle*>   GenParticleMap;
  typedef AssociationMap<const TrackingParticleRef,  mithep::MCParticle*>   SimParticleMap;
  typedef AssociationMap<const reco::TrackRef,       mithep::Electron*>     ConversionElectronMap;
  typedef AssociationMap<const reco::ConversionRef,  mithep::Conversion*>   ConversionMap;
}   
#endif
