# $Id: BAMBUProd_RECODEBUG.py,v 1.4 2009/07/22 19:26:24 loizides Exp $

import FWCore.ParameterSet.Config as cms

process = cms.Process('FILLER')

# import of standard configurations
process.load('Configuration/StandardSequences/Services_cff')
process.load('FWCore/MessageService/MessageLogger_cfi')
process.load('Configuration/StandardSequences/GeometryIdeal_cff')
process.load('Configuration/StandardSequences/MagneticField_38T_cff')
process.load('Configuration/StandardSequences/EndOfProcess_cff')
process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')
process.load('Configuration/EventContent/EventContent_cff')

process.configurationMetadata = cms.untracked.PSet(
    version = cms.untracked.string('Mit_011'),
    annotation = cms.untracked.string('RECODEBUG'),
    name = cms.untracked.string('BambuProduction')
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

process.options = cms.untracked.PSet(
   Rethrow = cms.untracked.vstring('ProductNotFound')
)

# input source
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring('file:/build/bendavid/RECODEBUG/WW_MC_31X_V2_preproduction_311_GEN-SIM-RECO-v1/C678170D-146C-DE11-A610-001C23C0F1F4.root')
)

# other statements
process.GlobalTag.globaltag = 'MC_31X_V3::All'

# load MitTreeFiller 
process.TreeService = cms.Service("TreeService",
    fileNames = cms.untracked.vstring('XX-MITDATASET-XX'),
)
process.add_(cms.Service("ObjectService"))

process.load("MitProd.BAMBUSequences.BambuFillRECODEBUG_cfi")

process.bambu_step  = cms.Path(process.BambuFillRECODEBUG)
process.endjob_step = cms.Path(process.endOfProcess)
#process.out_step = cms.EndPath(process.output)

# schedule definition
process.schedule = cms.Schedule(process.bambu_step,process.endjob_step)
