# $Id: BAMBUProd_RECO.py,v 1.6 2009/10/19 18:45:42 bendavid Exp $

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
    version = cms.untracked.string('Mit_011a'),
    annotation = cms.untracked.string('RECO'),
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
    fileNames = cms.untracked.vstring('file:/build/bendavid/RECOSIM/RelValProdTTbar_CMSSW_3_3_1-MC_31X_V9-v3_GEN-SIM-RECO/DAEBA2B7-3EC0-DE11-890D-000423D98BE8.root')
)
process.source.inputCommands = cms.untracked.vstring("keep *","drop *_MEtoEDMConverter_*_*")

# other statements
process.GlobalTag.globaltag = 'CRAFT09_R_V3::All'

process.add_(cms.Service("ObjectService"))

process.load("MitProd.BAMBUSequences.BambuFillRECO_cfi")

process.bambu_step  = cms.Path(process.BambuFillRECO)

# schedule definition
process.schedule = cms.Schedule(process.bambu_step)
