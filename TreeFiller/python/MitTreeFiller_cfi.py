import FWCore.ParameterSet.Config as cms

from RecoVertex.PrimaryVertexProducer.OfflinePrimaryVertices_cfi import offlinePrimaryVertices
# This python module is actually not the offlinePrimaryVertex saved in AOD
# This module is cloned to unsortedOfflinePrimaryVertices and used as an input to
# the real offlinePrimaryVertex, which is a clone of sortedPrimaryVertices.
# Nevertheless, the reference of the beam spot from offlinePrimaryVertex is ultimately
# a reference to the beam spot in this module.

MitTreeFiller = cms.EDAnalyzer("FillMitTree",
  fillers = cms.untracked.vstring(
    'MetaInfos',
    'Trigger',
    'MCEventInfo',
    'MCParticles',
    'MCAllVertexes',
    'PileupInfo',
    'PileupEnergyDensity',
    'EvtSelData',
    'BeamSpot',
    'PFEcalBarrelSuperClusters',
    'PFEcalEndcapSuperClusters',
    'BasicClusters',
    #'PsClusters', # GED preshower clusters are of type CaloClusterCollection - either FillerPsClusters or FillerBasicClusters must be updated
    'SuperClusters',
    'GeneralTracks',
    'ConversionStepTracks',
    'StandaloneMuonTracks',
    'StandaloneCosmicMuonTracks',
    'StandaloneMuonTracksWVtxConstraint',
    'GlobalMuonTracks',
    'GlobalCosmicMuonTracks',
    'FirstHitMuonTracks',
    'PickyMuonTracks',
    'DYTMuonTracks',
    'ConversionInOutTracks',
    'ConversionOutInTracks',
    'GsfTracks',
    'PrimaryVertexes',
    'PrimaryVertexesBS',
#     'InclusiveSecondaryVertexes',
    'PFCandidates',
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
    'Photons',
    'AKT4GenJets',
    'AKT8GenJets',
    'AKt4PFJets',
    'AKt4PFJetsCHS',
    'AKt4PuppiJets',
    'AKt8FatJetsCHS',
    'CA15FatJetsCHS',
    'AKt8FatJetsPuppi',
    'CA15FatJetsPuppi',
    'PFMet',
    'PuppiMet',
    'CaloMet',
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
    l1GtReadRecEdmName = cms.untracked.string('gtDigis'),
    evtMitName         = cms.untracked.string('EventHeader'),
    fillerType         = cms.untracked.string('FillerMetaInfos')
  ),

  Trigger = cms.untracked.PSet(
    active              = cms.untracked.bool(True),
    hltEvtEdmName       = cms.untracked.string('hltTriggerSummaryAOD'), # unset when filling from MiniAOD
    hltObjsEdmName      = cms.untracked.string(''), # set when filling from MiniAOD
    hltResEdmName       = cms.untracked.string('TriggerResults'),
    l1GtMenuLiteEdmName = cms.untracked.string('l1GtTriggerMenuLite'),
    l1GtRecordEdmName   = cms.untracked.string('l1GtRecord'),
    l1GtReadRecEdmName  = cms.untracked.string('gtDigis'),
    hltProcName         = cms.untracked.string('HLT'),
    hltTreeMitName      = cms.untracked.string('HLT'),
    hltTableMitName     = cms.untracked.string('HLTTriggerTable'),
    hltLabelMitName     = cms.untracked.string('HLTLabels'),
    hltMenuMitName      = cms.untracked.string('HLTMenus'),
    hltBitsMitName      = cms.untracked.string('HLTBits'),
    hltObjsMitName      = cms.untracked.string('HLTObjects'),
    l1Active            = cms.untracked.bool(True),
    l1TechBitsMitName   = cms.untracked.string('L1TechBits'),
    l1AlgoBitsMitName   = cms.untracked.string('L1AlgoBits'),
    fillerType          = cms.untracked.string('FillerTrigger')
  ),

  MCEventInfo = cms.untracked.PSet(
    active                = cms.untracked.bool(False),
    genHepMCEventEdmName  = cms.untracked.string('generator'),
    genEvtInfoEdmName     = cms.untracked.string('generator'),
    lheEventEdmName       = cms.untracked.string('externalLHEProducer'),
    lheRunInfoEdmName     = cms.untracked.string('externalLHEProducer'),
    flavorHistoryActive   = cms.untracked.bool(False),
    flavorHistEdmName     = cms.untracked.string('flavorHistoryFilter'),
    fillerType            = cms.untracked.string('FillerMCEventInfo')
  ),

  MCParticles = cms.untracked.PSet(
    active          = cms.untracked.bool(False),
    genActive       = cms.untracked.bool(True),
    simActive       = cms.untracked.bool(False),
    trackingActive  = cms.untracked.bool(False),
    fillTracking    = cms.untracked.bool(False),
    mitName         = cms.untracked.string('MCParticles'),
    mitTrackingName = cms.untracked.string('TrackingParticles'),
    vertexesName    = cms.untracked.string('MCAllVertexes'), # will find input vertices directly from MCAllVertexes array
    genSource       = cms.untracked.string('GenParticleCollection'),
    genEdmName      = cms.untracked.string('genParticles'),
    simEdmName      = cms.untracked.string('g4SimHits'),
    trackingEdmName = cms.untracked.string('mergedtruth:MergedTrackTruth'),
    genMapName      = cms.untracked.string('GenMap'),
    simMapName      = cms.untracked.string('SimMap'),
    trackingMapName = cms.untracked.string(''), # tracking is not active
    fillerType      = cms.untracked.string('FillerMCParticles')
  ),

  MCAllVertexes = cms.untracked.PSet(
    active        = cms.untracked.bool(True),
    simActive     = cms.untracked.bool(False),
    genSource     = cms.untracked.string('GenParticleCollection'),
    mitName       = cms.untracked.string('MCAllVertexes'),
    edmName       = cms.untracked.string('genParticles'),
    genVtxMapName = cms.untracked.string(''), # use if source is HepMCProduct
    simEdmName    = cms.untracked.string(''), # use if simActive = True
    simVtxMapName = cms.untracked.string(''), # use if simActive = True
    fillerType    = cms.untracked.string('FillerMCVertexes')
  ),

  EvtSelData = cms.untracked.PSet(
    active      = cms.untracked.bool(True),
    mitName     = cms.untracked.string('EvtSelData'),
    filterLabels = cms.untracked.vstring('HBHENoiseFilter',
                                         'HBHENoiseIsoFilter',
                                         'EcalDeadCellTriggerPrimitiveFilter',
                                         'EcalDeadCellBoundaryEnergyFilter',
                                         'EEBadScFilter',
                                         'CSCTightHaloFilter',
                                         'GlobalTightHaloFilter',
                                         'GlobalSuperTightHaloFilter',
                                         'CHTrackResolutionFilter',
                                         'MuBadTrackFilter',
                                         'BadPFMuonFilter',
                                         'BadChargedCandidateFilter'),
    inputLabels = cms.untracked.vstring('HBHENoiseFilterResultProducer:HBHENoiseFilterResultRun2Loose',
                                        'HBHENoiseFilterResultProducer:HBHEIsoNoiseFilterResult',
                                        'EcalDeadCellTriggerPrimitiveFilter',
                                        'EcalDeadCellBoundaryEnergyFilter',
                                        'eeBadScFilter',
                                        'CSCTightHalo2015Filter',
                                        'globalTightHalo2016Filter',
                                        'globalSuperTightHalo2016Filter',
                                        'chargedHadronTrackResolutionFilter',
                                        'muonBadTrackFilter',
                                        'BadPFMuonFilter',
                                        'BadChargedCandidateFilter'),
    invertedFilters = cms.untracked.vstring(),
    patFilterResultsName          = cms.untracked.string(''), # set when filling from miniAOD
    fillerType                    = cms.untracked.string('FillerEvtSelData')
  ),

  BeamSpot = cms.untracked.PSet(
    active     = cms.untracked.bool(True),
    mitName    = cms.untracked.string('BeamSpot'),
    edmName    = cms.untracked.string('offlineBeamSpot'),
    fillerType = cms.untracked.string('FillerBeamSpot')
  ),

  PrimaryVertexes = cms.untracked.PSet(
    active           = cms.untracked.bool(True),
    mitName          = cms.untracked.string('PrimaryVertexes'),
    edmName          = cms.untracked.string('offlinePrimaryVertices'),
    vertexMapName    = cms.untracked.string('PrimaryVertexMap'),
    trackMapName     = cms.untracked.string('TracksMap'),
    trkAssocByPacked = cms.untracked.bool(False),
    fillerType       = cms.untracked.string('FillerVertexes')
  ),

  PrimaryVertexesBS = cms.untracked.PSet(
    active           = cms.untracked.bool(True),
    mitName          = cms.untracked.string('PrimaryVertexesBeamSpot'),
    edmName          = cms.untracked.string('offlinePrimaryVerticesWithBS'),
    vertexMapName    = cms.untracked.string('PrimaryVertexBSMap'),
    trackMapName     = cms.untracked.string('TracksMap'),
    trkAssocByPacked = cms.untracked.bool(False),
    fillerType       = cms.untracked.string('FillerVertexes')
  ),

#  InclusiveSecondaryVertexes = cms.untracked.PSet(
#    active        = cms.untracked.bool(True),
#    mitName       = cms.untracked.string('InclusiveSecondaryVertexes'),
#    edmName       = cms.untracked.string('inclusiveSecondaryVertices'),
#    vertexMapName = cms.untracked.string('InclusiveSecondaryVertexMap'),
#    trackMapName  = cms.untracked.string('TracksMap'),
#    trkAssocByPacked = cms.untracked.bool(False),
#    fillerType    = cms.untracked.string('FillerVertexes')
#  ),

  PFEcalBarrelSuperClusters = cms.untracked.PSet(
    active                = cms.untracked.bool(True),
    mitName               = cms.untracked.string('PFEcalBarrelSuperClusters'),
    edmName               = cms.untracked.string('particleFlowSuperClusterECAL:particleFlowSuperClusterECALBarrel'),
    basicClusterMapName   = cms.untracked.string(''),
    superClusterMapName   = cms.untracked.string('pfEcalBarrelSuperClusterMap'),
    superClusterIdMapName = cms.untracked.string(''),
    #YI caloTowerName         = cms.untracked.string('towerMaker'),
    fillerType            = cms.untracked.string('FillerSuperClusters')
  ),

  PFEcalEndcapSuperClusters = cms.untracked.PSet(
    active                = cms.untracked.bool(True),
    mitName               = cms.untracked.string('PFEcalEndcapSuperClusters'),
    edmName               = cms.untracked.string('particleFlowSuperClusterECAL:particleFlowSuperClusterECALEndcapWithPreshower'),
    basicClusterMapName   = cms.untracked.string(''),
    superClusterMapName   = cms.untracked.string('pfEcalEndcapSuperClusterMap'),
    superClusterIdMapName = cms.untracked.string(''),
    #YI caloTowerName         = cms.untracked.string('towerMaker'),
    fillerType            = cms.untracked.string('FillerSuperClusters')
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
    basicClusterMapName  = cms.untracked.string(''),
    fillerType           = cms.untracked.string('FillerBasicClusters')
  ),

  SuperClusters = cms.untracked.PSet(
    active                = cms.untracked.bool(True),
    mitName               = cms.untracked.string('SuperClusters'),
    edmName               = cms.untracked.string('particleFlowEGamma'),
    ebRecHitsName         = cms.untracked.string('reducedEcalRecHitsEB'),
    eeRecHitsName         = cms.untracked.string('reducedEcalRecHitsEE'),
    esRecHitsName         = cms.untracked.string('reducedEcalRecHitsES'),
    basicClusterMapName   = cms.untracked.string('basicClusterMap'),
    psClusterMapName      = cms.untracked.string(''),
    psXClusterMapName     = cms.untracked.string(''),
    psYClusterMapName     = cms.untracked.string(''),
    superClusterMapName   = cms.untracked.string('superClusterMap'),
    superClusterIdMapName = cms.untracked.string('superClusterIdMap'),
    #YI caloTowerName         = cms.untracked.string('towerMaker'),
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
    edmNameFixedGridAll                  = cms.untracked.string('fixedGridRhoAll'),
    edmNameFixedGridFastjetAll           = cms.untracked.string('fixedGridRhoFastjetAll'),
    edmNameFixedGridFastjetAllCalo       = cms.untracked.string('fixedGridRhoFastjetAllCalo'),
    edmNameFixedGridFastjetCentral       = cms.untracked.string('fixedGridRhoFastjetCentral'),
    edmNameFixedGridFastjetCentralCalo   = cms.untracked.string('fixedGridRhoFastjetCentralCalo'),
    edmNameFixedGridFastjetCentralChargedPileUp = cms.untracked.string('fixedGridRhoFastjetCentralChargedPileUp'),
    edmNameFixedGridFastjetCentralNeutral = cms.untracked.string('fixedGridRhoFastjetCentralNeutral'),
    fillerType                           = cms.untracked.string('FillerPileupEnergyDensity')
  ),

  GeneralTracks  = cms.untracked.PSet(
    active                    = cms.untracked.bool(True),
    sourceType                = cms.untracked.string('Tracks'),
    mitName                   = cms.untracked.string('Tracks'),
    edmName                   = cms.untracked.string('generalTracks'),
    trackingMapName           = cms.untracked.string(''),
    trackMapName              = cms.untracked.string('TracksMap'),
    edmSimAssociationName     = cms.untracked.string('trackingParticleRecoTrackAsssociation'),
    fillerType                = cms.untracked.string('FillerTracks')
  ),

  ConversionStepTracks  = cms.untracked.PSet(
    active                    = cms.untracked.bool(True),
    sourceType                = cms.untracked.string('Tracks'),
    mitName                   = cms.untracked.string('ConversionStepTracks'),
    edmName                   = cms.untracked.string('conversionStepTracks'),
    trackMapName              = cms.untracked.string('ConversionTracksMap'),
    fillerType                = cms.untracked.string('FillerTracks')
  ),

  StandaloneMuonTracks = cms.untracked.PSet(
    active                    = cms.untracked.bool(True),
    sourceType                = cms.untracked.string('Tracks'),
    mitName                   = cms.untracked.string('StandaloneMuonTracks'),
    edmName                   = cms.untracked.string('standAloneMuons'),
    trackMapName              = cms.untracked.string('StandaloneMuonTracksMap'),
    fillerType                = cms.untracked.string('FillerTracks')
  ),

  StandaloneMuonTracksWVtxConstraint = cms.untracked.PSet(
    active                    = cms.untracked.bool(True),
    sourceType                = cms.untracked.string('Tracks'),
    mitName                   = cms.untracked.string('StandaloneMuonTracksWVtxConstraint'),
    edmName                   = cms.untracked.string('standAloneMuons:UpdatedAtVtx'),
    trackMapName              = cms.untracked.string('StandaloneMuonTracksWVtxConstraintMap'),
    fillerType                = cms.untracked.string('FillerTracks')
  ),

  StandaloneCosmicMuonTracks = cms.untracked.PSet(
    active                    = cms.untracked.bool(True),
    sourceType                = cms.untracked.string('Tracks'),
    mitName                   = cms.untracked.string('StandaloneCosmicMuonTracks'),
    edmName                   = cms.untracked.string('cosmicMuons'),
    trackMapName              = cms.untracked.string('StandaloneCosmicMuonTracksMap'),
    fillerType                = cms.untracked.string('FillerTracks')
  ),

  GlobalMuonTracks = cms.untracked.PSet(
    active                    = cms.untracked.bool(True),
    sourceType                = cms.untracked.string('Tracks'),
    mitName                   = cms.untracked.string('GlobalMuonTracks'),
    edmName                   = cms.untracked.string('globalMuons'),
    trackMapName              = cms.untracked.string('GlobalMuonTracksMap'),
    fillerType                = cms.untracked.string('FillerTracks')
  ),

  GlobalCosmicMuonTracks = cms.untracked.PSet(
    active                    = cms.untracked.bool(True),
    sourceType                = cms.untracked.string('Tracks'),
    mitName                   = cms.untracked.string('GlobalCosmicMuonTracks'),
    edmName                   = cms.untracked.string('displacedGlobalMuons'),
    trackMapName              = cms.untracked.string('GlobalCosmicMuonTracksMap'),
    fillerType                = cms.untracked.string('FillerTracks')
  ),

  FirstHitMuonTracks = cms.untracked.PSet(
    active                    = cms.untracked.bool(True),
    sourceType                = cms.untracked.string('Tracks'),
    mitName                   = cms.untracked.string('FirstHitMuonTracks'),
    edmName                   = cms.untracked.string('tevMuons:firstHit'),
    trackMapName              = cms.untracked.string('FirstHitMuonTracksMap'),
    fillerType                = cms.untracked.string('FillerTracks')
  ),

  PickyMuonTracks = cms.untracked.PSet(
    active                    = cms.untracked.bool(True),
    sourceType                = cms.untracked.string('Tracks'),
    mitName                   = cms.untracked.string('PickyMuonTracks'),
    edmName                   = cms.untracked.string('tevMuons:picky'),
    trackMapName              = cms.untracked.string('PickyMuonTracksMap'),
    fillerType                = cms.untracked.string('FillerTracks')
  ),

  DYTMuonTracks = cms.untracked.PSet(
    active                    = cms.untracked.bool(True),
    sourceType                = cms.untracked.string('Tracks'),
    mitName                   = cms.untracked.string('DYTMuonTracks'),
    edmName                   = cms.untracked.string('tevMuons:dyt'),
    trackMapName              = cms.untracked.string('DYTMuonTracksMap'),
    fillerType                = cms.untracked.string('FillerTracks')
  ),

  ConversionInOutTracks = cms.untracked.PSet(
    active                    = cms.untracked.bool(True),
    sourceType                = cms.untracked.string('Tracks'),
    mitName                   = cms.untracked.string('ConversionInOutTracks'),
    edmName                   = cms.untracked.string('ckfInOutTracksFromConversions'),
    trackingMapName           = cms.untracked.string(''),
    trackMapName              = cms.untracked.string('ConversionInOutTracksMap'),
    edmSimAssociationName     = cms.untracked.string('assocInOutConversionTracks'),
    fillerType                = cms.untracked.string('FillerTracks')
  ),

  ConversionOutInTracks = cms.untracked.PSet(
    active                    = cms.untracked.bool(True),
    sourceType                = cms.untracked.string('Tracks'),
    mitName                   = cms.untracked.string('ConversionOutInTracks'),
    edmName                   = cms.untracked.string('ckfOutInTracksFromConversions'),
    trackingMapName           = cms.untracked.string(''),
    trackMapName              = cms.untracked.string('ConversionOutInTracksMap'),
    edmSimAssociationName     = cms.untracked.string('assocOutInConversionTracks'),
    fillerType                = cms.untracked.string('FillerTracks')
  ),

  GsfTracks = cms.untracked.PSet(
    active                    = cms.untracked.bool(True),
    sourceType                = cms.untracked.string('Tracks'),
    mitName                   = cms.untracked.string('GsfTracks'),
    edmName                   = cms.untracked.string('electronGsfTracks'),
    trackingMapName           = cms.untracked.string(''),
    trackMapName              = cms.untracked.string('GsfTracksMap'),
    edmSimAssociationName     = cms.untracked.string('assoc2GsfTracks'),
    fillerType                = cms.untracked.string('FillerTracks')
  ),

  Muons = cms.untracked.PSet(
    active               = cms.untracked.bool(True),
    mitName              = cms.untracked.string('Muons'),
    edmName              = cms.untracked.string('muons'),
    globalTrackMapName   = cms.untracked.string('GlobalMuonTracksMap'),
    staTrackMapName      = cms.untracked.string('StandaloneMuonTracksMap'),
    staVtxTrackMapName   = cms.untracked.string('StandaloneMuonTracksWVtxConstraintMap'),
    trackerTrackMapName  = cms.untracked.string('TracksMap'),
    firstHitTrackMapName = cms.untracked.string('FirstHitMuonTracksMap'),
    pickyTrackMapName    = cms.untracked.string('PickyMuonTracksMap'),
    dytTrackMapName      = cms.untracked.string('DYTMuonTracksMap'),
    muonMapName          = cms.untracked.string('muonMap'),
    muonPFMapName        = cms.untracked.string(''),
    pvEdmName            = cms.untracked.string('offlinePrimaryVertices'),
    pvBSEdmName          = cms.untracked.string('offlinePrimaryVerticesWithBS'),
    pvBeamSpotName       = cms.untracked.string(offlinePrimaryVertices.beamSpotLabel.getModuleLabel()),
    pvbsBeamSpotName     = cms.untracked.string(offlinePrimaryVertices.beamSpotLabel.getModuleLabel()),
    fitUnbiasedVertex    = cms.untracked.bool(False),
    fillFromPAT          = cms.untracked.bool(False),
    fillerType           = cms.untracked.string('FillerMuons')
  ),

  CosmicMuons = cms.untracked.PSet(
    active              = cms.untracked.bool(True),
    mitName             = cms.untracked.string('CosmicMuons'),
    edmName             = cms.untracked.string('muonsFromCosmics'),
    globalTrackMapName  = cms.untracked.string('GlobalCosmicMuonTracksMap'),
    staTrackMapName     = cms.untracked.string('StandaloneCosmicMuonTracksMap'),
    muonMapName         = cms.untracked.string('cosmicMuonMap'),
    pvEdmName           = cms.untracked.string('offlinePrimaryVertices'),
    pvBSEdmName         = cms.untracked.string('offlinePrimaryVerticesWithBS'),
    fitUnbiasedVertex   = cms.untracked.bool(False),
    fillerType          = cms.untracked.string('FillerMuons')
  ),

  Electrons = cms.untracked.PSet(
    active                           = cms.untracked.bool(True),
    edmName                          = cms.untracked.string('gedGsfElectrons'),
    pvEdmName                        = cms.untracked.string('offlinePrimaryVertices'),
    pvBSEdmName                      = cms.untracked.string('offlinePrimaryVerticesWithBS'),
    generalTracksName                = cms.untracked.string('generalTracks'),
    gsfTracksName                    = cms.untracked.string('electronGsfTracks'),
#    conversionsName                  = cms.untracked.string('mvfConversionRemoval'),
    conversionsName                  = cms.untracked.string(''), # MVF currently broken
    pvBeamSpotName                   = cms.untracked.string('offlineBeamSpot'),
    pvbsBeamSpotName                 = cms.untracked.string('offlineBeamSpot'),
    footprintName                    = cms.untracked.string('particleBasedIsolation:gedGsfElectrons'),
    eIDCutBasedLooseName             = cms.untracked.string('eidLoose'),
    eIDCutBasedTightName             = cms.untracked.string('eidTight'),
    ecalPFClusterIsoMapName          = cms.untracked.string('electronEcalPFClusterIsolationProducer'),
    hcalPFClusterIsoMapName          = cms.untracked.string('electronHcalPFClusterIsolationProducer'),
    mitName                          = cms.untracked.string('Electrons'),
    electronMapName                  = cms.untracked.string('electronMap'),
    electronPFMapName                = cms.untracked.string(''),
    gsfTrackMapName                  = cms.untracked.string('GsfTracksMap'),
    trackerTrackMapName              = cms.untracked.string('TracksMap'),
    barrelSuperClusterMapName        = cms.untracked.string('superClusterMap'),
    endcapSuperClusterMapName        = cms.untracked.string('superClusterMap'),
    requireClusterAndGsfMap          = cms.untracked.bool(True),
    pfEcalBarrelSuperClusterMapName  = cms.untracked.string('pfEcalBarrelSuperClusterMap'),
    pfEcalEndcapSuperClusterMapName  = cms.untracked.string('pfEcalEndcapSuperClusterMap'),
    pfCandidateMapName               = cms.untracked.string('PFCandMap'),
    recomputeConversionInfo          = cms.untracked.bool(False),
    fitUnbiasedVertex                = cms.untracked.bool(False),
    fillFromPAT                      = cms.untracked.bool(False),
    fillerType                       = cms.untracked.string('FillerElectrons')
  ),

  StableParts = cms.untracked.PSet(
    active       = cms.untracked.bool(False),
    mitName      = cms.untracked.string('MuonsStable'),
    edmName      = cms.untracked.string('MuonsStable'),
    trackMapName = cms.untracked.string('TracksMap'),
    basePartMap  = cms.untracked.string('MuonsStableMap'),
    fillerType   = cms.untracked.string('FillerStableParts')
  ),

  DecayParts = cms.untracked.PSet(
    active       = cms.untracked.bool(False),
    mitName      = cms.untracked.string('Jpsi2MuMu'),
    edmName      = cms.untracked.string('Jpsi2MuMu'),
    basePartMaps = cms.untracked.vstring('MuonsStableMap'),
    fillerType   = cms.untracked.string('FillerDecayParts')
  ),

  MergedStableTest = cms.untracked.PSet(
    active        = cms.untracked.bool(False),
    mitName       = cms.untracked.string('MergedStableTest'),
    edmName       = cms.untracked.string('mergedConversionsStable'),
    trackMapNames = cms.untracked.vstring('ConversionInOutTracksMap',
                                          'ConversionOutInTracksMap'),
    basePartMap   = cms.untracked.string('MergedTestStableMap'),
    trackPartMap  = cms.untracked.string('MergedTestStableTrackMap'),
    fillerType    = cms.untracked.string('FillerStableParts')
  ),

  ElectronsStable = cms.untracked.PSet(
    active        = cms.untracked.bool(True),
    mitName       = cms.untracked.string('ElectronsStable'),
    edmName       = cms.untracked.string('generalElectronsStable'),
    trackMapNames = cms.untracked.vstring('TracksMap'),
    basePartMap   = cms.untracked.string('ElectronsStableMap'),
    trackPartMap  = cms.untracked.string('ElectronsStableTrackMap'),
    fillerType    = cms.untracked.string('FillerStableParts')
  ),

  ConversionInOutElectronsStable = cms.untracked.PSet(
    active        = cms.untracked.bool(True),
    mitName       = cms.untracked.string('ConversionInOutElectronsStable'),
    edmName       = cms.untracked.string('ckfInOutElectronsStable'),
    trackMapNames = cms.untracked.vstring('ConversionInOutTracksMap'),
    basePartMap   = cms.untracked.string('ConversionInOutElectronsStableMap'),
    trackPartMap  = cms.untracked.string('ConversionInOutElectronsStableTrackMap'),
    fillerType    = cms.untracked.string('FillerStableParts')
  ),

  ConversionOutInElectronsStable = cms.untracked.PSet(
    active        = cms.untracked.bool(True),
    mitName       = cms.untracked.string('ConversionOutInElectronsStable'),
    edmName       = cms.untracked.string('ckfOutInElectronsStable'),
    trackMapNames = cms.untracked.vstring('ConversionOutInTracksMap'),
    basePartMap   = cms.untracked.string('ConversionOutInElectronsStableMap'),
    trackPartMap  = cms.untracked.string('ConversionOutInElectronsStableTrackMap'),
    fillerType    = cms.untracked.string('FillerStableParts')
  ),

  GsfElectronsStable = cms.untracked.PSet(
    active        = cms.untracked.bool(True),
    mitName       = cms.untracked.string('GsfElectronsStable'),
    edmName       = cms.untracked.string('gsfElectronsStable'),
    trackMapNames = cms.untracked.vstring('GsfTracksMap'),
    basePartMap   = cms.untracked.string('ElectronsStableGsfMap'),
    trackPartMap  = cms.untracked.string('ElectronsStableGsfTrackMap'),
    fillerType    = cms.untracked.string('FillerStableParts')
  ),

  ConversionStepElectronsStable = cms.untracked.PSet(
    active        = cms.untracked.bool(True),
    mitName       = cms.untracked.string('ConversionStepElectronsStable'),
    edmName       = cms.untracked.string('conversionStepElectronsStable'),
    trackMapNames = cms.untracked.vstring('ConversionTracksMap'),
    basePartMap   = cms.untracked.string('ElectronsStableConvStepMap'),
    trackPartMap  = cms.untracked.string('ElectronsStableConvStepTrackMap'),
    fillerType    = cms.untracked.string('FillerStableParts')
  ),

  MergedElectronsStable = cms.untracked.PSet(
    active        = cms.untracked.bool(True),
    mitName       = cms.untracked.string('MergedElectronsStable'),
    edmName       = cms.untracked.string('mergedElectronsStable'),
    trackMapNames = cms.untracked.vstring('TracksMap',
                                          'ConversionInOutTracksMap',
                                          'ConversionOutInTracksMap',
                                          'GsfTracksMap'),
    basePartMap   = cms.untracked.string('MergedElectronsStableMap'),
    trackPartMap  = cms.untracked.string('MergedElectronsStableTrackMap'),
    fillerType    = cms.untracked.string('FillerStableParts')
  ),

  MergedConversions = cms.untracked.PSet(
    active              = cms.untracked.bool(True),
    mitName             = cms.untracked.string('MergedConversions'),
    edmName             = cms.untracked.string('allConversions'),
    beamspotName        = cms.untracked.string('offlineBeamSpot'),
    stablePartMaps      = cms.untracked.vstring('ElectronsStableTrackMap',
                                                'ConversionInOutElectronsStableTrackMap',
                                                'ConversionOutInElectronsStableTrackMap',
                                                'ElectronsStableGsfTrackMap',
                                                'ElectronsStableConvStepTrackMap'),
    conversionMapName   = cms.untracked.string('MergedConversionsMap'),
    fillerType          = cms.untracked.string('FillerConversionsDecay')
  ),

  Conversions = cms.untracked.PSet(
    active              = cms.untracked.bool(True),
    mitName             = cms.untracked.string('Conversions'),
    edmName             = cms.untracked.string('allConversions'),
    beamspotName        = cms.untracked.string('offlineBeamSpot'),
    stablePartMaps      = cms.untracked.vstring('ElectronsStableTrackMap',
                                                'ConversionInOutElectronsStableTrackMap',
                                                'ConversionOutInElectronsStableTrackMap',
                                                'ElectronsStableGsfTrackMap',
                                                'ElectronsStableConvStepTrackMap'),
    conversionMapName   = cms.untracked.string('ConversionsMap'),
    fillerType          = cms.untracked.string('FillerConversionsDecay')
  ),

  PFPhotonConversions = cms.untracked.PSet(
    active              = cms.untracked.bool(True),
    mitName             = cms.untracked.string('PFPhotonConversions'),
    edmName             = cms.untracked.string('gedPhotonCore'),
    beamspotName        = cms.untracked.string('offlineBeamSpot'),
    stablePartMaps      = cms.untracked.vstring('ElectronsStableTrackMap',
                                                'ConversionInOutElectronsStableTrackMap',
                                                'ConversionOutInElectronsStableTrackMap',
                                                'ElectronsStableGsfTrackMap',
                                                'ElectronsStableConvStepTrackMap'),
    conversionMapName   = cms.untracked.string('PFPhotonConversionsMap'),
    fillerType          = cms.untracked.string('FillerConversionsDecay')
  ),

  Photons = cms.untracked.PSet(
    active                    = cms.untracked.bool(True),
    mitName                   = cms.untracked.string('Photons'),
    edmName                   = cms.untracked.string('gedPhotons'),
    HBHERecHitsEdmName        = cms.untracked.string('reducedHcalRecHits:hbhereco'),
    phIDCutBasedTightName     = cms.untracked.string('PhotonIDProdGED:PhotonCutBasedIDTight'),
    phIDCutBasedLooseName     = cms.untracked.string('PhotonIDProdGED:PhotonCutBasedIDLoose'),
    footprintName             = cms.untracked.string('particleBasedIsolation:gedPhotons'),
    ecalPFClusterIsoMapName          = cms.untracked.string('photonEcalPFClusterIsolationProducer'),
    hcalPFClusterIsoMapName          = cms.untracked.string('photonHcalPFClusterIsolationProducer'),
    conversionMapName         = cms.untracked.string('ConversionsMap'),
    oneLegConversionMapName   = cms.untracked.string(''), # was never filled
    barrelSuperClusterMapName = cms.untracked.string('superClusterMap'),
    endcapSuperClusterMapName = cms.untracked.string('superClusterMap'),
    requireClusterMap         = cms.untracked.bool(True),
    fillFromPAT               = cms.untracked.bool(False),
    pfEcalBarrelSuperClusterMapName = cms.untracked.string('pfEcalBarrelSuperClusterMap'),
    pfEcalEndcapSuperClusterMapName = cms.untracked.string('pfEcalEndcapSuperClusterMap'),
    pfCandidateMapName        = cms.untracked.string('PFCandMap'),
    photonMapName             = cms.untracked.string('PhotonMap'),
    photonPFMapName           = cms.untracked.string(''),
    fillerType                = cms.untracked.string('FillerPhotons')
  ),

  PFCandidates = cms.untracked.PSet(
    active                        = cms.untracked.bool(True),
    mitName                       = cms.untracked.string('PFCandidates'),
    edmName                       = cms.untracked.string('particleFlowPtrs'),
    edmPfNoPileupName             = cms.untracked.string('pfNoElectron'),
    edmPuppiMapNames              = cms.untracked.vstring('pfCandNoLepMap', 'pfCandLepMap', 'puppiNoLep', 'puppiNoLepPlusLep'),
    edmPuppiName                  = cms.untracked.string('puppiNoLepPlusLep'),
    trackerTrackMapNames          = cms.untracked.vstring('TracksMap',
                                                          'ConversionTracksMap'),
    gsfTrackMapName               = cms.untracked.string('GsfTracksMap'),
    muonMapName                   = cms.untracked.string('muonMap'),
    conversionMapName             = cms.untracked.string(''),
    barrelSuperClusterMapName     = cms.untracked.string('superClusterMap'),
    endcapSuperClusterMapName     = cms.untracked.string('superClusterMap'),
    electronMapName               = cms.untracked.string('electronMap'),
    photonMapName                 = cms.untracked.string('PhotonMap'),
    pfCandMapName                 = cms.untracked.string('PFCandMap'),
    pfNoPileupCandMapName         = cms.untracked.string('PFNoPileupCandMap'),
    puppiMapName                  = cms.untracked.string('puppiMap'),
    fillerType                    = cms.untracked.string('FillerPFCandidates'),
    allowMissingTrackRef          = cms.untracked.bool(False),
    allowMissingClusterRef        = cms.untracked.bool(False),
    allowMissingPhotonRef         = cms.untracked.bool(False),
    fillPfNoPileup                = cms.untracked.bool(True),
    fillPuppiMap                  = cms.untracked.bool(True)
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
    bTaggingActive                           = cms.untracked.bool(True),
    jetToVertexActive                        = cms.untracked.bool(False),
    jetCorrectionsActive                     = cms.untracked.bool(False),
    fillFromPAT                              = cms.untracked.bool(False),
    mitName                                  = cms.untracked.string('AKt4PFJets'),
    edmName                                  = cms.untracked.string('ak4PFJets'),
    rhoName                                  = cms.untracked.string('fixedGridRhoFastjetAll'),
    jetToVertexAlphaName                     = cms.untracked.string('jetVertexAlphaAKT4PF:Var'),
    jetToVertexBetaName                      = cms.untracked.string('jetVertexBetaAKT4PF:Var'),
    L2JetCorrectorName                       = cms.untracked.string('ak4PFL2Relative'),
    L3JetCorrectorName                       = cms.untracked.string('ak4PFL3Absolute'),
    flavorMatchingByReferenceName            = cms.untracked.string('AKT4PFbyRef'),
    flavorMatchingDefinition                 = cms.untracked.string('Algorithmic'),
    JetProbabilityBJetTagsName               = cms.untracked.string('jetProbabilityBJetTagsAKt4PF'),
    JetBProbabilityBJetTagsName              = cms.untracked.string('jetBProbabilityBJetTagsAKt4PF'),
    SimpleSecondaryVertexHighEffBJetTagsName = cms.untracked.string('simpleSecondaryVertexHighEffBJetTagsAKt4PF'),
    SimpleInclusiveSecondaryVertexHighEffBJetTagsName = cms.untracked.string('simpleInclusiveSecondaryVertexHighEffBJetTagsAKt4PF'),
    CombinedSecondaryVertexV2BJetTagsName    = cms.untracked.string('combinedSecondaryVertexV2BJetTagsAKt4PF'),
    CombinedInclusiveSecondaryVertexV2BJetTagsName = cms.untracked.string('combinedInclusiveSecondaryVertexV2BJetTagsAKt4PF'),
    CombinedMVAV2BJetTagsName                = cms.untracked.string('combinedMVAV2BJetTagsAKt4PF'),
    TrackCountingHighEffBJetTagsName         = cms.untracked.string('trackCountingHighEffBJetTagsAKt4PF'),
    SoftPFMuonBJetTagsName                   = cms.untracked.string('softPFMuonBJetTagsAKt4PF'),
    SoftPFElectronBJetTagsName               = cms.untracked.string('softPFElectronBJetTagsAKt4PF'),
    CombinedCvsBJetTagsName                  = cms.untracked.string('combinedCvsBJetTagsAKt4PF'),
    CombinedCvsLBJetTagsName                 = cms.untracked.string('combinedCvsLJetTagsAKt4PF'),
    pfCandMapName                            = cms.untracked.string('PFCandMap'),
    jetMapName                               = cms.untracked.string('AKt4PFJetMap'),
    fillerType                               = cms.untracked.string('FillerPFJets')
  ),

  AKt4PFJetsCHS = cms.untracked.PSet(
    active                                   = cms.untracked.bool(True),
    flavorMatchingActive                     = cms.untracked.bool(False),
    bTaggingActive                           = cms.untracked.bool(True),
    jetToVertexActive                        = cms.untracked.bool(False),
    jetCorrectionsActive                     = cms.untracked.bool(False),
    fillFromPAT                              = cms.untracked.bool(False),
    mitName                                  = cms.untracked.string('AKt4PFJetsCHS'),
    edmName                                  = cms.untracked.string('ak4PFJetsCHS'),
    rhoName                                  = cms.untracked.string('fixedGridRhoFastjetAll'),
    jetToVertexAlphaName                     = cms.untracked.string('jetVertexAlphaAKT4PF:Var'),
    jetToVertexBetaName                      = cms.untracked.string('jetVertexBetaAKT4PF:Var'),
    L2JetCorrectorName                       = cms.untracked.string('ak4PFCHSL2Relative'),
    L3JetCorrectorName                       = cms.untracked.string('ak4PFCHSL3Absolute'),
    flavorMatchingByReferenceName            = cms.untracked.string('AKT4PFbyRef'),
    flavorMatchingDefinition                 = cms.untracked.string('Algorithmic'),
    JetProbabilityBJetTagsName               = cms.untracked.string('jetProbabilityBJetTagsAKt4PFCHS'),
    JetBProbabilityBJetTagsName              = cms.untracked.string('jetBProbabilityBJetTagsAKt4PFCHS'),
    SimpleSecondaryVertexHighEffBJetTagsName = cms.untracked.string('simpleSecondaryVertexHighEffBJetTagsAKt4PFCHS'),
    SimpleInclusiveSecondaryVertexHighEffBJetTagsName = cms.untracked.string('simpleInclusiveSecondaryVertexHighEffBJetTagsAKt4PFCHS'),
    CombinedSecondaryVertexV2BJetTagsName    = cms.untracked.string('combinedSecondaryVertexV2BJetTagsAKt4PFCHS'),
    CombinedInclusiveSecondaryVertexV2BJetTagsName = cms.untracked.string('combinedInclusiveSecondaryVertexV2BJetTagsAKt4PFCHS'),
    CombinedMVAV2BJetTagsName                = cms.untracked.string('combinedMVAV2BJetTagsAKt4PFCHS'),
    TrackCountingHighEffBJetTagsName         = cms.untracked.string('trackCountingHighEffBJetTagsAKt4PFCHS'),
    SoftPFMuonBJetTagsName                   = cms.untracked.string('softPFMuonBJetTagsAKt4PFCHS'),
    SoftPFElectronBJetTagsName               = cms.untracked.string('softPFElectronBJetTagsAKt4PFCHS'),
    CombinedCvsBJetTagsName                  = cms.untracked.string('combinedCvsBJetTagsAKt4PFCHS'),
    CombinedCvsLBJetTagsName                 = cms.untracked.string('combinedCvsLJetTagsAKt4PFCHS'),
    pfCandMapName                            = cms.untracked.string('PFCandMap'),
    jetMapName                               = cms.untracked.string('AKt4PFJetCHSMap'),
    fillerType                               = cms.untracked.string('FillerPFJets')
  ),

  AKt4PuppiJets = cms.untracked.PSet(
    active                                   = cms.untracked.bool(True),
    flavorMatchingActive                     = cms.untracked.bool(False),
    bTaggingActive                           = cms.untracked.bool(True),
    jetToVertexActive                        = cms.untracked.bool(False),
    jetCorrectionsActive                     = cms.untracked.bool(False),
    fillFromPAT                              = cms.untracked.bool(False),
    mitName                                  = cms.untracked.string('AKt4PuppiJets'),
    edmName                                  = cms.untracked.string('ak4PFJetsPuppi'),
    rhoName                                  = cms.untracked.string('fixedGridRhoFastjetAll'),
    jetToVertexAlphaName                     = cms.untracked.string('jetVertexAlphaAKT4PF:Var'),
    jetToVertexBetaName                      = cms.untracked.string('jetVertexBetaAKT4PF:Var'),
    L2JetCorrectorName                       = cms.untracked.string('ak4PFPuppiL2Relative'),
    L3JetCorrectorName                       = cms.untracked.string('ak4PFPuppiL3Absolute'),
    flavorMatchingByReferenceName            = cms.untracked.string('AKT4PFbyRef'),
    flavorMatchingDefinition                 = cms.untracked.string('Algorithmic'),
    JetProbabilityBJetTagsName               = cms.untracked.string('jetProbabilityBJetTagsAKt4PFPuppi'),
    JetBProbabilityBJetTagsName              = cms.untracked.string('jetBProbabilityBJetTagsAKt4PFPuppi'),
    SimpleSecondaryVertexHighEffBJetTagsName = cms.untracked.string('simpleSecondaryVertexHighEffBJetTagsAKt4PFPuppi'),
    SimpleInclusiveSecondaryVertexHighEffBJetTagsName = cms.untracked.string('simpleInclusiveSecondaryVertexHighEffBJetTagsAKt4PFPuppi'),
    CombinedSecondaryVertexV2BJetTagsName    = cms.untracked.string('combinedSecondaryVertexV2BJetTagsAKt4PFPuppi'),
    CombinedInclusiveSecondaryVertexV2BJetTagsName = cms.untracked.string('combinedInclusiveSecondaryVertexV2BJetTagsAKt4PFPuppi'),
    CombinedMVAV2BJetTagsName                = cms.untracked.string('combinedMVAV2BJetTagsAKt4PFPuppi'),
    TrackCountingHighEffBJetTagsName         = cms.untracked.string('trackCountingHighEffBJetTagsAKt4PFPuppi'),
    SoftPFMuonBJetTagsName                   = cms.untracked.string('softPFMuonBJetTagsAKt4PFPuppi'),
    SoftPFElectronBJetTagsName               = cms.untracked.string('softPFElectronBJetTagsAKt4PFPuppi'),
    CombinedCvsBJetTagsName                  = cms.untracked.string('combinedCvsBJetTagsAKt4PFPuppi'),
    CombinedCvsLBJetTagsName                 = cms.untracked.string('combinedCvsLJetTagsAKt4PFPuppi'),
    pfCandMapName                            = cms.untracked.string('puppiMap'),
    jetMapName                               = cms.untracked.string('AKt4PFJetPuppiMap'),
    fillerType                               = cms.untracked.string('FillerPFJets')
  ),

  AKt8PFJetsCHS = cms.untracked.PSet(
    active                                   = cms.untracked.bool(False),
    flavorMatchingActive                     = cms.untracked.bool(False),
    bTaggingActive                           = cms.untracked.bool(False),
    jetToVertexActive                        = cms.untracked.bool(False),
    jetCorrectionsActive                     = cms.untracked.bool(False),
    fillFromPAT                              = cms.untracked.bool(False),
    mitName                                  = cms.untracked.string('AKt8PFJetsCHS'),
    edmName                                  = cms.untracked.string('ak8PFJetsCHS'),
    rhoName                                  = cms.untracked.string(''),
    jetToVertexAlphaName                     = cms.untracked.string('jetVertexAlphaAKT8PF:Var'),
    jetToVertexBetaName                      = cms.untracked.string('jetVertexBetaAKT8PF:Var'),
    L2JetCorrectorName                       = cms.untracked.string('L2RelativeJetCorrectorAKT8PF'),
    L3JetCorrectorName                       = cms.untracked.string('L3AbsoluteJetCorrectorAKT8PF'),
    flavorMatchingByReferenceName            = cms.untracked.string('AKT8PFbyRef'),
    flavorMatchingDefinition                 = cms.untracked.string('Algorithmic'),
    pfCandMapName                            = cms.untracked.string('PFCandMap'), # ak8 CHS is not redone in BAMBU production
    jetMapName                               = cms.untracked.string('AKt8PFJetCHSMap'),
    fillerType                               = cms.untracked.string('FillerPFJets')
  ),

  AKt8FatJetsCHS = cms.untracked.PSet(
    active                                   = cms.untracked.bool(True),
    flavorMatchingActive                     = cms.untracked.bool(False),
    bTaggingActive                           = cms.untracked.bool(False),
    jetToVertexActive                        = cms.untracked.bool(False),
    jetCorrectionsActive                     = cms.untracked.bool(False),
    mitName                                  = cms.untracked.string('AKt8FatJetsCHS'),
    edmName                                  = cms.untracked.string('packedPatJetsPFCHSAK8'),
    rhoName                                  = cms.untracked.string(''),
    pfCandMapName                            = cms.untracked.string('PFCandMap'),
    jetMapName                               = cms.untracked.string('AKt8FatJetCHSMap'),
    SubJets                                  = cms.untracked.vstring('selectedPatJetsSoftDropPFCHSPackedAK8:SubJets'),
    SubJetLabels                             = cms.untracked.vstring('SoftDrop'), # this order is important,
    SubJetBTagName                           = cms.untracked.string('pfCombinedInclusiveSecondaryVertexV2BJetTags'),
    R0                                       = cms.untracked.double(.8),
    edmPrimaryVertices                       = cms.untracked.string('offlinePrimaryVertices'),
    fillerType                               = cms.untracked.string('FillerFatJets')
  ),

  CA15FatJetsCHS = cms.untracked.PSet(
     active                                   = cms.untracked.bool(True),
     flavorMatchingActive                     = cms.untracked.bool(False),
     bTaggingActive                           = cms.untracked.bool(False),
     jetToVertexActive                        = cms.untracked.bool(False),
     jetCorrectionsActive                     = cms.untracked.bool(False),
     mitName                                  = cms.untracked.string('CA15FatJetsCHS'),
     edmName                                  = cms.untracked.string('packedPatJetsPFCHSCA15'),
     rhoName                                  = cms.untracked.string(''),
     pfCandMapName                            = cms.untracked.string('PFCandMap'),
     jetMapName                               = cms.untracked.string('CA15FatJetCHSMap'),
     SubJets                                  = cms.untracked.vstring('selectedPatJetsSoftDropPFCHSPackedCA15:SubJets'),
     SubJetLabels                             = cms.untracked.vstring('SoftDrop'), # this order is important,
     SubJetBTagName                           = cms.untracked.string('pfCombinedInclusiveSecondaryVertexV2BJetTags'),
     R0                                       = cms.untracked.double(1.5),
     edmPrimaryVertices                       = cms.untracked.string('offlinePrimaryVertices'),
     fillerType                               = cms.untracked.string('FillerFatJets')
   ),


  AKt8FatJetsPuppi = cms.untracked.PSet(
    active                                   = cms.untracked.bool(True),
    flavorMatchingActive                     = cms.untracked.bool(False),
    bTaggingActive                           = cms.untracked.bool(False),
    jetToVertexActive                        = cms.untracked.bool(False),
    jetCorrectionsActive                     = cms.untracked.bool(False),
    mitName                                  = cms.untracked.string('AKt8FatJetsPuppi'),
    edmName                                  = cms.untracked.string('packedPatJetsPFPuppiAK8'),
    rhoName                                  = cms.untracked.string(''),
    pfCandMapName                            = cms.untracked.string('puppiMap'),
    jetMapName                               = cms.untracked.string('AKt8FatJetPuppiMap'),
    SubJets                                  = cms.untracked.vstring('selectedPatJetsSoftDropPFPuppiPackedAK8:SubJets'),
    SubJetLabels                             = cms.untracked.vstring('SoftDrop'), # this order is important,
    SubJetBTagName                           = cms.untracked.string('pfCombinedInclusiveSecondaryVertexV2BJetTags'),
    R0                                       = cms.untracked.double(.8),
    edmPrimaryVertices                       = cms.untracked.string('offlinePrimaryVertices'),
    fillerType                               = cms.untracked.string('FillerFatJets')
  ),

  CA15FatJetsPuppi = cms.untracked.PSet(
     active                                   = cms.untracked.bool(True),
     flavorMatchingActive                     = cms.untracked.bool(False),
     bTaggingActive                           = cms.untracked.bool(False),
     jetToVertexActive                        = cms.untracked.bool(False),
     jetCorrectionsActive                     = cms.untracked.bool(False),
     mitName                                  = cms.untracked.string('CA15FatJetsPuppi'),
     edmName                                  = cms.untracked.string('packedPatJetsPFPuppiCA15'),
     rhoName                                  = cms.untracked.string(''),
     pfCandMapName                            = cms.untracked.string('puppiMap'),
     jetMapName                               = cms.untracked.string('CA15FatJetPuppiMap'),
     SubJets                                  = cms.untracked.vstring('selectedPatJetsSoftDropPFPuppiPackedCA15:SubJets'),
     SubJetLabels                             = cms.untracked.vstring('SoftDrop'), # this order is important,
     SubJetBTagName                           = cms.untracked.string('pfCombinedInclusiveSecondaryVertexV2BJetTags'),
     R0                                       = cms.untracked.double(1.5),
     edmPrimaryVertices                       = cms.untracked.string('offlinePrimaryVertices'),
     fillerType                               = cms.untracked.string('FillerFatJets')
   ),

   GenMet = cms.untracked.PSet(
    active      = cms.untracked.bool(False),
    mitName     = cms.untracked.string('GenMet'),
    edmName     = cms.untracked.string('genMetTrue'),
    fillFromPAT = cms.untracked.bool(False),
    fillerType  = cms.untracked.string('FillerGenMet')
  ),

  PFMet = cms.untracked.PSet(
    active     = cms.untracked.bool(True),
    mitName    = cms.untracked.string('PFMet'),
    edmName    = cms.untracked.string('pfMet'),
    fillerType = cms.untracked.string('FillerPFMetFromPFMET')
  ),

  PuppiMet = cms.untracked.PSet(
    active     = cms.untracked.bool(True),
    mitName    = cms.untracked.string('PuppiMet'),
    edmName    = cms.untracked.string('puppiMet'),
    fillerType = cms.untracked.string('FillerPFMetFromPFMET')
  ),

  TCMet = cms.untracked.PSet(
    active     = cms.untracked.bool(False),
    mitName    = cms.untracked.string('TCMet'),
    edmName    = cms.untracked.string('tcMet'),
    fillerType = cms.untracked.string('FillerMet')
  ),

  CaloMet = cms.untracked.PSet(
    active     = cms.untracked.bool(True),
    mitName    = cms.untracked.string('CaloMet'),
    edmName    = cms.untracked.string('caloMet'),
    fillerType = cms.untracked.string('FillerCaloMet')
  ),

  HPSTaus = cms.untracked.PSet(
    active = cms.untracked.bool(True),
    hpsActive = cms.untracked.bool(True),
    mitName = cms.untracked.string('HPSTaus'),
    edmName = cms.untracked.string('hpsPFTauProducer'),
    AgainstElectronMVA6RawName = cms.untracked.string("hpsPFTauDiscriminationByMVA6rawElectronRejection"),
    AgainstElectronMVA6categoryName = cms.untracked.string("hpsPFTauDiscriminationByMVA6rawElectronRejection:category"),
    ByCombinedIsolationDeltaBetaCorrRaw3HitsName = cms.untracked.string("hpsPFTauDiscriminationByRawCombinedIsolationDBSumPtCorr3Hits"),
    ByIsolationMVArun2v1DBdR03oldDMwLTrawName = cms.untracked.string("hpsPFTauDiscriminationByIsolationMVArun2v1DBdR03oldDMwLTraw"),
    ByIsolationMVArun2v1DBnewDMwLTrawName = cms.untracked.string("hpsPFTauDiscriminationByIsolationMVArun2v1DBnewDMwLTraw"),
    ByIsolationMVArun2v1DBoldDMwLTrawName = cms.untracked.string("hpsPFTauDiscriminationByIsolationMVArun2v1DBoldDMwLTraw"),
    ByIsolationMVArun2v1PWdR03oldDMwLTrawName = cms.untracked.string("hpsPFTauDiscriminationByIsolationMVArun2v1PWdR03oldDMwLTraw"),
    ByIsolationMVArun2v1PWnewDMwLTrawName = cms.untracked.string("hpsPFTauDiscriminationByIsolationMVArun2v1PWnewDMwLTraw"),
    ByIsolationMVArun2v1PWoldDMwLTrawName = cms.untracked.string("hpsPFTauDiscriminationByIsolationMVArun2v1PWoldDMwLTraw"),
    ChargedIsoPtSumName = cms.untracked.string("hpsPFTauChargedIsoPtSum"),
    ChargedIsoPtSumdR03Name = cms.untracked.string("hpsPFTauChargedIsoPtSumdR03"),
    FootprintCorrectionName = cms.untracked.string("hpsPFTauFootprintCorrection"),
    FootprintCorrectiondR03Name = cms.untracked.string("hpsPFTauFootprintCorrectiondR03"),
    NeutralIsoPtSumName = cms.untracked.string("hpsPFTauNeutralIsoPtSum"),
    NeutralIsoPtSumWeightName = cms.untracked.string("hpsPFTauNeutralIsoPtSumWeight"),
    NeutralIsoPtSumWeightdR03Name = cms.untracked.string("hpsPFTauNeutralIsoPtSumWeightdR03"),
    NeutralIsoPtSumdR03Name = cms.untracked.string("hpsPFTauNeutralIsoPtSumdR03"),
    PhotonPtSumOutsideSignalConeName = cms.untracked.string("hpsPFTauPhotonPtSumOutsideSignalCone"),
    PhotonPtSumOutsideSignalConedR03Name = cms.untracked.string("hpsPFTauPhotonPtSumOutsideSignalConedR03"),
    PuCorrPtSumName = cms.untracked.string("hpsPFTauPUcorrPtSum"),    
    AgainstElectronLooseMVA6Name = cms.untracked.string("hpsPFTauDiscriminationByMVA6LooseElectronRejection"),
    AgainstElectronMediumMVA6Name = cms.untracked.string("hpsPFTauDiscriminationByMVA6MediumElectronRejection"),
    AgainstElectronTightMVA6Name = cms.untracked.string("hpsPFTauDiscriminationByMVA6TightElectronRejection"),
    AgainstElectronVLooseMVA6Name = cms.untracked.string("hpsPFTauDiscriminationByMVA6VLooseElectronRejection"),
    AgainstElectronVTightMVA6Name = cms.untracked.string("hpsPFTauDiscriminationByMVA6VTightElectronRejection"),
    AgainstMuonLoose3Name = cms.untracked.string("hpsPFTauDiscriminationByLooseMuonRejection3"),
    AgainstMuonTight3Name = cms.untracked.string("hpsPFTauDiscriminationByTightMuonRejection3"),
    ByLooseCombinedIsolationDeltaBetaCorr3HitsName = cms.untracked.string("hpsPFTauDiscriminationByLooseCombinedIsolationDBSumPtCorr3Hits"),
    ByLooseIsolationMVArun2v1DBdR03oldDMwLTName = cms.untracked.string("hpsPFTauDiscriminationByLooseIsolationMVArun2v1DBdR03oldDMwLT"),
    ByLooseIsolationMVArun2v1DBnewDMwLTName = cms.untracked.string("hpsPFTauDiscriminationByLooseIsolationMVArun2v1DBnewDMwLT"),
    ByLooseIsolationMVArun2v1DBoldDMwLTName = cms.untracked.string("hpsPFTauDiscriminationByLooseIsolationMVArun2v1DBoldDMwLT"),
    ByLooseIsolationMVArun2v1PWdR03oldDMwLTName = cms.untracked.string("hpsPFTauDiscriminationByLooseIsolationMVArun2v1PWdR03oldDMwLT"),
    ByLooseIsolationMVArun2v1PWnewDMwLTName = cms.untracked.string("hpsPFTauDiscriminationByLooseIsolationMVArun2v1PWnewDMwLT"),
    ByLooseIsolationMVArun2v1PWoldDMwLTName = cms.untracked.string("hpsPFTauDiscriminationByLooseIsolationMVArun2v1PWoldDMwLT"),
    ByMediumCombinedIsolationDeltaBetaCorr3HitsName = cms.untracked.string("hpsPFTauDiscriminationByMediumCombinedIsolationDBSumPtCorr3Hits"),
    ByMediumIsolationMVArun2v1DBdR03oldDMwLTName = cms.untracked.string("hpsPFTauDiscriminationByMediumIsolationMVArun2v1DBdR03oldDMwLT"),
    ByMediumIsolationMVArun2v1DBnewDMwLTName = cms.untracked.string("hpsPFTauDiscriminationByMediumIsolationMVArun2v1DBnewDMwLT"),
    ByMediumIsolationMVArun2v1DBoldDMwLTName = cms.untracked.string("hpsPFTauDiscriminationByMediumIsolationMVArun2v1DBoldDMwLT"),
    ByMediumIsolationMVArun2v1PWdR03oldDMwLTName = cms.untracked.string("hpsPFTauDiscriminationByMediumIsolationMVArun2v1PWdR03oldDMwLT"),
    ByMediumIsolationMVArun2v1PWnewDMwLTName = cms.untracked.string("hpsPFTauDiscriminationByMediumIsolationMVArun2v1PWnewDMwLT"),
    ByMediumIsolationMVArun2v1PWoldDMwLTName = cms.untracked.string("hpsPFTauDiscriminationByMediumIsolationMVArun2v1PWoldDMwLT"),
    ByPhotonPtSumOutsideSignalConeName = cms.untracked.string("hpsPFTauDiscriminationByPhotonPtSumOutsideSignalCone"),
    ByTightCombinedIsolationDeltaBetaCorr3HitsName = cms.untracked.string("hpsPFTauDiscriminationByTightCombinedIsolationDBSumPtCorr3Hits"),
    ByTightIsolationMVArun2v1DBdR03oldDMwLTName = cms.untracked.string("hpsPFTauDiscriminationByTightIsolationMVArun2v1DBdR03oldDMwLT"),
    ByTightIsolationMVArun2v1DBnewDMwLTName = cms.untracked.string("hpsPFTauDiscriminationByTightIsolationMVArun2v1DBnewDMwLT"),
    ByTightIsolationMVArun2v1DBoldDMwLTName = cms.untracked.string("hpsPFTauDiscriminationByTightIsolationMVArun2v1DBoldDMwLT"),
    ByTightIsolationMVArun2v1PWdR03oldDMwLTName = cms.untracked.string("hpsPFTauDiscriminationByTightIsolationMVArun2v1PWdR03oldDMwLT"),
    ByTightIsolationMVArun2v1PWnewDMwLTName = cms.untracked.string("hpsPFTauDiscriminationByTightIsolationMVArun2v1PWnewDMwLT"),
    ByTightIsolationMVArun2v1PWoldDMwLTName = cms.untracked.string("hpsPFTauDiscriminationByTightIsolationMVArun2v1PWoldDMwLT"),
    ByVLooseIsolationMVArun2v1DBdR03oldDMwLTName = cms.untracked.string("hpsPFTauDiscriminationByVLooseIsolationMVArun2v1DBdR03oldDMwLT"),
    ByVLooseIsolationMVArun2v1DBnewDMwLTName = cms.untracked.string("hpsPFTauDiscriminationByVLooseIsolationMVArun2v1DBnewDMwLT"),
    ByVLooseIsolationMVArun2v1DBoldDMwLTName = cms.untracked.string("hpsPFTauDiscriminationByVLooseIsolationMVArun2v1DBoldDMwLT"),
    ByVLooseIsolationMVArun2v1PWdR03oldDMwLTName = cms.untracked.string("hpsPFTauDiscriminationByVLooseIsolationMVArun2v1PWdR03oldDMwLT"),
    ByVLooseIsolationMVArun2v1PWnewDMwLTName = cms.untracked.string("hpsPFTauDiscriminationByVLooseIsolationMVArun2v1PWnewDMwLT"),
    ByVLooseIsolationMVArun2v1PWoldDMwLTName = cms.untracked.string("hpsPFTauDiscriminationByVLooseIsolationMVArun2v1PWoldDMwLT"),
    ByVTightIsolationMVArun2v1DBdR03oldDMwLTName = cms.untracked.string("hpsPFTauDiscriminationByVTightIsolationMVArun2v1DBdR03oldDMwLT"),
    ByVTightIsolationMVArun2v1DBnewDMwLTName = cms.untracked.string("hpsPFTauDiscriminationByVTightIsolationMVArun2v1DBnewDMwLT"),
    ByVTightIsolationMVArun2v1DBoldDMwLTName = cms.untracked.string("hpsPFTauDiscriminationByVTightIsolationMVArun2v1DBoldDMwLT"),
    ByVTightIsolationMVArun2v1PWdR03oldDMwLTName = cms.untracked.string("hpsPFTauDiscriminationByVTightIsolationMVArun2v1PWdR03oldDMwLT"),
    ByVTightIsolationMVArun2v1PWnewDMwLTName = cms.untracked.string("hpsPFTauDiscriminationByVTightIsolationMVArun2v1PWnewDMwLT"),
    ByVTightIsolationMVArun2v1PWoldDMwLTName = cms.untracked.string("hpsPFTauDiscriminationByVTightIsolationMVArun2v1PWoldDMwLT"),
    ByVVTightIsolationMVArun2v1DBdR03oldDMwLTName = cms.untracked.string("hpsPFTauDiscriminationByVVTightIsolationMVArun2v1DBdR03oldDMwLT"),
    ByVVTightIsolationMVArun2v1DBnewDMwLTName = cms.untracked.string("hpsPFTauDiscriminationByVVTightIsolationMVArun2v1DBnewDMwLT"),
    ByVVTightIsolationMVArun2v1DBoldDMwLTName = cms.untracked.string("hpsPFTauDiscriminationByVVTightIsolationMVArun2v1DBoldDMwLT"),
    ByVVTightIsolationMVArun2v1PWdR03oldDMwLTName = cms.untracked.string("hpsPFTauDiscriminationByVVTightIsolationMVArun2v1PWdR03oldDMwLT"),
    ByVVTightIsolationMVArun2v1PWnewDMwLTName = cms.untracked.string("hpsPFTauDiscriminationByVVTightIsolationMVArun2v1PWnewDMwLT"),
    ByVVTightIsolationMVArun2v1PWoldDMwLTName = cms.untracked.string("hpsPFTauDiscriminationByVVTightIsolationMVArun2v1PWoldDMwLT"),
    DecayModeFindingName = cms.untracked.string("hpsPFTauDiscriminationByDecayModeFinding"),
    DecayModeFindingNewDMsName = cms.untracked.string("hpsPFTauDiscriminationByDecayModeFindingNewDMs"),
    trackMapNames = cms.untracked.vstring('TracksMap', 'ConversionTracksMap'),
    jetMapName = cms.untracked.string(''),
    pfCandMapName = cms.untracked.string('PFCandMap'),
    tauMapName = cms.untracked.string('tauMap'),
    fillerType = cms.untracked.string('FillerPFTausFromPFTaus')
  ),

  DCASig     = cms.untracked.PSet(
    active          = cms.untracked.bool(True),
    mitName         = cms.untracked.string('DCASig'),
    edmElectronName = cms.untracked.string('gedGsfElectrons'),
    edmMuonName     = cms.untracked.string('muons'),
    edmTauName      = cms.untracked.string('hpsPFTauProducer'),
    edmTauType      = cms.untracked.string('PFTau'),
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
