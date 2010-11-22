# $Id: BAMBUProd_RECODEBUG.py,v 1.23 2010/10/18 01:35:27 bendavid Exp $

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
    annotation = cms.untracked.string('RECODEBUG'),
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
    fileNames = cms.untracked.vstring('/store/relval/CMSSW_3_9_0_pre6/RelValPhotonJets_Pt_10/GEN-SIM-RECO/START39_V1-v1/0040/E6E7D420-E7D2-DF11-BE6E-003048678F8C.root')
)
process.source.inputCommands = cms.untracked.vstring("keep *", "drop *_MEtoEDMConverter_*_*", "drop L1GlobalTriggerObjectMapRecord_hltL1GtObjectMap__HLT")

# other statements
process.GlobalTag.globaltag = 'START39_V6::All'

process.add_(cms.Service("ObjectService"))

process.load("MitProd.BAMBUSequences.BambuFillRECODEBUG_cfi")

process.MitTreeFiller.TreeWriter.fileName = 'XX-MITDATASET-XX'

#hack pixelLess tracking back (present in special startup MC samples)
#process.MitTreeFiller.PixelLessTracks.active          = True
    
process.bambu_step  = cms.Path(process.BambuFillRECODEBUG)

# schedule definition
process.schedule = cms.Schedule(process.bambu_step)
