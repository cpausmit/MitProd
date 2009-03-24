# $Id: MitTreeFiller_CRAFT_cfi.py,v 1.2 2008/11/14 16:25:00 bendavid Exp $

import FWCore.ParameterSet.Config as cms

MitTreeFiller = cms.EDAnalyzer("FillMitTree",
    fillers = cms.untracked.vstring('MetaInfos',
                                    'CaloTowers',
                                    'BarrelBasicClusters',
                                    'BarrelSuperClusters',
                                    'EndcapBasicClusters',
                                    'EndcapSuperClusters',
                                    'GeneralTracks',
                                    'StandaloneMuonTracks',
                                    'StandaloneMuonTracksWVtxConstraint',
                                    'GlobalMuonTracks',
                                    'ConversionInOutTracks',
                                    'ConversionOutInTracks',
                                    'RSWithMaterialTracks',
                                    'SplittedTracks',
                                    'CosmicTracks',
                                    'CtfWithMaterialTracksP5LHCNavigation',
                                    'Muons',
                                    'Conversions',
                                    'Photons',
                                    'ItrCone5Jets',
                                    'SisCone5Jets',
                                    'SisCone7Jets',
                                    'Kt4Jets',
                                    'Kt6Jets',
                                    'CaloMet',
                                    ),                                    

    MetaInfos = cms.untracked.PSet(
        active        = cms.untracked.bool(True),
        l1Active      = cms.untracked.bool(False),
        hltActive     = cms.untracked.bool(True),
        hltProcName   =  cms.untracked.string('HLT'),
        hltResEdmName = cms.untracked.string('TriggerResults::HLT'),
        hltEvtEdmName = cms.untracked.string('hltTriggerSummaryAOD::HLT'),
        fillerType    = cms.untracked.string('FillerMetaInfos')
    ),
                         
    CaloTowers = cms.untracked.PSet(
        active  = cms.untracked.bool(True),
        mitName = cms.untracked.string('CaloTowers'),
        edmName = cms.untracked.string('towerMaker'),    
        caloTowerMapName = cms.untracked.string('CaloTowerMap'),
        fillerType       = cms.untracked.string('FillerCaloTowers')
    ),

    BarrelBasicClusters = cms.untracked.PSet(
        active  = cms.untracked.bool(True),
        mitName = cms.untracked.string('BarrelBasicClusters'),
        edmName = cms.untracked.string('cosmicBasicClusters:CosmicBarrelBasicClusters'),    
        basicClusterMapName = cms.untracked.string('barrelBasicClusterMap'),
        fillerType          = cms.untracked.string('FillerBasicClusters')
    ),
                                
    BarrelSuperClusters = cms.untracked.PSet(
        active  = cms.untracked.bool(True),
        mitName = cms.untracked.string('BarrelSuperClusters'),
        edmName = cms.untracked.string('cosmicSuperClusters:CosmicBarrelSuperClusters'),
        basicClusterMapName = cms.untracked.string('barrelBasicClusterMap'),
        superClusterMapName = cms.untracked.string('barrelSuperClusterMap'),
        superClusterIdMapName = cms.untracked.string('barrelSuperClusterIdMap'),
        fillerType          = cms.untracked.string('FillerSuperClusters')
    ),

    EndcapBasicClusters = cms.untracked.PSet(
        active  = cms.untracked.bool(True),
        mitName = cms.untracked.string('EndcapBasicClusters'),
        edmName = cms.untracked.string('cosmicBasicClusters:CosmicEndcapBasicClusters'),  
        basicClusterMapName = cms.untracked.string('endcapBasicClusterMap'),
        fillerType          = cms.untracked.string('FillerBasicClusters')
    ),

    EndcapSuperClusters = cms.untracked.PSet(
        active  = cms.untracked.bool(True),
        mitName = cms.untracked.string('EndcapSuperClusters'),                     
        edmName = cms.untracked.string('cosmicSuperClusters:CosmicEndcapSuperClusters'),
        basicClusterMapName = cms.untracked.string('endcapBasicClusterMap'),
        superClusterMapName = cms.untracked.string('endcapSuperClusterMap'),
        superClusterIdMapName = cms.untracked.string('endcapSuperClusterIdMap'),
        fillerType            = cms.untracked.string('FillerSuperClusters')
    ),
                     
    GeneralTracks  = cms.untracked.PSet(
        active                = cms.untracked.bool(True),
        ecalAssocActive       = cms.untracked.bool(False),
        mitName               = cms.untracked.string('CtfWithMaterialTracks'),
        edmName               = cms.untracked.string('ctfWithMaterialTracksP5'),
        trackingMapName       = cms.untracked.string('TrackingMap'),
        barrelSuperClusterMapName = cms.untracked.string('barrelSuperClusterMap'),
        endcapSuperClusterMapName = cms.untracked.string('endcapSuperClusterMap'),
        trackMapName          = cms.untracked.string('TracksMapName'),
        edmSimAssociationName = cms.untracked.string('trackingParticleRecoTrackAsssociation'),
        fillerType            = cms.untracked.string('FillerTracks')
    ),

    StandaloneMuonTracks = cms.untracked.PSet(
        active                = cms.untracked.bool(True),
        ecalAssocActive       = cms.untracked.bool(False),
        mitName               = cms.untracked.string('CosmicMuonTracks'),
        edmName               = cms.untracked.string('cosmicMuons'),
        trackingMapName       = cms.untracked.string('TrackingMap'),
        barrelSuperClusterMapName = cms.untracked.string('barrelSuperClusterMap'),
        endcapSuperClusterMapName = cms.untracked.string('endcapSuperClusterMap'),
        trackMapName          = cms.untracked.string('StandaloneMuonTracksMapName'),
        edmSimAssociationName = cms.untracked.string(''),
        fillerType            = cms.untracked.string('FillerTracks')
    ),

    StandaloneMuonTracksWVtxConstraint = cms.untracked.PSet(
        active                = cms.untracked.bool(True),
        ecalAssocActive       = cms.untracked.bool(False),
        mitName               = cms.untracked.string('CosmicMuonTracksWitht0Correction'),
        edmName               = cms.untracked.string('cosmicMuonsWitht0Correction'),
        trackingMapName       = cms.untracked.string('TrackingMap'),
        barrelSuperClusterMapName = cms.untracked.string('barrelSuperClusterMap'),
        endcapSuperClusterMapName = cms.untracked.string('endcapSuperClusterMap'),
        trackMapName          = cms.untracked.string('StandaloneMuonTracksWT0CorrectionMapName'),
        edmSimAssociationName = cms.untracked.string(''),
        fillerType            = cms.untracked.string('FillerTracks')
    ),
                   
    GlobalMuonTracks = cms.untracked.PSet(
        active                = cms.untracked.bool(True),
        ecalAssocActive       = cms.untracked.bool(False),
        mitName               = cms.untracked.string('GlobalCosmicMuonTracks'),
        edmName               = cms.untracked.string('globalCosmicMuons'),
        trackingMapName       = cms.untracked.string('TrackingMap'),
        barrelSuperClusterMapName = cms.untracked.string('barrelSuperClusterMap'),
        endcapSuperClusterMapName = cms.untracked.string('endcapSuperClusterMap'),
        trackMapName          = cms.untracked.string('GlobalMuonTracksMapName'),
        edmSimAssociationName = cms.untracked.string(''),
        fillerType            = cms.untracked.string('FillerTracks')
    ),
                   
    ConversionInOutTracks = cms.untracked.PSet(
        active                = cms.untracked.bool(True),
        ecalAssocActive       = cms.untracked.bool(False),
        mitName               = cms.untracked.string('ConversionInOutTracks'),
        edmName               = cms.untracked.string('ckfInOutTracksFromConversions'),
        trackingMapName       = cms.untracked.string('TrackingMap'),
        barrelSuperClusterMapName = cms.untracked.string('barrelSuperClusterMap'),
        endcapSuperClusterMapName = cms.untracked.string('endcapSuperClusterMap'),
        trackMapName          = cms.untracked.string('ConversionInOutTracksMapName'),
        edmSimAssociationName = cms.untracked.string('assocInOutConversionTracks'),
        fillerType            = cms.untracked.string('FillerTracks')
    ),

    ConversionOutInTracks = cms.untracked.PSet(
        active                = cms.untracked.bool(True),
        ecalAssocActive       = cms.untracked.bool(False),
        mitName               = cms.untracked.string('ConversionOutInTracks'),
        edmName               = cms.untracked.string('ckfOutInTracksFromConversions'),
        trackingMapName       = cms.untracked.string('TrackingMap'),
        barrelSuperClusterMapName = cms.untracked.string('barrelSuperClusterMap'),
        endcapSuperClusterMapName = cms.untracked.string('endcapSuperClusterMap'),
        trackMapName          = cms.untracked.string('ConversionOutInTracksMapName'),
        edmSimAssociationName = cms.untracked.string('assocOutInConversionTracks'),
        fillerType            = cms.untracked.string('FillerTracks')
    ),
                   
    
    RSWithMaterialTracks  = cms.untracked.PSet(
        active                = cms.untracked.bool(True),
        ecalAssocActive       = cms.untracked.bool(False),
        mitName               = cms.untracked.string('RsWithMaterialTracks'),
        edmName               = cms.untracked.string('rsWithMaterialTracksP5'),
        trackingMapName       = cms.untracked.string('TrackingMap'),
        barrelSuperClusterMapName = cms.untracked.string('barrelSuperClusterMap'),
        endcapSuperClusterMapName = cms.untracked.string('endcapSuperClusterMap'),
        trackMapName          = cms.untracked.string('rsTracksMapName'),
        edmSimAssociationName = cms.untracked.string('trackingParticleRecoTrackAsssociation'),
        fillerType            = cms.untracked.string('FillerTracks')
    ),
    
    SplittedTracks  = cms.untracked.PSet(
        active                = cms.untracked.bool(True),
        ecalAssocActive       = cms.untracked.bool(False),
        mitName               = cms.untracked.string('SplittedTracks'),
        edmName               = cms.untracked.string('splittedTracksP5'),
        trackingMapName       = cms.untracked.string('TrackingMap'),
        barrelSuperClusterMapName = cms.untracked.string('barrelSuperClusterMap'),
        endcapSuperClusterMapName = cms.untracked.string('endcapSuperClusterMap'),
        trackMapName          = cms.untracked.string('splittedTracksMapName'),
        edmSimAssociationName = cms.untracked.string('trackingParticleRecoTrackAsssociation'),
        fillerType            = cms.untracked.string('FillerTracks')
    ),
    
    CosmicTracks  = cms.untracked.PSet(
        active                = cms.untracked.bool(True),
        ecalAssocActive       = cms.untracked.bool(False),
        mitName               = cms.untracked.string('CosmicTracks'),
        edmName               = cms.untracked.string('cosmictrackfinderP5'),
        trackingMapName       = cms.untracked.string('TrackingMap'),
        barrelSuperClusterMapName = cms.untracked.string('barrelSuperClusterMap'),
        endcapSuperClusterMapName = cms.untracked.string('endcapSuperClusterMap'),
        trackMapName          = cms.untracked.string('cosmicTracksMapName'),
        edmSimAssociationName = cms.untracked.string('trackingParticleRecoTrackAsssociation'),
        fillerType            = cms.untracked.string('FillerTracks')
    ),
    
    CtfWithMaterialTracksP5LHCNavigation = cms.untracked.PSet(
        active                = cms.untracked.bool(True),
        ecalAssocActive       = cms.untracked.bool(False),
        mitName               = cms.untracked.string('CtfWithMaterialTracksP5LHCNavigation'),
        edmName               = cms.untracked.string('ctfWithMaterialTracksP5LHCNavigation'),
        trackingMapName       = cms.untracked.string('TrackingMap'),
        barrelSuperClusterMapName = cms.untracked.string('barrelSuperClusterMap'),
        endcapSuperClusterMapName = cms.untracked.string('endcapSuperClusterMap'),
        trackMapName          = cms.untracked.string('ctfWithMaterialTracksP5LHCNavigationMapName'),
        edmSimAssociationName = cms.untracked.string('trackingParticleRecoTrackAsssociation'),
        fillerType            = cms.untracked.string('FillerTracks')
    ),

    Muons = cms.untracked.PSet(
        active  = cms.untracked.bool(True),
        mitName = cms.untracked.string('Muons'),
        edmName = cms.untracked.string('muons'),
        globalTrackMapName  = cms.untracked.string('GlobalMuonTracksMapName'),
        staTrackMapName     = cms.untracked.string('StandaloneMuonTracksMapName'),
        staVtxTrackMapName  = cms.untracked.string('StandaloneMuonTracksMapName'),
        trackerTrackMapName = cms.untracked.string('ctfWithMaterialTracksP5LHCNavigationMapName'),
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
        convElectronMapName = cms.untracked.string('ConversionElectronsMapName'),
        conversionMapName   = cms.untracked.string('ConversionsMapName'),
        fillerType          = cms.untracked.string('FillerConversions')
    ),

    Photons = cms.untracked.PSet(
        active            = cms.untracked.bool(True),
        mitName           = cms.untracked.string('Photons'),
        edmName           = cms.untracked.string('photons'),
        photonIDName      = cms.untracked.string('PhotonIDProd:PhotonAssociatedID'),
        conversionMapName = cms.untracked.string('ConversionsMapName'),
        barrelSuperClusterMapName = cms.untracked.string('barrelSuperClusterMap'),
        endcapSuperClusterMapName = cms.untracked.string('endcapSuperClusterMap'),
        fillerType        = cms.untracked.string('FillerPhotons')
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
        active  = cms.untracked.bool(True),
        mitName = cms.untracked.string('CaloMet'),
        edmName = cms.untracked.string('met'),
        fillerType = cms.untracked.string('FillerCaloMet')
    ),
                                                                           
)
   
