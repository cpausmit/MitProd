import FWCore.ParameterSet.Config as cms

def configureForMiniAOD(treeFiller):
    '''
    Confiugre MitTreeFiller for MiniAOD processing
    '''

    treeFiller.fillers = [
        'MetaInfos',
        'Trigger',
        'MCParticles',
        'MCEventInfo',
        'PileupInfo',
#        'PileupEnergyDensity',
#        'EvtSelData',
        'BeamSpot',
#        'BasicClusters',
        #'PsClusters', # GED preshower clusters are of type CaloClusterCollection - either FillerPsClusters or FillerBasicClusters must be updated
        'SuperClusters',
        'PrimaryVertexes',
        'Muons',
        'Electrons',
        'Conversions',
        'PFCandidates',
        'Photons',
        'AKt4PFJets',
        'AKt8PFJets',
        'PFMet',
        'HPSTaus',
        'DCASig'
    ]
    treeFiller.Trigger.hltEvtEdmName = ''
    treeFiller.Trigger.hltObsEdmName = 'selectedPatTrigger'
    treeFiller.MCParticles.genSource = 'PackedGenParticleCollection'
    treeFiller.MCParticles.genEdmName = 'packedGenParticles'
    treeFiller.SuperClusters = cms.untracked.PSet(
        active                = cms.untracked.bool(True),
        mitName               = cms.untracked.string('SuperClusters'),
        edmName               = cms.untracked.string('reducedEgamma:reducedSuperClusters'),
        ebRecHitsName         = cms.untracked.string(''),
        eeRecHitsName         = cms.untracked.string(''),
        esRecHitsName         = cms.untracked.string(''),
        basicClusterMapName   = cms.untracked.string(''),
        superClusterMapName   = cms.untracked.string('superClusterMap'),
        superClusterIdMapName = cms.untracked.string('superClusterIdMap'),
        fillerType            = cms.untracked.string('FillerSuperClusters')
    )
    treeFiller.PrimaryVertexes.edmName = 'offlineSlimmedPrimaryVertices'
    treeFiller.Muons = cms.untracked.PSet(
        active              = cms.untracked.bool(True),
        mitName             = cms.untracked.string('Muons'),
        edmName             = cms.untracked.string('slimmedMuons'),
        pvEdmName           = cms.untracked.string('offlineSlimmedPrimaryVertices'),
        pvBSEdmName         = cms.untracked.string(''),
        pvBeamSpotName      = cms.untracked.string('offlineBeamSpot'),
        pvbsBeamSpotName    = cms.untracked.string(''),
        globalTrackMapName  = cms.untracked.string(''),
        staTrackMapName     = cms.untracked.string(''),
        staVtxTrackMapName  = cms.untracked.string(''),
        trackerTrackMapName = cms.untracked.string(''),
        muonMapName         = cms.untracked.string('muonMap'),
        pfCandMapName       = cms.untracked.string('PFCandMapName'),
        fitUnbiasedVertex   = cms.untracked.bool(False),
        fillFromPAT         = cms.untracked.bool(True),
        fillerType          = cms.untracked.string('FillerMuons')
    )
    treeFiller.Electrons = cms.untracked.PSet(
        active                           = cms.untracked.bool(True),
        mitName                          = cms.untracked.string('Electrons'),
        edmName                          = cms.untracked.string('slimmedElectrons'),
        generalTracksName                = cms.untracked.string(''),
        gsfTracksName                    = cms.untracked.string(''),
        conversionsName                  = cms.untracked.string(''),
        pvBeamSpotName                   = cms.untracked.string('offlineBeamSpot'),
        pvbsBeamSpotName                 = cms.untracked.string(''),
        gsfTrackMapName                  = cms.untracked.string(''),
        trackerTrackMapName              = cms.untracked.string(''),
        barrelSuperClusterMapName        = cms.untracked.string('superClusterMap'),
        endcapSuperClusterMapName        = cms.untracked.string('superClusterMap'),
        pfEcalBarrelSuperClusterMapName  = cms.untracked.string(''),
        pfEcalEndcapSuperClusterMapName  = cms.untracked.string(''),
        electronMapName                  = cms.untracked.string('electronMap'),
        eIDCutBasedLooseName             = cms.untracked.string('eidLoose'),
        eIDCutBasedTightName             = cms.untracked.string('eidTight'),
        eIDLikelihoodName                = cms.untracked.string(''),
        pvEdmName                        = cms.untracked.string('offlineSlimmedPrimaryVertices'),
        pvBSEdmName                      = cms.untracked.string(''),
        requireClusterAndGsfMap          = cms.untracked.bool(True),
        fitUnbiasedVertex                = cms.untracked.bool(False),
        recomputeConversionInfo          = cms.untracked.bool(False),
        fillFromPAT                      = cms.untracked.bool(True),
        fillerType                       = cms.untracked.string('FillerElectrons')
    )
    treeFiller.Conversions.edmName = 'reducedEgamma:reducedConversions'
    treeFiller.Conversions.stablePartMaps = []
    treeFiller.PFCandidates = cms.untracked.PSet(
        active = cms.untracked.bool(True),
        mitName = cms.untracked.string('PFCandidates'),
        edmName = cms.untracked.string('packedPFCandidates'),
        pfCandMapName = cms.untracked.string('PFCandMapName'),
        pfNoPileupCandMapName = cms.untracked.string('PFNoPileupCandMapName'),
        fillPfNoPileup = cms.untracked.bool(True),
        fillerType = cms.untracked.string('FillerPackedPFCandidates')
    )
    treeFiller.Photons = cms.untracked.PSet(
        active                    = cms.untracked.bool(True),
        mitName                   = cms.untracked.string('Photons'),
        edmName                   = cms.untracked.string('slimmedPhotons'),
        HBHERecHitsEdmName        = cms.untracked.string(''),
        phIDCutBasedTightName     = cms.untracked.string('PhotonCutBasedIDTight'),
        phIDCutBasedLooseName     = cms.untracked.string('PhotonCutBasedIDLoose'),
        conversionMapName         = cms.untracked.string('ConversionsMapName'),
        oneLegConversionMapName   = cms.untracked.string(''), # was never filled
        barrelSuperClusterMapName = cms.untracked.string('superClusterMap'),
        endcapSuperClusterMapName = cms.untracked.string('superClusterMap'),
        requireClusterMap         = cms.untracked.bool(True),
        pfEcalBarrelSuperClusterMapName = cms.untracked.string(''),
        pfEcalEndcapSuperClusterMapName = cms.untracked.string(''),
        photonMapName             = cms.untracked.string('PhotonMapName'),
        fillerType                = cms.untracked.string('FillerPhotons')
    )
    treeFiller.AKt4PFJets.edmName = 'slimmedJets'
    treeFiller.AKt4PFJets.fillerType = 'FillerPFJetsFromPATJets'
    treeFiller.AKt8PFJets.edmName = 'slimmedJetsAK8'
    treeFiller.AKt8PFJets.fillerType = 'FillerPFJetsFromPATJets'
    treeFiller.PFMet.edmName = 'slimmedMETs'
    treeFiller.PFMet.fillerType = 'FillerPFMetFromPATMET'
    treeFiller.HPSTaus.edmName = 'slimmedTaus'
    treeFiller.HPSTaus.fillerType = 'FillerPFTausFromPATTaus'
    treeFiller.DCASig.edmElectronName = 'slimmedElectrons'
    treeFiller.DCASig.edmMuonName = 'slimmedMuons'
    treeFiller.DCASig.edmTauName = 'slimmedTaus'
    treeFiller.DCASig.edmTauType = 'PATTau'
