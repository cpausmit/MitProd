# $Id:#

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
  version    = cms.untracked.string('Mit_029'),
  annotation = cms.untracked.string('RECOSIMStartup'),
  name       = cms.untracked.string('BambuProduction')
)

process.maxEvents = cms.untracked.PSet(
  input = cms.untracked.int32(-1)
)
process.options = cms.untracked.PSet(
  Rethrow  = cms.untracked.vstring('ProductNotFound'),
  fileMode =  cms.untracked.string('NOMERGE'),
)

# input source
process.source = cms.Source("PoolSource",
  fileNames = cms.untracked.vstring('/store/relval/CMSSW_4_4_0/RelValProdTTbar/AODSIM/START44_V5-v2/0046/74C825F5-ACE6-E011-8C0D-00261894391F.root'),
)
process.source.inputCommands = cms.untracked.vstring("keep *",
                                                     "drop *_MEtoEDMConverter_*_*",
                                                     "drop L1GlobalTriggerObjectMapRecord_hltL1GtObjectMap__HLT")

# other statements
#special global tag for 900GeV startup beamspot conditions and startup (CRAFT-knowledge) alignment scenario
process.GlobalTag.globaltag = 'START53_V10::All'

process.add_(cms.Service("ObjectService"))

process.load("MitProd.BAMBUSequences.BambuFillRECOSIM_cfi")

process.MitTreeFiller.TreeWriter.fileName = 'XX-MITDATASET-XX'

process.bambu_step  = cms.Path(process.BambuFillRECOSIM)

# schedule definition
process.schedule = cms.Schedule(process.bambu_step)
