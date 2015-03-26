import FWCore.ParameterSet.Config as cms

# This python module is actually not the offlinePrimaryVertex saved in AOD
# This module is cloned to unsortedOfflinePrimaryVertices and used as an input to
# the real offlinePrimaryVertex, which is a clone of sortedPrimaryVertices.
# Nevertheless, the reference of the beam spot from offlinePrimaryVertex is ultimately
# a reference to the beam spot in this module.
from RecoVertex.PrimaryVertexProducer.OfflinePrimaryVertices_cfi import offlinePrimaryVertices

MitTreeFiller = cms.EDAnalyzer("FillMitTree",
  fillers = cms.untracked.vstring(
    'MetaInfos',
    'MCParticles',
    'MCEventInfo',
    'MCVertexes',
    'PileupInfo',
    'PileupEnergyDensity',
    'EvtSelData',
    'BeamSpot',
    'BasicClusters',
    'PsXClusters',
    'PsYClusters',
    #'PsClusters', # this is a joined (x/y)BasicCuster but lazyTools does not like those
    'SuperClusters',
    'GeneralTracks',
    'ConversionStepTracks',
    'StandaloneMuonTracks',
    'StandaloneMuonTracksWVtxConstraint',
    'StandaloneCosmicMuonTracks',
    'GlobalMuonTracks',
    'GlobalCosmicMuonTracks',
    'ConversionInOutTracks',
    'ConversionOutInTracks',
    'GsfTracks',
    'PrimaryVertexes',
    'PrimaryVertexesBS',
    'Muons',
    'CosmicMuons',
    'Electrons',
    'ElectronsStable',
    'ConversionInOutElectronsStable',
    'ConversionOutInElectronsStable',
    'GsfElectronsStable',
    'ConversionStepElectronsStable',
    'MergedElectronsStable',
    'MergedConversions',
    'Conversions',
    'PFCandidates',
    'Photons',
    'AKT4GenJets',
    'AKT8GenJets',
    'AKt4PFJets',
    'AKt4PFJetsCHS',
    'AKt8PFJets',
    'AKt8PFJetsCHS',
    'GenMet',
    'PFMet',
    'TCMet',
    'HPSTaus',
    'DCASig'
  ),

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

  MCParticles = cms.untracked.PSet(
    active          = cms.untracked.bool(False),
    genActive       = cms.untracked.bool(True),
    useAodGen       = cms.untracked.bool(True),
    simActive       = cms.untracked.bool(False),
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
    active                        = cms.untracked.bool(True),
    mitName                       = cms.untracked.string('EvtSelData'),
    HBHENoiseFilterName           = cms.untracked.string('HBHENoiseFilterResultProducer:'+
                                                         'HBHENoiseFilterResult'),  
    ECALDeadCellFilterName        = cms.untracked.string('EcalDeadCellTriggerPrimitiveFilter'),  
    trackingFailureFilterName     = cms.untracked.string('trackingFailureFilter'),  
    EEBadScFilterName             = cms.untracked.string('eeBadScFilter'),  
    ECALaserCorrFilterName        = cms.untracked.string('ecalLaserCorrFilter'),  
    tkManyStripClusName           = cms.untracked.string('manystripclus53X'),  
    tkTooManyStripClusName        = cms.untracked.string('toomanystripclus53X'),  
    tkLogErrorTooManyClustersName = cms.untracked.string('logErrorTooManyClusters'),  
    BeamHaloSummaryName           = cms.untracked.string('BeamHaloSummary'),  
    fillerType                    = cms.untracked.string('FillerEvtSelData')
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

  PrimaryVertexesBS = cms.untracked.PSet(
    active        = cms.untracked.bool(True),
    mitName       = cms.untracked.string('PrimaryVertexesBeamSpot'),
    edmName       = cms.untracked.string('offlinePrimaryVerticesWithBS'),
    vertexMapName = cms.untracked.string('PrimaryVertexBSMap'),
    trackMapName  = cms.untracked.string('TracksMapName'),
    fillerType    = cms.untracked.string('FillerVertexes')
  ),

  PsXClusters = cms.untracked.PSet(
    active              = cms.untracked.bool(True),
    mitName             = cms.untracked.string('PsXClusters'),
    edmName             = cms.untracked.string('multi5x5SuperClustersWithPreshower:preshowerXClusters'),
    psClusterMapName    = cms.untracked.string('PsXClusterMap'),
    doPsClusters        = cms.untracked.bool(True),
    fillerType          = cms.untracked.string('FillerPsClusters')
  ),    
  
  PsYClusters = cms.untracked.PSet(
    active              = cms.untracked.bool(True),
    mitName             = cms.untracked.string('PsYClusters'),
    edmName             = cms.untracked.string('multi5x5SuperClustersWithPreshower:preshowerYClusters'),
    psClusterMapName    = cms.untracked.string('PsYClusterMap'),
    doPsClusters        = cms.untracked.bool(True),
    fillerType          = cms.untracked.string('FillerPsClusters')
  ),

  BasicClusters = cms.untracked.PSet(
    active               = cms.untracked.bool(True),
    mitName              = cms.untracked.string('BasicClusters'),
    edmName              = cms.untracked.string('particleFlowEGamma:EBEEClusters'),
    barrelEcalRecHitName = cms.untracked.string('reducedEcalRecHitsEB'),
    endcapEcalRecHitName = cms.untracked.string('reducedEcalRecHitsEE'),
    basicClusterMapName  = cms.untracked.string('basicClusterMap'),
    fillerType           = cms.untracked.string('FillerBasicClusters')
  ),

  PsClusters = cms.untracked.PSet(
    active               = cms.untracked.bool(True),
    mitName              = cms.untracked.string('PsClusters'),
    edmName              = cms.untracked.string('particleFlowEGamma:ESClusters'),
    barrelEcalRecHitName = cms.untracked.string('reducedEcalRecHitsES'),
    endcapEcalRecHitName = cms.untracked.string('reducedEcalRecHitsES'),
    basicClusterMapName  = cms.untracked.string('psClusterMap'),
    fillerType           = cms.untracked.string('FillerBasicClusters')
  ),

  SuperClusters = cms.untracked.PSet(
    active                = cms.untracked.bool(True),
    mitName               = cms.untracked.string('SuperClusters'),
    edmName               = cms.untracked.string('particleFlowEGamma'),
    basicClusterMapName   = cms.untracked.string('basicClusterMap'),
    superClusterMapName   = cms.untracked.string('superClusterMap'),
    superClusterIdMapName = cms.untracked.string('superClusterIdMap'),
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
    active                               = cms.untracked.bool(True),
    mitName                              = cms.untracked.string('Rho'),
    edmName                              = cms.untracked.string('kt6PFJetsForRhoComputationVoronoiMIT:rho'),
    edmNameLowEta                        = cms.untracked.string('kt6PFJetsForRhoComputationVoronoi25MIT:rho'),
    edmNameRandom                        = cms.untracked.string('kt6PFJetsForRhoComputationRandomMIT:rho'),
    edmNameRandomLowEta                  = cms.untracked.string('kt6PFJetsForRhoComputationRandom25MIT:rho'),
    edmNameFixedGridAll                  = cms.untracked.string('fixedGridRhoAll'),
    edmNameFixedGridFastjetAll           = cms.untracked.string('fixedGridRhoFastjetAll'),
#    edmNameKt6CaloJets                   = cms.untracked.string('kt6CaloJets:rho'),
#    edmNameKt6CaloJetsCentral            = cms.untracked.string('kt6CaloJetsCentral:rho'),
    edmNameKt6CaloJets                   = cms.untracked.string('kt6PFJets:rho'),
    edmNameKt6CaloJetsCentral            = cms.untracked.string('kt6PFJets:rho'),
    edmNameKt6PFJets                     = cms.untracked.string('kt6PFJets:rho'),
    edmNameKt6PFJetsCentralChargedPileUp = cms.untracked.string('kt6PFJetsCentralChargedPileUp:rho'),
    edmNameKt6PFJetsCentralNeutral       = cms.untracked.string('kt6PFJetsCentralNeutral:rho'),
    edmNameKt6PFJetsCentralNeutralTight  = cms.untracked.string('kt6PFJetsCentralNeutralTight:rho'),
    fillerType                           = cms.untracked.string('FillerPileupEnergyDensity')
  ),

  GeneralTracks  = cms.untracked.PSet(
    active                    = cms.untracked.bool(True),
    ecalAssocActive           = cms.untracked.bool(False),
    mitName                   = cms.untracked.string('Tracks'),
    edmName                   = cms.untracked.string('generalTracks'),
    trackingMapName           = cms.untracked.string('TrackingMap'),
    barrelSuperClusterMapName = cms.untracked.string('superClusterMap'),
    endcapSuperClusterMapName = cms.untracked.string('superClusterMap'),
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
    barrelSuperClusterMapName = cms.untracked.string('superClusterMap'),
    endcapSuperClusterMapName = cms.untracked.string('superClusterMap'),
    trackMapName              = cms.untracked.string('ConversionTracksMapName'),
    edmSimAssociationName     = cms.untracked.string(''),
    fillerType                = cms.untracked.string('FillerTracks')
  ),

  StandaloneMuonTracks = cms.untracked.PSet(
    active                    = cms.untracked.bool(True),
    ecalAssocActive           = cms.untracked.bool(False),
    mitName                   = cms.untracked.string('StandaloneMuonTracks'),
    edmName                   = cms.untracked.string('standAloneMuons'),
    trackingMapName           = cms.untracked.string('TrackingMap'),
    barrelSuperClusterMapName = cms.untracked.string('superClusterMap'),
    endcapSuperClusterMapName = cms.untracked.string('superClusterMap'),
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
    barrelSuperClusterMapName = cms.untracked.string('superClusterMap'),
    endcapSuperClusterMapName = cms.untracked.string('superClusterMap'),
    trackMapName              = cms.untracked.string('StandaloneMuonTracksWVtxConstraintMapName'),
    edmSimAssociationName     = cms.untracked.string(''),
    fillerType                = cms.untracked.string('FillerTracks')
  ),

  StandaloneCosmicMuonTracks = cms.untracked.PSet(
    active                    = cms.untracked.bool(True),
    ecalAssocActive           = cms.untracked.bool(False),
    mitName                   = cms.untracked.string('StandaloneCosmicMuonTracks'),
    edmName                   = cms.untracked.string('cosmicMuons'),
    trackingMapName           = cms.untracked.string('TrackingMap'),
    barrelSuperClusterMapName = cms.untracked.string('superClusterMap'),
    endcapSuperClusterMapName = cms.untracked.string('superClusterMap'),
    trackMapName              = cms.untracked.string('StandaloneCosmicMuonTracksMapName'),
    edmSimAssociationName     = cms.untracked.string(''),
    fillerType                = cms.untracked.string('FillerTracks')
  ),

  GlobalMuonTracks = cms.untracked.PSet(
    active                    = cms.untracked.bool(True),
    ecalAssocActive           = cms.untracked.bool(False),
    mitName                   = cms.untracked.string('GlobalMuonTracks'),
    edmName                   = cms.untracked.string('globalMuons'),
    trackingMapName           = cms.untracked.string('TrackingMap'),
    barrelSuperClusterMapName = cms.untracked.string('superClusterMap'),
    endcapSuperClusterMapName = cms.untracked.string('superClusterMap'),
    trackMapName              = cms.untracked.string('GlobalMuonTracksMapName'),
    edmSimAssociationName     = cms.untracked.string(''),
    fillerType                = cms.untracked.string('FillerTracks')
  ),

  GlobalCosmicMuonTracks = cms.untracked.PSet(
    active                    = cms.untracked.bool(True),
    ecalAssocActive           = cms.untracked.bool(False),
    mitName                   = cms.untracked.string('GlobalCosmicMuonTracks'),
    edmName                   = cms.untracked.string('globalCosmicMuons'),
    trackingMapName           = cms.untracked.string('TrackingMap'),
    barrelSuperClusterMapName = cms.untracked.string('superClusterMap'),
    endcapSuperClusterMapName = cms.untracked.string('superClusterMap'),
    trackMapName              = cms.untracked.string('GlobalCosmicMuonTracksMapName'),
    edmSimAssociationName     = cms.untracked.string(''),
    fillerType                = cms.untracked.string('FillerTracks')
  ),

  ConversionInOutTracks = cms.untracked.PSet(
    active                    = cms.untracked.bool(True),
    ecalAssocActive           = cms.untracked.bool(False),
    mitName                   = cms.untracked.string('ConversionInOutTracks'),
    edmName                   = cms.untracked.string('ckfInOutTracksFromConversions'),
    trackingMapName           = cms.untracked.string('TrackingMap'),
    barrelSuperClusterMapName = cms.untracked.string('superClusterMap'),
    endcapSuperClusterMapName = cms.untracked.string('superClusterMap'),
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
    barrelSuperClusterMapName = cms.untracked.string('superClusterMap'),
    endcapSuperClusterMapName = cms.untracked.string('superClusterMap'),
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
    barrelSuperClusterMapName = cms.untracked.string('superClusterMap'),
    endcapSuperClusterMapName = cms.untracked.string('superClusterMap'),
    trackMapName              = cms.untracked.string('GsfTracksMapName'),
    edmSimAssociationName     = cms.untracked.string('assoc2GsfTracks'),
    fillerType                = cms.untracked.string('FillerTracks')
  ),

  Muons = cms.untracked.PSet(
    active              = cms.untracked.bool(True),
    mitName             = cms.untracked.string('Muons'),
    edmName             = cms.untracked.string('muons'),
    expectedHitsName    = cms.untracked.string(''),
    globalTrackMapName  = cms.untracked.string('GlobalMuonTracksMapName'),
    staTrackMapName     = cms.untracked.string('StandaloneMuonTracksMapName'),
    staVtxTrackMapName  = cms.untracked.string('StandaloneMuonTracksWVtxConstraintMapName'),
    trackerTrackMapName = cms.untracked.string('TracksMapName'),
    muonMapName         = cms.untracked.string('muonMap'),
    pvEdmName           = cms.untracked.string('offlinePrimaryVertices'),
    pvBSEdmName         = cms.untracked.string('offlinePrimaryVerticesWithBS'),
    beamSpotName        = cms.untracked.string('offlineBeamSpot'),
    pvBeamSpotName      = cms.untracked.string(offlinePrimaryVertices.beamSpotLabel.getModuleLabel()),
    pvbsBeamSpotNam     = cms.untracked.string(offlinePrimaryVertices.beamSpotLabel.getModuleLabel()),
    fitUnbiasedVertex   = cms.untracked.bool(False),
    fillerType          = cms.untracked.string('FillerMuons')
  ),

  CosmicMuons = cms.untracked.PSet(
    active              = cms.untracked.bool(True),
    mitName             = cms.untracked.string('CosmicMuons'),
    edmName             = cms.untracked.string('muonsFromCosmics'),
    expectedHitsName    = cms.untracked.string(''),
    globalTrackMapName  = cms.untracked.string('GlobalCosmicMuonTracksMapName'),
    staTrackMapName     = cms.untracked.string('StandaloneCosmicMuonTracksMapName'),
    staVtxTrackMapName  = cms.untracked.string('StandaloneCosmicMuonTracksMapName'),
    muonMapName         = cms.untracked.string('cosmicMuonMap'),
    pvEdmName           = cms.untracked.string('offlinePrimaryVertices'),
    pvBSEdmName         = cms.untracked.string('offlinePrimaryVerticesWithBS'),
    fitUnbiasedVertex   = cms.untracked.bool(False),
    fillerType          = cms.untracked.string('FillerMuons')
  ),

  Electrons = cms.untracked.PSet(
    active                           = cms.untracked.bool(True),
    mitName                          = cms.untracked.string('Electrons'),
    edmName                          = cms.untracked.string('gedGsfElectrons'),
    expectedHitsName                 = cms.untracked.string(''),
    gsfTrackMapName                  = cms.untracked.string('GsfTracksMapName'),
    trackerTrackMapName              = cms.untracked.string('TracksMapName'),
    barrelEcalRecHitName             = cms.untracked.string('reducedEcalRecHitsEB'),
    endcapEcalRecHitName             = cms.untracked.string('reducedEcalRecHitsEE'),
    barrelBasicClusterName           = cms.untracked.string('hybridSuperClusters:hybridBarrelBasicClusters'),
    endcapBasicClusterName           = cms.untracked.string('multi5x5BasicClusters:multi5x5EndcapBasicClusters'),
    barrelSuperClusterName           = cms.untracked.string('correctedHybridSuperClusters'),
    endcapSuperClusterName           = cms.untracked.string('multi5x5SuperClusters:multi5x5EndcapSuperClusters'),
    barrelSuperClusterMapName        = cms.untracked.string('superClusterMap'),
    endcapSuperClusterMapName        = cms.untracked.string('superClusterMap'),
    requireClusterAndGsfMap          = cms.untracked.bool(True),
    pfSuperClusterMapName            = cms.untracked.string('PFSuperClusterMap'),
    pfClusterMapName                 = cms.untracked.string('PFBasicClusterMap'),
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
    active        = cms.untracked.bool(False),
    mitName       = cms.untracked.string('MergedStableTest'),
    edmName       = cms.untracked.string('mergedConversionsStable'),
    trackMapNames = cms.untracked.vstring('ConversionInOutTracksMapName',
                                          'ConversionOutInTracksMapName'),
    basePartMap   = cms.untracked.string('MergedTestStableMapName'),
    trackPartMap  = cms.untracked.string('MergedTestStableTrackMapName'),
    fillerType    = cms.untracked.string('FillerStableParts')
  ),

  ElectronsStable = cms.untracked.PSet(
    active        = cms.untracked.bool(True),
    mitName       = cms.untracked.string('ElectronsStable'),
    edmName       = cms.untracked.string('generalElectronsStable'),
    trackMapNames = cms.untracked.vstring('TracksMapName'),
    basePartMap   = cms.untracked.string('ElectronsStableMapName'),
    trackPartMap  = cms.untracked.string('ElectronsStableTrackMapName'),
    fillerType    = cms.untracked.string('FillerStableParts')
  ),

  ConversionInOutElectronsStable = cms.untracked.PSet(
    active        = cms.untracked.bool(True),
    mitName       = cms.untracked.string('ConversionInOutElectronsStable'),
    edmName       = cms.untracked.string('ckfInOutElectronsStable'),
    trackMapNames = cms.untracked.vstring('ConversionInOutTracksMapName'),
    basePartMap   = cms.untracked.string('ConversionInOutElectronsStableMapName'),
    trackPartMap  = cms.untracked.string('ConversionInOutElectronsStableTrackMapName'),
    fillerType    = cms.untracked.string('FillerStableParts')
  ),

  ConversionOutInElectronsStable = cms.untracked.PSet(
    active        = cms.untracked.bool(True),
    mitName       = cms.untracked.string('ConversionOutInElectronsStable'),
    edmName       = cms.untracked.string('ckfOutInElectronsStable'),
    trackMapNames = cms.untracked.vstring('ConversionOutInTracksMapName'),
    basePartMap   = cms.untracked.string('ConversionOutInElectronsStableMapName'),
    trackPartMap  = cms.untracked.string('ConversionOutInElectronsStableTrackMapName'),
    fillerType    = cms.untracked.string('FillerStableParts')
  ),

  GsfElectronsStable = cms.untracked.PSet(
    active        = cms.untracked.bool(True),
    mitName       = cms.untracked.string('GsfElectronsStable'),
    edmName       = cms.untracked.string('gsfElectronsStable'),
    trackMapNames = cms.untracked.vstring('GsfTracksMapName'),
    basePartMap   = cms.untracked.string('ElectronsStableGsfMapName'),
    trackPartMap  = cms.untracked.string('ElectronsStableGsfTrackMapName'),
    fillerType    = cms.untracked.string('FillerStableParts')
  ),

  ConversionStepElectronsStable = cms.untracked.PSet(
    active        = cms.untracked.bool(True),
    mitName       = cms.untracked.string('ConversionStepElectronsStable'),
    edmName       = cms.untracked.string('conversionStepElectronsStable'),
    trackMapNames = cms.untracked.vstring('ConversionTracksMapName'),
    basePartMap   = cms.untracked.string('ElectronsStableConvStepMapName'),
    trackPartMap  = cms.untracked.string('ElectronsStableConvStepTrackMapName'),
    fillerType    = cms.untracked.string('FillerStableParts')
  ),

  MergedElectronsStable = cms.untracked.PSet(
    active        = cms.untracked.bool(True),
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
    edmName             = cms.untracked.string('allConversions'),
    stablePartMaps      = cms.untracked.vstring('ElectronsStableTrackMapName',
                                                'ConversionInOutElectronsStableTrackMapName',
                                                'ConversionOutInElectronsStableTrackMapName',
                                                'ElectronsStableGsfTrackMapName',
                                                'ElectronsStableConvStepTrackMapName'),
    conversionMapName   = cms.untracked.string('ConversionsMapName'),
    fillerType          = cms.untracked.string('FillerConversionsDecay')
  ),

  PFPhotonConversions = cms.untracked.PSet(
    active              = cms.untracked.bool(True),
    mitName             = cms.untracked.string('PFPhotonConversions'),
    edmName             = cms.untracked.string('gedPhotonCore'),
    stablePartMaps      = cms.untracked.vstring('ElectronsStableTrackMapName',
                                                'ConversionInOutElectronsStableTrackMapName',
                                                'ConversionOutInElectronsStableTrackMapName',
                                                'ElectronsStableGsfTrackMapName',
                                                'ElectronsStableConvStepTrackMapName'),
    conversionMapName   = cms.untracked.string('PFPhotonConversionsMapName'),
    fillerType          = cms.untracked.string('FillerConversionsDecay')
  ),

  Photons = cms.untracked.PSet(
    active                    = cms.untracked.bool(True),
    mitName                   = cms.untracked.string('Photons'),
    edmName                   = cms.untracked.string('gedPhotons'),
    phIDCutBasedTightName     = cms.untracked.string('PhotonIDProdGED:PhotonCutBasedIDTight'),
    phIDCutBasedLooseName     = cms.untracked.string('PhotonIDProdGED:PhotonCutBasedIDLoose'),
    conversionMapName         = cms.untracked.string('ConversionsMapName'),
    barrelSuperClusterMapName = cms.untracked.string('superClusterMap'),
    endcapSuperClusterMapName = cms.untracked.string('superClusterMap'),
    pfCandMapName             = cms.untracked.string('PFCandMapName'),
    photonMapName             = cms.untracked.string('PhotonMapName'),
    fillerType                = cms.untracked.string('FillerPhotons')
  ),

  PFCandidates = cms.untracked.PSet(
    active                        = cms.untracked.bool(True),
    mitName                       = cms.untracked.string('PFCandidates'),
    edmName                       = cms.untracked.string('particleFlowPtrs'),
    edmPfNoPileupName             = cms.untracked.string('pfNoElectron'),
    trackerTrackMapNames          = cms.untracked.vstring('TracksMapName',
                                                          'ConversionTracksMapName'),
    gsfTrackMapName               = cms.untracked.string('GsfTracksMapName'),
    muonMapName                   = cms.untracked.string('MuonMapName'),
    conversionMapName             = cms.untracked.string('ConversionsMapName'),
    barrelSuperClusterMapName     = cms.untracked.string('superClusterMap'),
    endcapSuperClusterMapName     = cms.untracked.string('superClusterMap'),
    pfElectronSuperClusterMapName = cms.untracked.string('PFSuperClusterMap'),        
    electronMapName               = cms.untracked.string('electronMap'),
    photonMapName                 = cms.untracked.string('PFPhotonMapName'),
    pfCandMapName                 = cms.untracked.string('PFCandMapName'),
    pfNoPileupCandMapName         = cms.untracked.string('PFNoPileupCandMapName'),
    fillerType                    = cms.untracked.string('FillerPFCandidates'),
    allowMissingPhotonRef         = cms.untracked.bool  (False)
  ),

  AKT4GenJets = cms.untracked.PSet(
    active                        = cms.untracked.bool(True),
    flavorMatchingActive          = cms.untracked.bool(False),
    mitName                       = cms.untracked.string('AKT4GenJets'),
    edmName                       = cms.untracked.string('ak4GenJets'),
    flavorMatchingByReferenceName = cms.untracked.string('GenAKT4byRef'),
    flavorMatchingDefinition      = cms.untracked.string('Algorithmic'),
    fillerType                    = cms.untracked.string('FillerGenJets')
  ),

  AKT8GenJets = cms.untracked.PSet(
    active                        = cms.untracked.bool(True),
    flavorMatchingActive          = cms.untracked.bool(False),
    mitName                       = cms.untracked.string('AKT8GenJets'),
    edmName                       = cms.untracked.string('ak8GenJets'),
    flavorMatchingByReferenceName = cms.untracked.string('GenAKT8byRef'),
    flavorMatchingDefinition      = cms.untracked.string('Algorithmic'),
    fillerType                    = cms.untracked.string('FillerGenJets')
  ),

  AKt4PFJets = cms.untracked.PSet(
    active                                   = cms.untracked.bool(True),
    flavorMatchingActive                     = cms.untracked.bool(False),
    bTaggingActive                           = cms.untracked.bool(False),
    jetToVertexActive                        = cms.untracked.bool(False),
    jetCorrectionsActive                     = cms.untracked.bool(False),
    fastJetCorrectionsActive                 = cms.untracked.bool(False),
    mitName                                  = cms.untracked.string('AKt4PFJets'),
    edmName                                  = cms.untracked.string('ak4PFJets'),
    rhoName                                  = cms.untracked.string('kt6PFJetsForRhoComputation:rho'),
    jetToVertexAlphaName                     = cms.untracked.string('jetVertexAlphaAKT4PF:Var'),
    jetToVertexBetaName                      = cms.untracked.string('jetVertexBetaAKT4PF:Var'),
    L2JetCorrectorName                       = cms.untracked.string('L2RelativeJetCorrectorAKT4PF'),
    L3JetCorrectorName                       = cms.untracked.string('L3AbsoluteJetCorrectorAKT4PF'),
    flavorMatchingByReferenceName            = cms.untracked.string('AKT4PFbyRef'),
    flavorMatchingDefinition                 = cms.untracked.string('Algorithmic'),
    JetProbabilityBJetTagsName               = cms.untracked.string('newJetProbabilityBJetTags'),
    JetBProbabilityBJetTagsName              = cms.untracked.string('newJetBProbabilityBJetTags'),
    SimpleSecondaryVertexHighEffBJetTagsName = cms.untracked.string('newSimpleSecondaryVertexHighEffBJetTags'),
    SimpleSecondaryVertexHighPurBJetTagsName = cms.untracked.string('newSimpleSecondaryVertexHighPurBJetTags'),
    CombinedSecondaryVertexBJetTagsName      = cms.untracked.string('newCombinedSecondaryVertexBJetTags'),
    CombinedSecondaryVertexMVABJetTagsName   = cms.untracked.string('newCombinedSecondaryVertexMVABJetTags'),
    GhostTrackBJetTagsName                   = cms.untracked.string('newGhostTrackBJetTagsName'),
    TrackCountingHighEffBJetTagsName         = cms.untracked.string('newTrackCountingHighEffBJetTags'),
    TrackCountingHighPurBJetTagsName         = cms.untracked.string('newTrackCountingHighPurBJetTags'),
    pfCandMapName                            = cms.untracked.string('PFCandMapName'),
    jetMapName                               = cms.untracked.string('AKT4PFJetMap'),
    fillerType                               = cms.untracked.string('FillerPFJets')
  ),

  AKt4PFJetsCHS = cms.untracked.PSet(
    active                                   = cms.untracked.bool(True),
    flavorMatchingActive                     = cms.untracked.bool(False),
    bTaggingActive                           = cms.untracked.bool(False),
    jetToVertexActive                        = cms.untracked.bool(False),
    jetCorrectionsActive                     = cms.untracked.bool(False),
    fastJetCorrectionsActive                 = cms.untracked.bool(False),
    mitName                                  = cms.untracked.string('AKt4PFJetsCHS'),
    edmName                                  = cms.untracked.string('pfJets'),
    rhoName                                  = cms.untracked.InputTag('kt6PFJetsForRhoComputationNoPileup','rho'),
    jetToVertexAlphaName                     = cms.untracked.string('jetVertexAlphaAKT4PF:Var'),
    jetToVertexBetaName                      = cms.untracked.string('jetVertexBetaAKT4PF:Var'),
    L2JetCorrectorName                       = cms.untracked.string('L2RelativeJetCorrectorAKT4PF'),
    L3JetCorrectorName                       = cms.untracked.string('L3AbsoluteJetCorrectorAKT4PF'),
    flavorMatchingByReferenceName            = cms.untracked.string('AKT4PFbyRef'),
    flavorMatchingDefinition                 = cms.untracked.string('Algorithmic'),
    JetProbabilityBJetTagsName               = cms.untracked.string('newJetProbabilityBJetTags'),
    JetBProbabilityBJetTagsName              = cms.untracked.string('newJetBProbabilityBJetTags'),
    SimpleSecondaryVertexHighEffBJetTagsName = cms.untracked.string('newSimpleSecondaryVertexHighEffBJetTags'),
    SimpleSecondaryVertexHighPurBJetTagsName = cms.untracked.string('newSimpleSecondaryVertexHighPurBJetTags'),
    CombinedSecondaryVertexBJetTagsName      = cms.untracked.string('newCombinedSecondaryVertexBJetTags'),
    CombinedSecondaryVertexMVABJetTagsName   = cms.untracked.string('newCombinedSecondaryVertexMVABJetTags'),
    GhostTrackBJetTagsName                   = cms.untracked.string('newGhostTrackBJetTagsName'),
    TrackCountingHighEffBJetTagsName         = cms.untracked.string('newTrackCountingHighEffBJetTags'),
    TrackCountingHighPurBJetTagsName         = cms.untracked.string('newTrackCountingHighPurBJetTags'),
    #SoftMuonBJetTagsName                     = cms.untracked.string('newSoftMuonBJetTags'),
    #SoftMuonByIP3dBJetTagsName               = cms.untracked.string('newSoftMuonByIP3dBJetTags'),
    #SoftMuonByPtBJetTagsName                 = cms.untracked.string('newSoftMuonByPtBJetTags'),
    #SoftElectronByIP3dBJetTagsName           = cms.untracked.string('newSoftElectronByIP3dBJetTags'),
    #SoftElectronByPtBJetTagsName             = cms.untracked.string('newSoftElectronByPtBJetTags'),
    pfCandMapName                            = cms.untracked.string('PFNoPileupCandMapName'),
    jetMapName                               = cms.untracked.string('AKt4PFJetCHSMap'),
    fillerType                               = cms.untracked.string('FillerPFJets')
  ),

  AKt8PFJets = cms.untracked.PSet(
    active                                   = cms.untracked.bool(True),
    flavorMatchingActive                     = cms.untracked.bool(False),
    bTaggingActive                           = cms.untracked.bool(False),
    jetToVertexActive                        = cms.untracked.bool(False),
    jetCorrectionsActive                     = cms.untracked.bool(False),
    fastJetCorrectionsActive                 = cms.untracked.bool(False),
    mitName                                  = cms.untracked.string('AKt8PFJets'),
    edmName                                  = cms.untracked.string('ak8PFJets'),
    rhoName                                  = cms.untracked.InputTag('kt6PFJetsForRhoComputation','rho'),
    jetToVertexAlphaName                     = cms.untracked.string('jetVertexAlphaAKT8PF:Var'),
    jetToVertexBetaName                      = cms.untracked.string('jetVertexBetaAKT8PF:Var'),
    L2JetCorrectorName                       = cms.untracked.string('L2RelativeJetCorrectorAKT8PF'),
    L3JetCorrectorName                       = cms.untracked.string('L3AbsoluteJetCorrectorAKT8PF'),
    flavorMatchingByReferenceName            = cms.untracked.string('AKT8PFbyRef'),
    flavorMatchingDefinition                 = cms.untracked.string('Algorithmic'),
    JetProbabilityBJetTagsName               = cms.untracked.string('newJetProbabilityBJetTags'),
    JetBProbabilityBJetTagsName              = cms.untracked.string('newJetBProbabilityBJetTags'),
    SimpleSecondaryVertexHighEffBJetTagsName = cms.untracked.string('newSimpleSecondaryVertexHighEffBJetTags'),
    SimpleSecondaryVertexHighPurBJetTagsName = cms.untracked.string('newSimpleSecondaryVertexHighPurBJetTags'),
    CombinedSecondaryVertexBJetTagsName      = cms.untracked.string('newCombinedSecondaryVertexBJetTags'),
    CombinedSecondaryVertexMVABJetTagsName   = cms.untracked.string('newCombinedSecondaryVertexMVABJetTags'),
    GhostTrackBJetTagsName                   = cms.untracked.string('newGhostTrackBJetTagsName'),
    TrackCountingHighEffBJetTagsName         = cms.untracked.string('newTrackCountingHighEffBJetTags'),
    TrackCountingHighPurBJetTagsName         = cms.untracked.string('newTrackCountingHighPurBJetTags'),
    pfCandMapName                            = cms.untracked.string('PFCandMapName'),
    jetMapName                               = cms.untracked.string('AKT8PFJetMap'),
    fillerType                               = cms.untracked.string('FillerPFJets')
  ),

  AKt8PFJetsCHS = cms.untracked.PSet(
    active                                   = cms.untracked.bool(True),
    flavorMatchingActive                     = cms.untracked.bool(False),
    bTaggingActive                           = cms.untracked.bool(False),
    jetToVertexActive                        = cms.untracked.bool(False),
    jetCorrectionsActive                     = cms.untracked.bool(False),
    fastJetCorrectionsActive                 = cms.untracked.bool(False),
    mitName                                  = cms.untracked.string('AKt8PFJetsCHS'),
    edmName                                  = cms.untracked.string('pfJets'),
    rhoName                                  = cms.untracked.InputTag('kt6PFJetsForRhoComputationNoPileup','rho'),
    jetToVertexAlphaName                     = cms.untracked.string('jetVertexAlphaAKT8PF:Var'),
    jetToVertexBetaName                      = cms.untracked.string('jetVertexBetaAKT8PF:Var'),
    L2JetCorrectorName                       = cms.untracked.string('L2RelativeJetCorrectorAKT8PF'),
    L3JetCorrectorName                       = cms.untracked.string('L3AbsoluteJetCorrectorAKT8PF'),
    flavorMatchingByReferenceName            = cms.untracked.string('AKT8PFbyRef'),
    flavorMatchingDefinition                 = cms.untracked.string('Algorithmic'),
    JetProbabilityBJetTagsName               = cms.untracked.string('newJetProbabilityBJetTags'),
    JetBProbabilityBJetTagsName              = cms.untracked.string('newJetBProbabilityBJetTags'),
    SimpleSecondaryVertexHighEffBJetTagsName = cms.untracked.string('newSimpleSecondaryVertexHighEffBJetTags'),
    SimpleSecondaryVertexHighPurBJetTagsName = cms.untracked.string('newSimpleSecondaryVertexHighPurBJetTags'),
    CombinedSecondaryVertexBJetTagsName      = cms.untracked.string('newCombinedSecondaryVertexBJetTags'),
    CombinedSecondaryVertexMVABJetTagsName   = cms.untracked.string('newCombinedSecondaryVertexMVABJetTags'),
    GhostTrackBJetTagsName                   = cms.untracked.string('newGhostTrackBJetTagsName'),
    TrackCountingHighEffBJetTagsName         = cms.untracked.string('newTrackCountingHighEffBJetTags'),
    TrackCountingHighPurBJetTagsName         = cms.untracked.string('newTrackCountingHighPurBJetTags'),
    #SoftMuonBJetTagsName                     = cms.untracked.string('newSoftMuonBJetTags'),
    #SoftMuonByIP3dBJetTagsName               = cms.untracked.string('newSoftMuonByIP3dBJetTags'),
    #SoftMuonByPtBJetTagsName                 = cms.untracked.string('newSoftMuonByPtBJetTags'),
    #SoftElectronByIP3dBJetTagsName           = cms.untracked.string('newSoftElectronByIP3dBJetTags'),
    #SoftElectronByPtBJetTagsName             = cms.untracked.string('newSoftElectronByPtBJetTags'),
    pfCandMapName                            = cms.untracked.string('PFNoPileupCandMapName'),
    jetMapName                               = cms.untracked.string('AKt8PFJetCHSMap'),
    fillerType                               = cms.untracked.string('FillerPFJets')
  ),

  GenMet = cms.untracked.PSet(
    active     = cms.untracked.bool(False),
    mitName    = cms.untracked.string('GenMet'),
    edmName    = cms.untracked.string('genMetTrue'),
    fillerType = cms.untracked.string('FillerGenMet')
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

  HPSTaus = cms.untracked.PSet(
    active                                             = cms.untracked.bool(True),
    hpsActive                                          = cms.untracked.bool(True),
    mitName                                            = cms.untracked.string('HPSTaus'),
    edmName                                            = cms.untracked.string('hpsPFTauProducer'),
    discriminationByLooseElectronRejectionName         = cms.untracked.string('hpsPFTauDiscriminationByLooseElectronRejection'),
    discriminationByMediumElectronRejectionName        = cms.untracked.string('hpsPFTauDiscriminationByMediumElectronRejection'),
    discriminationByTightElectronRejectionName         = cms.untracked.string('hpsPFTauDiscriminationByTightElectronRejection'),
    discriminationByMVAElectronRejectionName           = cms.untracked.string('hpsPFTauDiscriminationByMVAElectronRejection'),
    discriminationByLooseMuonRejectionName             = cms.untracked.string('hpsPFTauDiscriminationByLooseMuonRejection'),
    discriminationByMediumMuonRejectionName            = cms.untracked.string('hpsPFTauDiscriminationByMediumMuonRejection'),
    discriminationByTightMuonRejectionName             = cms.untracked.string('hpsPFTauDiscriminationByTightMuonRejection'),
    discriminationByDecayModeFindingName               = cms.untracked.string('hpsPFTauDiscriminationByDecayModeFinding'),
    discriminationByVLooseIsolationName                = cms.untracked.string('hpsPFTauDiscriminationByVLooseIsolation'),
    discriminationByLooseIsolationName                 = cms.untracked.string('hpsPFTauDiscriminationByLooseIsolation'),
    discriminationByMediumIsolationName                = cms.untracked.string('hpsPFTauDiscriminationByMediumIsolation'),
    discriminationByTightIsolationName                 = cms.untracked.string('hpsPFTauDiscriminationByTightIsolation'),
    discriminationByVLooseCombinedIsolationDBSumPtCorr = cms.untracked.string('hpsPFTauDiscriminationByVLooseCombinedIsolationDBSumPtCorr'),
    discriminationByLooseCombinedIsolationDBSumPtCorr  = cms.untracked.string('hpsPFTauDiscriminationByLooseCombinedIsolationDBSumPtCorr'),
    discriminationByMediumCombinedIsolationDBSumPtCorr = cms.untracked.string('hpsPFTauDiscriminationByMediumCombinedIsolationDBSumPtCorr'),
    discriminationByTightCombinedIsolationDBSumPtCorr  = cms.untracked.string('hpsPFTauDiscriminationByTightCombinedIsolationDBSumPtCorr'),
    discriminationByRawCombinedIsolationDBSumPtCorr    = cms.untracked.string('hpsPFTauDiscriminationByRawCombinedIsolationDBSumPtCorr'),
    trackMapNames                                      = cms.untracked.vstring('TracksMapName',
                                                                               'ConversionTracksMapName'),
    jetMapName                                         = cms.untracked.string(''),
    pfCandMapName                                      = cms.untracked.string('PFCandMapName'),
    tauMapName                                         = cms.untracked.string('tauMap'),
    fillerType                                         = cms.untracked.string('FillerPFTaus')
  ),

  DCASig     = cms.untracked.PSet(
    active          = cms.untracked.bool(True),
    mitName         = cms.untracked.string('DCASig'),
    edmElectronName = cms.untracked.string('gedGsfElectrons'),
    edmMuonName     = cms.untracked.string('muons'),
    edmTauName      = cms.untracked.string('hpsPFTauProducer'),
    electronMapName = cms.untracked.string('electronMap'),
    muonMapName     = cms.untracked.string('muonMap'),
    tauMapName      = cms.untracked.string('tauMap'),
    electronPtMin   = cms.untracked.double(5),
    muonPtMin       = cms.untracked.double(5),
    tauPtMin        = cms.untracked.double(10),
    checkDCARef     = cms.untracked.bool(True),
    fillerType      = cms.untracked.string('FillerDCASig')
  )
)
