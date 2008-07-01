//--------------------------------------------------------------------------------------------------
// $Id: Collections.h,v 1.8 2008/06/18 13:23:13 paus Exp $
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
#include "MitAna/DataTree/interface/SimParticle.h"
#include "MitAna/DataTree/interface/Electron.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticleFwd.h"

namespace mithep 
{
  typedef AssociationMap<const reco::TrackRef,      mithep::Track*>        TrackMap;
  typedef AssociationMap<const reco::GsfTrackRef,   mithep::Track*>        GsfTrackMap;
  typedef AssociationMap<const TrackingParticleRef, mithep::SimParticle*>  SimParticleMap;
  typedef AssociationMap<const reco::TrackRef,      mithep::Electron*>     ConversionElectronMap;
}   
#endif
