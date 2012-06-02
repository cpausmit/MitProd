# $Id: BAMBUProd_AODtauembedded.py,v 1.4 2012/05/12 15:55:10 paus Exp $

import FWCore.ParameterSet.Config as cms

process = cms.Process('FILEFI')

# import of standard configurations
process.load('Configuration/StandardSequences/Services_cff')
process.load('FWCore/MessageService/MessageLogger_cfi')
process.load('Configuration.StandardSequences.GeometryDB_cff')
process.load('Configuration/StandardSequences/MagneticField_38T_cff')
process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')
process.load('Configuration/EventContent/EventContent_cff')

process.configurationMetadata = cms.untracked.PSet(
  version    = cms.untracked.string('Mit_028'),
  annotation = cms.untracked.string('AOD'),
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
    fileNames = cms.untracked.vstring('file:/tmp/pharris/FED442F4-549F-E111-ABD5-00221988F96C.root')
)
#process.source.inputCommands = cms.untracked.vstring(#"keep *",
#                                                     "drop *_MEtoEDMConverter_*_*",
#                                                     "drop L1GlobalTriggerObjectMapRecord_hltL1GtObjectMap__HLT")

# other statements
process.GlobalTag.globaltag = 'START52_V9::All'

process.add_(cms.Service("ObjectService"))

process.load("MitProd.BAMBUSequences.BambuFillAOD_cfi")

process.MitTreeFiller.TreeWriter.fileName = 'XX-MITDATASET-XX'


from MitProd.TreeFiller.filltauembedded_cff import *
filltauembedded(process.MitTreeFiller)

process.MitTreeFiller.Electrons.requireClusterAndGsfMap          = cms.untracked.bool(False)

process.bambu_step  = cms.Path(process.BambuFillAOD)

# schedule definition
process.schedule = cms.Schedule(process.bambu_step)
