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
    treeFiller.Trigger.hltEvtEdmName = ''
    treeFiller.Trigger.hltObjsEdmName = 'selectedPatTrigger'

    treeFiller.MCParticles.genSource = 'PackedGenParticleCollection'
    treeFiller.MCParticles.genEdmName = 'packedGenParticles'

    treeFiller.EvtSelData.patFilterResultsName = 'TriggerResults::PAT'
    treeFiller.EvtSelData.HBHENoiseFilterName = 'Flag_HBHENoiseFilter'
    treeFiller.EvtSelData.ECALDeadCellFilterName = 'Flag_EcalDeadCellTriggerPrimitiveFilter'  
    treeFiller.EvtSelData.trackingFailureFilterName = 'Flag_trackingFailureFilter'  
    treeFiller.EvtSelData.EEBadScFilterName = 'Flag_eeBadScFilter'  
    treeFiller.EvtSelData.ECALaserCorrFilterName = 'Flag_ecalLaserCorrFilter'  
    treeFiller.EvtSelData.tkManyStripClusName = 'Flag_trkPOG_manystripclus53X'  
    treeFiller.EvtSelData.tkTooManyStripClusName = 'Flag_trkPOG_toomanystripclus53X'  
    treeFiller.EvtSelData.tkLogErrorTooManyClustersName = 'Flag_trkPOG_logErrorTooManyClusters'  
    treeFiller.EvtSelData.BeamHaloSummaryName = 'Flag_CSCTightHaloFilter'  

    treeFiller.SuperClusters.edmName = 'reducedEgamma:reducedSuperClusters'
    treeFiller.SuperClusters.ebRecHitsName = ''
    treeFiller.SuperClusters.eeRecHitsName = ''
    treeFiller.SuperClusters.esRecHitsName = ''
    treeFiller.SuperClusters.basicClusterMapName = ''

    treeFiller.GeneralTracks.edmName = 'unpackedTracksAndVertices'
    treeFiller.GeneralTracks.edmSimAssociationName = ''

    treeFiller.PrimaryVertexes.edmName = 'offlineSlimmedPrimaryVertices'

    treeFiller.Muons.edmName = 'slimmedMuons'
    treeFiller.Muons.pvEdmName = 'offlineSlimmedPrimaryVertices'
    treeFiller.Muons.pvBSEdmName = ''
    treeFiller.Muons.pvBeamSpotName = 'offlineBeamSpot'
    treeFiller.Muons.pvbsBeamSpotName = ''
    treeFiller.Muons.globalTrackMapName = ''
    treeFiller.Muons.staTrackMapName = ''
    treeFiller.Muons.staVtxTrackMapName = ''
    treeFiller.Muons.trackerTrackMapName = ''
    treeFiller.Muons.muonPFMapName = 'muonPFMap'
    treeFiller.Muons.fillFromPAT = True

    treeFiller.Electrons.edmName = 'slimmedElectrons'
    treeFiller.Electrons.generalTracksName = ''
    treeFiller.Electrons.gsfTracksName = ''
    treeFiller.Electrons.conversionsName = ''
    treeFiller.Electrons.pvbsBeamSpotName = ''
    treeFiller.Electrons.gsfTrackMapName = ''
    treeFiller.Electrons.trackerTrackMapName = ''
    treeFiller.Electrons.pfEcalBarrelSuperClusterMapName = ''
    treeFiller.Electrons.pfEcalEndcapSuperClusterMapName = ''
    treeFiller.Electrons.electronPFMapName = 'electronPFMap'
    treeFiller.Electrons.pvEdmName = 'offlineSlimmedPrimaryVertices'
    treeFiller.Electrons.pvBSEdmName = ''
    treeFiller.Electrons.fillFromPAT = True

    treeFiller.Conversions.edmName = 'reducedEgamma:reducedConversions'
    treeFiller.Conversions.stablePartMaps = []

    treeFiller.PFCandidates.edmName = 'packedPFCandidates'
    treeFiller.PFCandidates.electronMapName = 'electronPFMap'
    treeFiller.PFCandidates.muonMapName = 'muonPFMap'
    treeFiller.PFCandidates.photonMapName = 'photonPFMap'
    treeFiller.PFCandidates.fillerType = 'FillerPackedPFCandidates'

    treeFiller.Photons.edmName = 'slimmedPhotons'
    treeFiller.Photons.HBHERecHitsEdmName = ''
    treeFiller.Photons.phIDCutBasedTightName = 'PhotonCutBasedIDTight'
    treeFiller.Photons.phIDCutBasedLooseName = 'PhotonCutBasedIDLoose'
    treeFiller.Photons.pfEcalBarrelSuperClusterMapName = ''
    treeFiller.Photons.pfEcalEndcapSuperClusterMapName = ''
    treeFiller.Photons.photonPFMapName = 'photonPFMap'
    treeFiller.Photons.fillFromPAT = True

    treeFiller.AKt4PFJets.edmName = 'ak4PFJets'

    treeFiller.AKt8PFJets.edmName = 'ak8PFJets'

    treeFiller.AKt4PFJetsCHS.edmName = 'slimmedJets'
    treeFiller.AKt4PFJetsCHS.JetProbabilityBJetTagsName = 'pfJetProbabilityBJetTags'
    treeFiller.AKt4PFJetsCHS.JetBProbabilityBJetTagsName = 'pfJetBProbabilityBJetTags'
    treeFiller.AKt4PFJetsCHS.SimpleSecondaryVertexHighEffBJetTagsName = 'pfSimpleSecondaryVertexHighEffBJetTags'
    treeFiller.AKt4PFJetsCHS.SimpleSecondaryVertexHighPurBJetTagsName = 'pfSimpleSecondaryVertexHighPurBJetTags'
    treeFiller.AKt4PFJetsCHS.CombinedSecondaryVertexBJetTagsName = 'pfCombinedSecondaryVertexBJetTags'
    treeFiller.AKt4PFJetsCHS.CombinedSecondaryVertexV2BJetTagsName = 'pfCombinedSecondaryVertexV2BJetTags'
    treeFiller.AKt4PFJetsCHS.CombinedSecondaryVertexSoftLeptonBJetTagsName = 'pfCombinedSecondaryVertexSoftLeptonBJetTags'
    treeFiller.AKt4PFJetsCHS.CombinedInclusiveSecondaryVertexV2BJetTagsName = 'pfCombinedInclusiveSecondaryVertexV2BJetTags'
    treeFiller.AKt4PFJetsCHS.CombinedMVABJetTagsName = 'pfCombinedMVABJetTags'
    treeFiller.AKt4PFJetsCHS.TrackCountingHighEffBJetTagsName = 'pfTrackCountingHighEffBJetTags'
    treeFiller.AKt4PFJetsCHS.TrackCountingHighPurBJetTagsName = 'pfTrackCountingHighPurBJetTags'
    treeFiller.AKt4PFJetsCHS.fillerType = 'FillerPFJetsFromPATJets'

    treeFiller.AKt8PFJetsCHS.edmName = 'slimmedJetsAK8'
    treeFiller.AKt8PFJetsCHS.fillerType = 'FillerPFJetsFromPATJets'

    treeFiller.PFMet.edmName = 'slimmedMETs'
    treeFiller.PFMet.fillerType = 'FillerPFMetFromPATMET'

    # HPS ID translation table -> http://cmslxr.fnal.gov/lxr/source/PhysicsTools/PatAlgos/python/tools/tauTools.py
    # List of IDs included in pat::Tau -> vhttp://cmslxr.fnal.gov/lxr/source/PhysicsTools/PatAlgos/python/producersLayer1/tauProducer_cfi.py
    treeFiller.HPSTaus.edmName = 'slimmedTaus'
    treeFiller.HPSTaus.DiscriminationByLooseElectronRejectionName = 'againstElectronLoose'
    treeFiller.HPSTaus.DiscriminationByMediumElectronRejectionName = 'againstElectronMedium'
    treeFiller.HPSTaus.DiscriminationByTightElectronRejectionName = 'againstElectronTight'
    treeFiller.HPSTaus.DiscriminationByMVA5VLooseElectronRejectionName = 'againstElectronVLooseMVA5'
    treeFiller.HPSTaus.DiscriminationByMVA5LooseElectronRejectionName = 'againstElectronLooseMVA5'
    treeFiller.HPSTaus.DiscriminationByMVA5MediumElectronRejectionName = 'againstElectronMediumMVA5'
    treeFiller.HPSTaus.DiscriminationByMVA5TightElectronRejectionName = 'againstElectronTightMVA5'
    treeFiller.HPSTaus.DiscriminationByLooseMuonRejectionName = 'againstMuonLoose'
    treeFiller.HPSTaus.DiscriminationByMediumMuonRejectionName = 'againstMuonMedium'
    treeFiller.HPSTaus.DiscriminationByTightMuonRejectionName = 'againstMuonTight'
    treeFiller.HPSTaus.DiscriminationByLooseMuonRejection2Name = 'againstMuonLoose2'
    treeFiller.HPSTaus.DiscriminationByMediumMuonRejection2Name = 'againstMuonMedium2'
    treeFiller.HPSTaus.DiscriminationByTightMuonRejection2Name = 'againstMuonTight2'
    treeFiller.HPSTaus.DiscriminationByLooseMuonRejection3Name = 'againstMuonLoose3'
    treeFiller.HPSTaus.DiscriminationByTightMuonRejection3Name = 'againstMuonTight3'
    treeFiller.HPSTaus.DiscriminationByDecayModeFindingName = 'decayModeFinding'
    treeFiller.HPSTaus.DiscriminationByDecayModeFindingNewDMsName = 'decayModeFindingNewDMs'
    treeFiller.HPSTaus.DiscriminationByDecayModeFindingOldDMsName = ''
    treeFiller.HPSTaus.DiscriminationByVLooseCombinedIsolationDBSumPtCorrName = ''
    treeFiller.HPSTaus.DiscriminationByLooseCombinedIsolationDBSumPtCorrName = ''
    treeFiller.HPSTaus.DiscriminationByMediumCombinedIsolationDBSumPtCorrName = ''
    treeFiller.HPSTaus.DiscriminationByTightCombinedIsolationDBSumPtCorrName = ''
    treeFiller.HPSTaus.DiscriminationByRawCombinedIsolationDBSumPtCorrName = ''
    treeFiller.HPSTaus.DiscriminationByLooseCombinedIsolationDBSumPtCorr3HitsName = 'byLooseCombinedIsolationDeltaBetaCorr3Hits'
    treeFiller.HPSTaus.DiscriminationByMediumCombinedIsolationDBSumPtCorr3HitsName = 'byMediumCombinedIsolationDeltaBetaCorr3Hits'
    treeFiller.HPSTaus.DiscriminationByTightCombinedIsolationDBSumPtCorr3HitsName = 'byTightCombinedIsolationDeltaBetaCorr3Hits'
    treeFiller.HPSTaus.DiscriminationByRawCombinedIsolationDBSumPtCorr3HitsName = 'byCombinedIsolationDeltaBetaCorrRaw3Hits'
    treeFiller.HPSTaus.MVA3IsolationChargedIsoPtSumName = 'chargedIsoPtSum'
    treeFiller.HPSTaus.MVA3IsolationNeutralIsoPtSumName = 'chargedIsoPtSum'
    treeFiller.HPSTaus.MVA3IsolationPUcorrPtSumName = 'puCorrPtSum'
    treeFiller.HPSTaus.fillerType = 'FillerPFTausFromPATTaus'

    treeFiller.DCASig.edmElectronName = 'slimmedElectrons'
    treeFiller.DCASig.edmMuonName = 'slimmedMuons'
    treeFiller.DCASig.edmTauName = 'slimmedTaus'
    treeFiller.DCASig.edmTauType = 'PATTau'

    treeFiller.GenMet.edmName = 'slimmedMETs'
    treeFiller.GenMet.fillFromPAT = True
