# $Id:#

import FWCore.ParameterSet.Config as cms

process = cms.Process('FILLER')

# import of standard configurations
process.load('Configuration/StandardSequences/Services_cff')
process.load('FWCore/MessageService/MessageLogger_cfi')

process.configurationMetadata = cms.untracked.PSet(
    version = cms.untracked.string('Mit_014'),
    annotation = cms.untracked.string('MCONLY'),
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
#    fileNames = cms.untracked.vstring('file:/home/loizides/work/cms/CMSSW_3_3_5/src/phojet.root')
    fileNames = cms.untracked.vstring('file:/home/loizides/work/cms/CMSSW_3_3_5/src/p8mb.root')
)
process.source.inputCommands = cms.untracked.vstring("keep *", 
                                                     "drop *_MEtoEDMConverter_*_*", 
                                                     "drop L1GlobalTriggerObjectMapRecord_hltL1GtObjectMap__HLT")

process.add_(cms.Service("ObjectService"))

process.load('MitProd/TreeFiller/MitTreeFiller_cfi')
process.MitTreeFiller.fillers = ('MetaInfos',
                                 'MCParticles',
                                 'MCEventInfo',
                                 'MCVertexes'
                                 )

process.MitTreeFiller.TreeWriter.fileName = 'bambu'
process.MitTreeFiller.MetaInfos.l1Active  = False
process.MitTreeFiller.MetaInfos.hltActive = False
process.MitTreeFiller.MCParticles.active     = True
process.MitTreeFiller.MCParticles.simActive  = False
process.MitTreeFiller.MCParticles.useAodGen  = False
process.MitTreeFiller.MCParticles.genEdmName = 'generator'
#process.MitTreeFiller.MCParticles.genEdmName = 'source'
process.MitTreeFiller.MCEventInfo.active               = True
process.MitTreeFiller.MCEventInfo.genHepMCEventEdmName = 'generator'
#process.MitTreeFiller.MCEventInfo.genHepMCEventEdmName = 'source'
process.MitTreeFiller.MCVertexes.active    = True
process.MitTreeFiller.MCVertexes.useAodGen = False
process.MitTreeFiller.MCVertexes.edmName   = 'generator'
#process.MitTreeFiller.MCVertexes.edmName   = 'source'

process.bambu_step  = cms.Path(process.MitTreeFiller)

# schedule definition
process.schedule = cms.Schedule(process.bambu_step)
