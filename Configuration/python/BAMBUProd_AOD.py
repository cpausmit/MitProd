# $Id: BAMBUProd_AOD.py,v 1.5 2009/11/05 21:38:09 bendavid Exp $

import FWCore.ParameterSet.Config as cms

process = cms.Process('FILLER')

# import of standard configurations
process.load('Configuration/StandardSequences/Services_cff')
process.load('FWCore/MessageService/MessageLogger_cfi')
process.load('Configuration/StandardSequences/GeometryIdeal_cff')
process.load('Configuration/StandardSequences/MagneticField_AutoFromDBCurrent_cff')
process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')
process.load('Configuration/EventContent/EventContent_cff')

process.configurationMetadata = cms.untracked.PSet(
    version = cms.untracked.string('Mit_012'),
    annotation = cms.untracked.string('AOD'),
    name = cms.untracked.string('BambuProduction')
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
    fileNames = cms.untracked.vstring('file:/build/bendavid/AOD/BeamCommissioning09ComsicsCollisionReReco/promptReco_RAW2DIGI_L1Reco_RECO_DQM_ALCA.root')
)
process.source.inputCommands = cms.untracked.vstring("keep *","drop *_MEtoEDMConverter_*_*")

# other statements
process.GlobalTag.globaltag = 'GR09_P_V5::All'

process.add_(cms.Service("ObjectService"))

process.load("MitProd.BAMBUSequences.BambuFillAOD_cfi")

process.bambu_step  = cms.Path(process.BambuFillAOD)

# schedule definition
process.schedule = cms.Schedule(process.bambu_step)
