# $Id: BAMBUProd_AODSIM42tauembedded.py,v 1.1 2011/10/22 15:07:33 bendavid Exp $

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
    annotation = cms.untracked.string('AODSIM'),
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
  fileNames = cms.untracked.vstring('rfio:/castor/cern.ch/user/z/zeise/embedding/4_2_4p1/pfembTauTau_data_SingleMu_2011A_Aug05thRR_v1_1_116_pt0_2680_embedded.root'),
)
process.source.inputCommands = cms.untracked.vstring("keep *", "drop *_MEtoEDMConverter_*_*", "drop L1GlobalTriggerObjectMapRecord_hltL1GtObjectMap__HLT")

# other statements
process.GlobalTag.globaltag = 'START44_V6::All'

process.add_(cms.Service("ObjectService"))

process.load("MitProd.BAMBUSequences42.BambuFillAODSIM_cfi")

process.MitTreeFiller.TreeWriter.fileName = 'XX-MITDATASET-XX'

from MitProd.TreeFiller.filltauembedded_cff import *
filltauembedded(process.MitTreeFiller)    
process.newJetTracksAssociatorAtVertex.tracks = "tmfTracks"

process.bambu_step  = cms.Path(process.BambuFillAODSIM)

# schedule definition
process.schedule = cms.Schedule(process.bambu_step)
