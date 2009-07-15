# Auto generated configuration file
# using: 
# Revision: 1.120 
# Source: /cvs_server/repositories/CMSSW/CMSSW/Configuration/PyReleaseValidation/python/ConfigBuilder.py,v 
# with command line options: testingconf -s RECO --conditions FrontierConditions_GlobalTag,IDEAL_31X::All
import FWCore.ParameterSet.Config as cms

process = cms.Process('FILLER')

# import of standard configurations
process.load('Configuration/StandardSequences/Services_cff')
process.load('FWCore/MessageService/MessageLogger_cfi')
#process.load('Configuration/StandardSequences/MixingNoPileUp_cff')
process.load('Configuration/StandardSequences/GeometryIdeal_cff')
process.load('Configuration/StandardSequences/MagneticField_38T_cff')
#process.load('Configuration/StandardSequences/Reconstruction_cff')
process.load('Configuration/StandardSequences/EndOfProcess_cff')
process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')
process.load('Configuration/EventContent/EventContent_cff')

process.configurationMetadata = cms.untracked.PSet(
    version = cms.untracked.string('$Revision: 1.1 $'),
    annotation = cms.untracked.string('testingconf nevts:1'),
    name = cms.untracked.string('PyReleaseValidation')
)
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)
process.options = cms.untracked.PSet(
   Rethrow = cms.untracked.vstring('ProductNotFound')
)
# Input source
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring('file:/build/bendavid/RECODEBUG/WW_MC_31X_V2_preproduction_311_GEN-SIM-RECO-v1/C678170D-146C-DE11-A610-001C23C0F1F4.root')
)

# Other statements
process.GlobalTag.globaltag = 'MC_31X_V2::All'

#Load MitTreeFiller 
process.TreeService = cms.Service("TreeService",
    fileNames = cms.untracked.vstring('XX-MITDATASET-XX'),
)
process.add_(cms.Service("ObjectService"))

process.load("MitProd.BAMBUSequences.BambuFillRECODEBUG_cfi")

process.bambu_step  = cms.Path(process.BambuFillRECODEBUG)
process.endjob_step = cms.Path(process.endOfProcess)
#process.out_step = cms.EndPath(process.output)

# Schedule definition
process.schedule = cms.Schedule(process.bambu_step,process.endjob_step)
