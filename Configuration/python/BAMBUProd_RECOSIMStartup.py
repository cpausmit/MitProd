# $Id:#

import FWCore.ParameterSet.Config as cms

process = cms.Process('FILLER')

# import of standard configurations
process.load('Configuration/StandardSequences/Services_cff')
process.load('FWCore/MessageService/MessageLogger_cfi')
process.load('Configuration.StandardSequences.GeometryDB_cff')
process.load('Configuration/StandardSequences/MagneticField_38T_cff')
process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')
process.load('Configuration/EventContent/EventContent_cff')

process.configurationMetadata = cms.untracked.PSet(
    version = cms.untracked.string('Mit_017'),
    annotation = cms.untracked.string('RECOSIMStartup'),
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
  fileNames = cms.untracked.vstring('/store/relval/CMSSW_4_2_3/RelValProdTTbar/GEN-SIM-RECO/MC_42_V12-v2/0062/82A88DE4-FB7A-E011-B69E-001A928116CC.root')
#    fileNames = cms.untracked.vstring('/store/relval/CMSSW_3_11_1/RelValZEE/GEN-SIM-RECO/START311_V1_64bit-v1/0089/6E2A6DB2-EA34-E011-B663-0018F34D0D62.root')
)
process.source.inputCommands = cms.untracked.vstring("keep *", "drop *_MEtoEDMConverter_*_*", "drop L1GlobalTriggerObjectMapRecord_hltL1GtObjectMap__HLT")

# other statements

#special global tag for 900GeV startup beamspot conditions and startup (CRAFT-knowledge) alignment scenario
process.GlobalTag.globaltag = 'START42_V12::All'

process.add_(cms.Service("ObjectService"))

process.load("MitProd.BAMBUSequences.BambuFillRECOSIM_cfi")

process.MitTreeFiller.TreeWriter.fileName = 'XX-MITDATASET-XX'

process.bambu_step  = cms.Path(process.BambuFillRECOSIM)

# schedule definition
process.schedule = cms.Schedule(process.bambu_step)
