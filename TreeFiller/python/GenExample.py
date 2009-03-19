# $Id: GenExample.py,v 1.4 2009/03/17 17:21:09 loizides Exp $

import FWCore.ParameterSet.Config as cms

process = cms.Process('HLT')

process.load('Configuration.StandardSequences.SimulationRandomNumberGeneratorSeeds_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')

   
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(100)
)

process.source = cms.Source("PythiaSource",
    pythiaPylistVerbosity = cms.untracked.int32(0),
    filterEfficiency      = cms.untracked.double(1.0),
    pythiaHepMCVerbosity  = cms.untracked.bool(False),
    comEnergy             = cms.untracked.double(10000.0),    
    maxEventsToPrint      = cms.untracked.int32(0),

    PythiaParameters = cms.PSet(
        parameterSets = cms.vstring()
    )
)                           
    
# load generator sequence (VtxSmeared is needed inside, missing dependence)
process.load('Configuration.StandardSequences.VtxSmearedBetafuncEarlyCollision_cff')
process.load('Configuration.StandardSequences.Generator_cff')

# include the MIT services and standard filler
process.ObjectService = cms.Service("ObjectService")

process.TreeService = cms.Service("TreeService",
    fileNames = cms.untracked.vstring("mit-gen")
)

process.MitTreeFiller = cms.EDAnalyzer("FillMitTree",
    fillers = cms.untracked.vstring('MetaInfos',
                                    'MCParticles',
                                    'MCEventInfo'), 

    MetaInfos = cms.untracked.PSet(
        hltActive  = cms.untracked.bool(False),
        fillerType = cms.untracked.string('FillerMetaInfos')
    ),

    MCParticles = cms.untracked.PSet(
        simActive  = cms.untracked.bool(False),
        fillerType = cms.untracked.string('FillerMCParticles')
    )
)
 
process.p0 = cms.Path( process.pgen * process.MitTreeFiller)

# also make Edm output for the events we generate
process.load('Configuration.EventContent.EventContent_cff')

process.FEVT = cms.OutputModule("PoolOutputModule",
    process.FEVTSIMEventContent,
    fileName = cms.untracked.string('edm-gen.root'),        
)
   
process.end = cms.EndPath( process.FEVT )
