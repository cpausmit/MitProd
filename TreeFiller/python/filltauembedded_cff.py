# $Id: filltauembedded_cff.py,v 1.1 2011/09/14 15:24:42 bendavid Exp $

import FWCore.ParameterSet.Config as cms

def filltauembedded(filler):

    filler.fillers.insert(filler.fillers.index('GeneralTracks'),'MergedEmbeddedTracks')

    filler.PrimaryVertexes.trackMapName = 'tmfTracksMapName'
    filler.PrimaryVertexesBS.trackMapName = 'tmfTracksMapName'
    filler.PFCandidates.trackerTrackMapNames = cms.untracked.vstring('tmfTracksMapName')
    filler.PFCandidates.gsfTrackMapName = ''        
    filler.PFCandidates.muonMapName = ''        
    filler.ShrinkingConePFTaus.trackMapNames = cms.untracked.vstring('tmfTracksMapName')
    filler.HPSTaus.trackMapNames = cms.untracked.vstring('tmfTracksMapName')
    
    filler.PFCandidates.allowMissingTrackRef = cms.untracked.bool(True)
    filler.ShrinkingConePFTaus.allowMissingTrackRef = cms.untracked.bool(True)    
    filler.HPSTaus.allowMissingTrackRef = cms.untracked.bool(True)        
    
    filler.GeneralTracks.mitName = 'EmbeddedTracks'

    filler.MergedEmbeddedTracks  = cms.untracked.PSet(
        active                    = cms.untracked.bool(True),
        ecalAssocActive           = cms.untracked.bool(False),
        mitName                   = cms.untracked.string('Tracks'),
        edmName                   = cms.untracked.string('tmfTracks'),
        trackingMapName           = cms.untracked.string('TrackingMap'),
        barrelSuperClusterMapName = cms.untracked.string('barrelSuperClusterMap'),
        endcapSuperClusterMapName = cms.untracked.string('endcapSuperClusterMap'),
        trackMapName              = cms.untracked.string('tmfTracksMapName'),
        edmSimAssociationName     = cms.untracked.string('trackingParticleRecoTrackAsssociation'),
        fillerType                = cms.untracked.string('FillerTracks')
    )