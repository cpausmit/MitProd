# $Id:#

import FWCore.ParameterSet.Config as cms

process = cms.Process('FILLER')

# import of standard configurations
process.load('Configuration/StandardSequences/Services_cff')
process.load('FWCore/MessageService/MessageLogger_cfi')
process.load('Configuration/StandardSequences/GeometryIdeal_cff')
process.load('Configuration/StandardSequences/MagneticField_38T_cff')
process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')
process.load('Configuration/EventContent/EventContent_cff')

process.configurationMetadata = cms.untracked.PSet(
    version = cms.untracked.string('Mit_013'),
    annotation = cms.untracked.string('RECOSIM'),
    name = cms.untracked.string('BambuProduction')
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)
process.options = cms.untracked.PSet(
   Rethrow = cms.untracked.vstring('ProductNotFound'),
   fileMode =  cms.untracked.string('NOMERGE'),
)

# input source
process.source = cms.Source("PoolSource",
    #fileNames = cms.untracked.vstring('/store/relval/CMSSW_3_5_2/RelValTTbar/GEN-SIM-RECO/MC_3XY_V21-v1/0016/62FDDFEB-391E-DF11-890A-001731A28BE1.root')
    fileNames = cms.untracked.vstring('file:/build/bendavid/RECOSIMSummer09/TTbar_Summer09-MC_3XY_V25_preproduction-v1/969AE5D9-DA2B-DF11-931D-001CC4C10E02.root')
)
process.source.inputCommands = cms.untracked.vstring("keep *", "drop *_MEtoEDMConverter_*_*", "drop L1GlobalTriggerObjectMapRecord_hltL1GtObjectMap__HLT")

# other statements
process.GlobalTag.globaltag = 'MC_3XY_V25::All'

process.add_(cms.Service("ObjectService"))

process.load("MitProd.BAMBUSequences.BambuFillRECOSIM_cfi")

process.MitTreeFiller.TreeWriter.fileName = 'XX-MITDATASET-XX'

process.bambu_step  = cms.Path(process.BambuFillRECOSIM)

# schedule definition
process.schedule = cms.Schedule(process.bambu_step)
