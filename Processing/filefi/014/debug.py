# $Id: debug.py,v 1.3 2010/10/23 12:43:55 ceballos Exp $

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
    version    = cms.untracked.string('Mit_014a'),
    annotation = cms.untracked.string('RECODEBUG'),
    name       = cms.untracked.string('BambuProduction')
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

process.options = cms.untracked.PSet(
   Rethrow = cms.untracked.vstring('ProductNotFound'),
   fileMode = cms.untracked.string('NOMERGE')
)

# input source
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring('file:/build/bendavid/RECODEBUG/Zee7TeVTrackingParticles/D87A5BD6-40A0-DE11-A3A9-00E08178C0B1.root')
)
process.source.inputCommands = cms.untracked.vstring("keep *", "drop *_MEtoEDMConverter_*_*", "drop L1GlobalTriggerObjectMapRecord_hltL1GtObjectMap__HLT")

# other statements
process.GlobalTag.globaltag = 'START38_V12::All'

process.add_(cms.Service("ObjectService"))

process.load("MitProd.BAMBUSequences.BambuFillRECODEBUG_cfi")

process.MitTreeFiller.TreeWriter.fileName = 'XX-MITDATASET-XX'
process.MitTreeFiller.TreeWriter.maxSize  = cms.untracked.uint32(1790)

#hack pixelLess tracking back (present in special startup MC samples)
#process.MitTreeFiller.PixelLessTracks.active          = True
    
process.bambu_step  = cms.Path(process.BambuFillRECODEBUG)

# schedule definition
process.schedule = cms.Schedule(process.bambu_step)
