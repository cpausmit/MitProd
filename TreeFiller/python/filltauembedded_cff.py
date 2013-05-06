# $Id: filltauembedded_cff.py,v 1.6 2012/12/28 17:27:20 pharris Exp $

import FWCore.ParameterSet.Config as cms

def filltauembedded(filler):

    filler.fillers.insert(filler.fillers.index('GeneralTracks'),'MergedEmbeddedTracks')
    filler.PFBasicClusters.active              = True
    filler.PFPsClusters.active                 = True
    filler.PFSuperClusters.active              = True
    filler.PFPhotonBasicClusters.active        = True
    filler.PFPhotonPsClusters.active           = True
    
    #filler.PrimaryVertexes.trackMapName        = 'tmfTracksMapName'
    #filler.PrimaryVertexesBS.trackMapName      = 'tmfTracksMapName'
    
    filler.Electrons.trackerTrackMapName       = ''
    ##filler.Electrons.gsfTrackMapName           = ''
    filler.Electrons.requireClusterAndGsfMap   = False
    #filler.PFCandidates.trackerTrackMapNames   = cms.untracked.vstring('tmfTracksMapName')
    #filler.PFCandidates.gsfTrackMapName        = ''        
    #filler.PFCandidates.muonMapName            = ''
    #filler.PFCandidates.allowMissingPhotonRef  = True
    #filler.ShrinkingConePFTaus.trackMapNames = cms.untracked.vstring('tmfTracksMapName')
    #filler.HPSTaus.trackMapNames             = cms.untracked.vstring('tmfTracksMapName')
    
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
        edmName                   = cms.untracked.string('generalTracksORG'),
        trackingMapName           = cms.untracked.string('TrackingMap'),
        barrelSuperClusterMapName = cms.untracked.string('barrelSuperClusterMap'),
        endcapSuperClusterMapName = cms.untracked.string('endcapSuperClusterMap'),
        trackMapName              = cms.untracked.string('generalTracksMapName'),
        edmSimAssociationName     = cms.untracked.string('trackingParticleRecoTrackAsssociation'),
        fillerType                = cms.untracked.string('FillerTracks')
    )
