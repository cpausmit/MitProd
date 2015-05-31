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
        'PileupEnergyDensity',
        'EvtSelData',
        'BeamSpot',
#        #'BasicClusters',
#        #'PsClusters', # GED preshower clusters are of type CaloClusterCollection - either FillerPsClusters or FillerBasicClusters must be updated
        'SuperClusters',
        'GeneralTracks',
        'PrimaryVertexes',
        'Muons',
        'Electrons',
        'Conversions',
        'Photons',
        'PFCandidates',
        'AKt4PFJetsCHS',
        'AKt8PFJetsCHS',
        'PFMet',
        'HPSTaus',
        'DCASig',
        'AKT4GenJets',
        'AKT8GenJets',
        'GenMet'
    ]
    treeFiller.Trigger.hltEvtEdmName = ''
    treeFiller.Trigger.hltObjsEdmName = 'selectedPatTrigger'
    treeFiller.MCParticles.genSource = 'PackedGenParticleCollection'
    treeFiller.MCParticles.genEdmName = 'packedGenParticles'
    treeFiller.EvtSelData = cms.untracked.PSet(
        active = cms.untracked.bool(True),
        mitName = cms.untracked.string('EvtSelData'),
        patFilterResultsName = cms.untracked.string('TriggerResults::PAT'),
        HBHENoiseFilterName = cms.untracked.string('Flag_HBHENoiseFilter'),
        ECALDeadCellFilterName = cms.untracked.string('Flag_EcalDeadCellTriggerPrimitiveFilter'),  
        trackingFailureFilterName = cms.untracked.string('Flag_trackingFailureFilter'),  
        EEBadScFilterName = cms.untracked.string('Flag_eeBadScFilter'),  
        ECALaserCorrFilterName = cms.untracked.string('Flag_ecalLaserCorrFilter'),  
        tkManyStripClusName = cms.untracked.string('Flag_trkPOG_manystripclus53X'),  
        tkTooManyStripClusName = cms.untracked.string('Flag_trkPOG_toomanystripclus53X'),  
        tkLogErrorTooManyClustersName = cms.untracked.string('Flag_trkPOG_logErrorTooManyClusters'),  
        BeamHaloSummaryName = cms.untracked.string('Flag_CSCTightHaloFilter'),  
        fillerType = cms.untracked.string('FillerEvtSelData')
    )
    treeFiller.SuperClusters = cms.untracked.PSet(
        active = cms.untracked.bool(True),
        mitName = cms.untracked.string('SuperClusters'),
        edmName = cms.untracked.string('reducedEgamma:reducedSuperClusters'),
        ebRecHitsName = cms.untracked.string(''),
        eeRecHitsName = cms.untracked.string(''),
        esRecHitsName = cms.untracked.string(''),
        basicClusterMapName = cms.untracked.string(''),
        superClusterMapName = cms.untracked.string('superClusterMap'),
        superClusterIdMapName = cms.untracked.string('superClusterIdMap'),
        fillerType = cms.untracked.string('FillerSuperClusters')
    )
    treeFiller.GeneralTracks.edmName = 'unpackedTracksAndVertices'
    treeFiller.GeneralTracks.edmSimAssociationName = ''
    treeFiller.PrimaryVertexes.edmName = 'offlineSlimmedPrimaryVertices'
    treeFiller.Muons = cms.untracked.PSet(
        active = cms.untracked.bool(True),
        mitName = cms.untracked.string('Muons'),
        edmName = cms.untracked.string('slimmedMuons'),
        pvEdmName = cms.untracked.string('offlineSlimmedPrimaryVertices'),
        pvBSEdmName = cms.untracked.string(''),
        pvBeamSpotName = cms.untracked.string('offlineBeamSpot'),
        pvbsBeamSpotName = cms.untracked.string(''),
        globalTrackMapName = cms.untracked.string(''),
        staTrackMapName = cms.untracked.string(''),
        staVtxTrackMapName = cms.untracked.string(''),
        trackerTrackMapName = cms.untracked.string(''),
        muonMapName = cms.untracked.string('muonMap'),
        muonPFMapName = cms.untracked.string('muonPFMap'),
        fitUnbiasedVertex = cms.untracked.bool(False),
        fillFromPAT = cms.untracked.bool(True),
        fillerType = cms.untracked.string('FillerMuons')
    )
    treeFiller.Electrons = cms.untracked.PSet(
        active = cms.untracked.bool(True),
        mitName = cms.untracked.string('Electrons'),
        edmName = cms.untracked.string('slimmedElectrons'),
        generalTracksName = cms.untracked.string(''),
        gsfTracksName = cms.untracked.string(''),
        conversionsName = cms.untracked.string(''),
        pvBeamSpotName = cms.untracked.string('offlineBeamSpot'),
        pvbsBeamSpotName = cms.untracked.string(''),
        gsfTrackMapName = cms.untracked.string(''),
        trackerTrackMapName = cms.untracked.string(''),
        barrelSuperClusterMapName = cms.untracked.string('superClusterMap'),
        endcapSuperClusterMapName = cms.untracked.string('superClusterMap'),
        pfEcalBarrelSuperClusterMapName = cms.untracked.string(''),
        pfEcalEndcapSuperClusterMapName = cms.untracked.string(''),
        electronMapName = cms.untracked.string('electronMap'),
        electronPFMapName = cms.untracked.string('electronPFMap'),
        eIDCutBasedLooseName = cms.untracked.string('eidLoose'),
        eIDCutBasedTightName = cms.untracked.string('eidTight'),
        eIDLikelihoodName = cms.untracked.string(''),
        pvEdmName = cms.untracked.string('offlineSlimmedPrimaryVertices'),
        pvBSEdmName = cms.untracked.string(''),
        requireClusterAndGsfMap = cms.untracked.bool(True),
        fitUnbiasedVertex = cms.untracked.bool(False),
        recomputeConversionInfo = cms.untracked.bool(False),
        fillFromPAT = cms.untracked.bool(True),
        fillerType = cms.untracked.string('FillerElectrons')
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
        electronMapName = cms.untracked.string('electronPFMap'),
        muonMapName = cms.untracked.string('muonPFMap'),
        photonMapName = cms.untracked.string('photonPFMap'),
        fillerType = cms.untracked.string('FillerPackedPFCandidates')
    )
    treeFiller.Photons = cms.untracked.PSet(
        active = cms.untracked.bool(True),
        mitName = cms.untracked.string('Photons'),
        edmName = cms.untracked.string('slimmedPhotons'),
        HBHERecHitsEdmName = cms.untracked.string(''),
        phIDCutBasedTightName = cms.untracked.string('PhotonCutBasedIDTight'),
        phIDCutBasedLooseName = cms.untracked.string('PhotonCutBasedIDLoose'),
        conversionMapName = cms.untracked.string('ConversionsMapName'),
        oneLegConversionMapName = cms.untracked.string(''), # was never filled
        barrelSuperClusterMapName = cms.untracked.string('superClusterMap'),
        endcapSuperClusterMapName = cms.untracked.string('superClusterMap'),
        requireClusterMap = cms.untracked.bool(True),
        pfEcalBarrelSuperClusterMapName = cms.untracked.string(''),
        pfEcalEndcapSuperClusterMapName = cms.untracked.string(''),
        photonMapName = cms.untracked.string('PhotonMapName'),
        photonPFMapName = cms.untracked.string('photonPFMap'),
        fillFromPAT = cms.untracked.bool(True),
        fillerType = cms.untracked.string('FillerPhotons')
    )
    AKt4PFJetsCHS = cms.untracked.PSet(
        active = cms.untracked.bool(True),
        flavorMatchingActive = cms.untracked.bool(False),
        bTaggingActive = cms.untracked.bool(True),
        jetToVertexActive = cms.untracked.bool(False),
        jetCorrectionsActive = cms.untracked.bool(False),
        fastJetCorrectionsActive = cms.untracked.bool(False),
        mitName = cms.untracked.string('AKt4PFJetsCHS'),
        edmName = cms.untracked.string('slimmedJets'),
        rhoName = cms.untracked.string(''),
        jetToVertexAlphaName = cms.untracked.string(''),
        jetToVertexBetaName = cms.untracked.string(''),
        L2JetCorrectorName = cms.untracked.string(''),
        L3JetCorrectorName = cms.untracked.string(''),
        flavorMatchingByReferenceName = cms.untracked.string('AKT4PFbyRef'),
        flavorMatchingDefinition = cms.untracked.string('Algorithmic'),
        JetProbabilityBJetTagsName = cms.untracked.string('pfJetProbabilityBJetTags'),
        JetBProbabilityBJetTagsName = cms.untracked.string('pfJetBProbabilityBJetTags'),
        SimpleSecondaryVertexHighEffBJetTagsName = cms.untracked.string('pfSimpleSecondaryVertexHighEffBJetTags'),
        SimpleSecondaryVertexHighPurBJetTagsName = cms.untracked.string('pfSimpleSecondaryVertexHighPurBJetTags'),
        CombinedSecondaryVertexBJetTagsName = cms.untracked.string('pfCombinedSecondaryVertexV2BJetTags'),
        CombinedSecondaryVertexMVABJetTagsName = cms.untracked.string('newCombinedMVABJetTags'),
        TrackCountingHighEffBJetTagsName = cms.untracked.string('pfTrackCountingHighEffBJetTags'),
        TrackCountingHighPurBJetTagsName = cms.untracked.string('pfTrackCountingHighPurBJetTags'),
        pfCandMapName = cms.untracked.string('PFCandMapName'),
        jetMapName = cms.untracked.string('AKt4PFJetCHSMap'),
        fillerType = cms.untracked.string('FillerPFJetsFromPATJets')
    ),
    treeFiller.AKt8PFJetsCHS.edmName = 'slimmedJetsAK8'
    treeFiller.AKt8PFJetsCHS.fillerType = 'FillerPFJetsFromPATJets'
    treeFiller.AKt8PFJetsCHS.pfCandMapName = 'PFCandMapName'
    treeFiller.PFMet.edmName = 'slimmedMETs'
    treeFiller.PFMet.fillerType = 'FillerPFMetFromPATMET'
    treeFiller.HPSTaus.edmName = 'slimmedTaus'
    treeFiller.HPSTaus.fillerType = 'FillerPFTausFromPATTaus'
    treeFiller.DCASig.edmElectronName = 'slimmedElectrons'
    treeFiller.DCASig.edmMuonName = 'slimmedMuons'
    treeFiller.DCASig.edmTauName = 'slimmedTaus'
    treeFiller.DCASig.edmTauType = 'PATTau'
    treeFiller.GenMet.edmName = 'slimmedMETs'
    treeFiller.GenMet.fillFromPAT = True
