# $Id: MitTreeFiller_cfi.py,v 1.110 2012/03/11 23:11:55 pharris Exp $

import FWCore.ParameterSet.Config as cms

MitTreeFiller = cms.EDAnalyzer("FillMitTree",

    fillers = cms.untracked.vstring('MetaInfos',
                                    'MetaInfosE29',
                                    'MCParticles',
                                    'MCEventInfo',
                                    'MCVertexes',
                                    'PileupInfo',
                                    'PileupEnergyDensity',
                                    'PileupEnergyDensityCHS',
                                    'EvtSelData',
                                    'BeamSpot',
                                    'CaloTowers',
                                    'BarrelBasicClusters',
                                    'BarrelSuperClusters',
                                    'EndcapBasicClusters',
                                    'EndcapSuperClusters',
                                    'PixelHits',
                                    'StripHits',
                                    'PFBasicClusters',
                                    'PFSuperClusters',
                                    'GeneralTracks',
                                    'ConversionStepTracks',
                                    'PixelTracks',
                                    'PixelLessTracks',
                                    'StandaloneMuonTracks',
                                    'StandaloneMuonTracksWVtxConstraint',
                                    'GlobalMuonTracks',
                                    'ConversionInOutTracks',
                                    'ConversionOutInTracks',
                                    'GsfTracks',
                                    'PFGsfTracks',
                                    'PrimaryVertexes',
                                    'DAPrimaryVertexes',
                                    'DAPrimaryVertexesBS',
                                    'PrimaryVertexesBS',
                                    'PixelVertexes',
                                    'TrackletVertexes',
                                    'ClusterVertexes',
                                    'Muons',
                                    'Electrons',
                                    'ElectronsStable',
                                    'ConversionInOutElectronsStable',
                                    'ConversionOutInElectronsStable',
                                    'GsfElectronsStable',
                                    'ConversionStepElectronsStable',
                                    'MergedElectronsStable',
                                    'MergedConversions',
                                    'Conversions',
                                    'Photons',
                                    'PFCandidates',
#                                    'PFCandidatesCHS',
                                    'IC5GenJets',
                                    'KT4GenJets',
                                    'KT6GenJets',
                                    'AKT5GenJets',
                                    'AKT7GenJets',
                                    'ItrCone5Jets',
                                    'Kt4Jets',
                                    'Kt6Jets',
                                    'AKt5Jets',
                                    'AKt7Jets',
#                                    'IC5JetPlusTrack',
                                    'AK5JetPlusTrack',
                                    'AKt5TrackJets',
                                    'Kt4TrackJets',
                                    'ItrCone5PFJets',
                                    'Kt4PFJets',
                                    'Kt4PFJetsNoArea',
                                    'Kt6PFJets',
                                    'AKt5PFJets',
                                    'AKt5PFJetsCHS',
                                    'AKt5PFJetsNoArea',
                                    'AKt7PFJets',
                                    'GenMet',
                                    'CorMuonMet',
#                                     'ItrCone5Met',
#                                     'SisCone5Met',
#                                     'SisCone7Met',
#                                     'Kt4Met',
#                                     'Kt6Met',
                                    'PFMet',
                                    'TCMet',
#                                    'ItrCone5Mht',
#                                    'SisCone5Mht',
#                                    'SisCone7Mht',
#                                    'Kt4Mht',
#                                    'Kt6Mht',
#                                    'IC5JPTMht',
                                    'AssocPFMet',
				    'TrkPFMet',
				    'AssocOtherVtxPFMet',
				    'CentralPFMet',
				    'CleanPFMet',
				    'CaloTaus',
                                    'PFTaus',
                                    'ShrinkingConePFTaus',
				    'HPSTaus',
                                    'StableParts',
                                    'DecayParts',
                                    'EmbedWeight',
                                    'DCASig'),

    TreeWriter = cms.untracked.PSet(
        fileName  = cms.untracked.string('XX-MITDATASET-XX'),
        maxSize   = cms.untracked.uint32(20000),
        compLevel = cms.untracked.uint32(9)
    ),

    MetaInfos = cms.untracked.PSet(
        active             = cms.untracked.bool(True),
        hltActive          = cms.untracked.bool(True),
        hltTreeMitName     = cms.untracked.string('HLT'),
        hltBitsMitName     = cms.untracked.string('HLTBits'),
        hltObjsMitName     = cms.untracked.string('HLTObjects'),
        hltResEdmName      = cms.untracked.string('TriggerResults'),
        hltEvtEdmName      = cms.untracked.string('hltTriggerSummaryAOD'),
        hltProcName        = cms.untracked.string('HLT'),
        l1Active           = cms.untracked.bool(True),
        l1GtRecordEdmName  = cms.untracked.string('l1GtRecord'),   
        l1GtReadRecEdmName = cms.untracked.string('gtDigis'),
        l1TechBitsMitName  = cms.untracked.string('L1TechBits'),
        l1AlgoBitsMitName  = cms.untracked.string('L1AlgoBits'),
        fillerType         = cms.untracked.string('FillerMetaInfos')
    ),

    MetaInfosE29 = cms.untracked.PSet(
        active             = cms.untracked.bool(False),
        hltActive          = cms.untracked.bool(True),
        hltTreeMitName     = cms.untracked.string('HLT_E29'),
        hltBitsMitName     = cms.untracked.string('HLTBits_E29'),
        hltObjsMitName     = cms.untracked.string('HLTObjects_E29'),
        hltResEdmName      = cms.untracked.string('TriggerResults'),
        hltEvtEdmName      = cms.untracked.string('hltTriggerSummaryAOD'),
        hltProcName        = cms.untracked.string('HLT8E29'),
        l1Active           = cms.untracked.bool(False),
        l1GtRecordEdmName  = cms.untracked.string('l1GtRecord8E29'),   
        l1GtReadRecEdmName = cms.untracked.string('gtDigis8E29'),
        l1TechBitsMitName  = cms.untracked.string('L1TechBits_E29'),
        l1AlgoBitsMitName  = cms.untracked.string('L1AlgoBits_E29'),
        fillerType         = cms.untracked.string('FillerMetaInfos')
    ),

    MCParticles = cms.untracked.PSet(
        active          = cms.untracked.bool(False),
        genActive       = cms.untracked.bool(True),
        useAodGen       = cms.untracked.bool(True),
        simActive       = cms.untracked.bool(True),
        trackingActive  = cms.untracked.bool(False),
        fillTracking    = cms.untracked.bool(False),
        mitName         = cms.untracked.string('MCParticles'),
        mitTrackingName = cms.untracked.string('TrackingParticles'),
        genEdmName      = cms.untracked.string('genParticles'),
        simEdmName      = cms.untracked.string('g4SimHits'),
        trackingEdmName = cms.untracked.string('mergedtruth:MergedTrackTruth'),
        genMapName      = cms.untracked.string('GenMap'),
        simMapName      = cms.untracked.string('SimMap'),
        trackingMapName = cms.untracked.string('TrackingMap'), 
        fillerType      = cms.untracked.string('FillerMCParticles')
    ),

    MCEventInfo = cms.untracked.PSet(
        active                = cms.untracked.bool(False),
        genHepMCEventEdmName  = cms.untracked.string('generator'),
        genEvtInfoEdmName     = cms.untracked.string('generator'),
        flavorHistoryActive   = cms.untracked.bool(False),
        flavorHistEdmName     = cms.untracked.string('flavorHistoryFilter'),
        fillerType            = cms.untracked.string('FillerMCEventInfo')
    ),

    MCVertexes = cms.untracked.PSet(
        active        = cms.untracked.bool(False),
        useAodGen     = cms.untracked.bool(True),
        mitName       = cms.untracked.string('MCVertexes'),
        edmName       = cms.untracked.string('genParticles'),    
        fillerType    = cms.untracked.string('FillerMCVertexes')
    ),

    EvtSelData = cms.untracked.PSet(
        active     = cms.untracked.bool(False),
        mitName    = cms.untracked.string('EvtSelData'),
        edmName    = cms.untracked.string('evtSelData'),    
        fillerType = cms.untracked.string('FillerEvtSelData')
    ),

    BeamSpot = cms.untracked.PSet(
        active     = cms.untracked.bool(True),
        mitName    = cms.untracked.string('BeamSpot'),
        edmName    = cms.untracked.string('offlineBeamSpot'),    
        fillerType = cms.untracked.string('FillerBeamSpot')
    ),
                
    PrimaryVertexes = cms.untracked.PSet(
        active        = cms.untracked.bool(True),
        mitName       = cms.untracked.string('PrimaryVertexes'),
        edmName       = cms.untracked.string('offlinePrimaryVertices'),    
        vertexMapName = cms.untracked.string('PrimaryVertexMap'),
        trackMapName  = cms.untracked.string('TracksMapName'),
        fillerType    = cms.untracked.string('FillerVertexes')
    ),

    DAPrimaryVertexes = cms.untracked.PSet(
        active        = cms.untracked.bool(False),
        mitName       = cms.untracked.string('DAPrimaryVertexes'),
        edmName       = cms.untracked.string('offlinePrimaryVerticesDA'),    
        vertexMapName = cms.untracked.string('PrimaryVertexDAMap'),
        trackMapName  = cms.untracked.string('TracksMapName'),
        fillerType    = cms.untracked.string('FillerVertexes')
    ),

    DAPrimaryVertexesBS = cms.untracked.PSet(
        active        = cms.untracked.bool(False),
        mitName       = cms.untracked.string('DAPrimaryVertexesBS'),
        edmName       = cms.untracked.string('offlinePrimaryVerticesDABS'),    
        vertexMapName = cms.untracked.string('PrimaryVertexDABSMap'),
        trackMapName  = cms.untracked.string('TracksMapName'),
        fillerType    = cms.untracked.string('FillerVertexes')
    ),

    PrimaryVertexesBS = cms.untracked.PSet(
        active        = cms.untracked.bool(True),
        mitName       = cms.untracked.string('PrimaryVertexesBeamSpot'),
        edmName       = cms.untracked.string('offlinePrimaryVerticesWithBS'),    
        vertexMapName = cms.untracked.string('PrimaryVertexBSMap'),
        trackMapName  = cms.untracked.string('TracksMapName'),
        fillerType    = cms.untracked.string('FillerVertexes')
    ),

    PixelVertexes = cms.untracked.PSet(
        active        = cms.untracked.bool(False),
        mitName       = cms.untracked.string('PixelVertexes'),
        edmName       = cms.untracked.string('pixelVertices'),    
        vertexMapName = cms.untracked.string('PixelVertMap'),
        fillerType    = cms.untracked.string('FillerVertexes')
    ),

    TrackletVertexes = cms.untracked.PSet(
        active        = cms.untracked.bool(False),
        mitName       = cms.untracked.string('TrackletVertexes'),
        edmName       = cms.untracked.string('trackletVertices'),    
        vertexMapName = cms.untracked.string('TrackletVertMap'),
        fillerType    = cms.untracked.string('FillerVertexes')
    ),

    ClusterVertexes = cms.untracked.PSet(
        active        = cms.untracked.bool(False),
        mitName       = cms.untracked.string('ClusterVertexes'),
        edmName       = cms.untracked.string('clusterVertices'),    
        vertexMapName = cms.untracked.string('ClusterVertMap'),
        fillerType    = cms.untracked.string('FillerVertexes')
    ),

    CaloTowers = cms.untracked.PSet(
        active           = cms.untracked.bool(True),
        mitName          = cms.untracked.string('CaloTowers'),
        edmName          = cms.untracked.string('towerMaker'),    
        caloTowerMapName = cms.untracked.string('CaloTowerMap'),
        fillerType       = cms.untracked.string('FillerCaloTowers')
    ),

    BarrelBasicClusters = cms.untracked.PSet(
        active               = cms.untracked.bool(True),
        mitName              = cms.untracked.string('BarrelBasicClusters'),
        edmName              = cms.untracked.string('hybridSuperClusters:hybridBarrelBasicClusters'),
        barrelEcalRecHitName = cms.untracked.string('reducedEcalRecHitsEB'),
        endcapEcalRecHitName = cms.untracked.string('reducedEcalRecHitsEE'),
        basicClusterMapName  = cms.untracked.string('barrelBasicClusterMap'),
        fillerType           = cms.untracked.string('FillerBasicClusters')
    ),
                                
    BarrelSuperClusters = cms.untracked.PSet(
        active                = cms.untracked.bool(True),
        mitName               = cms.untracked.string('BarrelSuperClusters'),
        edmName               = cms.untracked.string('correctedHybridSuperClusters'),
        basicClusterMapName   = cms.untracked.string('barrelBasicClusterMap'),
        superClusterMapName   = cms.untracked.string('barrelSuperClusterMap'),
        superClusterIdMapName = cms.untracked.string('barrelSuperClusterIdMap'),
        caloTowerName         = cms.untracked.string('towerMaker'),  
        fillerType            = cms.untracked.string('FillerSuperClusters')
    ),

    EndcapBasicClusters = cms.untracked.PSet(
        active              = cms.untracked.bool(True),
        mitName             = cms.untracked.string('EndcapBasicClusters'),
        edmName             = cms.untracked.string('multi5x5SuperClusters:multi5x5EndcapBasicClusters'),  
        barrelEcalRecHitName = cms.untracked.string('reducedEcalRecHitsEB'),
        endcapEcalRecHitName = cms.untracked.string('reducedEcalRecHitsEE'),
        basicClusterMapName = cms.untracked.string('endcapBasicClusterMap'),
        fillerType          = cms.untracked.string('FillerBasicClusters')
    ),

    EndcapSuperClusters = cms.untracked.PSet(
        active                = cms.untracked.bool(True),
        mitName               = cms.untracked.string('EndcapSuperClusters'),                     
        edmName               = cms.untracked.string('correctedMulti5x5SuperClustersWithPreshower'),
        basicClusterMapName   = cms.untracked.string('endcapBasicClusterMap'),
        superClusterMapName   = cms.untracked.string('endcapSuperClusterMap'),
        superClusterIdMapName = cms.untracked.string('endcapSuperClusterIdMap'),
        caloTowerName         = cms.untracked.string('towerMaker'),  
        fillerType            = cms.untracked.string('FillerSuperClusters')
    ),

    PileupInfo = cms.untracked.PSet(
        active                = cms.untracked.bool(False),
        mitName               = cms.untracked.string('PileupInfo'),                     
        edmName               = cms.untracked.string('addPileupInfo'),
        fillerType            = cms.untracked.string('FillerPileupInfo')
    ),

    PileupEnergyDensity = cms.untracked.PSet(
        active                = cms.untracked.bool(False),
        mitName               = cms.untracked.string('Rho'),                     
        edmName               = cms.untracked.InputTag('kt6PFJetsForRhoComputationVoronoi','rho'),
        edmNameLowEta         = cms.untracked.InputTag('kt6PFJetsForRhoComputationVoronoi25','rho'),
        edmNameRandom         = cms.untracked.InputTag('kt6PFJets','rho'),
        edmNameRandomLowEta   = cms.untracked.InputTag('kt6PFJetsForRhoComputationRandom25','rho'),        
        fillerType            = cms.untracked.string('FillerPileupEnergyDensity')
    ),

    PileupEnergyDensityCHS = cms.untracked.PSet(
        active                = cms.untracked.bool(True),
        mitName               = cms.untracked.string('RhoCHS'),                     
        edmName               = cms.untracked.InputTag('kt6PFJetsForRhoComputationVoronoiCHS','rho'),
        edmNameLowEta         = cms.untracked.InputTag('kt6PFJetsForRhoComputationVoronoi25CHS','rho'),
        edmNameRandom         = cms.untracked.InputTag('kt6PFJets','rho'),
        edmNameRandomLowEta   = cms.untracked.InputTag('kt6PFJetsForRhoComputationRandom25CHS','rho'),        
        fillerType            = cms.untracked.string('FillerPileupEnergyDensity')
    ),

    PixelHits = cms.untracked.PSet(
        active                = cms.untracked.bool(False),
        mitName               = cms.untracked.string('PixelHits'),                     
        edmName               = cms.untracked.string('siPixelRecHits'),
        fillerType            = cms.untracked.string('FillerPixelHits')
    ),
  
    StripHits = cms.untracked.PSet(
        active                = cms.untracked.bool(False),
        mitName               = cms.untracked.string('StripHits'),                     
        edmName               = cms.untracked.string('siStripMatchedRecHits:matchedRecHit'),
        fillerType            = cms.untracked.string('FillerStripHits')
    ),
  
    PFBasicClusters = cms.untracked.PSet(
        active              = cms.untracked.bool(True),
        mitName             = cms.untracked.string('PFBasicClusters'),
        edmName             = cms.untracked.string('pfElectronTranslator:pf'),  
        barrelEcalRecHitName = cms.untracked.string('reducedEcalRecHitsEB'),
        endcapEcalRecHitName = cms.untracked.string('reducedEcalRecHitsEE'),
        basicClusterMapName = cms.untracked.string('PFBasicClusterMap'),
        fillerType          = cms.untracked.string('FillerBasicClusters')
    ),
    
    PFSuperClusters = cms.untracked.PSet(
        active                = cms.untracked.bool(True),
        mitName               = cms.untracked.string('PFSuperClusters'),                     
        edmName               = cms.untracked.string('pfElectronTranslator:pf'),
        basicClusterMapName   = cms.untracked.string('PFBasicClusterMap'),
        superClusterMapName   = cms.untracked.string('PFSuperClusterMap'),
        superClusterIdMapName = cms.untracked.string('PFSuperClusterIdMap'),
        fillerType            = cms.untracked.string('FillerSuperClusters')
    ),

    GeneralTracks  = cms.untracked.PSet(
        active                    = cms.untracked.bool(True),
        ecalAssocActive           = cms.untracked.bool(False),
        mitName                   = cms.untracked.string('Tracks'),
        edmName                   = cms.untracked.string('generalTracks'),
        trackingMapName           = cms.untracked.string('TrackingMap'),
        barrelSuperClusterMapName = cms.untracked.string('barrelSuperClusterMap'),
        endcapSuperClusterMapName = cms.untracked.string('endcapSuperClusterMap'),
        trackMapName              = cms.untracked.string('TracksMapName'),
        edmSimAssociationName     = cms.untracked.string('trackingParticleRecoTrackAsssociation'),
        fillerType                = cms.untracked.string('FillerTracks')
    ),

    ConversionStepTracks  = cms.untracked.PSet(
        active                    = cms.untracked.bool(True),
        ecalAssocActive           = cms.untracked.bool(False),
        mitName                   = cms.untracked.string('ConversionStepTracks'),
        edmName                   = cms.untracked.string('conversionStepTracks'),
        trackingMapName           = cms.untracked.string('TrackingMap'),
        barrelSuperClusterMapName = cms.untracked.string('barrelSuperClusterMap'),
        endcapSuperClusterMapName = cms.untracked.string('endcapSuperClusterMap'),
        trackMapName              = cms.untracked.string('ConversionTracksMapName'),
        edmSimAssociationName     = cms.untracked.string(''),
        fillerType                = cms.untracked.string('FillerTracks')
    ),

    PixelTracks  = cms.untracked.PSet(
        active                    = cms.untracked.bool(False),
        ecalAssocActive           = cms.untracked.bool(False),
        mitName                   = cms.untracked.string('PixelTracks'),
        edmName                   = cms.untracked.string('pixelTracks'),
        trackingMapName           = cms.untracked.string(''),
        barrelSuperClusterMapName = cms.untracked.string('barrelSuperClusterMap'),
        endcapSuperClusterMapName = cms.untracked.string('endcapSuperClusterMap'),
        trackMapName              = cms.untracked.string('PixelTracksMapName'),
        edmSimAssociationName     = cms.untracked.string(''),
        fillerType                = cms.untracked.string('FillerTracks')
    ),

    PixelLessTracks  = cms.untracked.PSet(
        active                    = cms.untracked.bool(False),
        ecalAssocActive           = cms.untracked.bool(False),
        mitName                   = cms.untracked.string('PixelLessTracks'),
        edmName                   = cms.untracked.string('ctfPixelLess'),
        trackingMapName           = cms.untracked.string(''),
        barrelSuperClusterMapName = cms.untracked.string('barrelSuperClusterMap'),
        endcapSuperClusterMapName = cms.untracked.string('endcapSuperClusterMap'),
        trackMapName              = cms.untracked.string('PixelLessTracksMapName'),
        edmSimAssociationName     = cms.untracked.string(''),
        fillerType                = cms.untracked.string('FillerTracks')
    ),

    StandaloneMuonTracks = cms.untracked.PSet(
        active                    = cms.untracked.bool(True),
        ecalAssocActive           = cms.untracked.bool(False),
        mitName                   = cms.untracked.string('StandaloneMuonTracks'),
        edmName                   = cms.untracked.string('standAloneMuons'),
        trackingMapName           = cms.untracked.string('TrackingMap'),
        barrelSuperClusterMapName = cms.untracked.string('barrelSuperClusterMap'),
        endcapSuperClusterMapName = cms.untracked.string('endcapSuperClusterMap'),
        trackMapName              = cms.untracked.string('StandaloneMuonTracksMapName'),
        edmSimAssociationName     = cms.untracked.string(''),
        fillerType                = cms.untracked.string('FillerTracks')
    ),

    StandaloneMuonTracksWVtxConstraint = cms.untracked.PSet(
        active                    = cms.untracked.bool(True),
        ecalAssocActive           = cms.untracked.bool(False),
        mitName                   = cms.untracked.string('StandaloneMuonTracksWVtxConstraint'),
        edmName                   = cms.untracked.string('standAloneMuons:UpdatedAtVtx'),
        trackingMapName           = cms.untracked.string('TrackingMap'),
        barrelSuperClusterMapName = cms.untracked.string('barrelSuperClusterMap'),
        endcapSuperClusterMapName = cms.untracked.string('endcapSuperClusterMap'),
        trackMapName              = cms.untracked.string('StandaloneMuonTracksWVtxConstraintMapName'),
        edmSimAssociationName     = cms.untracked.string(''),
        fillerType                = cms.untracked.string('FillerTracks')
    ),
                   
    GlobalMuonTracks = cms.untracked.PSet(
        active                    = cms.untracked.bool(True),
        ecalAssocActive           = cms.untracked.bool(False),
        mitName                   = cms.untracked.string('GlobalMuonTracks'),
        edmName                   = cms.untracked.string('globalMuons'),
        trackingMapName           = cms.untracked.string('TrackingMap'),
        barrelSuperClusterMapName = cms.untracked.string('barrelSuperClusterMap'),
        endcapSuperClusterMapName = cms.untracked.string('endcapSuperClusterMap'),
        trackMapName              = cms.untracked.string('GlobalMuonTracksMapName'),
        edmSimAssociationName     = cms.untracked.string(''),
        fillerType                = cms.untracked.string('FillerTracks')
    ),
                   
    ConversionInOutTracks = cms.untracked.PSet(
        active                    = cms.untracked.bool(True),
        ecalAssocActive           = cms.untracked.bool(False),
        mitName                   = cms.untracked.string('ConversionInOutTracks'),
        edmName                   = cms.untracked.string('ckfInOutTracksFromConversions'),
        trackingMapName           = cms.untracked.string('TrackingMap'),
        barrelSuperClusterMapName = cms.untracked.string('barrelSuperClusterMap'),
        endcapSuperClusterMapName = cms.untracked.string('endcapSuperClusterMap'),
        trackMapName              = cms.untracked.string('ConversionInOutTracksMapName'),
        edmSimAssociationName     = cms.untracked.string('assocInOutConversionTracks'),
        fillerType                = cms.untracked.string('FillerTracks')
    ),

    ConversionOutInTracks = cms.untracked.PSet(
        active                    = cms.untracked.bool(True),
        ecalAssocActive           = cms.untracked.bool(False),
        mitName                   = cms.untracked.string('ConversionOutInTracks'),
        edmName                   = cms.untracked.string('ckfOutInTracksFromConversions'),
        trackingMapName           = cms.untracked.string('TrackingMap'),
        barrelSuperClusterMapName = cms.untracked.string('barrelSuperClusterMap'),
        endcapSuperClusterMapName = cms.untracked.string('endcapSuperClusterMap'),
        trackMapName              = cms.untracked.string('ConversionOutInTracksMapName'),
        edmSimAssociationName     = cms.untracked.string('assocOutInConversionTracks'),
        fillerType                = cms.untracked.string('FillerTracks')
    ),
                   
    GsfTracks = cms.untracked.PSet(
        active                    = cms.untracked.bool(True),
        ecalAssocActive           = cms.untracked.bool(False),
        mitName                   = cms.untracked.string('GsfTracks'),
        edmName                   = cms.untracked.string('electronGsfTracks'),
        trackingMapName           = cms.untracked.string('TrackingMap'),
        barrelSuperClusterMapName = cms.untracked.string('barrelSuperClusterMap'),
        endcapSuperClusterMapName = cms.untracked.string('endcapSuperClusterMap'),
        trackMapName              = cms.untracked.string('GsfTracksMapName'),
        edmSimAssociationName     = cms.untracked.string('assoc2GsfTracks'),
        fillerType                = cms.untracked.string('FillerTracks')
    ),
    
    PFGsfTracks = cms.untracked.PSet(
        active                    = cms.untracked.bool(False),
        ecalAssocActive           = cms.untracked.bool(False),
        mitName                   = cms.untracked.string('PFGsfTracks'),
        edmName                   = cms.untracked.string('gsfPFtracks'),
        trackingMapName           = cms.untracked.string('TrackingMap'),
        barrelSuperClusterMapName = cms.untracked.string('barrelSuperClusterMap'),
        endcapSuperClusterMapName = cms.untracked.string('endcapSuperClusterMap'),
        trackMapName              = cms.untracked.string('PFGsfTracksMapName'),
        edmSimAssociationName     = cms.untracked.string(''),
        fillerType                = cms.untracked.string('FillerTracks')
    ),
  
    Muons = cms.untracked.PSet(
        active              = cms.untracked.bool(True),
        mitName             = cms.untracked.string('Muons'),
        edmName             = cms.untracked.string('muons'),
        expectedHitsName                 = cms.untracked.string(''),        
        globalTrackMapName  = cms.untracked.string('GlobalMuonTracksMapName'),
        staTrackMapName     = cms.untracked.string('StandaloneMuonTracksMapName'),
        staVtxTrackMapName  = cms.untracked.string('StandaloneMuonTracksWVtxConstraintMapName'),
        trackerTrackMapName = cms.untracked.string('TracksMapName'),
        muonMapName         = cms.untracked.string('muonMap'),
        pvEdmName           = cms.untracked.string('offlinePrimaryVertices'),
        pvBSEdmName         = cms.untracked.string('offlinePrimaryVerticesWithBS'),        
        fitUnbiasedVertex   = cms.untracked.bool(False),
        fillerType          = cms.untracked.string('FillerMuons')
    ),
                   
    Electrons = cms.untracked.PSet(
        active                           = cms.untracked.bool(True),
        mitName                          = cms.untracked.string('Electrons'),
        edmName                          = cms.untracked.string('gsfElectrons'),
        expectedHitsName                 = cms.untracked.string(''),
        gsfTrackMapName                  = cms.untracked.string('GsfTracksMapName'), 
        trackerTrackMapName              = cms.untracked.string('TracksMapName'),
        barrelEcalRecHitName             = cms.untracked.string('reducedEcalRecHitsEB'),
        endcapEcalRecHitName             = cms.untracked.string('reducedEcalRecHitsEE'),
        barrelBasicClusterName           = cms.untracked.string('hybridSuperClusters:hybridBarrelBasicClusters'),
        endcapBasicClusterName           = cms.untracked.string('multi5x5BasicClusters:multi5x5EndcapBasicClusters'),
        barrelSuperClusterName           = cms.untracked.string('correctedHybridSuperClusters'),
        endcapSuperClusterName           = cms.untracked.string('multi5x5SuperClusters:multi5x5EndcapSuperClusters'),
        barrelSuperClusterMapName        = cms.untracked.string('barrelSuperClusterMap'),
        endcapSuperClusterMapName        = cms.untracked.string('endcapSuperClusterMap'),
        requireClusterAndGsfMap          = cms.untracked.bool(True),
        pfSuperClusterMapName            = cms.untracked.string('PFSuperClusterMap'),
        electronMapName                  = cms.untracked.string('electronMap'),
        eIDCutBasedLooseName             = cms.untracked.string('eidLoose'),
        eIDCutBasedTightName             = cms.untracked.string('eidTight'),
        eIDLikelihoodName                = cms.untracked.string(''),        
        IsolationTrackCollectionName     = cms.untracked.string('generalTracks'),
        IsolationCaloTowerCollectionName = cms.untracked.string('towerMaker'),
        EcalJurassicIsolationName        = cms.untracked.string('eleIsoFromDepsEcalFromHits'),
        HcalJurassicIsolationName        = cms.untracked.string('eleIsoFromDepsHcalFromHits'),
        TrackerIsolationName             = cms.untracked.string('eleIsoFromDepsTk'),
        gsfTrackAssocName                = cms.untracked.string(''),
        pvEdmName                        = cms.untracked.string('offlinePrimaryVertices'),
        pvBSEdmName                      = cms.untracked.string('offlinePrimaryVerticesWithBS'),
        fitUnbiasedVertex                = cms.untracked.bool(False),
        recomputeConversionInfo          = cms.untracked.bool(False),
        fillerType                       = cms.untracked.string('FillerElectrons')
    ),
    
    MergedConversions = cms.untracked.PSet(
        active              = cms.untracked.bool(True),
        mitName             = cms.untracked.string('MergedConversions'),
        edmName             = cms.untracked.string('allConversions'),
        stablePartMaps      = cms.untracked.vstring('ElectronsStableTrackMapName',
                                                    'ConversionInOutElectronsStableTrackMapName',
                                                    'ConversionOutInElectronsStableTrackMapName',
                                                    'ElectronsStableGsfTrackMapName',
                                                    'ElectronsStableConvStepTrackMapName'),
        conversionMapName   = cms.untracked.string('MergedConversionsMapName'),
        fillerType          = cms.untracked.string('FillerConversionsDecay')
    ),      
      
    Conversions = cms.untracked.PSet(
        active              = cms.untracked.bool(True),
        mitName             = cms.untracked.string('Conversions'),
        edmName             = cms.untracked.string('conversions'),
        stablePartMaps      = cms.untracked.vstring('ElectronsStableTrackMapName',
                                                    'ConversionInOutElectronsStableTrackMapName',
                                                    'ConversionOutInElectronsStableTrackMapName'),
        conversionMapName   = cms.untracked.string('ConversionsMapName'),
        fillerType          = cms.untracked.string('FillerConversions')
    ),

    Photons = cms.untracked.PSet(
        active                    = cms.untracked.bool(True),
        mitName                   = cms.untracked.string('Photons'),
        edmName                   = cms.untracked.string('photons'),
        photonIDName              = cms.untracked.string('PhotonIDProd:PhotonAssociatedID'),
        conversionMapName         = cms.untracked.string('ConversionsMapName'),
        barrelSuperClusterMapName = cms.untracked.string('barrelSuperClusterMap'),
        endcapSuperClusterMapName = cms.untracked.string('endcapSuperClusterMap'),
        enablePhotonFix           = cms.untracked.bool(False),
        fillerType                = cms.untracked.string('FillerPhotons')
    ),

    PFCandidates = cms.untracked.PSet(
        active              = cms.untracked.bool(True),
        mitName             = cms.untracked.string('PFCandidates'),
        edmName             = cms.untracked.string('particleFlow'),
        edmNoPileupName     = cms.untracked.string('pfNoElectron'),
        trackerTrackMapNames = cms.untracked.vstring('TracksMapName', 'ConversionTracksMapName'),
        gsfTrackMapName     = cms.untracked.string('GsfTracksMapName'),
        muonMapName         = cms.untracked.string('MuonMapName'),
        conversionMapName   = cms.untracked.string('ConversionsMapName'),
        pfCandMapName       = cms.untracked.string('PFCandMapName'),
        pfNoPileupCandCandMapName = cms.untracked.string('PFNoPileupCandMapName'),
        fillerType          = cms.untracked.string('FillerPFCandidates')
    ),

    IC5GenJets = cms.untracked.PSet(
        active                        = cms.untracked.bool(False),
        flavorMatchingActive          = cms.untracked.bool(False),
        mitName                       = cms.untracked.string('IC5GenJets'),
        edmName                       = cms.untracked.string('iterativeCone5GenJets'),
        flavorMatchingByReferenceName = cms.untracked.string('GenIC5byRef'),
        flavorMatchingDefinition      = cms.untracked.string('Algorithmic'),
        fillerType                    = cms.untracked.string('FillerGenJets')
    ),

    SC5GenJets = cms.untracked.PSet(
        active                        = cms.untracked.bool(False),
        flavorMatchingActive          = cms.untracked.bool(False),
        mitName                       = cms.untracked.string('SC5GenJets'),
        edmName                       = cms.untracked.string('sisCone5GenJets'),
        flavorMatchingByReferenceName = cms.untracked.string('GenSC5byRef'),
        flavorMatchingDefinition      = cms.untracked.string('Algorithmic'),
        fillerType                    = cms.untracked.string('FillerGenJets')
    ),  
                             
    SC7GenJets = cms.untracked.PSet(
        active                        = cms.untracked.bool(False),
        flavorMatchingActive          = cms.untracked.bool(False),
        mitName                       = cms.untracked.string('SC7GenJets'),
        edmName                       = cms.untracked.string('sisCone7GenJets'),
        flavorMatchingByReferenceName = cms.untracked.string('GenSC7byRef'),
        flavorMatchingDefinition      = cms.untracked.string('Algorithmic'),
        fillerType                    = cms.untracked.string('FillerGenJets')
    ),  
    
    KT4GenJets = cms.untracked.PSet(
        active                        = cms.untracked.bool(False),
        flavorMatchingActive          = cms.untracked.bool(False),
        mitName                       = cms.untracked.string('KT4GenJets'),
        edmName                       = cms.untracked.string('kt4GenJets'),
        flavorMatchingByReferenceName = cms.untracked.string('GenKT4byRef'),
        flavorMatchingDefinition      = cms.untracked.string('Algorithmic'),
        fillerType                    = cms.untracked.string('FillerGenJets')
    ), 

    KT6GenJets = cms.untracked.PSet(
        active                        = cms.untracked.bool(False),
        flavorMatchingActive          = cms.untracked.bool(False),
        mitName                       = cms.untracked.string('KT6GenJets'),
        edmName                       = cms.untracked.string('kt6GenJets'),
        flavorMatchingByReferenceName = cms.untracked.string('GenKT6byRef'),
        flavorMatchingDefinition      = cms.untracked.string('Algorithmic'),
        fillerType                    = cms.untracked.string('FillerGenJets')
    ),     

    AKT5GenJets = cms.untracked.PSet(
        active                        = cms.untracked.bool(False),
        flavorMatchingActive          = cms.untracked.bool(False),
        mitName                       = cms.untracked.string('AKT5GenJets'),
        edmName                       = cms.untracked.string('ak5GenJets'),
        flavorMatchingByReferenceName = cms.untracked.string('GenAKT5byRef'),
        flavorMatchingDefinition      = cms.untracked.string('Algorithmic'),
        fillerType                    = cms.untracked.string('FillerGenJets')
    ), 

    AKT7GenJets = cms.untracked.PSet(
        active                        = cms.untracked.bool(False),
        flavorMatchingActive          = cms.untracked.bool(False),
        mitName                       = cms.untracked.string('AKT7GenJets'),
        edmName                       = cms.untracked.string('ak7GenJets'),
        flavorMatchingByReferenceName = cms.untracked.string('GenAKT7byRef'),
        flavorMatchingDefinition      = cms.untracked.string('Algorithmic'),
        fillerType                    = cms.untracked.string('FillerGenJets')
    ), 

    ItrCone5Jets = cms.untracked.PSet(
        active                                 = cms.untracked.bool(True),
        flavorMatchingActive                   = cms.untracked.bool(False),
        bTaggingActive                         = cms.untracked.bool(False),
        jetToVertexActive                      = cms.untracked.bool(False),
        jetCorrectionsActive                   = cms.untracked.bool(False),
        jetIDActive                            = cms.untracked.bool(True),
        mitName                                = cms.untracked.string('ItrCone5Jets'),
        edmName                                = cms.untracked.string('iterativeCone5CaloJets'),
        jetToVertexAlphaName                   = cms.untracked.string('jetVertexAlphaIC5:Var'),
        jetToVertexBetaName                    = cms.untracked.string('jetVertexBetaIC5:Var'),
        L2JetCorrectorName                     = cms.untracked.string('L2RelativeJetCorrectorIC5Calo'),
        L3JetCorrectorName                     = cms.untracked.string('L3AbsoluteJetCorrectorIC5Calo'),
        jetIDName                              = cms.untracked.string('ic5JetID'),
        flavorMatchingByReferenceName          = cms.untracked.string('IC5byRef'),
        flavorMatchingDefinition               = cms.untracked.string('Algorithmic'),      
        caloTowerMapName                       = cms.untracked.string('CaloTowerMap'),
        jetMapName                             = cms.untracked.string('ItrCone5JetMap'),
        fillerType                             = cms.untracked.string('FillerCaloJets')
    ),

    SisCone5Jets = cms.untracked.PSet(
        active                        = cms.untracked.bool(True),
        flavorMatchingActive          = cms.untracked.bool(False),
        bTaggingActive                = cms.untracked.bool(False),
        jetToVertexActive             = cms.untracked.bool(False),
        jetCorrectionsActive          = cms.untracked.bool(False),
        jetIDActive                   = cms.untracked.bool(True),
        mitName                       = cms.untracked.string('SisCone5Jets'),
        edmName                       = cms.untracked.string('sisCone5CaloJets'),
        jetToVertexAlphaName          = cms.untracked.string('jetVertexAlphaSC5:Var'),
        jetToVertexBetaName           = cms.untracked.string('jetVertexBetaSC5:Var'),
        L2JetCorrectorName            = cms.untracked.string('L2RelativeJetCorrectorSC5Calo'),
        L3JetCorrectorName            = cms.untracked.string('L3AbsoluteJetCorrectorSC5Calo'),
        jetIDName                     = cms.untracked.string('sc5JetID'),
        flavorMatchingByReferenceName = cms.untracked.string('SC5byRef'),
        flavorMatchingDefinition      = cms.untracked.string('Algorithmic'),
        caloTowerMapName              = cms.untracked.string('CaloTowerMap'),
        jetMapName                    = cms.untracked.string('SisCone5JetMap'),
        fillerType                    = cms.untracked.string('FillerCaloJets')
    ),                          

    SisCone7Jets = cms.untracked.PSet(
        active                        = cms.untracked.bool(True),
        flavorMatchingActive          = cms.untracked.bool(False),
        bTaggingActive                = cms.untracked.bool(False),
        jetToVertexActive             = cms.untracked.bool(False),
        jetCorrectionsActive          = cms.untracked.bool(False),
        jetIDActive                   = cms.untracked.bool(True),
        mitName                       = cms.untracked.string('SisCone7Jets'),
        edmName                       = cms.untracked.string('sisCone7CaloJets'),
        jetToVertexAlphaName          = cms.untracked.string('jetVertexAlphaSC7:Var'),
        jetToVertexBetaName           = cms.untracked.string('jetVertexBetaSC7:Var'),
        L2JetCorrectorName            = cms.untracked.string('L2RelativeJetCorrectorSC7Calo'),
        L3JetCorrectorName            = cms.untracked.string('L3AbsoluteJetCorrectorSC7Calo'),
        jetIDName                     = cms.untracked.string('sc7JetID'),
        flavorMatchingByReferenceName = cms.untracked.string('SC7byRef'),
        flavorMatchingDefinition      = cms.untracked.string('Algorithmic'),
        caloTowerMapName              = cms.untracked.string('CaloTowerMap'),
        jetMapName                    = cms.untracked.string('SisCone7JetMap'),
        fillerType                    = cms.untracked.string('FillerCaloJets')
    ),

    Kt4Jets = cms.untracked.PSet(
        active                        = cms.untracked.bool(True),
        flavorMatchingActive          = cms.untracked.bool(False),
        bTaggingActive                = cms.untracked.bool(False),
        jetToVertexActive             = cms.untracked.bool(False),
        jetCorrectionsActive          = cms.untracked.bool(False),
        jetIDActive                   = cms.untracked.bool(True),
        mitName                       = cms.untracked.string('Kt4Jets'),
        edmName                       = cms.untracked.string('kt4CaloJets'),
        jetToVertexAlphaName          = cms.untracked.string('jetVertexAlphaKT4:Var'),
        jetToVertexBetaName           = cms.untracked.string('jetVertexBetaKT4:Var'),
        L2JetCorrectorName            = cms.untracked.string('L2RelativeJetCorrectorKT4Calo'),
        L3JetCorrectorName            = cms.untracked.string('L3AbsoluteJetCorrectorKT4Calo'),
        jetIDName                     = cms.untracked.string('kt4JetID'),
        flavorMatchingByReferenceName = cms.untracked.string('KT4byRef'),
        flavorMatchingDefinition      = cms.untracked.string('Algorithmic'),
        caloTowerMapName              = cms.untracked.string('CaloTowerMap'),
        jetMapName                    = cms.untracked.string('KT4JetMap'),
        fillerType                    = cms.untracked.string('FillerCaloJets')
    ),

    Kt6Jets = cms.untracked.PSet(
        active                        = cms.untracked.bool(True),
        flavorMatchingActive          = cms.untracked.bool(False),
        bTaggingActive                = cms.untracked.bool(False),
        jetToVertexActive             = cms.untracked.bool(False),
        jetCorrectionsActive          = cms.untracked.bool(False),
        jetIDActive                   = cms.untracked.bool(True),
        mitName                       = cms.untracked.string('Kt6Jets'),
        edmName                       = cms.untracked.string('kt6CaloJets'),
        jetToVertexAlphaName          = cms.untracked.string('jetVertexAlphaKT6:Var'),
        jetToVertexBetaName           = cms.untracked.string('jetVertexBetaKT6:Var'),
        L2JetCorrectorName            = cms.untracked.string('L2RelativeJetCorrectorKT6Calo'),
        L3JetCorrectorName            = cms.untracked.string('L3AbsoluteJetCorrectorKT6Calo'),
        jetIDName                     = cms.untracked.string('kt6JetID'),
        flavorMatchingByReferenceName = cms.untracked.string('KT6byRef'),
        flavorMatchingDefinition      = cms.untracked.string('Algorithmic'),
        caloTowerMapName              = cms.untracked.string('CaloTowerMap'),
        jetMapName                    = cms.untracked.string('KT6JetMap'),
        fillerType                    = cms.untracked.string('FillerCaloJets')
    ),

    AKt5Jets = cms.untracked.PSet(
        active                        = cms.untracked.bool(True),
        flavorMatchingActive          = cms.untracked.bool(False),
        bTaggingActive                = cms.untracked.bool(True),
        jetToVertexActive             = cms.untracked.bool(False),
        jetCorrectionsActive          = cms.untracked.bool(False),
        jetIDActive                   = cms.untracked.bool(True),
        mitName                       = cms.untracked.string('AKt5Jets'),
        edmName                       = cms.untracked.string('ak5CaloJets'),
        jetToVertexAlphaName          = cms.untracked.string('jetVertexAlphaAKT5:Var'),
        jetToVertexBetaName           = cms.untracked.string('jetVertexBetaAKT5:Var'),
        L2JetCorrectorName            = cms.untracked.string('L2RelativeJetCorrectorAK5Calo'),
        L3JetCorrectorName            = cms.untracked.string('L3AbsoluteJetCorrectorAK5Calo'),
        jetIDName                     = cms.untracked.string('ak5JetID'),
        flavorMatchingByReferenceName = cms.untracked.string('AKT5byRef'),
        flavorMatchingDefinition      = cms.untracked.string('Algorithmic'),
        caloTowerMapName              = cms.untracked.string('CaloTowerMap'),
        jetMapName                    = cms.untracked.string('AKT5JetMap'),
        fillerType                    = cms.untracked.string('FillerCaloJets')
    ),

    AKt7Jets = cms.untracked.PSet(
        active                        = cms.untracked.bool(True),
        flavorMatchingActive          = cms.untracked.bool(False),
        bTaggingActive                = cms.untracked.bool(False),
        jetToVertexActive             = cms.untracked.bool(False),
        jetCorrectionsActive          = cms.untracked.bool(False),
        jetIDActive                   = cms.untracked.bool(True),
        mitName                       = cms.untracked.string('AKt7Jets'),
        edmName                       = cms.untracked.string('ak7CaloJets'),
        jetToVertexAlphaName          = cms.untracked.string('jetVertexAlphaAKT7:Var'),
        jetToVertexBetaName           = cms.untracked.string('jetVertexBetaAKT7:Var'),
        L2JetCorrectorName            = cms.untracked.string('L2RelativeJetCorrectorAKT7Calo'),
        L3JetCorrectorName            = cms.untracked.string('L3AbsoluteJetCorrectorAKT7Calo'),
        jetIDName                     = cms.untracked.string('ak7JetID'),
        flavorMatchingByReferenceName = cms.untracked.string('AKT7byRef'),
        flavorMatchingDefinition      = cms.untracked.string('Algorithmic'),
        caloTowerMapName              = cms.untracked.string('CaloTowerMap'),
        jetMapName                    = cms.untracked.string('AKT7JetMap'),
        fillerType                    = cms.untracked.string('FillerCaloJets')
    ),

    IC5JetPlusTrack = cms.untracked.PSet(
        active               = cms.untracked.bool(True),
        flavorMatchingActive = cms.untracked.bool(False),
        bTaggingActive       = cms.untracked.bool(False),
        jetToVertexActive    = cms.untracked.bool(False),
        jetCorrectionsActive = cms.untracked.bool(False),
        mitName              = cms.untracked.string('IC5JetPlusTrack'),
        edmName              = cms.untracked.string('JetPlusTrackZSPCorJetIcone5'),
        L2JetCorrectorName   = cms.untracked.string('L2RelativeJetCorrectorIC5JPT'),
        L3JetCorrectorName   = cms.untracked.string('L3AbsoluteJetCorrectorIC5JPT'),
        caloJetMapName       = cms.untracked.string('ItrCone5JetMap'),
        jetMapName           = cms.untracked.string('IC5JPTJetMap'),
        fillerType           = cms.untracked.string('FillerJPTJets')
    ),
    
    SC5JetPlusTrack = cms.untracked.PSet(
        active               = cms.untracked.bool(False),
        flavorMatchingActive = cms.untracked.bool(False),
        bTaggingActive       = cms.untracked.bool(False),
        jetToVertexActive    = cms.untracked.bool(False),
        jetCorrectionsActive = cms.untracked.bool(False),
        mitName              = cms.untracked.string('SC5JetPlusTrack'),
        edmName              = cms.untracked.string('JetPlusTrackZSPCorJetSiscone5'),
        L2JetCorrectorName   = cms.untracked.string('L2RelativeJetCorrectorSC5JPT'),
        L3JetCorrectorName   = cms.untracked.string('L3AbsoluteJetCorrectorSC5JPT'),
        caloJetMapName       = cms.untracked.string('SisCone5JetMap'),
        jetMapName           = cms.untracked.string('SC5JPTJetMap'),
        fillerType           = cms.untracked.string('FillerJPTJets')
    ),
    
    AK5JetPlusTrack = cms.untracked.PSet(
        active               = cms.untracked.bool(True),
        flavorMatchingActive = cms.untracked.bool(False),
        bTaggingActive       = cms.untracked.bool(False),
        jetToVertexActive    = cms.untracked.bool(False),
        jetCorrectionsActive = cms.untracked.bool(False),
        mitName              = cms.untracked.string('AK5JetPlusTrack'),
        edmName              = cms.untracked.string('JetPlusTrackZSPCorJetAntiKt5'),
        L2JetCorrectorName   = cms.untracked.string('L2RelativeJetCorrectorAK5JPT'),
        L3JetCorrectorName   = cms.untracked.string('L3AbsoluteJetCorrectorAK5JPT'),
        caloJetMapName       = cms.untracked.string('AKT5JetMap'),
        jetMapName           = cms.untracked.string('AK5JPTJetMap'),
        fillerType           = cms.untracked.string('FillerJPTJets')
    ),               

    AKt5TrackJets = cms.untracked.PSet(
        active               = cms.untracked.bool(True),
        flavorMatchingActive = cms.untracked.bool(False),
        bTaggingActive       = cms.untracked.bool(False),
        jetToVertexActive    = cms.untracked.bool(False),
        jetCorrectionsActive = cms.untracked.bool(False),
        mitName              = cms.untracked.string('AKt5TrackJets'),
        edmName              = cms.untracked.string('ak5TrackJets'),
        L2JetCorrectorName   = cms.untracked.string(''),
        L3JetCorrectorName   = cms.untracked.string(''),
        trackMapName         = cms.untracked.string('TracksMapName'),
        vertexMapName        = cms.untracked.string('PrimaryVertexMap'),
        jetMapName           = cms.untracked.string('AK5TrackJetMap'),
        fillerType           = cms.untracked.string('FillerTrackJets')
    ),
                
    Kt4TrackJets = cms.untracked.PSet(
        active               = cms.untracked.bool(True),
        flavorMatchingActive = cms.untracked.bool(False),
        bTaggingActive       = cms.untracked.bool(False),
        jetToVertexActive    = cms.untracked.bool(False),
        jetCorrectionsActive = cms.untracked.bool(False),
        mitName              = cms.untracked.string('Kt4TrackJets'),
        edmName              = cms.untracked.string('kt4TrackJets'),
        L2JetCorrectorName   = cms.untracked.string(''),
        L3JetCorrectorName   = cms.untracked.string(''),
        trackMapName         = cms.untracked.string('TracksMapName'),
        vertexMapName        = cms.untracked.string('PrimaryVertexMap'),
        jetMapName           = cms.untracked.string('KT4TrackJetMap'),
        fillerType           = cms.untracked.string('FillerTrackJets')
    ),
    
    ItrCone5PFJets = cms.untracked.PSet(
        active                        = cms.untracked.bool(True),
        flavorMatchingActive          = cms.untracked.bool(False),
        bTaggingActive                = cms.untracked.bool(False),
        jetToVertexActive             = cms.untracked.bool(False),
        jetCorrectionsActive          = cms.untracked.bool(False),
        fastJetCorrectionsActive      = cms.untracked.bool(False),        
        mitName                       = cms.untracked.string('ItrCone5PFJets'),
        edmName                       = cms.untracked.string('iterativeCone5PFJets'),
        rhoName                       = cms.untracked.InputTag('kt6PFJetsForRhoComputation','rho'),
        jetToVertexAlphaName          = cms.untracked.string('jetVertexAlphaIC5PF:Var'),
        jetToVertexBetaName           = cms.untracked.string('jetVertexBetaIC5PF:Var'),
        L2JetCorrectorName            = cms.untracked.string('L2RelativeJetCorrectorIC5PF'),
        L3JetCorrectorName            = cms.untracked.string('L3AbsoluteJetCorrectorIC5PF'),
        flavorMatchingByReferenceName = cms.untracked.string('IC5PFbyRef'),
        flavorMatchingDefinition      = cms.untracked.string('Algorithmic'),
        pfCandMapName                 = cms.untracked.string('PFCandMapName'),
        jetMapName                    = cms.untracked.string('ItrCone5PFJetMap'),
        fillerType                    = cms.untracked.string('FillerPFJets')
    ),  
    
    SisCone5PFJets = cms.untracked.PSet(
        active                        = cms.untracked.bool(True),
        flavorMatchingActive          = cms.untracked.bool(False),
        bTaggingActive                = cms.untracked.bool(False),
        jetToVertexActive             = cms.untracked.bool(False),
        jetCorrectionsActive          = cms.untracked.bool(False),
        fastJetCorrectionsActive      = cms.untracked.bool(False),
        mitName                       = cms.untracked.string('SisCone5PFJets'),
        edmName                       = cms.untracked.string('sisCone5PFJets'),
        rhoName                       = cms.untracked.InputTag('kt6PFJetsForRhoComputation','rho'),
        jetToVertexAlphaName          = cms.untracked.string('jetVertexAlphaSC5PF:Var'),
        jetToVertexBetaName           = cms.untracked.string('jetVertexBetaSC5PF:Var'),
        L2JetCorrectorName            = cms.untracked.string('L2RelativeJetCorrectorSC5PF'),
        L3JetCorrectorName            = cms.untracked.string('L3AbsoluteJetCorrectorSC5PF'),
        flavorMatchingByReferenceName = cms.untracked.string('SC5PFbyRef'),
        flavorMatchingDefinition      = cms.untracked.string('Algorithmic'),
        pfCandMapName                 = cms.untracked.string('PFCandMapName'),
        jetMapName                    = cms.untracked.string('SisCone5PFJetMap'),
        fillerType                    = cms.untracked.string('FillerPFJets')
    ),                          

    SisCone7PFJets = cms.untracked.PSet(
        active                        = cms.untracked.bool(True),
        flavorMatchingActive          = cms.untracked.bool(False),
        bTaggingActive                = cms.untracked.bool(False),
        jetToVertexActive             = cms.untracked.bool(False),
        jetCorrectionsActive          = cms.untracked.bool(False),
        fastJetCorrectionsActive      = cms.untracked.bool(False),        
        mitName                       = cms.untracked.string('SisCone7PFJets'),
        edmName                       = cms.untracked.string('sisCone7PFJets'),
        rhoName                       = cms.untracked.InputTag('kt6PFJetsForRhoComputation','rho'),
        jetToVertexAlphaName          = cms.untracked.string('jetVertexAlphaSC7PF:Var'),
        jetToVertexBetaName           = cms.untracked.string('jetVertexBetaSC7PF:Var'),
        L2JetCorrectorName            = cms.untracked.string('L2RelativeJetCorrectorSC7PF'),
        L3JetCorrectorName            = cms.untracked.string('L3AbsoluteJetCorrectorSC7PF'),
        flavorMatchingByReferenceName = cms.untracked.string('SC7PFbyRef'),
        flavorMatchingDefinition      = cms.untracked.string('Algorithmic'),
        pfCandMapName                 = cms.untracked.string('PFCandMapName'),
        jetMapName                    = cms.untracked.string('SisCone7PFJetMap'),
        fillerType                    = cms.untracked.string('FillerPFJets')
    ),

    Kt4PFJets = cms.untracked.PSet(
        active                        = cms.untracked.bool(True),
        flavorMatchingActive          = cms.untracked.bool(False),
        bTaggingActive                = cms.untracked.bool(False),
        jetToVertexActive             = cms.untracked.bool(False),
        jetCorrectionsActive          = cms.untracked.bool(False),
        fastJetCorrectionsActive      = cms.untracked.bool(False),        
        mitName                       = cms.untracked.string('Kt4PFJets'),
        edmName                       = cms.untracked.string('kt4PFJets'),
        rhoName                       = cms.untracked.InputTag('kt6PFJetsForRhoComputation','rho'),
        jetToVertexAlphaName          = cms.untracked.string('jetVertexAlphaKT4PF:Var'),
        jetToVertexBetaName           = cms.untracked.string('jetVertexBetaKT4PF:Var'),
        L2JetCorrectorName            = cms.untracked.string('L2RelativeJetCorrectorKT4PF'),
        L3JetCorrectorName            = cms.untracked.string('L3AbsoluteJetCorrectorKT4PF'),
        flavorMatchingByReferenceName = cms.untracked.string('KT4PFbyRef'),
        flavorMatchingDefinition      = cms.untracked.string('Algorithmic'),
        pfCandMapName                 = cms.untracked.string('PFCandMapName'),
        jetMapName                    = cms.untracked.string('KT4PFJetMap'),
        fillerType                    = cms.untracked.string('FillerPFJets')
    ),

    Kt4PFJetsNoArea = cms.untracked.PSet(
        active                        = cms.untracked.bool(False),
        flavorMatchingActive          = cms.untracked.bool(False),
        bTaggingActive                = cms.untracked.bool(False),
        jetToVertexActive             = cms.untracked.bool(False),
        jetCorrectionsActive          = cms.untracked.bool(False),
        fastJetCorrectionsActive      = cms.untracked.bool(False),        
        mitName                       = cms.untracked.string('Kt4PFJetsNoArea'),
        edmName                       = cms.untracked.string('kt4PFJets'),
        rhoName                       = cms.untracked.InputTag('kt6PFJetsForRhoComputation','rho'),
        jetToVertexAlphaName          = cms.untracked.string('jetVertexAlphaKT4PF:Var'),
        jetToVertexBetaName           = cms.untracked.string('jetVertexBetaKT4PF:Var'),
        L2JetCorrectorName            = cms.untracked.string('L2RelativeJetCorrectorKT4PF'),
        L3JetCorrectorName            = cms.untracked.string('L3AbsoluteJetCorrectorKT4PF'),
        flavorMatchingByReferenceName = cms.untracked.string('KT4PFbyRef'),
        flavorMatchingDefinition      = cms.untracked.string('Algorithmic'),
        pfCandMapName                 = cms.untracked.string('PFCandMapName'),
        jetMapName                    = cms.untracked.string('KT4PFJetNoAreaMap'),
        fillerType                    = cms.untracked.string('FillerPFJets')
    ),


    Kt6PFJets = cms.untracked.PSet(
        active                        = cms.untracked.bool(True),
        flavorMatchingActive          = cms.untracked.bool(False),
        bTaggingActive                = cms.untracked.bool(False),
        jetToVertexActive             = cms.untracked.bool(False),
        jetCorrectionsActive          = cms.untracked.bool(False),
        fastJetCorrectionsActive      = cms.untracked.bool(False),        
        mitName                       = cms.untracked.string('Kt6PFJets'),
        edmName                       = cms.untracked.string('kt6PFJets'),
        rhoName                       = cms.untracked.InputTag('kt6PFJetsForRhoComputation','rho'),
        jetToVertexAlphaName          = cms.untracked.string('jetVertexAlphaKT6PF:Var'),
        jetToVertexBetaName           = cms.untracked.string('jetVertexBetaKT6PF:Var'),
        L2JetCorrectorName            = cms.untracked.string('L2RelativeJetCorrectorKT6PF'),
        L3JetCorrectorName            = cms.untracked.string('L3AbsoluteJetCorrectorKT6PF'),
        flavorMatchingByReferenceName = cms.untracked.string('KT6PFbyRef'),
        flavorMatchingDefinition      = cms.untracked.string('Algorithmic'),
        pfCandMapName                 = cms.untracked.string('PFCandMapName'),
        jetMapName                    = cms.untracked.string('KT6PFJetMap'),
        fillerType                    = cms.untracked.string('FillerPFJets')
    ),
    AKt5PFJets = cms.untracked.PSet(
        active                        = cms.untracked.bool(True),
        flavorMatchingActive          = cms.untracked.bool(False),
        bTaggingActive                = cms.untracked.bool(False),
        jetToVertexActive             = cms.untracked.bool(False),
        jetCorrectionsActive          = cms.untracked.bool(False),
        fastJetCorrectionsActive      = cms.untracked.bool(False),        
        mitName                       = cms.untracked.string('AKt5PFJets'),
        edmName                       = cms.untracked.string('ak5PFJets'),
        rhoName                       = cms.untracked.InputTag('kt6PFJetsForRhoComputation','rho'),
        jetToVertexAlphaName          = cms.untracked.string('jetVertexAlphaAKT5PF:Var'),
        jetToVertexBetaName           = cms.untracked.string('jetVertexBetaAKT5PF:Var'),
        L2JetCorrectorName            = cms.untracked.string('L2RelativeJetCorrectorAKT5PF'),
        L3JetCorrectorName            = cms.untracked.string('L3AbsoluteJetCorrectorAKT5PF'),
        flavorMatchingByReferenceName = cms.untracked.string('AKT5PFbyRef'),
        flavorMatchingDefinition      = cms.untracked.string('Algorithmic'),
        JetProbabilityBJetTagsName             = cms.untracked.string('newJetProbabilityBJetTags'),
        JetBProbabilityBJetTagsName            = cms.untracked.string('newJetBProbabilityBJetTags'),
        SimpleSecondaryVertexHighEffBJetTagsName      = cms.untracked.string('newSimpleSecondaryVertexHighEffBJetTags'),
        SimpleSecondaryVertexHighPurBJetTagsName      = cms.untracked.string('newSimpleSecondaryVertexHighPurBJetTags'),
        CombinedSecondaryVertexBJetTagsName    = cms.untracked.string('newCombinedSecondaryVertexBJetTags'),
        CombinedSecondaryVertexMVABJetTagsName = cms.untracked.string('newCombinedSecondaryVertexMVABJetTags'),
        GhostTrackBJetTagsName                 = cms.untracked.string('newGhostTrackBJetTagsName'),
        TrackCountingHighEffBJetTagsName       = cms.untracked.string('newTrackCountingHighEffBJetTags'),
        TrackCountingHighPurBJetTagsName       = cms.untracked.string('newTrackCountingHighPurBJetTags'),
        SoftMuonBJetTagsName                   = cms.untracked.string('newSoftMuonBJetTags'),
        SoftMuonByIP3dBJetTagsName               = cms.untracked.string('newSoftMuonByIP3dBJetTags'),
        SoftMuonByPtBJetTagsName               = cms.untracked.string('newSoftMuonByPtBJetTags'),
        SoftElectronByIP3dBJetTagsName               = cms.untracked.string('newSoftElectronByIP3dBJetTags'),
        SoftElectronByPtBJetTagsName               = cms.untracked.string('newSoftElectronByPtBJetTags'),
        pfCandMapName                 = cms.untracked.string('PFCandMapName'),
        jetMapName                    = cms.untracked.string('AKT5PFJetMap'),
        fillerType                    = cms.untracked.string('FillerPFJets')
    ),

    AKt5PFJetsCHS = cms.untracked.PSet(
        active                        = cms.untracked.bool(True),
        flavorMatchingActive          = cms.untracked.bool(False),
        bTaggingActive                = cms.untracked.bool(False),
        jetToVertexActive             = cms.untracked.bool(False),
        jetCorrectionsActive          = cms.untracked.bool(False),
        fastJetCorrectionsActive      = cms.untracked.bool(False),        
        mitName                       = cms.untracked.string('AKt5PFJetsCHS'),
        edmName                       = cms.untracked.string('pfJets'),
        rhoName                       = cms.untracked.InputTag('kt6PFJetsForRhoComputationNoPileup','rho'),
        jetToVertexAlphaName          = cms.untracked.string('jetVertexAlphaAKT5PF:Var'),
        jetToVertexBetaName           = cms.untracked.string('jetVertexBetaAKT5PF:Var'),
        L2JetCorrectorName            = cms.untracked.string('L2RelativeJetCorrectorAKT5PF'),
        L3JetCorrectorName            = cms.untracked.string('L3AbsoluteJetCorrectorAKT5PF'),
        flavorMatchingByReferenceName = cms.untracked.string('AKT5PFbyRef'),
        flavorMatchingDefinition      = cms.untracked.string('Algorithmic'),
        JetProbabilityBJetTagsName             = cms.untracked.string('newJetProbabilityBJetTags'),
        JetBProbabilityBJetTagsName            = cms.untracked.string('newJetBProbabilityBJetTags'),
        SimpleSecondaryVertexHighEffBJetTagsName      = cms.untracked.string('newSimpleSecondaryVertexHighEffBJetTags'),
        SimpleSecondaryVertexHighPurBJetTagsName      = cms.untracked.string('newSimpleSecondaryVertexHighPurBJetTags'),
        CombinedSecondaryVertexBJetTagsName    = cms.untracked.string('newCombinedSecondaryVertexBJetTags'),
        CombinedSecondaryVertexMVABJetTagsName = cms.untracked.string('newCombinedSecondaryVertexMVABJetTags'),
        GhostTrackBJetTagsName                 = cms.untracked.string('newGhostTrackBJetTagsName'),
        TrackCountingHighEffBJetTagsName       = cms.untracked.string('newTrackCountingHighEffBJetTags'),
        TrackCountingHighPurBJetTagsName       = cms.untracked.string('newTrackCountingHighPurBJetTags'),
        SoftMuonBJetTagsName                   = cms.untracked.string('newSoftMuonBJetTags'),
        SoftMuonByIP3dBJetTagsName               = cms.untracked.string('newSoftMuonByIP3dBJetTags'),
        SoftMuonByPtBJetTagsName               = cms.untracked.string('newSoftMuonByPtBJetTags'),
        SoftElectronByIP3dBJetTagsName               = cms.untracked.string('newSoftElectronByIP3dBJetTags'),
        SoftElectronByPtBJetTagsName               = cms.untracked.string('newSoftElectronByPtBJetTags'),
        pfCandMapName                 = cms.untracked.string('PFNoPileupCandMapName'),
        jetMapName                    = cms.untracked.string('pfJetMap'),
        fillerType                    = cms.untracked.string('FillerPFJets')
    ),

    AKt5PFJetsNoArea = cms.untracked.PSet( 
        active                        = cms.untracked.bool(False),
        flavorMatchingActive          = cms.untracked.bool(False),
        bTaggingActive                = cms.untracked.bool(False),
        jetToVertexActive             = cms.untracked.bool(False),
        jetCorrectionsActive          = cms.untracked.bool(False),
        fastJetCorrectionsActive      = cms.untracked.bool(False), 
        mitName                       = cms.untracked.string('AKt5PFJetsNoArea'),
        edmName                       = cms.untracked.string('ak5PFJets'),
        rhoName                       = cms.untracked.InputTag('kt6PFJetsForRhoComputation','rho'), 
        jetToVertexAlphaName          = cms.untracked.string('jetVertexAlphaAKT5PF:Var'),
        jetToVertexBetaName           = cms.untracked.string('jetVertexBetaAKT5PF:Var'),
        L2JetCorrectorName            = cms.untracked.string('L2RelativeJetCorrectorAKT5PF'),
        L3JetCorrectorName            = cms.untracked.string('L3AbsoluteJetCorrectorAKT5PF'),
        flavorMatchingByReferenceName = cms.untracked.string('AKT5PFbyRef'),
        flavorMatchingDefinition      = cms.untracked.string('Algorithmic'),
        pfCandMapName                 = cms.untracked.string('PFCandMapName'),
        jetMapName                    = cms.untracked.string('AKT5PFJetNoAreaMap'),
        fillerType                    = cms.untracked.string('FillerPFJets')
    ),


    AKt7PFJets = cms.untracked.PSet(
        active                        = cms.untracked.bool(True),
        flavorMatchingActive          = cms.untracked.bool(False),
        bTaggingActive                = cms.untracked.bool(False),
        jetToVertexActive             = cms.untracked.bool(False),
        jetCorrectionsActive          = cms.untracked.bool(False),
        fastJetCorrectionsActive      = cms.untracked.bool(False),        
        mitName                       = cms.untracked.string('AKt7PFJets'),
        edmName                       = cms.untracked.string('ak7PFJets'),
        rhoName                       = cms.untracked.InputTag('kt6PFJetsForRhoComputation','rho'),
        jetToVertexAlphaName          = cms.untracked.string('jetVertexAlphaAKT7PF:Var'),
        jetToVertexBetaName           = cms.untracked.string('jetVertexBetaAKT7PF:Var'),
        L2JetCorrectorName            = cms.untracked.string('L2RelativeJetCorrectorAKT7PF'),
        L3JetCorrectorName            = cms.untracked.string('L3AbsoluteJetCorrectorAKT7PF'),
        flavorMatchingByReferenceName = cms.untracked.string('AKT7PFbyRef'),
        flavorMatchingDefinition      = cms.untracked.string('Algorithmic'),
        pfCandMapName                 = cms.untracked.string('PFCandMapName'),
        jetMapName                    = cms.untracked.string('AKT7PFJetMap'),
        fillerType                    = cms.untracked.string('FillerPFJets')
    ),

    GenMet = cms.untracked.PSet(
        active     = cms.untracked.bool(False),
        mitName    = cms.untracked.string('GenMet'),
        edmName    = cms.untracked.string('genMetTrue'),
        fillerType = cms.untracked.string('FillerGenMet')
    ),
    
    CorMuonMet = cms.untracked.PSet(
        active     = cms.untracked.bool(False),
        mitName    = cms.untracked.string('CorMuonMet'),
        edmName    = cms.untracked.string('corMetGlobalMuons'),
        fillerType = cms.untracked.string('FillerCaloMet')
    ),
        
    ItrCone5Met = cms.untracked.PSet(
        active     = cms.untracked.bool(True),
        mitName    = cms.untracked.string('ItrCone5Met'),
        edmName    = cms.untracked.string('corMetType1Icone5'),
        fillerType = cms.untracked.string('FillerCaloMet')
    ),
    
    SisCone5Met = cms.untracked.PSet(
        active     = cms.untracked.bool(True),
        mitName    = cms.untracked.string('SisCone5Met'),
        edmName    = cms.untracked.string('corMetType1Scone5'),
        fillerType = cms.untracked.string('FillerCaloMet')
    ),

    SisCone7Met = cms.untracked.PSet(
        active     = cms.untracked.bool(True),
        mitName    = cms.untracked.string('SisCone7Met'),
        edmName    = cms.untracked.string('corMetType1Scone7'), 
        fillerType = cms.untracked.string('FillerCaloMet')
    ),  
    
    Kt4Met = cms.untracked.PSet(
        active     = cms.untracked.bool(True),
        mitName    = cms.untracked.string('Kt4Met'),
        edmName    = cms.untracked.string('corMetType1Kt4'),
        fillerType = cms.untracked.string('FillerCaloMet')
    ),

    Kt6Met = cms.untracked.PSet(
        active     = cms.untracked.bool(True),
        mitName    = cms.untracked.string('Kt6Met'),
        edmName    = cms.untracked.string('corMetType1Kt6'),
        fillerType = cms.untracked.string('FillerCaloMet')
    ),
    
    PFMet = cms.untracked.PSet(
        active     = cms.untracked.bool(True),
        mitName    = cms.untracked.string('PFMet'),
        edmName    = cms.untracked.InputTag('pfMet'),
        fillerType = cms.untracked.string('FillerPFMet')
    ),      
        
    TCMet = cms.untracked.PSet(
        active     = cms.untracked.bool(True),
        mitName    = cms.untracked.string('TCMet'),
        edmName    = cms.untracked.string('tcMet'),
        fillerType = cms.untracked.string('FillerMet')
    ),      
           
    ItrCone5Mht    = cms.untracked.PSet(
        active     = cms.untracked.bool(True),
        mitName    = cms.untracked.string('ItrCone5Mht'),
        edmName    = cms.untracked.string('htMetIC5'),
        fillerType = cms.untracked.string('FillerMet')
    ),
    
    SisCone5Mht = cms.untracked.PSet(
        active     = cms.untracked.bool(True),
        mitName    = cms.untracked.string('SisCone5Mht'),
        edmName    = cms.untracked.string('htMetSC5'),
        fillerType = cms.untracked.string('FillerMet')
    ),

    SisCone7Mht = cms.untracked.PSet(
        active     = cms.untracked.bool(True),
        mitName    = cms.untracked.string('SisCone7Mht'),
        edmName    = cms.untracked.string('htMetSC7'), 
        fillerType = cms.untracked.string('FillerMet')
    ),  
    
    Kt4Mht = cms.untracked.PSet(
        active     = cms.untracked.bool(True),
        mitName    = cms.untracked.string('Kt4Mht'),
        edmName    = cms.untracked.string('htMetKT4'),
        fillerType = cms.untracked.string('FillerMet')
    ),

    Kt6Mht = cms.untracked.PSet(
        active     = cms.untracked.bool(True),
        mitName    = cms.untracked.string('Kt6Mht'),
        edmName    = cms.untracked.string('htMetKT6'),
        fillerType = cms.untracked.string('FillerMet')
    ),     
         
    IC5JPTMht = cms.untracked.PSet(
        active     = cms.untracked.bool(True),
        mitName    = cms.untracked.string('IC5JPTMht'),
        edmName    = cms.untracked.string('htMetIC5JPT'),
        fillerType = cms.untracked.string('FillerMet')
    ),


    AssocPFMet = cms.untracked.PSet(
        active     = cms.untracked.bool(False),
        mitName    = cms.untracked.string('AssocPFMet'),
        edmName    = cms.untracked.InputTag('ClusteredPFMetProducer','assocPfMet'),
        fillerType = cms.untracked.string('FillerPFMet')
    ),
    
    TrkPFMet = cms.untracked.PSet(
        active     = cms.untracked.bool(False),
        mitName    = cms.untracked.string('TrkPFMet'),
        edmName    = cms.untracked.InputTag('ClusteredPFMetProducer','trkPfMet'),
        fillerType = cms.untracked.string('FillerPFMet')
    ),   

    AssocOtherVtxPFMet = cms.untracked.PSet(
        active     = cms.untracked.bool(False),
        mitName    = cms.untracked.string('AssocOtherVtxPFMet'),
        edmName    = cms.untracked.InputTag('ClusteredPFMetProducer','assocOtherVtxPfMet'),
        fillerType = cms.untracked.string('FillerPFMet')
    ),
    
    CentralPFMet = cms.untracked.PSet(
        active     = cms.untracked.bool(False),
        mitName    = cms.untracked.string('CentralPFMet'),
        edmName    = cms.untracked.InputTag('ClusteredPFMetProducer','centralPfMet'),
        fillerType = cms.untracked.string('FillerPFMet')
    ),
    
    CleanPFMet = cms.untracked.PSet(
        active     = cms.untracked.bool(False),
        mitName    = cms.untracked.string('CleanPFMet'),
        edmName    = cms.untracked.InputTag('ClusteredPFMetProducer','cleanPfMet'),
        fillerType = cms.untracked.string('FillerPFMet')
    ), 
              
    CaloTaus = cms.untracked.PSet(
        active          = cms.untracked.bool(True),
        mitName         = cms.untracked.string('CaloTaus'),
        edmName         = cms.untracked.string('caloRecoTauProducer'),
        trackMapName    = cms.untracked.string('TracksMapName'),
        jetMapName      = cms.untracked.string(''),
        barrelBCMapName = cms.untracked.string('barrelBasicClusterMap'),
        endcapBCMapName = cms.untracked.string('endcapBasicClusterMap'),
        fillerType      = cms.untracked.string('FillerCaloTaus')
    ),
    
    PFTaus = cms.untracked.PSet(
        active          = cms.untracked.bool(False),
        mitName         = cms.untracked.string('PFTaus'),
        edmName         = cms.untracked.string('fixedConePFTauProducer'),
        trackMapNames   = cms.untracked.vstring('TracksMapName', 'ConversionTracksMapName'),
        jetMapName      = cms.untracked.string(''),
        pfCandMapName   = cms.untracked.string('PFCandMapName'),
        fillerType      = cms.untracked.string('FillerPFTaus')
    ),
    
    ShrinkingConePFTaus = cms.untracked.PSet(
        active          = cms.untracked.bool(True),
        mitName         = cms.untracked.string('ShrinkingConePFTaus'),
        edmName         = cms.untracked.string('shrinkingConePFTauProducer'),
        trackMapNames   = cms.untracked.vstring('TracksMapName', 'ConversionTracksMapName'),
        jetMapName      = cms.untracked.string(''),
        pfCandMapName   = cms.untracked.string('PFCandMapName'),
        fillerType      = cms.untracked.string('FillerPFTaus'),
    ),    
          
    HPSTaus = cms.untracked.PSet(
	active = cms.untracked.bool(True),
	hpsActive = cms.untracked.bool(True),
	mitName = cms.untracked.string('HPSTaus'),
	edmName = cms.untracked.string('hpsPFTauProducer'),
	discriminationByLooseElectronRejectionName = cms.untracked.string('hpsPFTauDiscriminationByLooseElectronRejection'),
	discriminationByMediumElectronRejectionName = cms.untracked.string('hpsPFTauDiscriminationByMediumElectronRejection'),
	discriminationByTightElectronRejectionName = cms.untracked.string('hpsPFTauDiscriminationByTightElectronRejection'),
        discriminationByMVAElectronRejectionName = cms.untracked.string('hpsPFTauDiscriminationByMVAElectronRejection'),
	discriminationByLooseMuonRejectionName = cms.untracked.string('hpsPFTauDiscriminationByLooseMuonRejection'),
	discriminationByTightMuonRejectionName = cms.untracked.string('hpsPFTauDiscriminationByTightMuonRejection'),
	discriminationByDecayModeFindingName = cms.untracked.string('hpsPFTauDiscriminationByDecayModeFinding'),
	discriminationByVLooseIsolationName = cms.untracked.string('hpsPFTauDiscriminationByVLooseIsolation'),
	discriminationByLooseIsolationName = cms.untracked.string('hpsPFTauDiscriminationByLooseIsolation'),
	discriminationByMediumIsolationName = cms.untracked.string('hpsPFTauDiscriminationByMediumIsolation'),
	discriminationByTightIsolationName = cms.untracked.string('hpsPFTauDiscriminationByTightIsolation'),
        discriminationByVLooseCombinedIsolationDBSumPtCorr = cms.untracked.string('hpsPFTauDiscriminationByVLooseCombinedIsolationDBSumPtCorr'),
        discriminationByLooseCombinedIsolationDBSumPtCorr = cms.untracked.string('hpsPFTauDiscriminationByLooseCombinedIsolationDBSumPtCorr'),
        discriminationByMediumCombinedIsolationDBSumPtCorr = cms.untracked.string('hpsPFTauDiscriminationByMediumCombinedIsolationDBSumPtCorr'),
        discriminationByTightCombinedIsolationDBSumPtCorr = cms.untracked.string('hpsPFTauDiscriminationByTightCombinedIsolationDBSumPtCorr'),
        discriminationByRawCombinedIsolationDBSumPtCorr = cms.untracked.string('hpsPFTauDiscriminationByRawCombinedIsolationDBSumPtCorr'),
	trackMapNames = cms.untracked.vstring('TracksMapName', 'ConversionTracksMapName'),
	jetMapName    = cms.untracked.string(''),
	pfCandMapName = cms.untracked.string('PFCandMapName'),
        tauMapName    = cms.untracked.string('tauMap'),
	fillerType    = cms.untracked.string('FillerPFTaus')
    ),
    
    StableParts = cms.untracked.PSet(
        active       = cms.untracked.bool(False),
        mitName      = cms.untracked.string('MuonsStable'),
        edmName      = cms.untracked.string('MuonsStable'),
        trackMapName = cms.untracked.string('TracksMapName'),
        basePartMap  = cms.untracked.string('MuonsStableMapName'),
        fillerType   = cms.untracked.string('FillerStableParts')
    ),

    DecayParts = cms.untracked.PSet(
        active       = cms.untracked.bool(False),
        mitName      = cms.untracked.string('Jpsi2MuMu'),
        edmName      = cms.untracked.string('Jpsi2MuMu'),
        basePartMaps = cms.untracked.vstring('MuonsStableMapName'),
        fillerType   = cms.untracked.string('FillerDecayParts')
    ),       
    
    MergedStableTest = cms.untracked.PSet(
      active        = cms.untracked.bool(True),
      mitName       = cms.untracked.string('MergedStableTest'),
      edmName       = cms.untracked.string('mergedConversionsStable'),
      trackMapNames = cms.untracked.vstring('ConversionInOutTracksMapName', 
                                            'ConversionOutInTracksMapName'),
      basePartMap   = cms.untracked.string('MergedTestStableMapName'),
      trackPartMap  = cms.untracked.string('MergedTestStableTrackMapName'),
      fillerType    = cms.untracked.string('FillerStableParts')
    ),       

    ElectronsStable = cms.untracked.PSet(
      active        = cms.untracked.bool(False),
      mitName       = cms.untracked.string('ElectronsStable'),
      edmName       = cms.untracked.string('generalElectronsStable'),
      trackMapNames = cms.untracked.vstring('TracksMapName'),
      basePartMap   = cms.untracked.string('ElectronsStableMapName'),
      trackPartMap  = cms.untracked.string('ElectronsStableTrackMapName'),
      fillerType    = cms.untracked.string('FillerStableParts')
    ),       
  
    ConversionInOutElectronsStable = cms.untracked.PSet(
      active        = cms.untracked.bool(False),
      mitName       = cms.untracked.string('ConversionInOutElectronsStable'),
      edmName       = cms.untracked.string('ckfInOutElectronsStable'),
      trackMapNames = cms.untracked.vstring('ConversionInOutTracksMapName'),
      basePartMap   = cms.untracked.string('ConversionInOutElectronsStableMapName'),
      trackPartMap  = cms.untracked.string('ConversionInOutElectronsStableTrackMapName'),
      fillerType    = cms.untracked.string('FillerStableParts')
    ),     
    
    ConversionOutInElectronsStable = cms.untracked.PSet(
      active        = cms.untracked.bool(False),
      mitName       = cms.untracked.string('ConversionOutInElectronsStable'),
      edmName       = cms.untracked.string('ckfOutInElectronsStable'),
      trackMapNames = cms.untracked.vstring('ConversionOutInTracksMapName'),
      basePartMap   = cms.untracked.string('ConversionOutInElectronsStableMapName'),
      trackPartMap  = cms.untracked.string('ConversionOutInElectronsStableTrackMapName'),
      fillerType    = cms.untracked.string('FillerStableParts')
    ),   

    GsfElectronsStable = cms.untracked.PSet(
      active        = cms.untracked.bool(False),
      mitName       = cms.untracked.string('GsfElectronsStable'),
      edmName       = cms.untracked.string('gsfElectronsStable'),
      trackMapNames = cms.untracked.vstring('GsfTracksMapName'),
      basePartMap   = cms.untracked.string('ElectronsStableGsfMapName'),
      trackPartMap  = cms.untracked.string('ElectronsStableGsfTrackMapName'),      
      fillerType    = cms.untracked.string('FillerStableParts')
    ),

    ConversionStepElectronsStable = cms.untracked.PSet(
      active        = cms.untracked.bool(False),
      mitName       = cms.untracked.string('ConversionStepElectronsStable'),
      edmName       = cms.untracked.string('conversionStepElectronsStable'),
      trackMapNames = cms.untracked.vstring('ConversionTracksMapName'),
      basePartMap   = cms.untracked.string('ElectronsStableConvStepMapName'),
      trackPartMap  = cms.untracked.string('ElectronsStableConvStepTrackMapName'),      
      fillerType    = cms.untracked.string('FillerStableParts')
    ),

    MergedElectronsStable = cms.untracked.PSet(
      active        = cms.untracked.bool(False),
      mitName       = cms.untracked.string('MergedElectronsStable'),
      edmName       = cms.untracked.string('mergedElectronsStable'),
      trackMapNames = cms.untracked.vstring('TracksMapName',
                                            'ConversionInOutTracksMapName',
                                            'ConversionOutInTracksMapName',
                                            'GsfTracksMapName'),
      basePartMap   = cms.untracked.string('MergedElectronsStableMapName'),
      trackPartMap  = cms.untracked.string('MergedElectronsStableTrackMapName'),
      fillerType    = cms.untracked.string('FillerStableParts')
    ),  
    EmbedWeight = cms.untracked.PSet(
       active        = cms.untracked.bool(False),
       mitName       = cms.untracked.string('EmbedWeight'),
       edmName       = cms.untracked.string('generator'),
       fillerType    = cms.untracked.string('FillerEmbedWeight')
     ),
     DCASig     = cms.untracked.PSet(
       active                = cms.untracked.bool(True),
       mitName               = cms.untracked.string('DCASig'),
       edmElectronName       = cms.untracked.string('gsfElectrons'),
       edmMuonName           = cms.untracked.string('muons'),
       edmTauName            = cms.untracked.string('hpsPFTauProducer'),
       electronMapName       = cms.untracked.string('electronMap'),
       muonMapName           = cms.untracked.string('muonMap'),
       tauMapName            = cms.untracked.string('tauMap'),
       electronPtMin         = cms.untracked.double(5),
       muonPtMin             = cms.untracked.double(5),
       tauPtMin              = cms.untracked.double(10),
       checkDCARef           = cms.untracked.bool(True),
       fillerType            = cms.untracked.string('FillerDCASig')
     ),   
)
