# $Id: BAMBUProd_AODSIM.py,v 1.34 2012/03/11 23:16:02 pharris Exp $

import FWCore.ParameterSet.Config as cms

process = cms.Process('FILEFI')

# import of standard configurations
process.load('Configuration/StandardSequences/Services_cff')
process.load('FWCore/MessageService/MessageLogger_cfi')
process.load('Configuration.StandardSequences.GeometryDB_cff')
process.load('Configuration/StandardSequences/MagneticField_38T_cff')
process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')
process.load('Configuration/EventContent/EventContent_cff')
process.load('RecoVertex/PrimaryVertexProducer/OfflinePrimaryVertices_cfi')
 
process.configurationMetadata = cms.untracked.PSet(
    version    = cms.untracked.string('Mit_026'),
    annotation = cms.untracked.string('AODSIM'),
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
  #fileNames = cms.untracked.vstring('/store/relval/CMSSW_4_2_3/RelValZTT/GEN-SIM-RECO/START42_V12-v2/0062/A0DB0D50-1E7B-E011-AE93-003048678B0E.root')
  # fileNames = cms.untracked.vstring('/store/relval/CMSSW_4_2_3/RelValProdTTbar/AODSIM/MC_42_V12-v2/0067/8A939AD6-DB7B-E011-89F6-0018F3D09682.root')
   #fileNames = cms.untracked.vstring('file:/data/blue/bendavid/423aodsim/884FBED5-AC91-E011-B64A-90E6BA442EEB.root'),
  #fileNames = cms.untracked.vstring('file:FED1A710-ED33-E111-B889-003048FFCC1E.root')
                            #eos/cms/store/mc/Fall11/WJetsToLNu_TuneZ2_7TeV-madgraph-tauola/AODSIM/PU_Chamonix12_START44_V10-v2/0000/EABA4F3E-0134-E111-AFA4-0030486792DE.root')
   #fileNames = cms.untracked.vstring('/store/relval/CMSSW_4_4_0/RelValProdTTbar/AODSIM/START44_V5-v2/0046/74C825F5-ACE6-E011-8C0D-00261894391F.root'),
   #fileNames = cms.untracked.vstring('/store/relval/CMSSW_4_4_0/RelValProdTTbar/AODSIM/START44_V5-v2/0046/74C825F5-ACE6-E011-8C0D-00261894391F.root'),
                           #fileNames = cms.untracked.vstring('file:/data/blue/bendavid/fall11test/AE775E4D-1DE8-E011-9EB7-E0CB4E553665.root'),
   #skipEvents = cms.untracked.uint32(1740),
                            #fileNames = cms.untracked.vstring('/store/mc/Fall11/WJetsToLNu_TuneZ2_7TeV-madgraph-tauola/AODSIM/PU_Chamonix12_START44_V10-v2/0000/FED1A710-ED33-E111-B889-003048FFCC1E.root')
                            #fileNames = cms.untracked.vstring('/store/relval/CMSSW_5_1_2/RelValProdTTbar/AODSIM/START50_V15A-v1/0240/44FB321E-1C61-E111-BEC0-001A92810AC8.root',)
                            fileNames = cms.untracked.vstring('/store/relval/CMSSW_5_2_2/RelValProdTTbar/GEN-SIM-RECO/START52_V4-v1/0004/DCA3A2E4-C973-E111-9839-003048F1183E.root')
                            )

process.source.inputCommands = cms.untracked.vstring("keep *", "drop *_MEtoEDMConverter_*_*", "drop L1GlobalTriggerObjectMapRecord_hltL1GtObjectMap__HLT")

# other statements
process.GlobalTag.globaltag = 'START52_V4::All'

process.add_(cms.Service("ObjectService"))

process.load("MitProd.BAMBUSequences.BambuFillAODSIM_cfi")

process.MitTreeFiller.TreeWriter.fileName = 'XX-MITDATASET-XX'

#process.output = cms.OutputModule("PoolOutputModule",
#                                  outputCommands = cms.untracked.vstring('keep *'),
#                                  fileName = cms.untracked.string("test.root")
#)


process.bambu_step  = cms.Path(process.BambuFillAODSIM)
# schedule definition
process.schedule = cms.Schedule(process.bambu_step)
#process.outpath  = cms.EndPath(process.output)
