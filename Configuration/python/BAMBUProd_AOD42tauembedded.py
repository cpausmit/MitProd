# $Id: BAMBUProd_AOD42tauembedded.py,v 1.6 2012/05/12 15:55:10 paus Exp $

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
  fileNames = cms.untracked.vstring('file:/tmp/0008FF93-5F0E-E111-ACD9-002219889165.root'),
)
process.source.inputCommands = cms.untracked.vstring("keep *",
                                                     "drop *_MEtoEDMConverter_*_*",
                                                     "drop L1GlobalTriggerObjectMapRecord_hltL1GtObjectMap__HLT")

# other statements
process.GlobalTag.globaltag = 'GR_R_44_V6::All'

process.add_(cms.Service("ObjectService"))

process.load("MitProd.BAMBUSequences42.BambuFillAOD_embed_cfi")

process.MitTreeFiller.TreeWriter.fileName = 'XX-MITDATASET-XX'

from MitProd.TreeFiller.filltauembedded_cff import *
filltauembedded(process.MitTreeFiller)
process.newJetTracksAssociatorAtVertex.tracks = "tmfTracks"

process.bambu_step  = cms.Path(process.BambuFillAOD)

# schedule definition
process.schedule = cms.Schedule(process.bambu_step)
