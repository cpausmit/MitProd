import FWCore.ParameterSet.Config as cms

from MitProd.TreeFiller.MitTreeFiller_cfi import *

MitTreeFiller.fillers = [
#    'MetaInfos',
    'MCParticles',
    'MCEventInfo',
    'PileupInfo',
    'PileupEnergyDensity',
    'EvtSelData',
    'BeamSpot',
    'BasicClusters',
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
MitTreeFiller.MCParticles.genSource = 'PackedGenParticleCollection'
MitTreeFiller.SuperClusters.edmName = 'reducedEgamma'
MitTreeFiller.PrimaryVertexes.edmName = 'offlineSlimmedPrimaryVertices'
MitTreeFiller.Muons = cms.untracked.PSet(
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
MitTreeFiller.Electrons = cms.untracked.PSet(
    active                           = cms.untracked.bool(True),
    mitName                          = cms.untracked.string('Electrons'),
    edmName                          = cms.untracked.string('slimmedElectrons'),
    generalTracksName                = cms.untracked.string(''),
    gsfTracksName                    = cms.untracked.string(''),
    conversionsName                  = cms.untracked.string('reducedConversions'),
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
MitTreeFiller.Conversions.edmName = 'reducedEgamma:reducedConversions'
MitTreeFiller.Conversions.stablePartMaps = []
MitTreeFiller.PFCandidates = cms.untracked.PSet(
    active = cms.untracked.bool(True),
    mitName = cms.untracked.string('PFCandidates'),
    edmName = cms.untracked.string('packedPFCandidates'),
    pfCandMapName = cms.untracked.string('PFCandMapName'),
    pfNoPileupCandMapName = cms.untracked.string('PFNoPileupCandMapName'),
    fillPfNoPileup = cms.untracked.bool(True),
    fillerType = cms.untracked.string('FillerPackedPFCandidates')
)
MitTreeFiller.Photons = cms.untracked.PSet(
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
MitTreeFiller.AKt4PFJets.fillerType = 'FillerPFJetsFromPATJets'
MitTreeFiller.AKt8PFJets.fillerType = 'FillerPFJetsFromPATJets'
MitTreeFiller.PFMet.fillerType = 'FillerPFMetFromPATMET'
MitTreeFiller.HPSTaus.edmName = 'slimmedTaus'
MitTreeFiller.HPSTaus.fillerType = 'FillerPFTausFromPATTaus'
MitTreeFiller.DCASig.edmElectronName = 'slimmedElectrons'
MitTreeFiller.DCASig.edmMuonName = 'slimmedMuons'
MitTreeFiller.DCASig.edmTauName = 'slimmedTaus'
