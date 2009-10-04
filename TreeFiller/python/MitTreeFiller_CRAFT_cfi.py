# $Id: MitTreeFiller_CRAFT_cfi.py,v 1.5 2009/07/10 13:50:02 loizides Exp $

import FWCore.ParameterSet.Config as cms

MitTreeFiller = cms.EDAnalyzer("FillMitTree",
    fillers = cms.untracked.vstring('MetaInfos',
                                    'CaloTowers',
                                    'BarrelBasicClusters',
                                    'BarrelSuperClusters',
                                    'EndcapBasicClusters',
                                    'EndcapSuperClusters',
                                    'CtfWithMaterialTracksP5',
                                    'CtfWithMaterialTracksP5LHCNavigation',
                                    'CtfWithMaterialTracksP5Bottom',
                                    'CtfWithMaterialTracksP5Top',
                                    'CtfWithMaterialTracksBeamHaloMuon',
                                    'CosmicMuonTracks',
                                    'CosmicMuonTracks1Leg',
                                    'CosmicMuonTracksBarrelOnly',
                                    'CosmicMuonTracksEndCapsOnly',
                                    'CosmicMuonTracksNoRPC',
                                    'CosmicMuonTracksWitht0Correction',
                                    'GlobalBeamHaloMuonEndCapsOnlyTracks',
                                    'GlobalCosmicMuonTracks',
                                    'GlobalCosmicMuon1LegTracks',
                                    'GlobalCosmicMuonBarrelOnlyTracks',
                                    'GlobalCosmicMuonEndCapsOnlyTracks',
                                    'GlobalCosmicMuonNoRPCTracks',
                                    'GlobalCosmicMuonWitht0CorrectionTracks',
                                    'GlobalCosmicMuonSplitTracks',
                                    'ConversionInOutTracks',
                                    'ConversionOutInTracks',
                                    'RSWithMaterialTracksP5',
                                    'RSWithMaterialTracksP5Top',
                                    'RSWithMaterialTracksP5Bottom',
                                    'SplittedTracksP5',
                                    'CosmicTrackFinderTracksP5',
                                    'CosmicTrackFinderTracksP5Top',
                                    'CosmicTrackFinderTracksP5Bottom',
                                    'LhcStandAloneMuonsBarrelOnly',
                                    'LhcStandAloneMuonsEndCapsOnly',
                                    'LhcStandAloneMuonsBarrelOnlyUpdatedAtVtx',
                                    'LhcStandAloneMuonsEndCapsOnlyUpdatedAtVtx',
                                    'TevMuonTracksDefault',
                                    'TevMuonTracksFirstHit',
                                    'TevMuonTracksPicky',
                                    'Muons',
                                    'LhcSTAMuonsBarrelOnly',
                                    'LhcSTAMuonsEndCapsOnly',
                                    'Muons1Leg',
                                    'MuonsBarrelOnly',
                                    'MuonsEndCapsOnly',
                                    'MuonsBeamHaloEndCapsOnly',
                                    'MuonsNoRPC',
                                    'MuonsWitht0Correction',
                                    'ConversionInOutElectronsStable',
                                    'ConversionOutInElectronsStable',
                                    'Conversions',
                                    'Photons',
                                    'ItrCone5Jets',
                                    'SisCone5Jets',
                                    'SisCone7Jets',
                                    'Kt4Jets',
                                    'Kt6Jets',
                                    'CaloMet',
                                    'CorMuonMet'),                                    

    MetaInfos = cms.untracked.PSet(
        active        = cms.untracked.bool(True),
        hltActive     = cms.untracked.bool(True),
        hltResEdmName = cms.untracked.string('TriggerResults'),
        hltEvtEdmName = cms.untracked.string('hltTriggerSummaryAOD'),
        hltProcNames  = cms.untracked.vstring('HLT','FU'),
        fillerType    = cms.untracked.string('FillerMetaInfos')
    ),
                         
    CaloTowers = cms.untracked.PSet(
        active           = cms.untracked.bool(True),
        mitName          = cms.untracked.string('CaloTowers'),
        edmName          = cms.untracked.string('towerMaker'),    
        caloTowerMapName = cms.untracked.string('CaloTowerMap'),
        fillerType       = cms.untracked.string('FillerCaloTowers')
    ),

    BarrelBasicClusters = cms.untracked.PSet(
        active              = cms.untracked.bool(True),
        mitName             = cms.untracked.string('BarrelBasicClusters'),
        edmName             = cms.untracked.string('cosmicBasicClusters:CosmicBarrelBasicClusters'),    
        basicClusterMapName = cms.untracked.string('barrelBasicClusterMap'),
        fillerType          = cms.untracked.string('FillerBasicClusters')
    ),
                                
    BarrelSuperClusters = cms.untracked.PSet(
        active                = cms.untracked.bool(True),
        mitName               = cms.untracked.string('BarrelSuperClusters'),
        edmName               = cms.untracked.string('cosmicSuperClusters:CosmicBarrelSuperClusters'),
        basicClusterMapName   = cms.untracked.string('barrelBasicClusterMap'),
        superClusterMapName   = cms.untracked.string('barrelSuperClusterMap'),
        superClusterIdMapName = cms.untracked.string('barrelSuperClusterIdMap'),
        fillerType            = cms.untracked.string('FillerSuperClusters')
    ),

    EndcapBasicClusters = cms.untracked.PSet(
        active              = cms.untracked.bool(True),
        mitName             = cms.untracked.string('EndcapBasicClusters'),
        edmName             = cms.untracked.string('cosmicBasicClusters:CosmicEndcapBasicClusters'),  
        basicClusterMapName = cms.untracked.string('endcapBasicClusterMap'),
        fillerType          = cms.untracked.string('FillerBasicClusters')
    ),

    EndcapSuperClusters = cms.untracked.PSet(
        active                = cms.untracked.bool(True),
        mitName               = cms.untracked.string('EndcapSuperClusters'),                     
        edmName               = cms.untracked.string('cosmicSuperClusters:CosmicEndcapSuperClusters'),
        basicClusterMapName   = cms.untracked.string('endcapBasicClusterMap'),
        superClusterMapName   = cms.untracked.string('endcapSuperClusterMap'),
        superClusterIdMapName = cms.untracked.string('endcapSuperClusterIdMap'),
        fillerType            = cms.untracked.string('FillerSuperClusters')
    ),
                     
    CtfWithMaterialTracksP5  = cms.untracked.PSet(
        active                    = cms.untracked.bool(True),
        ecalAssocActive           = cms.untracked.bool(False),
        mitName                   = cms.untracked.string('CtfWithMaterialTracksP5'),
        edmName                   = cms.untracked.string('ctfWithMaterialTracksP5'),
        trackingMapName           = cms.untracked.string('TrackingMap'),
        barrelSuperClusterMapName = cms.untracked.string('barrelSuperClusterMap'),
        endcapSuperClusterMapName = cms.untracked.string('endcapSuperClusterMap'),
        trackMapName              = cms.untracked.string('ctfWithMaterialTracksP5MapName'),
        edmSimAssociationName     = cms.untracked.string('trackingParticleRecoTrackAsssociation'),
        fillerType                = cms.untracked.string('FillerTracks')
    ),
    
    CtfWithMaterialTracksP5LHCNavigation = cms.untracked.PSet(
        active                    = cms.untracked.bool(True),
        ecalAssocActive           = cms.untracked.bool(False),
        mitName                   = cms.untracked.string('CtfWithMaterialTracksP5LHCNavigation'),
        edmName                   = cms.untracked.string('ctfWithMaterialTracksP5LHCNavigation'),
        trackingMapName           = cms.untracked.string('TrackingMap'),
        barrelSuperClusterMapName = cms.untracked.string('barrelSuperClusterMap'),
        endcapSuperClusterMapName = cms.untracked.string('endcapSuperClusterMap'),
        trackMapName              = cms.untracked.string('ctfWithMaterialTracksP5LHCNavigationMapName'),
        edmSimAssociationName     = cms.untracked.string('trackingParticleRecoTrackAsssociation'),
        fillerType                = cms.untracked.string('FillerTracks')
    ),
    
    CtfWithMaterialTracksP5Bottom = cms.untracked.PSet(
        active                    = cms.untracked.bool(True),
        ecalAssocActive           = cms.untracked.bool(False),
        mitName                   = cms.untracked.string('CtfWithMaterialTracksP5Bottom'),
        edmName                   = cms.untracked.string('ctfWithMaterialTracksP5Bottom'),
        trackingMapName           = cms.untracked.string('TrackingMap'),
        barrelSuperClusterMapName = cms.untracked.string('barrelSuperClusterMap'),
        endcapSuperClusterMapName = cms.untracked.string('endcapSuperClusterMap'),
        trackMapName              = cms.untracked.string('ctfWithMaterialTracksP5BottomMapName'),
        edmSimAssociationName     = cms.untracked.string('trackingParticleRecoTrackAsssociation'),
        fillerType                = cms.untracked.string('FillerTracks')
    ),
    
    CtfWithMaterialTracksP5Top = cms.untracked.PSet(
        active                    = cms.untracked.bool(True),
        ecalAssocActive           = cms.untracked.bool(False),
        mitName                   = cms.untracked.string('CtfWithMaterialTracksP5Top'),
        edmName                   = cms.untracked.string('ctfWithMaterialTracksP5Top'),
        trackingMapName           = cms.untracked.string('TrackingMap'),
        barrelSuperClusterMapName = cms.untracked.string('barrelSuperClusterMap'),
        endcapSuperClusterMapName = cms.untracked.string('endcapSuperClusterMap'),
        trackMapName              = cms.untracked.string('ctfWithMaterialTracksP5TopMapName'),
        edmSimAssociationName     = cms.untracked.string('trackingParticleRecoTrackAsssociation'),
        fillerType                = cms.untracked.string('FillerTracks')
    ),
    
    CtfWithMaterialTracksBeamHaloMuon = cms.untracked.PSet(
        active                    = cms.untracked.bool(True),
        ecalAssocActive           = cms.untracked.bool(False),
        mitName                   = cms.untracked.string('CtfWithMaterialTracksBeamHaloMuon'),
        edmName                   = cms.untracked.string('ctfWithMaterialTracksBeamHaloMuon'),
        trackingMapName           = cms.untracked.string('TrackingMap'),
        barrelSuperClusterMapName = cms.untracked.string('barrelSuperClusterMap'),
        endcapSuperClusterMapName = cms.untracked.string('endcapSuperClusterMap'),
        trackMapName              = cms.untracked.string('ctfWithMaterialTracksBeamHaloMuonMapName'),
        edmSimAssociationName     = cms.untracked.string('trackingParticleRecoTrackAsssociation'),
        fillerType                = cms.untracked.string('FillerTracks')
    ),

    CosmicMuonTracks = cms.untracked.PSet(
        active                    = cms.untracked.bool(True),
        ecalAssocActive           = cms.untracked.bool(False),
        mitName                   = cms.untracked.string('CosmicMuonTracks'),
        edmName                   = cms.untracked.string('cosmicMuons'),
        trackingMapName           = cms.untracked.string('TrackingMap'),
        barrelSuperClusterMapName = cms.untracked.string('barrelSuperClusterMap'),
        endcapSuperClusterMapName = cms.untracked.string('endcapSuperClusterMap'),
        trackMapName              = cms.untracked.string('StandaloneMuonTracksMapName'),
        edmSimAssociationName     = cms.untracked.string(''),
        fillerType                = cms.untracked.string('FillerTracks')
    ),
    
    CosmicMuonTracks1Leg = cms.untracked.PSet(
        active                    = cms.untracked.bool(True),
        ecalAssocActive           = cms.untracked.bool(False),
        mitName                   = cms.untracked.string('CosmicMuonTracks1Leg'),
        edmName                   = cms.untracked.string('cosmicMuons1Leg'),
        trackingMapName           = cms.untracked.string('TrackingMap'),
        barrelSuperClusterMapName = cms.untracked.string('barrelSuperClusterMap'),
        endcapSuperClusterMapName = cms.untracked.string('endcapSuperClusterMap'),
        trackMapName              = cms.untracked.string('CosmicMuonTracks1LegMapName'),
        edmSimAssociationName     = cms.untracked.string(''),
        fillerType                = cms.untracked.string('FillerTracks')
    ),
    
    CosmicMuonTracksBarrelOnly = cms.untracked.PSet(
        active                    = cms.untracked.bool(True),
        ecalAssocActive           = cms.untracked.bool(False),
        mitName                   = cms.untracked.string('CosmicMuonTracksBarrelOnly'),
        edmName                   = cms.untracked.string('cosmicMuonsBarrelOnly'),
        trackingMapName           = cms.untracked.string('TrackingMap'),
        barrelSuperClusterMapName = cms.untracked.string('barrelSuperClusterMap'),
        endcapSuperClusterMapName = cms.untracked.string('endcapSuperClusterMap'),
        trackMapName              = cms.untracked.string('CosmicMuonTracksBarrelOnlyMapName'),
        edmSimAssociationName     = cms.untracked.string(''),
        fillerType                = cms.untracked.string('FillerTracks')
    ),
    
    CosmicMuonTracksEndCapsOnly = cms.untracked.PSet(
        active                    = cms.untracked.bool(True),
        ecalAssocActive           = cms.untracked.bool(False),
        mitName                   = cms.untracked.string('CosmicMuonTracksEndCapsOnly'),
        edmName                   = cms.untracked.string('cosmicMuonsEndCapsOnly'),
        trackingMapName           = cms.untracked.string('TrackingMap'),
        barrelSuperClusterMapName = cms.untracked.string('barrelSuperClusterMap'),
        endcapSuperClusterMapName = cms.untracked.string('endcapSuperClusterMap'),
        trackMapName              = cms.untracked.string('CosmicMuonTracksEndCapsOnlyMapName'),
        edmSimAssociationName     = cms.untracked.string(''),
        fillerType                = cms.untracked.string('FillerTracks')
    ),
    
    CosmicMuonTracksNoRPC = cms.untracked.PSet(
        active                    = cms.untracked.bool(True),
        ecalAssocActive           = cms.untracked.bool(False),
        mitName                   = cms.untracked.string('CosmicMuonTracksNoRPC'),
        edmName                   = cms.untracked.string('cosmicMuonsNoRPC'),
        trackingMapName           = cms.untracked.string('TrackingMap'),
        barrelSuperClusterMapName = cms.untracked.string('barrelSuperClusterMap'),
        endcapSuperClusterMapName = cms.untracked.string('endcapSuperClusterMap'),
        trackMapName              = cms.untracked.string('CosmicMuonTracksNoRPCMapName'),
        edmSimAssociationName     = cms.untracked.string(''),
        fillerType                = cms.untracked.string('FillerTracks')
    ),

    CosmicMuonTracksWitht0Correction = cms.untracked.PSet(
        active                    = cms.untracked.bool(True),
        ecalAssocActive           = cms.untracked.bool(False),
        mitName                   = cms.untracked.string('CosmicMuonTracksWitht0Correction'),
        edmName                   = cms.untracked.string('cosmicMuonsWitht0Correction'),
        trackingMapName           = cms.untracked.string('TrackingMap'),
        barrelSuperClusterMapName = cms.untracked.string('barrelSuperClusterMap'),
        endcapSuperClusterMapName = cms.untracked.string('endcapSuperClusterMap'),
        trackMapName              = cms.untracked.string('StandaloneMuonTracksWT0CorrectionMapName'),
        edmSimAssociationName     = cms.untracked.string(''),
        fillerType                = cms.untracked.string('FillerTracks')
    ),
    
    GlobalBeamHaloMuonEndCapsOnlyTracks = cms.untracked.PSet(
        active                    = cms.untracked.bool(True),
        ecalAssocActive           = cms.untracked.bool(False),
        mitName                   = cms.untracked.string('GlobalBeamHaloMuonEndCapsOnlyTracks'),
        edmName                   = cms.untracked.string('globalBeamHaloMuonEndCapslOnly'),
        trackingMapName           = cms.untracked.string('TrackingMap'),
        barrelSuperClusterMapName = cms.untracked.string('barrelSuperClusterMap'),
        endcapSuperClusterMapName = cms.untracked.string('endcapSuperClusterMap'),
        trackMapName              = cms.untracked.string('GlobalBeamHaloMuonEndCapsOnlyTracksMapName'),
        edmSimAssociationName     = cms.untracked.string(''),
        fillerType                = cms.untracked.string('FillerTracks')
    ),
                   
    GlobalCosmicMuonTracks = cms.untracked.PSet(
        active                    = cms.untracked.bool(True),
        ecalAssocActive           = cms.untracked.bool(False),
        mitName                   = cms.untracked.string('GlobalCosmicMuonTracks'),
        edmName                   = cms.untracked.string('globalCosmicMuons'),
        trackingMapName           = cms.untracked.string('TrackingMap'),
        barrelSuperClusterMapName = cms.untracked.string('barrelSuperClusterMap'),
        endcapSuperClusterMapName = cms.untracked.string('endcapSuperClusterMap'),
        trackMapName              = cms.untracked.string('GlobalMuonTracksMapName'),
        edmSimAssociationName     = cms.untracked.string(''),
        fillerType                = cms.untracked.string('FillerTracks')
    ),
    
    GlobalCosmicMuon1LegTracks = cms.untracked.PSet(
        active                    = cms.untracked.bool(True),
        ecalAssocActive           = cms.untracked.bool(False),
        mitName                   = cms.untracked.string('GlobalCosmicMuon1LegTracks'),
        edmName                   = cms.untracked.string('globalCosmicMuons1Leg'),
        trackingMapName           = cms.untracked.string('TrackingMap'),
        barrelSuperClusterMapName = cms.untracked.string('barrelSuperClusterMap'),
        endcapSuperClusterMapName = cms.untracked.string('endcapSuperClusterMap'),
        trackMapName              = cms.untracked.string('GlobalMuon1LegTracksMapName'),
        edmSimAssociationName     = cms.untracked.string(''),
        fillerType                = cms.untracked.string('FillerTracks')
    ),
    
    GlobalCosmicMuonBarrelOnlyTracks = cms.untracked.PSet(
        active                    = cms.untracked.bool(True),
        ecalAssocActive           = cms.untracked.bool(False),
        mitName                   = cms.untracked.string('GlobalCosmicMuonBarrelOnlyTracks'),
        edmName                   = cms.untracked.string('globalCosmicMuonsBarrelOnly'),
        trackingMapName           = cms.untracked.string('TrackingMap'),
        barrelSuperClusterMapName = cms.untracked.string('barrelSuperClusterMap'),
        endcapSuperClusterMapName = cms.untracked.string('endcapSuperClusterMap'),
        trackMapName              = cms.untracked.string('GlobalMuonBarrelOnlyTracksMapName'),
        edmSimAssociationName     = cms.untracked.string(''),
        fillerType                = cms.untracked.string('FillerTracks')
    ),
    
    GlobalCosmicMuonEndCapsOnlyTracks = cms.untracked.PSet(
        active                    = cms.untracked.bool(True),
        ecalAssocActive           = cms.untracked.bool(False),
        mitName                   = cms.untracked.string('GlobalCosmicMuonEndCapsOnlyTracks'),
        edmName                   = cms.untracked.string('globalCosmicMuonsEndCapsOnly'),
        trackingMapName           = cms.untracked.string('TrackingMap'),
        barrelSuperClusterMapName = cms.untracked.string('barrelSuperClusterMap'),
        endcapSuperClusterMapName = cms.untracked.string('endcapSuperClusterMap'),
        trackMapName              = cms.untracked.string('GlobalMuonEndCapsOnlyTracksMapName'),
        edmSimAssociationName     = cms.untracked.string(''),
        fillerType                = cms.untracked.string('FillerTracks')
    ),
  
    GlobalCosmicMuonNoRPCTracks = cms.untracked.PSet(
        active                    = cms.untracked.bool(True),
        ecalAssocActive           = cms.untracked.bool(False),
        mitName                   = cms.untracked.string('GlobalCosmicMuonNoRPCTracks'),
        edmName                   = cms.untracked.string('globalCosmicMuonsNoRPC'),
        trackingMapName           = cms.untracked.string('TrackingMap'),
        barrelSuperClusterMapName = cms.untracked.string('barrelSuperClusterMap'),
        endcapSuperClusterMapName = cms.untracked.string('endcapSuperClusterMap'),
        trackMapName              = cms.untracked.string('GlobalMuonNoRPCTracksMapName'),
        edmSimAssociationName     = cms.untracked.string(''),
        fillerType                = cms.untracked.string('FillerTracks')
    ), 
     
    GlobalCosmicMuonWitht0CorrectionTracks = cms.untracked.PSet(
        active                    = cms.untracked.bool(True),
        ecalAssocActive           = cms.untracked.bool(False),
        mitName                   = cms.untracked.string('GlobalCosmicMuonWitht0CorrectionTracks'),
        edmName                   = cms.untracked.string('globalCosmicMuonsWitht0Correction'),
        trackingMapName           = cms.untracked.string('TrackingMap'),
        barrelSuperClusterMapName = cms.untracked.string('barrelSuperClusterMap'),
        endcapSuperClusterMapName = cms.untracked.string('endcapSuperClusterMap'),
        trackMapName              = cms.untracked.string('GlobalMuonWitht0CorrectionTracksMapName'),
        edmSimAssociationName     = cms.untracked.string(''),
        fillerType                = cms.untracked.string('FillerTracks')
    ),
    
    GlobalCosmicMuonSplitTracks = cms.untracked.PSet(
        active                    = cms.untracked.bool(True),
        ecalAssocActive           = cms.untracked.bool(False),
        mitName                   = cms.untracked.string('GlobalCosmicMuonSplitTracks'),
        edmName                   = cms.untracked.string('globalCosmicSplitMuons'),
        trackingMapName           = cms.untracked.string('TrackingMap'),
        barrelSuperClusterMapName = cms.untracked.string('barrelSuperClusterMap'),
        endcapSuperClusterMapName = cms.untracked.string('endcapSuperClusterMap'),
        trackMapName              = cms.untracked.string('GlobalMuonSplitTracksMapName'),
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
                   
    
    RSWithMaterialTracksP5  = cms.untracked.PSet(
        active                    = cms.untracked.bool(True),
        ecalAssocActive           = cms.untracked.bool(False),
        mitName                   = cms.untracked.string('RsWithMaterialTracksP5'),
        edmName                   = cms.untracked.string('rsWithMaterialTracksP5'),
        trackingMapName           = cms.untracked.string('TrackingMap'),
        barrelSuperClusterMapName = cms.untracked.string('barrelSuperClusterMap'),
        endcapSuperClusterMapName = cms.untracked.string('endcapSuperClusterMap'),
        trackMapName              = cms.untracked.string('RSWithMaterialTracksP5MapName'),
        edmSimAssociationName     = cms.untracked.string('trackingParticleRecoTrackAsssociation'),
        fillerType                = cms.untracked.string('FillerTracks')
    ),
    
    RSWithMaterialTracksP5Top  = cms.untracked.PSet(
        active                    = cms.untracked.bool(True),
        ecalAssocActive           = cms.untracked.bool(False),
        mitName                   = cms.untracked.string('RsWithMaterialTracksP5Top'),
        edmName                   = cms.untracked.string('rsWithMaterialTracksP5Top'),
        trackingMapName           = cms.untracked.string('TrackingMap'),
        barrelSuperClusterMapName = cms.untracked.string('barrelSuperClusterMap'),
        endcapSuperClusterMapName = cms.untracked.string('endcapSuperClusterMap'),
        trackMapName              = cms.untracked.string('RSWithMaterialTracksP5TopMapName'),
        edmSimAssociationName     = cms.untracked.string('trackingParticleRecoTrackAsssociation'),
        fillerType                = cms.untracked.string('FillerTracks')
    ),
    
    RSWithMaterialTracksP5Bottom  = cms.untracked.PSet(
        active                    = cms.untracked.bool(True),
        ecalAssocActive           = cms.untracked.bool(False),
        mitName                   = cms.untracked.string('RsWithMaterialTracksP5Bottom'),
        edmName                   = cms.untracked.string('rsWithMaterialTracksP5Bottom'),
        trackingMapName           = cms.untracked.string('TrackingMap'),
        barrelSuperClusterMapName = cms.untracked.string('barrelSuperClusterMap'),
        endcapSuperClusterMapName = cms.untracked.string('endcapSuperClusterMap'),
        trackMapName              = cms.untracked.string('RSWithMaterialTracksP5BottomMapName'),
        edmSimAssociationName     = cms.untracked.string('trackingParticleRecoTrackAsssociation'),
        fillerType                = cms.untracked.string('FillerTracks')
    ),
    
    SplittedTracksP5  = cms.untracked.PSet(
        active                    = cms.untracked.bool(True),
        ecalAssocActive           = cms.untracked.bool(False),
        mitName                   = cms.untracked.string('SplittedTracksP5'),
        edmName                   = cms.untracked.string('splittedTracksP5'),
        trackingMapName           = cms.untracked.string('TrackingMap'),
        barrelSuperClusterMapName = cms.untracked.string('barrelSuperClusterMap'),
        endcapSuperClusterMapName = cms.untracked.string('endcapSuperClusterMap'),
        trackMapName              = cms.untracked.string('splittedTracksMapName'),
        edmSimAssociationName     = cms.untracked.string('trackingParticleRecoTrackAsssociation'),
        fillerType                = cms.untracked.string('FillerTracks')
    ),
    
    CosmicTrackFinderTracksP5  = cms.untracked.PSet(
        active                    = cms.untracked.bool(True),
        ecalAssocActive           = cms.untracked.bool(False),
        mitName                   = cms.untracked.string('CosmicTrackFinderTracksP5'),
        edmName                   = cms.untracked.string('cosmictrackfinderP5'),
        trackingMapName           = cms.untracked.string('TrackingMap'),
        barrelSuperClusterMapName = cms.untracked.string('barrelSuperClusterMap'),
        endcapSuperClusterMapName = cms.untracked.string('endcapSuperClusterMap'),
        trackMapName              = cms.untracked.string('cosmicTrackFinderTracksP5MapName'),
        edmSimAssociationName     = cms.untracked.string('trackingParticleRecoTrackAsssociation'),
        fillerType                = cms.untracked.string('FillerTracks')
    ),
    
    CosmicTrackFinderTracksP5Top  = cms.untracked.PSet(
        active                    = cms.untracked.bool(True),
        ecalAssocActive           = cms.untracked.bool(False),
        mitName                   = cms.untracked.string('CosmicTrackFinderTracksP5Top'),
        edmName                   = cms.untracked.string('cosmictrackfinderP5Top'),
        trackingMapName           = cms.untracked.string('TrackingMap'),
        barrelSuperClusterMapName = cms.untracked.string('barrelSuperClusterMap'),
        endcapSuperClusterMapName = cms.untracked.string('endcapSuperClusterMap'),
        trackMapName              = cms.untracked.string('cosmicTrackFinderTracksP5TopMapName'),
        edmSimAssociationName     = cms.untracked.string('trackingParticleRecoTrackAsssociation'),
        fillerType                = cms.untracked.string('FillerTracks')
    ),
    
    CosmicTrackFinderTracksP5Bottom  = cms.untracked.PSet(
        active                    = cms.untracked.bool(True),
        ecalAssocActive           = cms.untracked.bool(False),
        mitName                   = cms.untracked.string('CosmicTrackFinderTracksP5Bottom'),
        edmName                   = cms.untracked.string('cosmictrackfinderP5Bottom'),
        trackingMapName           = cms.untracked.string('TrackingMap'),
        barrelSuperClusterMapName = cms.untracked.string('barrelSuperClusterMap'),
        endcapSuperClusterMapName = cms.untracked.string('endcapSuperClusterMap'),
        trackMapName              = cms.untracked.string('cosmicTrackFinderTracksP5BottomMapName'),
        edmSimAssociationName     = cms.untracked.string('trackingParticleRecoTrackAsssociation'),
        fillerType                = cms.untracked.string('FillerTracks')
    ),

    LhcStandAloneMuonsBarrelOnly  = cms.untracked.PSet(
        active                    = cms.untracked.bool(True),
        ecalAssocActive           = cms.untracked.bool(False),
        mitName                   = cms.untracked.string('LhcStandAloneMuonsBarrelOnly'),
        edmName                   = cms.untracked.string('lhcStandAloneMuonsBarrelOnly'),
        trackingMapName           = cms.untracked.string('TrackingMap'),
        barrelSuperClusterMapName = cms.untracked.string('barrelSuperClusterMap'),
        endcapSuperClusterMapName = cms.untracked.string('endcapSuperClusterMap'),
        trackMapName              = cms.untracked.string('lhcStandAloneMuonsBarrelOnlyMapName'),
        edmSimAssociationName     = cms.untracked.string('trackingParticleRecoTrackAsssociation'),
        fillerType                = cms.untracked.string('FillerTracks')
    ),
    
    LhcStandAloneMuonsEndCapsOnly  = cms.untracked.PSet(
        active                    = cms.untracked.bool(True),
        ecalAssocActive           = cms.untracked.bool(False),
        mitName                   = cms.untracked.string('LhcStandAloneMuonsEndCapsOnly'),
        edmName                   = cms.untracked.string('lhcStandAloneMuonsEndCapsOnly'),
        trackingMapName           = cms.untracked.string('TrackingMap'),
        barrelSuperClusterMapName = cms.untracked.string('barrelSuperClusterMap'),
        endcapSuperClusterMapName = cms.untracked.string('endcapSuperClusterMap'),
        trackMapName              = cms.untracked.string('lhcStandAloneMuonsEndCapsOnlyMapName'),
        edmSimAssociationName     = cms.untracked.string('trackingParticleRecoTrackAsssociation'),
        fillerType                = cms.untracked.string('FillerTracks')
    ),
    
    LhcStandAloneMuonsBarrelOnlyUpdatedAtVtx  = cms.untracked.PSet(
        active                    = cms.untracked.bool(True),
        ecalAssocActive           = cms.untracked.bool(False),
        mitName                   = cms.untracked.string('LhcStandAloneMuonsBarrelOnlyUpdatedAtVtx'),
        edmName                   = cms.untracked.string('lhcStandAloneMuonsBarrelOnly:UpdatedAtVtx'),
        trackingMapName           = cms.untracked.string('TrackingMap'),
        barrelSuperClusterMapName = cms.untracked.string('barrelSuperClusterMap'),
        endcapSuperClusterMapName = cms.untracked.string('endcapSuperClusterMap'),
        trackMapName              = cms.untracked.string('lhcStandAloneMuonsBarrelOnlyUpdatedAtVtxMapName'),
        edmSimAssociationName     = cms.untracked.string('trackingParticleRecoTrackAsssociation'),
        fillerType                = cms.untracked.string('FillerTracks')
    ),
    
    LhcStandAloneMuonsEndCapsOnlyUpdatedAtVtx  = cms.untracked.PSet(
        active                    = cms.untracked.bool(True),
        ecalAssocActive           = cms.untracked.bool(False),
        mitName                   = cms.untracked.string('LhcStandAloneMuonsEndCapsOnlyUpdatedAtVtx'),
        edmName                   = cms.untracked.string('lhcStandAloneMuonsEndCapsOnly:UpdatedAtVtx'),
        trackingMapName           = cms.untracked.string('TrackingMap'),
        barrelSuperClusterMapName = cms.untracked.string('barrelSuperClusterMap'),
        endcapSuperClusterMapName = cms.untracked.string('endcapSuperClusterMap'),
        trackMapName              = cms.untracked.string('lhcStandAloneMuonsEndCapsOnlyUpdatedAtVtxMapName'),
        edmSimAssociationName     = cms.untracked.string('trackingParticleRecoTrackAsssociation'),
        fillerType                = cms.untracked.string('FillerTracks')
    ),
    
    TevMuonTracksDefault  = cms.untracked.PSet(
        active                    = cms.untracked.bool(True),
        ecalAssocActive           = cms.untracked.bool(False),
        mitName                   = cms.untracked.string('TevMuonTracksDefault'),
        edmName                   = cms.untracked.string('tevMuons:default'),
        trackingMapName           = cms.untracked.string('TrackingMap'),
        barrelSuperClusterMapName = cms.untracked.string('barrelSuperClusterMap'),
        endcapSuperClusterMapName = cms.untracked.string('endcapSuperClusterMap'),
        trackMapName              = cms.untracked.string('tevMuonTracksDefaultMapName'),
        edmSimAssociationName     = cms.untracked.string('trackingParticleRecoTrackAsssociation'),
        fillerType                = cms.untracked.string('FillerTracks')
    ),
    
    TevMuonTracksFirstHit  = cms.untracked.PSet(
        active                    = cms.untracked.bool(True),
        ecalAssocActive           = cms.untracked.bool(False),
        mitName                   = cms.untracked.string('TevMuonTracksFirstHit'),
        edmName                   = cms.untracked.string('tevMuons:firstHit'),
        trackingMapName           = cms.untracked.string('TrackingMap'),
        barrelSuperClusterMapName = cms.untracked.string('barrelSuperClusterMap'),
        endcapSuperClusterMapName = cms.untracked.string('endcapSuperClusterMap'),
        trackMapName              = cms.untracked.string('tevMuonTracksFirstHitMapName'),
        edmSimAssociationName     = cms.untracked.string('trackingParticleRecoTrackAsssociation'),
        fillerType                = cms.untracked.string('FillerTracks')
    ),
    
    TevMuonTracksPicky  = cms.untracked.PSet(
        active                    = cms.untracked.bool(True),
        ecalAssocActive           = cms.untracked.bool(False),
        mitName                   = cms.untracked.string('TevMuonTracksPicky'),
        edmName                   = cms.untracked.string('tevMuons:picky'),
        trackingMapName           = cms.untracked.string('TrackingMap'),
        barrelSuperClusterMapName = cms.untracked.string('barrelSuperClusterMap'),
        endcapSuperClusterMapName = cms.untracked.string('endcapSuperClusterMap'),
        trackMapName              = cms.untracked.string('tevMuonTracksPickyMapName'),
        edmSimAssociationName     = cms.untracked.string('trackingParticleRecoTrackAsssociation'),
        fillerType                = cms.untracked.string('FillerTracks')
    ),

    
    Muons = cms.untracked.PSet(
        active              = cms.untracked.bool(True),
        mitName             = cms.untracked.string('Muons'),
        edmName             = cms.untracked.string('muons'),
        globalTrackMapName  = cms.untracked.string('GlobalMuonTracksMapName'),
        staTrackMapName     = cms.untracked.string('StandaloneMuonTracksMapName'),
        staVtxTrackMapName  = cms.untracked.string('StandaloneMuonTracksMapName'),
        trackerTrackMapName = cms.untracked.string('ctfWithMaterialTracksP5LHCNavigationMapName'),
        fillerType          = cms.untracked.string('FillerMuons')
    ),
    
    LhcSTAMuonsBarrelOnly = cms.untracked.PSet(
        active              = cms.untracked.bool(True),
        mitName             = cms.untracked.string('LhcSTAMuonsBarrelOnly'),
        edmName             = cms.untracked.string('lhcSTAMuonsBarrelOnly'),
        globalTrackMapName  = cms.untracked.string('GlobalMuonBarrelOnlyTracksMapName'),
        staTrackMapName     = cms.untracked.string('lhcStandAloneMuonsBarrelOnlyMapName'),
        staVtxTrackMapName  = cms.untracked.string('lhcStandAloneMuonsBarrelOnlyUpdatedAtVtxMapName'),
        trackerTrackMapName = cms.untracked.string('ctfWithMaterialTracksP5LHCNavigationMapName'),
        fillerType          = cms.untracked.string('FillerMuons')
    ),
    
    LhcSTAMuonsEndCapsOnly = cms.untracked.PSet(
        active              = cms.untracked.bool(True),
        mitName             = cms.untracked.string('LhcSTAMuonsEndCapsOnly'),
        edmName             = cms.untracked.string('lhcSTAMuonsEndCapsOnly'),
        globalTrackMapName  = cms.untracked.string('GlobalMuonEndCapsOnlyTracksMapName'),
        staTrackMapName     = cms.untracked.string('lhcStandAloneMuonsEndCapsOnlyMapName'),
        staVtxTrackMapName  = cms.untracked.string('lhcStandAloneMuonsEndCapsOnlyUpdatedAtVtxMapName'),
        trackerTrackMapName = cms.untracked.string('ctfWithMaterialTracksP5LHCNavigationMapName'),
        fillerType          = cms.untracked.string('FillerMuons')
    ),
    
    Muons1Leg = cms.untracked.PSet(
        active              = cms.untracked.bool(True),
        mitName             = cms.untracked.string('Muons1Leg'),
        edmName             = cms.untracked.string('muons1Leg'),
        globalTrackMapName  = cms.untracked.string('GlobalMuon1LegTracksMapName'),
        staTrackMapName     = cms.untracked.string('CosmicMuonTracks1LegMapName'),
        staVtxTrackMapName  = cms.untracked.string('CosmicMuonTracks1LegMapName'),
        trackerTrackMapName = cms.untracked.string('ctfWithMaterialTracksP5MapName'),
        fillerType          = cms.untracked.string('FillerMuons')
    ),
        
    MuonsBarrelOnly = cms.untracked.PSet(
        active              = cms.untracked.bool(True),
        mitName             = cms.untracked.string('MuonsBarrelOnly'),
        edmName             = cms.untracked.string('muonsBarrelOnly'),
        globalTrackMapName  = cms.untracked.string('GlobalMuonBarrelOnlyTracksMapName'),
        staTrackMapName     = cms.untracked.string('CosmicMuonTracksBarrelOnlyMapName'),
        staVtxTrackMapName  = cms.untracked.string('CosmicMuonTracksBarrelOnlyMapName'),
        trackerTrackMapName = cms.untracked.string('ctfWithMaterialTracksP5MapName'),
        fillerType          = cms.untracked.string('FillerMuons')
    ),
         
    MuonsEndCapsOnly = cms.untracked.PSet(
        active              = cms.untracked.bool(True),
        mitName             = cms.untracked.string('MuonsEndCapsOnly'),
        edmName             = cms.untracked.string('muonsEndCapsOnly'),
        globalTrackMapName  = cms.untracked.string('GlobalMuonEndCapsOnlyTracksMapName'),
        staTrackMapName     = cms.untracked.string('CosmicMuonTracksEndCapsOnlyMapName'),
        staVtxTrackMapName  = cms.untracked.string('CosmicMuonTracksEndCapsOnlyMapName'),
        trackerTrackMapName = cms.untracked.string('ctfWithMaterialTracksP5MapName'),
        fillerType          = cms.untracked.string('FillerMuons')
    ),
    
    MuonsBeamHaloEndCapsOnly = cms.untracked.PSet(
        active              = cms.untracked.bool(True),
        mitName             = cms.untracked.string('MuonsBeamHaloEndCapsOnly'),
        edmName             = cms.untracked.string('muonsBeamHaloEndCapsOnly'),
        globalTrackMapName  = cms.untracked.string('GlobalBeamHaloMuonEndCapsOnlyTracksMapName'),
        staTrackMapName     = cms.untracked.string('CosmicMuonTracksEndCapsOnlyMapName'),
        staVtxTrackMapName  = cms.untracked.string('CosmicMuonTracksEndCapsOnlyMapName'),
        trackerTrackMapName = cms.untracked.string('ctfWithMaterialTracksBeamHaloMuonMapName'),
        fillerType          = cms.untracked.string('FillerMuons')
    ),        
    
    MuonsNoRPC = cms.untracked.PSet(
        active              = cms.untracked.bool(True),
        mitName             = cms.untracked.string('MuonsNoRPC'),
        edmName             = cms.untracked.string('muonsNoRPC'),
        globalTrackMapName  = cms.untracked.string('GlobalMuonNoRPCTracksMapName'),
        staTrackMapName     = cms.untracked.string('CosmicMuonTracksNoRPCMapName'),
        staVtxTrackMapName  = cms.untracked.string('CosmicMuonTracksNoRPCMapName'),
        trackerTrackMapName = cms.untracked.string('ctfWithMaterialTracksP5MapName'),
        fillerType          = cms.untracked.string('FillerMuons')
    ),   
    
    MuonsWitht0Correction = cms.untracked.PSet(
        active              = cms.untracked.bool(True),
        mitName             = cms.untracked.string('MuonsWitht0Correction'),
        edmName             = cms.untracked.string('muonsWitht0Correction'),
        globalTrackMapName  = cms.untracked.string('GlobalMuonWitht0CorrectionTracksMapName'),
        staTrackMapName     = cms.untracked.string('CosmicMuonTracksWitht0CorrectionMapName'),
        staVtxTrackMapName  = cms.untracked.string('CosmicMuonTracksWitht0CorrectionMapName'),
        trackerTrackMapName = cms.untracked.string('ctfWithMaterialTracksP5MapName'),
        fillerType          = cms.untracked.string('FillerMuons')
    ),

    ConversionElectrons = cms.untracked.PSet(
        active                = cms.untracked.bool(True),
        mitName               = cms.untracked.string('ConversionElectrons'),
        convInOutTracksName   = cms.untracked.string('ConversionInOutTracks'),
        convOutInTracksName   = cms.untracked.string('ConversionOutInTracks'),
        convInOutTrackMapName = cms.untracked.string('ConversionInOutTracksMapName'),
        convOutInTrackMapName = cms.untracked.string('ConversionOutInTracksMapName'),
        convElectronMapName   = cms.untracked.string('ConversionElectronsMapName'),
        fillerType            = cms.untracked.string('FillerConversionElectrons')
    ),
                   
    Conversions = cms.untracked.PSet(
        active              = cms.untracked.bool(True),
        mitName             = cms.untracked.string('Conversions'),
        edmName             = cms.untracked.string('conversions'),
        stablePartMaps      = cms.untracked.vstring('ConversionInOutElectronsStableTrackMapName',
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
        fillerType                = cms.untracked.string('FillerPhotons')
    ),

    ItrCone5Jets = cms.untracked.PSet(
        active                                 = cms.untracked.bool(True),
        flavorMatchingActive                   = cms.untracked.bool(False),
        bTaggingActive                         = cms.untracked.bool(False),
        jetToVertexActive                      = cms.untracked.bool(False),
        jetCorrectionsActive                   = cms.untracked.bool(False),
        mitName                                = cms.untracked.string('ItrCone5Jets'),
        edmName                                = cms.untracked.string('iterativeCone5CaloJets'),
        jetToVertexAlphaName                   = cms.untracked.string('jetVertexAlphaIC5:Var'),
        jetToVertexBetaName                    = cms.untracked.string('jetVertexBetaIC5:Var'),
        L2JetCorrectorName                     = cms.untracked.string('L2RelativeJetCorrectorIC5Calo'),
        L3JetCorrectorName                     = cms.untracked.string('L3AbsoluteJetCorrectorIC5Calo'),
        flavorMatchingByReferenceName          = cms.untracked.string('IC5byRef'),
        flavorMatchingDefinition               = cms.untracked.string('Algorithmic'),
        JetProbabilityBJetTagsName             = cms.untracked.string('jetProbabilityBJetTags'),
        JetBProbabilityBJetTagsName            = cms.untracked.string('jetBProbabilityBJetTags'),
        SimpleSecondaryVertexBJetTagsName      = cms.untracked.string('simpleSecondaryVertexBJetTags'),
        CombinedSecondaryVertexBJetTagsName    = cms.untracked.string('combinedSecondaryVertexBJetTags'),
        CombinedSecondaryVertexMVABJetTagsName = cms.untracked.string('combinedSecondaryVertexMVABJetTags'),
        ImpactParameterMVABJetTagsName         = cms.untracked.string('impactParameterMVABJetTags'),
        TrackCountingHighEffBJetTagsName       = cms.untracked.string('trackCountingHighEffBJetTags'),
        TrackCountingHighPurBJetTagsName       = cms.untracked.string('trackCountingHighPurBJetTags'),
        SoftMuonBJetTagsName                   = cms.untracked.string('softMuonBJetTags'),
        SoftMuonNoIPBJetTagsName               = cms.untracked.string('softMuonNoIPBJetTags'),
        SoftElectronBJetTagsName               = cms.untracked.string('softElectronBJetTags'),        
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
        mitName                       = cms.untracked.string('SisCone5Jets'),
        edmName                       = cms.untracked.string('sisCone5CaloJets'),
        jetToVertexAlphaName          = cms.untracked.string('jetVertexAlphaSC5:Var'),
        jetToVertexBetaName           = cms.untracked.string('jetVertexBetaSC5:Var'),
        L2JetCorrectorName            = cms.untracked.string('L2RelativeJetCorrectorSC5Calo'),
        L3JetCorrectorName            = cms.untracked.string('L3AbsoluteJetCorrectorSC5Calo'),
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
        mitName                       = cms.untracked.string('SisCone7Jets'),
        edmName                       = cms.untracked.string('sisCone7CaloJets'),
        jetToVertexAlphaName          = cms.untracked.string('jetVertexAlphaSC7:Var'),
        jetToVertexBetaName           = cms.untracked.string('jetVertexBetaSC7:Var'),
        L2JetCorrectorName            = cms.untracked.string('L2RelativeJetCorrectorSC7Calo'),
        L3JetCorrectorName            = cms.untracked.string('L3AbsoluteJetCorrectorSC7Calo'),
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
        mitName                       = cms.untracked.string('Kt4Jets'),
        edmName                       = cms.untracked.string('kt4CaloJets'),
        jetToVertexAlphaName          = cms.untracked.string('jetVertexAlphaKT4:Var'),
        jetToVertexBetaName           = cms.untracked.string('jetVertexBetaKT4:Var'),
        L2JetCorrectorName            = cms.untracked.string('L2RelativeJetCorrectorKT4Calo'),
        L3JetCorrectorName            = cms.untracked.string('L3AbsoluteJetCorrectorKT4Calo'),
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
        mitName                       = cms.untracked.string('Kt6Jets'),
        edmName                       = cms.untracked.string('kt6CaloJets'),
        jetToVertexAlphaName          = cms.untracked.string('jetVertexAlphaKT6:Var'),
        jetToVertexBetaName           = cms.untracked.string('jetVertexBetaKT6:Var'),
        L2JetCorrectorName            = cms.untracked.string('L2RelativeJetCorrectorKT6Calo'),
        L3JetCorrectorName            = cms.untracked.string('L3AbsoluteJetCorrectorKT6Calo'),
        flavorMatchingByReferenceName = cms.untracked.string('KT6byRef'),
        flavorMatchingDefinition      = cms.untracked.string('Algorithmic'),
        caloTowerMapName              = cms.untracked.string('CaloTowerMap'),
        jetMapName                    = cms.untracked.string('KT6JetMap'),
        fillerType                    = cms.untracked.string('FillerCaloJets')
    ),
                               
    CaloMet = cms.untracked.PSet(
        active     = cms.untracked.bool(True),
        mitName    = cms.untracked.string('CaloMet'),
        edmName    = cms.untracked.string('met'),
        fillerType = cms.untracked.string('FillerCaloMet')
    ),
    
    CorMuonMet = cms.untracked.PSet(
        active     = cms.untracked.bool(True),
        mitName    = cms.untracked.string('CorMuonMet'),
        edmName    = cms.untracked.string('corMetGlobalMuons'),
        fillerType = cms.untracked.string('FillerCaloMet')
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
)
