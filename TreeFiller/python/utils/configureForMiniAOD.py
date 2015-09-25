import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.Types as Types

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
        'MuonTracks', # miniAOD-specific
        'StandaloneMuonTracks',
        'GlobalMuonTracks',
        'FirstHitMuonTracks',
        'PickyMuonTracks',
        'DYTMuonTracks',
        'ElectronTracks', # miniAOD-specific
        'GsfTracks',
        'PrimaryVertexes',
        'Muons',
        'Electrons',
        'Conversions',
        'Photons',
        'PFCandidates',
        'AKt4PFJets',
        'AKt8PFJets',
        'AKt4PFJetsCHS',
        'AKt8PFJetsCHS',
        'PFMet',
        'HPSTaus',
        'DCASig',
        'AKT4GenJets',
        'AKT8GenJets',
        'GenMet'
    ]

    psets = {
        'Trigger': {
            'hltEvtEdmName': '',
            'hltObjsEdmName': 'selectedPatTrigger'
        },

        'MCParticles': {
            'genSource': 'PackedGenParticleCollection',
            'genEdmName': 'packedGenParticles'
        },

        'EvtSelData': {
            'patFilterResultsName': 'TriggerResults::PAT',
            'HBHENoiseFilterName': 'Flag_HBHENoiseFilter',
            'ECALDeadCellFilterName': 'Flag_EcalDeadCellTriggerPrimitiveFilter',
            'trackingFailureFilterName': 'Flag_trackingFailureFilter',
            'EEBadScFilterName': 'Flag_eeBadScFilter',
            'ECALaserCorrFilterName': 'Flag_ecalLaserCorrFilter',
            'tkManyStripClusName': 'Flag_trkPOG_manystripclus53X',
            'tkTooManyStripClusName': 'Flag_trkPOG_toomanystripclus53X',
            'tkLogErrorTooManyClustersName': 'Flag_trkPOG_logErrorTooManyClusters',
            'BeamHaloSummaryName': 'Flag_CSCTightHaloFilter'
        },

        'SuperClusters': {
            'edmName': 'reducedEgamma:reducedSuperClusters',
            'ebRecHitsName': '',
            'eeRecHitsName': '',
            'esRecHitsName': '',
            'basicClusterMapName': ''
        },

        'GeneralTracks': {
            'edmName': 'packedPFCandidates',
            'edmSimAssociationName': '',
            'trackingMapName': '',
            'sourceType': 'PackedCandidates'
        },

        'MuonTracks': cms.untracked.PSet(
            active = cms.untracked.bool(True),
            sourceType = cms.untracked.string('MuonInner'),
            mitName = cms.untracked.string('MuonTracks'),
            edmName = cms.untracked.string('slimmedMuons'),
            trackMapName = cms.untracked.string('MuonTracksMapName'),
            fillerType = cms.untracked.string('FillerTracks')
        ),

        'StandaloneMuonTracks': {
            'edmName': 'slimmedMuons',
            'sourceType': 'MuonStandalone'
        },
    
        'GlobalMuonTracks': {
            'edmName': 'slimmedMuons',
            'sourceType': 'MuonCombined'
        },
    
        'FirstHitMuonTracks': {
            'edmName': 'slimmedMuons',
            'sourceType': 'MuonTPFMS'
        },
    
        'PickyMuonTracks': {
            'edmName': 'slimmedMuons',
            'sourceType': 'MuonPicky'
        },
    
        'DYTMuonTracks': {
            'edmName': 'slimmedMuons',
            'sourceType': 'MuonDYT'
        },
    
        'ElectronTracks': cms.untracked.PSet(
            active = cms.untracked.bool(True),
            sourceType = cms.untracked.string('ElectronCtf'),
            mitName = cms.untracked.string('ElectronTracks'),
            edmName = cms.untracked.string('slimmedElectrons'),
            trackMapName = cms.untracked.string('ElectronTracksMapName'),
            fillerType = cms.untracked.string('FillerTracks')
        ),
    
        'GsfTracks': {
            'edmName': 'slimmedElectrons',
            'sourceType': 'ElectronGsf',
            'trackingMapName': '',
            'edmSimAssociationName': ''
        },
    
        'PrimaryVertexes': {
            'edmName': 'offlineSlimmedPrimaryVertices',
            'trkAssocByPacked': True
        },

        'Muons': {
            'edmName': 'slimmedMuons',
            'pvEdmName': 'offlineSlimmedPrimaryVertices',
            'pvBSEdmName': '',
            'pvBeamSpotName': 'offlineBeamSpot',
            'pvbsBeamSpotName': '',
            'trackerTrackMapName': 'MuonTracksMapName',
            'muonPFMapName': 'muonPFMap',
            'fillFromPAT': True
        },
    
        'Electrons': {
            'edmName': 'slimmedElectrons',
            'generalTracksName': '',
            'gsfTracksName': '',
            'conversionsName': '',
            'pvbsBeamSpotName': '',
            'footprintName': '',
            'trackerTrackMapName': 'ElectronTracksMapName',
            'pfEcalBarrelSuperClusterMapName': '',
            'pfEcalEndcapSuperClusterMapName': '',
            'electronPFMapName': 'electronPFMap',
            'pvEdmName': 'offlineSlimmedPrimaryVertices',
            'pvBSEdmName': '',
            'fillFromPAT': True
        },

        'Conversions': {
            'edmName': 'reducedEgamma:reducedConversions',
            'stablePartMaps': []
        },
    
        'PFCandidates': cms.untracked.PSet(
            active = cms.untracked.bool(True),
            mitName = cms.untracked.string('PFCandidates'),
            edmName = cms.untracked.string('packedPFCandidates'),
            pfCandMapName = cms.untracked.string('PFCandMapName'),
            fillPfNoPileup = cms.untracked.bool(False),
            electronMapName = cms.untracked.string('electronPFMap'),
            muonMapName = cms.untracked.string('muonPFMap'),
            photonMapName = cms.untracked.string('photonPFMap'),
            trackMapName = cms.untracked.string('TracksMapName'),
            fillerType = cms.untracked.string('FillerPackedPFCandidates')
        ),
    
        'Photons': {
            'edmName': 'slimmedPhotons',
            'HBHERecHitsEdmName': '',
            'phIDCutBasedTightName': 'PhotonCutBasedIDTight',
            'phIDCutBasedLooseName': 'PhotonCutBasedIDLoose',
            'footprintName': '',
            'pfEcalBarrelSuperClusterMapName': '',
            'pfEcalEndcapSuperClusterMapName': '',
            'photonPFMapName': 'photonPFMap',
            'fillFromPAT': True
        },
    
        'AKt4PFJetsCHS': {
            'edmName': 'slimmedJets',
            'fillFromPAT': True,
            'JetProbabilityBJetTagsName': 'pfJetProbabilityBJetTags',
            'JetBProbabilityBJetTagsName': 'pfJetBProbabilityBJetTags',
            'SimpleSecondaryVertexHighEffBJetTagsName': 'pfSimpleSecondaryVertexHighEffBJetTags',
            'SimpleSecondaryVertexHighPurBJetTagsName': 'pfSimpleSecondaryVertexHighPurBJetTags',
            'CombinedSecondaryVertexBJetTagsName': 'pfCombinedSecondaryVertexBJetTags',
            'CombinedSecondaryVertexV2BJetTagsName': 'pfCombinedSecondaryVertexV2BJetTags',
            'CombinedSecondaryVertexSoftLeptonBJetTagsName': 'pfCombinedSecondaryVertexSoftLeptonBJetTags',
            'CombinedInclusiveSecondaryVertexV2BJetTagsName': 'pfCombinedInclusiveSecondaryVertexV2BJetTags',
            'CombinedMVABJetTagsName': 'pfCombinedMVABJetTags',
            'TrackCountingHighEffBJetTagsName': 'pfTrackCountingHighEffBJetTags',
            'TrackCountingHighPurBJetTagsName': 'pfTrackCountingHighPurBJetTags',
            'fillerType': 'FillerPFJets'
        },
    
        'AKt8PFJetsCHS': {
            'edmName': 'slimmedJetsAK8',
            'fillFromPAT': True,
            'fillerType': 'FillerPFJets'
        },
    
        'PFMet': {
            'edmName': 'slimmedMETs',
            'fillerType': 'FillerPFMetFromPATMET'
        },
    
        # HPS ID translation table -> http://cmslxr.fnal.gov/lxr/source/PhysicsTools/PatAlgos/python/tools/tauTools.py
        # List of IDs included in pat::Tau -> vhttp://cmslxr.fnal.gov/lxr/source/PhysicsTools/PatAlgos/python/producersLayer1/tauProducer_cfi.py
        'HPSTaus': {
            'edmName': 'slimmedTaus',
            'DiscriminationByLooseElectronRejectionName': 'againstElectronLoose',
            'DiscriminationByMediumElectronRejectionName': 'againstElectronMedium',
            'DiscriminationByTightElectronRejectionName': 'againstElectronTight',
            'DiscriminationByMVA5VLooseElectronRejectionName': 'againstElectronVLooseMVA5',
            'DiscriminationByMVA5LooseElectronRejectionName': 'againstElectronLooseMVA5',
            'DiscriminationByMVA5MediumElectronRejectionName': 'againstElectronMediumMVA5',
            'DiscriminationByMVA5TightElectronRejectionName': 'againstElectronTightMVA5',
            'DiscriminationByLooseMuonRejectionName': 'againstMuonLoose',
            'DiscriminationByMediumMuonRejectionName': 'againstMuonMedium',
            'DiscriminationByTightMuonRejectionName': 'againstMuonTight',
            'DiscriminationByLooseMuonRejection2Name': 'againstMuonLoose2',
            'DiscriminationByMediumMuonRejection2Name': 'againstMuonMedium2',
            'DiscriminationByTightMuonRejection2Name': 'againstMuonTight2',
            'DiscriminationByLooseMuonRejection3Name': 'againstMuonLoose3',
            'DiscriminationByTightMuonRejection3Name': 'againstMuonTight3',
            'DiscriminationByDecayModeFindingName': 'decayModeFinding',
            'DiscriminationByDecayModeFindingNewDMsName': 'decayModeFindingNewDMs',
            'DiscriminationByDecayModeFindingOldDMsName': '',
            'DiscriminationByVLooseCombinedIsolationDBSumPtCorrName': '',
            'DiscriminationByLooseCombinedIsolationDBSumPtCorrName': '',
            'DiscriminationByMediumCombinedIsolationDBSumPtCorrName': '',
            'DiscriminationByTightCombinedIsolationDBSumPtCorrName': '',
            'DiscriminationByRawCombinedIsolationDBSumPtCorrName': '',
            'DiscriminationByLooseCombinedIsolationDBSumPtCorr3HitsName': 'byLooseCombinedIsolationDeltaBetaCorr3Hits',
            'DiscriminationByMediumCombinedIsolationDBSumPtCorr3HitsName': 'byMediumCombinedIsolationDeltaBetaCorr3Hits',
            'DiscriminationByTightCombinedIsolationDBSumPtCorr3HitsName': 'byTightCombinedIsolationDeltaBetaCorr3Hits',
            'DiscriminationByRawCombinedIsolationDBSumPtCorr3HitsName': 'byCombinedIsolationDeltaBetaCorrRaw3Hits',
            'fillerType': 'FillerPFTausFromPATTaus'
        },
    
        'DCASig': {
            'edmElectronName': 'slimmedElectrons',
            'edmMuonName': 'slimmedMuons',
            'edmTauName': 'slimmedTaus',
            'edmTauType': 'PATTau'
        },
    
        'GenMet': {
            'edmName': 'slimmedMETs',
            'fillFromPAT': True
        }
    }

    # now configure the treeFiller
    for fillerName, params in psets.items():
        if type(params) == Types.PSet:
            setattr(treeFiller, fillerName, params)

        else:
            filler = getattr(treeFiller, fillerName)
            for paramName, value in params.items():
                setattr(filler, paramName, value)
