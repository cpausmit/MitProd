# $Id: BAMBUProd_RECO.py,v 1.5 2009/08/14 13:10:00 bendavid Exp $

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
    annotation = cms.untracked.string('RECO'),
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
    fileNames = cms.untracked.vstring('file:/build/bendavid/AOD/TTbar_Summer09-MC_31X_V3_AODSIM-v1/C0258D28-A688-DE11-929F-001CC4BF6D0C.root')
)

# other statements
process.GlobalTag.globaltag = 'MC_31X_V3::All'

# load MitTreeFiller 
process.TreeService = cms.Service("TreeService",
    fileNames = cms.untracked.vstring('XX-MITDATASET-XX'),
)
process.add_(cms.Service("ObjectService"))

process.load("MitProd.BAMBUSequences.BambuFillAODSIM_cfi")

process.bambu_step  = cms.Path(process.BambuFillAODSIM)
process.endjob_step = cms.Path(process.endOfProcess)
#process.out_step = cms.EndPath(process.output)

# schedule definition
process.schedule = cms.Schedule(process.bambu_step,process.endjob_step)
