# $Id: filltauembedded_cff.py,v 1.3 2011/11/24 11:40:39 pharris Exp $

import FWCore.ParameterSet.Config as cms

def filltauembedded(filler):

    filler.fillers.insert(filler.fillers.index('GeneralTracks'),'MergedEmbeddedTracks')
    filler.PFBasicClusters.active              = False
    filler.PFPsClusters.active                 = False
    filler.PFSuperClusters.active              = False
    filler.PFPhotonBasicClusters.active        = False
    filler.PFPhotonPsClusters.active           = False
    
    filler.PrimaryVertexes.trackMapName        = 'tmfTracksMapName'
    filler.PrimaryVertexesBS.trackMapName      = 'tmfTracksMapName'
    filler.Electrons.trackerTrackMapName       = ''
    #filler.Electrons.gsfTrackMapName           = ''
    #filler.Electrons.requireClusterAndGsfMap   = False
    filler.PFCandidates.trackerTrackMapNames   = cms.untracked.vstring('tmfTracksMapName')
    filler.PFCandidates.gsfTrackMapName        = ''        
    filler.PFCandidates.muonMapName            = ''        
    filler.ShrinkingConePFTaus.trackMapNames = cms.untracked.vstring('tmfTracksMapName')
    filler.HPSTaus.trackMapNames             = cms.untracked.vstring('tmfTracksMapName')
    
    filler.PFCandidates.allowMissingTrackRef        = cms.untracked.bool(True)
    filler.ShrinkingConePFTaus.allowMissingTrackRef = cms.untracked.bool(True)    
    filler.HPSTaus.allowMissingTrackRef             = cms.untracked.bool(True)        
    filler.EmbedWeight.active                       = cms.untracked.bool(True)
    filler.PFMet.active                             = cms.untracked.bool(True)
    filler.MCParticles.active    =  cms.untracked.bool(True)
    filler.MCParticles.simActive =  cms.untracked.bool(False)

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
