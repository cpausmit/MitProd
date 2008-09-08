# $Id: MitTreeFiller_cfi.py,v 1.6 2008/09/06 18:10:41 sixie Exp $

import FWCore.ParameterSet.Config as cms

MitTreeFiller = cms.EDAnalyzer("FillMitTree",
    defactive = cms.untracked.bool(True),

    MetaInfos = cms.untracked.PSet(
        active = cms.untracked.bool(True),
        hltActive = cms.untracked.bool(False),
        hltName = cms.untracked.string('TriggerResults::HLT')
    ),
                   
    MCParticles = cms.untracked.PSet(
        active        = cms.untracked.bool(True),
        genActive     = cms.untracked.bool(True),
        simActive     = cms.untracked.bool(True),
        trackingActive = cms.untracked.bool(False),
        mitName       = cms.untracked.string('MCParticles'),
        genJetMitName = cms.untracked.string('GenJets'),
        genEdmName    = cms.untracked.string('source'),
        simEdmName    = cms.untracked.string('g4SimHits'),
        trackingEdmName    = cms.untracked.string('mergedtruth:MergedTrackTruth'),
        genMapName    = cms.untracked.string('GenMap'),
        simMapName    = cms.untracked.string('SimMap'),
        trackingMapName    = cms.untracked.string('TrackingMap') 
    ),
                                     
    GeneralTracks  = cms.untracked.PSet(
        active                = cms.untracked.bool(True),
        mitName               = cms.untracked.string('Tracks'),
        edmName               = cms.untracked.string('generalTracks'),
        trackingMapName            = cms.untracked.string('SimMap'),
        trackMapName          = cms.untracked.string('TracksMapName'),
        edmSimAssociationName = cms.untracked.string('')
    ),

    StandaloneMuonTracks = cms.untracked.PSet(
        active                = cms.untracked.bool(True),
        mitName               = cms.untracked.string('StandaloneMuonTracks'),
        edmName               = cms.untracked.string('standAloneMuons'),
        trackingMapName            = cms.untracked.string('SimMap'),
        trackMapName          = cms.untracked.string('StandaloneMuonTracksMapName'),
        edmSimAssociationName = cms.untracked.string('')
    ),

    StandaloneMuonTracksWVtxConstraint = cms.untracked.PSet(
        active                = cms.untracked.bool(True),
        mitName               = cms.untracked.string('StandaloneMuonTracksWVtxConstraint'),
        edmName               = cms.untracked.string('standAloneMuons:UpdatedAtVtx'),
        trackingMapName            = cms.untracked.string('SimMap'),
        trackMapName          = cms.untracked.string('StandaloneMuonTracksWVtxConstraintMapName'),
        edmSimAssociationName = cms.untracked.string('')
    ),
                   
    GlobalMuonTracks = cms.untracked.PSet(
        active                = cms.untracked.bool(True),
        mitName               = cms.untracked.string('GlobalMuonTracks'),
        edmName               = cms.untracked.string('globalMuons'),
        trackingMapName            = cms.untracked.string('SimMap'),
        trackMapName          = cms.untracked.string('GlobalMuonTracksMapName'),
        edmSimAssociationName = cms.untracked.string('')
    ),
                   
    ConversionInOutTracks = cms.untracked.PSet(
        active                = cms.untracked.bool(True),
        mitName               = cms.untracked.string('ConversionInOutTracks'),
        edmName               = cms.untracked.string('ckfInOutTracksFromConversions'),
        trackingMapName            = cms.untracked.string('SimMap'),
        trackMapName          = cms.untracked.string('ConversionInOutTracksMapName'),
        edmSimAssociationName = cms.untracked.string('')
    ),

    ConversionOutInTracks = cms.untracked.PSet(
        active                = cms.untracked.bool(True),
        mitName               = cms.untracked.string('ConversionOutInTracks'),
        edmName               = cms.untracked.string('ckfOutInTracksFromConversions'),
        trackingMapName            = cms.untracked.string('SimMap'),
        trackMapName          = cms.untracked.string('ConversionOutInTracksMapName'),
        edmSimAssociationName = cms.untracked.string('')
    ),
                   
    GsfTracks = cms.untracked.PSet(
        active                = cms.untracked.bool(True),
        mitName               = cms.untracked.string('GsfTracks'),
        edmName               = cms.untracked.string('pixelMatchGsfFit'),
        trackingMapName            = cms.untracked.string('SimMap'),
        trackMapName          = cms.untracked.string('GsfTracksMapName'),
        edmSimAssociationName = cms.untracked.string('')
    ),
                               
    CaloTowers = cms.untracked.PSet(
        active  = cms.untracked.bool(True),
        mitName = cms.untracked.string('CaloTowers'),
        edmName = cms.untracked.string('towerMaker'),    
        caloTowerMapName = cms.untracked.string('CaloTowerMap')
    ),

    BarrelBasicClusters = cms.untracked.PSet(
        active  = cms.untracked.bool(True),
        mitName = cms.untracked.string('BarrelBasicClusters'),
        edmName = cms.untracked.string('hybridSuperClusters:hybridBarrelBasicClusters'),    
        basicClusterMapName = cms.untracked.string('barrelBasicClusterMap')
    ),
                                
    BarrelSuperClusters = cms.untracked.PSet(
        active  = cms.untracked.bool(True),
        mitName = cms.untracked.string('BarrelSuperClusters'),
        edmName = cms.untracked.string('correctedHybridSuperClusters'),
        basicClusterMapName = cms.untracked.string('barrelBasicClusterMap'),
        superClusterMapName = cms.untracked.string('barrelSuperClusterMap')
    ),

    EndcapBasicClusters = cms.untracked.PSet(
        active  = cms.untracked.bool(True),
        mitName = cms.untracked.string('EndcapBasicClusters'),
        edmName = cms.untracked.string('multi5x5BasicClusters:multi5x5EndcapBasicClusters'),  
        basicClusterMapName = cms.untracked.string('endcapBasicClusterMap')
    ),

    EndcapSuperClusters = cms.untracked.PSet(
        active  = cms.untracked.bool(True),
        mitName = cms.untracked.string('EndcapSuperClusters'),                     
        edmName = cms.untracked.string('correctedMulti5x5SuperClustersWithPreshower'),
        basicClusterMapName = cms.untracked.string('endcapBasicClusterMap'),
        superClusterMapName = cms.untracked.string('endcapSuperClusterMap')
    ),

    Muons = cms.untracked.PSet(
        active  = cms.untracked.bool(True),
        mitName = cms.untracked.string('Muons'),
        edmName = cms.untracked.string('muons'),
        globalTrackMapName  = cms.untracked.string('GlobalMuonTracksMapName'),
        staTrackMapName     = cms.untracked.string('StandaloneMuonTracksMapName'),
        staVtxTrackMapName  = cms.untracked.string('StandaloneMuonTracksWVtxConstraintMapName'),
        trackerTrackMapName = cms.untracked.string('TracksMapName')
    ),
                   
    Electrons = cms.untracked.PSet(
        active                 = cms.untracked.bool(True),
        mitName                = cms.untracked.string('Electrons'),
        edmName                = cms.untracked.string('pixelMatchGsfElectrons'),
        gsfTrackMapName        = cms.untracked.string('GsfTracksMapName'), 
        trackerTrackMapName    = cms.untracked.string('TracksMapName'),
        barrelEcalRecHitName   = cms.untracked.string('reducedEcalRecHitsEB'),
        endcapEcalRecHitName   = cms.untracked.string('reducedEcalRecHitsEE'),
        barrelBasicClusterName = cms.untracked.string(
                                     'hybridSuperClusters:hybridBarrelBasicClusters'),
        endcapBasicClusterName = cms.untracked.string(
                                     'multi5x5BasicClusters:multi5x5EndcapBasicClusters'),
        barrelSuperClusterName = cms.untracked.string('correctedHybridSuperClusters'),
        endcapSuperClusterName = cms.untracked.string(
                                     'multi5x5SuperClusters:multi5x5EndcapSuperClusters'),
        barrelSuperClusterMapName = cms.untracked.string('barrelSuperClusterMap'),
        endcapSuperClusterMapName = cms.untracked.string('endcapSuperClusterMap'),
        eIDCutBasedLooseName      = cms.untracked.string('eidLoose'),
        eIDCutBasedTightName      = cms.untracked.string('eidTight'),
        eIDLikelihood             = cms.untracked.string('eidLikelihoodExt'),
        eIDNeuralNet              = cms.untracked.string('eidNeuralNet')
    ),

    Taus = cms.untracked.PSet(
        active              = cms.untracked.bool(True),
        mitName             = cms.untracked.string('Taus'),
        edmName             = cms.untracked.string('caloRecoTauProducer'),
        gsfTrackMapName     = cms.untracked.string('GsfTracksMapName'), 
        trackerTrackMapName = cms.untracked.string('TracksMapName'),
        barrelSuperClusterMapName = cms.untracked.string('barrelSuperClusterMap'),
        endcapSuperClusterMapName = cms.untracked.string('endcapSuperClusterMap')
    ),

    ConversionElectrons = cms.untracked.PSet(
        active                = cms.untracked.bool(True),
        mitName               = cms.untracked.string('ConversionElectrons'),
        convInOutTracksName   = cms.untracked.string('ConversionInOutTracks'),
        convOutInTracksName   = cms.untracked.string('ConversionOutInTracks'),
        convInOutTrackMapName = cms.untracked.string('ConversionInOutTracksMapName'),
        convOutInTrackMapName = cms.untracked.string('ConversionOutInTracksMapName'),
        convElectronMapName   = cms.untracked.string('ConversionElectronsMapName')
    ),
                   
    Conversions = cms.untracked.PSet(
        active              = cms.untracked.bool(True),
        mitName             = cms.untracked.string('Conversions'),
        edmName             = cms.untracked.string('conversions'),
        convElectronMapName = cms.untracked.string('ConversionElectronsMapName'),
        conversionMapName   = cms.untracked.string('ConversionsMapName') 
    ),

    Photons = cms.untracked.PSet(
        active            = cms.untracked.bool(True),
        mitName           = cms.untracked.string('Photons'),
        edmName           = cms.untracked.string('photons'),
        conversionMapName = cms.untracked.string('ConversionsMapName')
    ),

    CaloJets = cms.untracked.PSet(
        active  = cms.untracked.bool(True),
        flavorMatchingActive = cms.untracked.bool(True),
        mitName = cms.untracked.string('CaloJets'),
        edmName = cms.untracked.string('iterativeCone5CaloJets'),
        flavorMatchingByReferenceName = cms.untracked.string('IC5byRef'),
        flavorMatchingDefinition = cms.untracked.string('Algorithmic'),
        caloTowerMapName = cms.untracked.string('CaloTowerMap')
    ),

    SisCone5Jets = cms.untracked.PSet(
        active  = cms.untracked.bool(True),
        flavorMatchingActive = cms.untracked.bool(True),
        mitName = cms.untracked.string('SisCone5Jets'),
        edmName = cms.untracked.string('sisCone5CaloJets'),
        flavorMatchingByReferenceName = cms.untracked.string('SC5byRef'),
        flavorMatchingDefinition = cms.untracked.string('Algorithmic'),
        caloTowerMapName = cms.untracked.string('CaloTowerMap')
    ),                          

    CaloMet = cms.untracked.PSet(
        active  = cms.untracked.bool(True),
        mitName = cms.untracked.string('CaloMet'),
        edmName = cms.untracked.string('met')  
    ),
                  
    StableParts = cms.untracked.PSet(
        active       = cms.untracked.bool(False),
        mitName      = cms.untracked.string('MuonsStable'),
        edmName      = cms.untracked.string('MuonsStable'),
        trackMapName = cms.untracked.string('TracksMapName'),
        basePartMap  = cms.untracked.string('MuonsStableMapName')
    ),

    DecayParts = cms.untracked.PSet(
        active       = cms.untracked.bool(False),
        mitName      = cms.untracked.string('psi2MuMu'),
        edmName      = cms.untracked.string('Jpsi2MuMu'),
        basePartMap  = cms.untracked.string('MuonsStableMapName')
    )                                                            
)
   
