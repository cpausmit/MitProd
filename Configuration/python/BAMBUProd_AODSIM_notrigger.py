# $Id: BAMBUProd_AODSIM_notrigger.py,v 1.1 2012/06/17 01:33:03 bendavid Exp $

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
  version    = cms.untracked.string('Mit_029'),
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
  fileNames = cms.untracked.vstring('/store/group/phys_higgs/meridian/HGGProd/DiPhotonJets_8TeV-madgraph-pythia6-PU_S7_START52_V9/meridian/DiPhotonJets_8TeV-madgraph-tarball_LHE_v1/DiPhotonJets_8TeV-madgraph-pythia6-PU_S7_START52_V9/94808b8ed32dbd9febdd955c71d6eb1e/Hadronizer_MgmMatchTuneZ2star_8TeV_madgraph_tauola_cff_py_GEN_SIM_DIGI_L1_DIGI2RAW_RAW2DIGI_L1Reco_RECO_PU_83_1_D7Z.root')
)

process.source.inputCommands = cms.untracked.vstring("keep *",
                                                     "drop *_MEtoEDMConverter_*_*",
                                                     "drop L1GlobalTriggerObjectMapRecord_hltL1GtObjectMap__HLT")

# other statements
process.GlobalTag.globaltag = 'START53_V10::All'

process.add_(cms.Service("ObjectService"))

process.load("MitProd.BAMBUSequences.BambuFillAODSIM_cfi")

process.MitTreeFiller.TreeWriter.fileName = 'XX-MITDATASET-XX'

#disable hlt filling
process.MitTreeFiller.MetaInfos.hltActive = False
process.MitTreeFiller.MetaInfos.l1Active  = False

#process.output = cms.OutputModule("PoolOutputModule",
#                                  outputCommands = cms.untracked.vstring('keep *'),
#                                  fileName       = cms.untracked.string ("test.root")
#)


process.bambu_step  = cms.Path(process.BambuFillAODSIM)
# schedule definition
process.schedule = cms.Schedule(process.bambu_step)
#process.outpath  = cms.EndPath(process.output)
