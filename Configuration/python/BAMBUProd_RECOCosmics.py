# $Id: BAMBUProd_RECO.py,v 1.5 2009/08/14 13:10:00 bendavid Exp $

import FWCore.ParameterSet.Config as cms

process = cms.Process('FILLER')

# import of standard configurations
process.load('Configuration/StandardSequences/Services_cff')
process.load('FWCore/MessageService/MessageLogger_cfi')
process.load('Configuration/StandardSequences/GeometryIdeal_cff')
process.load('Configuration/StandardSequences/MagneticField_AutoFromDBCurrent_cff')
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
    fileNames = cms.untracked.vstring('/store/data/CRAFT09/Cosmics/RECO/v1/000/110/972/EA5AF112-A18A-DE11-9BC8-000423D9997E.root')
)

# other statements
process.GlobalTag.globaltag = 'GR09_P_V2::All'

# load MitTreeFiller 
process.TreeService = cms.Service("TreeService",
    fileNames = cms.untracked.vstring('XX-MITDATASET-XX'),
)
process.add_(cms.Service("ObjectService"))

process.load("MitProd.BAMBUSequences.BambuFillRECOCosmics_cfi")

process.bambu_step  = cms.Path(process.BambuFillRECOCosmics)
process.endjob_step = cms.Path(process.endOfProcess)
#process.out_step = cms.EndPath(process.output)

# schedule definition
process.schedule = cms.Schedule(process.bambu_step,process.endjob_step)
