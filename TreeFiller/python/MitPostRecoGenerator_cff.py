# $Id: MitPostRecoGenerator_cff.py,v 1.5 2009/03/22 08:49:00 loizides Exp $

import FWCore.ParameterSet.Config as cms

#track match
from TrackingTools.TransientTrack.TransientTrackBuilder_cfi import *
from SimGeneral.TrackingAnalysis.trackingParticles_cfi import *
from SimTracker.TrackAssociation.TrackAssociatorByChi2_cfi import *
from SimTracker.TrackAssociation.TrackAssociatorByHits_cfi import *
TrackAssociatorByHits.Cut_RecoToSim = 0.5
from SimTracker.TrackAssociation.trackMCMatch_cfi import *
#from SimTracker.TrackAssociation.standAloneMuonsMCMatch_cfi import *
#from SimTracker.TrackAssociation.globalMuonsMCMatch_cfi import *
#from SimTracker.TrackAssociation.allTrackMCMatch_cfi import *
from SimTracker.TrackAssociation.trackingParticleRecoTrackAsssociation_cff import *

#import SimTracker.TrackAssociation.trackingParticleRecoTrackAsssociation_cfi

#assoc2StandaloneMuons = SimTracker.TrackAssociation.trackingParticleRecoTrackAsssociation_cfi.trackingParticleRecoTrackAsssociation.clone()
#assoc2StandaloneMuons.label_tr = 'standAloneMuons'

#assoc2StandaloneMuonsVtx = SimTracker.TrackAssociation.trackingParticleRecoTrackAsssociation_cfi.trackingParticleRecoTrackAsssociation.clone()
#assoc2StandaloneMuonsVtx.label_tr = 'standAloneMuons:UpdatedAtVtx'

#assoc2GlobalMuons = SimTracker.TrackAssociation.trackingParticleRecoTrackAsssociation_cfi.trackingParticleRecoTrackAsssociation.clone()
#assoc2GlobalMuons.label_tr = 'globalMuons'


#import SimMuon.MCTruth.MuonAssociatorByHits_cfi

#assoc2StandaloneMuons = SimMuon.MCTruth.MuonAssociatorByHits_cfi.muonAssociatorByHits.clone()
#assoc2StandaloneMuons.tracksTag = 'standAloneMuons'

#assoc2StandaloneMuonsVtx = SimMuon.MCTruth.MuonAssociatorByHits_cfi.muonAssociatorByHits.clone()
#assoc2StandaloneMuonsVtx.tracksTag = 'standAloneMuons:UpdatedAtVtx'

#assoc2GlobalMuons = SimMuon.MCTruth.MuonAssociatorByHits_cfi.muonAssociatorByHits.clone()
#assoc2GlobalMuons.tracksTag = 'globalMuons'


trackMCMatchSequence = cms.Sequence(trackingParticleRecoTrackAsssociation*
                                    assoc2GsfTracks*
                                    assocOutInConversionTracks*
                                    assocInOutConversionTracks
                                    )

# define post-reco generator sequence
mit_postreco_generator = cms.Sequence(trackMCMatchSequence)
