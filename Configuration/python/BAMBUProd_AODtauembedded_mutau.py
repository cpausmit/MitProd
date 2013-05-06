# $Id: BAMBUProd_AODtauembedded.py,v 1.7 2012/12/28 17:36:20 pharris Exp $

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
                            fileNames = cms.untracked.vstring('file:1AB39CBC-B7B0-E211-BDE0-00266CF3DFE0.root')
                            #fileNames = cms.untracked.vstring('/store/results/higgs/DoubleMu/StoreResults-DoubleMu_2012A_PromptReco_v1_embedded_trans1_tau115_ptelec1_17had1_17_v1-f456bdbb960236e5c696adfe9b04eaae/DoubleMu/USER/StoreResults-DoubleMu_2012A_PromptReco_v1_embedded_trans1_tau115_ptelec1_17had1_17_v1-f456bdbb960236e5c696adfe9b04eaae/0000/EE75CD02-539F-E111-85D5-0023AEFDE490.root')
                            #fileNames = cms.untracked.vstring('/store/results/higgs/DoubleMu/StoreResults-DoubleMu_2012B_PromptReco_v1_Run193752to195135_embedded_trans1_tau115_ptelec1_17had1_17_v2-f456bdbb960236e5c696adfe9b04eaae/DoubleMu/USER/StoreResults-DoubleMu_2012B_PromptReco_v1_Run193752to195135_embedded_trans1_tau115_ptelec1_17had1_17_v2-f456bdbb960236e5c696adfe9b04eaae/0000/FC0EB0FE-FAAD-E111-BC4E-001EC9DB2BFA.root')
#                            fileNames = cms.untracked.vstring('/store/results/higgs/DoubleMu/StoreResults-DoubleMu_2012A_PromptReco_v1_embedded_trans1_tau115_ptelec1_17had1_17_v2-f456bdbb960236e5c696adfe9b04eaae//DoubleMu/USER/StoreResults-DoubleMu_2012A_PromptReco_v1_embedded_trans1_tau115_ptelec1_17had1_17_v2-f456bdbb960236e5c696adfe9b04eaae/0000/E4004575-B2AC-E111-9B88-001E4F3DED7B.root')
)
#process.source.inputCommands = cms.untracked.vstring(#"keep *",
#                                                     "drop *_MEtoEDMConverter_*_*",
#                                                     "drop L1GlobalTriggerObjectMapRecord_hltL1GtObjectMap__HLT")

# other statements
process.GlobalTag.globaltag = 'START53_V15::All'


process.add_(cms.Service("ObjectService"))

process.load("MitProd.BAMBUSequences.BambuFillAOD_cfi")

process.MitTreeFiller.TreeWriter.fileName = 'XX-MITDATASET-XX'

process.load('TauAnalysis/MCEmbeddingTools/embeddingKineReweight_cff')
process.embeddingKineReweightRECembedding.inputFileName = cms.FileInPath('TauAnalysis/MCEmbeddingTools/data/embeddingKineReweight_recEmbedding_mutau.root')
# process.embeddingKineReweightRECembedding.inputFileName = cms.FileInPath('TauAnalysis/MCEmbeddingTools/data/embeddingKineReweight_recEmbedding_etau.root')
# process.embeddingKineReweightRECembedding.inputFileName = cms.FileInPath('TauAnalysis/MCEmbeddingTools/data/embeddingKineReweight_recEmbedding_emu.root')

from MitProd.TreeFiller.filltauembedded_cff import *
filltauembedded(process.MitTreeFiller)
process.MitTreeFiller.MergedConversions.checkTrackRef           = cms.untracked.bool(False)
process.MitTreeFiller.EmbedWeight.useGenInfo = True
#process.newJetTracksAssociatorAtVertex.tracks = "tmfTracks"
process.bambu_step  = cms.Path( process.embeddingKineReweightSequenceRECembedding*process.embeddingKineReweightSequenceGENembedding*process.BambuFillAOD)

process.output = cms.OutputModule("PoolOutputModule",
                                  outputCommands = cms.untracked.vstring('keep *'),
                                  fileName       = cms.untracked.string ("test.root")
                                  )
# schedule definition
process.outpath  = cms.EndPath(process.output)
process.schedule = cms.Schedule(process.bambu_step)
