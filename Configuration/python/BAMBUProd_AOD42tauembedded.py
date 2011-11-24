# $Id: BAMBUProd_AOD42tauembedded.py,v 1.3 2011/11/08 16:41:31 pharris Exp $

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
    annotation = cms.untracked.string('AOD'),
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
    fileNames = cms.untracked.vstring('file:/tmp/0008FF93-5F0E-E111-ACD9-002219889165.root'),

    #/store/results/higgs/DoubleMu/StoreResults-DoubleMu_2011B_PR_v1_embedded_trans1_tau115_ptelec1_17had1_17_v2-f456bdbb960236e5c696adfe9b04eaae/DoubleMu/USER/StoreResults-DoubleMu_2011B_PR_v1_embedded_trans1_tau115_ptelec1_17had1_17_v2-f456bdbb960236e5c696adfe9b04eaae/0000/FC7992FD-580E-E111-B872-0023AEFDEBEC.root')
                            #/store/results/higgs/DoubleMu/StoreResults-DoubleMu_2011B_PR_v1_embedded_trans1_tau123_pttau1_18tau2_8_v2-f456bdbb960236e5c696adfe9b04eaae/DoubleMu/USER/StoreResults-DoubleMu_2011B_PR_v1_embedded_trans1_tau123_pttau1_18tau2_8_v2-f456bdbb960236e5c696adfe9b04eaae/0000/32EA7551-6C0E-E111-AAD9-001EC9F8FCF0.root'),
    #'file:/tmp/0008FF93-5F0E-E111-ACD9-002219889165.root'),                        
    #skipEvents = cms.untracked.uint32()                         
                            #fileNames = cms.untracked.vstring('/store/data/Run2011A/Interfill/AOD/PromptReco-v3/000/164/393/EA56AD49-A07C-E011-B1A5-003048F1C836.root')
    #fileNames = cms.untracked.vstring('rfio:/castor/cern.ch/user/z/zeise/embedding/4_2_4p1/pfembTauTau_data_SingleMu_2011A_Aug05thRR_v1_1_116_pt0_2680_embedded.root'),
    #skipEvents=cms.untracked.uint32(30888),
                            )
process.source.inputCommands = cms.untracked.vstring("keep *", "drop *_MEtoEDMConverter_*_*", "drop L1GlobalTriggerObjectMapRecord_hltL1GtObjectMap__HLT")

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
