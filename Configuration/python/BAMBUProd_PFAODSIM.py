
# $Id: BAMBUProd_AODSIM.py,v 1.38 2012/05/12 15:55:10 paus Exp $

import FWCore.ParameterSet.Config as cms

process = cms.Process('FILEFI')

# import of standard configurations
process.load('Configuration/StandardSequences/Services_cff')
process.load('FWCore/MessageService/MessageLogger_cfi')
process.load('Configuration.StandardSequences.GeometryDB_cff')
process.load('Configuration/StandardSequences/MagneticField_38T_cff')
process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')
process.load('Configuration/EventContent/EventContent_cff')
process.load('RecoVertex/PrimaryVertexProducer/OfflinePrimaryVertices_cfi')
 
process.configurationMetadata = cms.untracked.PSet(
  version    = cms.untracked.string('Mit_029'),
  annotation = cms.untracked.string('AODSIM'),
  name       = cms.untracked.string('BambuProduction')
)

process.maxEvents = cms.untracked.PSet(
  input = cms.untracked.int32(-1)
)

process.options = cms.untracked.PSet(
  Rethrow = cms.untracked.vstring('ProductNotFound'),
  fileMode = cms.untracked.string('NOMERGE'),
)

# input source
process.source = cms.Source("PoolSource",
  fileNames = cms.untracked.vstring('/store/cmst3/group/cmgtools/CMG/GluGluToHToTauTau_M-125_8TeV-powheg-pythia6/Summer12_DR53X-PU_S10_START53_V7A-v1/AODSIM/V5_B/PFAOD_95_1_UfF.root')
)

process.source.inputCommands = cms.untracked.vstring("keep *",
                                                     "drop *_MEtoEDMConverter_*_*",
                                                     "drop L1GlobalTriggerObjectMapRecord_hltL1GtObjectMap__HLT")

# other statements
process.GlobalTag.globaltag = 'START53_V15::All'

process.add_(cms.Service("ObjectService"))

process.load("MitProd.BAMBUSequences.BambuFillAODSIM_cfi")

process.MitTreeFiller.TreeWriter.fileName = 'XX-MITDATASET-XX'
process.MitTreeFiller.ElectronsStable.active                    = True
process.MitTreeFiller.GsfElectronsStable.active                 = True
process.MitTreeFiller.ConversionStepElectronsStable.active      = True


process.MitTreeFiller.Conversions.active                        = False
process.MitTreeFiller.ConversionInOutTracks.active              = False
process.MitTreeFiller.ConversionInOutElectronsStable.active     = False
process.MitTreeFiller.ConversionOutInTracks.active              = False
process.MitTreeFiller.ConversionOutInElectronsStable.active     = False
process.MitTreeFiller.MergedConversions.active                  = True
process.MitTreeFiller.MergedConversions.checkTrackRef           = cms.untracked.bool(False)
process.MitTreeFiller.MergedConversions.stablePartMaps          = cms.untracked.vstring('ElectronsStableTrackMapName',
                                                                                        'ElectronsStableGsfTrackMapName',
                                                                                        'ElectronsStableConvStepTrackMapName'
                                                                                        )
process.MitTreeFiller.PFPhotonSuperClusters.active              = False
process.MitTreeFiller.PFPhotonConversions.active                = False
process.MitTreeFiller.PFPhotons.active                          = False

process.MitTreeFiller.Kt4Jets.active                            = False
process.MitTreeFiller.Kt6Jets.active                            = False
process.MitTreeFiller.AKt5Jets.active                           = False
process.MitTreeFiller.AKt7Jets.active                           = False
process.MitTreeFiller.AKt5TrackJets.active                      = False
process.MitTreeFiller.Kt4PFJets.active                          = False
process.MitTreeFiller.Kt6PFJets.active                          = False
process.MitTreeFiller.AKt7PFJets.active                         = False
process.MitTreeFiller.TCMet.active                              = False
process.MitTreeFiller.CaloTaus.active                           = False
process.MitTreeFiller.ShrinkingConePFTaus.active                = False
process.MitTreeFiller.PFTaus.active                             = False
process.MitTreeFiller.HPSTaus.trackMapNames                     = cms.untracked.vstring('')
process.MitTreeFiller.HPSTaus.allowMissingTrackRef              = cms.untracked.bool(True)

process.MitTreeFiller.BarrelSuperClusters.caloTowerName = ''
process.MitTreeFiller.EndcapSuperClusters.caloTowerName = ''
process.MitTreeFiller.PFSuperClusters.caloTowerName = cms.untracked.string('')
process.MitTreeFiller.DCASig.active                             = False

process.output = cms.OutputModule("PoolOutputModule",
                                  outputCommands = cms.untracked.vstring('keep *'),
                                  fileName       = cms.untracked.string ("test.root")
)
process.bambu_step  = cms.Path(process.BambuFillAODSIM)
# schedule definition
process.schedule = cms.Schedule(process.bambu_step)
process.outpath  = cms.EndPath(process.output)
