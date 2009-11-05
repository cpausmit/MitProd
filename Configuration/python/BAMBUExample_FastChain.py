# $Id: BAMBUExample_FastChain.py,v 1.5 2009/11/03 14:03:25 bendavid Exp $
#
# Source: /cvs_server/repositories/CMSSW/CMSSW/Configuration/PyReleaseValidation/python/ConfigBuilder.py,v 
# with command line options:
#  TTbar_cfi -s GEN,FASTSIM --conditions FrontierConditions_GlobalTag,MC_31X_V3::All
#  -n 100 --no_exec --no_output --pileup NoPileUp

import FWCore.ParameterSet.Config as cms

process = cms.Process('HLT')

# import of standard configurations
process.load('FastSimulation/Configuration/RandomServiceInitialization_cff')
process.load('FastSimulation.PileUpProducer.PileUpSimulator10TeV_cfi')
process.load('Configuration/StandardSequences/MagneticField_38T_cff')
process.load('Configuration/StandardSequences/Generator_cff')
process.load('FastSimulation/Configuration/FamosSequences_cff')
process.load('FastSimulation/Configuration/HLT_1E31_cff')
process.load('IOMC.EventVertexGenerators.VtxSmearedParameters_cfi')
process.load('FastSimulation/Configuration/CommonInputs_cff')
process.load('FastSimulation/Configuration/EventContent_cff')

process.configurationMetadata = cms.untracked.PSet(
    version = cms.untracked.string('CMSSW_3_2_1 + Mit_010'),
    annotation = cms.untracked.string('TTbar'),
    name = cms.untracked.string('BambuFastChainExample')
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(100)
)
process.options = cms.untracked.PSet(
    Rethrow = cms.untracked.vstring('ProductNotFound')
)

# input source
process.source = cms.Source("EmptySource")

# output definition
#process.output = cms.OutputModule("PoolOutputModule",
#    splitLevel = cms.untracked.int32(0),
#    outputCommands = process.RECOSIMEventContent.outputCommands,
#    fileName = cms.untracked.string('TTbar_cfi_GEN_FASTSIM.root'),
#    dataset = cms.untracked.PSet(
#       dataTier = cms.untracked.string(''),
#       filterName = cms.untracked.string('')
#    ),
#    SelectEvents = cms.untracked.PSet(
#       SelectEvents = cms.vstring('generation_step')
#    )
#)

# Additional output definition

# other statements
process.famosPileUp.PileUpSimulator = process.PileUpSimulatorBlock.PileUpSimulator
process.famosPileUp.PileUpSimulator.averageNumber = 0
process.famosSimHits.SimulateCalorimetry = True
process.famosSimHits.SimulateTracking = True
process.famosSimHits.ActivateDecays.comEnergy = 10000
process.simulation = cms.Sequence(process.simulationWithFamos)
process.HLTEndSequence = cms.Sequence(process.reconstructionWithFamos)

# set correct vertex smearing
process.Early10TeVCollisionVtxSmearingParameters.type = cms.string("BetaFunc")
process.famosSimHits.VertexGenerator = process.Early10TeVCollisionVtxSmearingParameters
process.famosPileUp.VertexGenerator = process.Early10TeVCollisionVtxSmearingParameters

# apply tracker misalignment
process.famosSimHits.ApplyAlignment = True
process.misalignedTrackerGeometry.applyAlignment = True

process.GlobalTag.globaltag = 'MC_31X_V9::All'
process.generator = cms.EDFilter("Pythia6GeneratorFilter",
    pythiaPylistVerbosity = cms.untracked.int32(0),
    filterEfficiency = cms.untracked.double(1.0),
    pythiaHepMCVerbosity = cms.untracked.bool(False),
    comEnergy = cms.double(10000.0),
    maxEventsToPrint = cms.untracked.int32(0),
    PythiaParameters = cms.PSet(
        pythiaUESettings = cms.vstring('MSTJ(11)=3     ! Choice of the fragmentation function', 
            'MSTJ(22)=2     ! Decay those unstable particles', 
            'PARJ(71)=10 .  ! for which ctau  10 mm', 
            'MSTP(2)=1      ! which order running alphaS', 
            'MSTP(33)=0     ! no K factors in hard cross sections', 
            'MSTP(51)=10042 ! structure function chosen (external PDF CTEQ6L1)', 
            'MSTP(52)=2     ! work with LHAPDF', 
            'MSTP(81)=1     ! multiple parton interactions 1 is Pythia default', 
            'MSTP(82)=4     ! Defines the multi-parton model', 
            'MSTU(21)=1     ! Check on possible errors during program execution', 
            'PARP(82)=1.8387   ! pt cutoff for multiparton interactions', 
            'PARP(89)=1960. ! sqrts for which PARP82 is set', 
            'PARP(83)=0.5   ! Multiple interactions: matter distrbn parameter', 
            'PARP(84)=0.4   ! Multiple interactions: matter distribution parameter', 
            'PARP(90)=0.16  ! Multiple interactions: rescaling power', 
            'PARP(67)=2.5    ! amount of initial-state radiation', 
            'PARP(85)=1.0  ! gluon prod. mechanism in MI', 
            'PARP(86)=1.0  ! gluon prod. mechanism in MI', 
            'PARP(62)=1.25   ! ', 
            'PARP(64)=0.2    ! ', 
            'MSTP(91)=1      !', 
            'PARP(91)=2.1   ! kt distribution', 
            'PARP(93)=15.0  ! '),
        processParameters = cms.vstring('MSEL      = 0     ! User defined processes', 
            'MSUB(81)  = 1     ! qqbar to QQbar', 
            'MSUB(82)  = 1     ! gg to QQbar', 
            'MSTP(7)   = 6     ! flavour = top', 
            'PMAS(6,1) = 175.  ! top quark mass'),
        parameterSets = cms.vstring('pythiaUESettings', 
            'processParameters')
    )
)
#process.ProductionFilterSequence = cms.Sequence(process.generator)

# path and end path definitions
process.generation_step = cms.Path(process.generator +
                                   cms.SequencePlaceholder("randomEngineStateProducer") +
                                   process.GeneInfo+process.genJetMET)
process.reconstruction = cms.Path(process.reconstructionWithFamos)
#process.out_step = cms.EndPath(process.output)

process.add_(cms.Service("ObjectService"))

process.load("MitProd.TreeFiller.MitTreeFiller_cfi")

# enable fillers of MC Truth information
process.MitTreeFiller.MCParticles.active = True
process.MitTreeFiller.MCEventInfo.active = True
process.MitTreeFiller.IC5GenJets.active  = True
process.MitTreeFiller.SC5GenJets.active  = True
process.MitTreeFiller.SC7GenJets.active  = True
process.MitTreeFiller.KT4GenJets.active  = True
process.MitTreeFiller.KT6GenJets.active  = True
process.MitTreeFiller.GenMet.active      = True

# hack out unavailable stuff, pending proper fastsim and aod sequences
process.MitTreeFiller.MetaInfos.hltActive                   = False
process.MitTreeFiller.MCParticles.simActive                 = False
process.MitTreeFiller.ConversionInOutTracks.active          = False
process.MitTreeFiller.ConversionOutInTracks.active          = False
process.MitTreeFiller.Conversions.active                    = False
process.MitTreeFiller.ConversionInOutElectronsStable.active = False
process.MitTreeFiller.ConversionOutInElectronsStable.active = False

process.bambu_step  = cms.Path(process.MitTreeFiller)

# schedule definition
process.schedule = cms.Schedule(process.generation_step)
process.schedule.extend(process.HLTSchedule)
process.schedule.extend([process.reconstruction,process.bambu_step])

# special treatment in case of production filter sequence  
#for path in process.paths: 
#getattr(process,path)._seq = process.ProductionFilterSequence*getattr(process,path)._seq
