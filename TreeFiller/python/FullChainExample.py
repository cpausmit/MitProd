# Auto generated configuration file
# using: 
# Revision: 1.77.2.1 
# Source: /cvs_server/repositories/CMSSW/CMSSW/Configuration/PyReleaseValidation/python/ConfigBuilder.py,v 
# with command line options: Configuration/GenProduction/python/PYTHIA6_EWK_Zmumu_10TeV_cff.py --step=GEN,SIM,DIGI,L1,DIGI2RAW,RAW2DIGI,RECO --eventcontent=FEVTSIM --datatier=GEN-SIM-RAW-RECO --conditions=FrontierConditions_GlobalTag,IDEAL_V9::All -n 10 --no_exec
import FWCore.ParameterSet.Config as cms

process = cms.Process('RECO')

# import of standard configurations
process.load('Configuration/StandardSequences/Services_cff')
process.load('FWCore/MessageService/MessageLogger_cfi')
process.load('Configuration/StandardSequences/MixingNoPileUp_cff')
process.load('Configuration/StandardSequences/GeometryPilot2_cff')
process.load('Configuration/StandardSequences/MagneticField_38T_cff')
process.load('Configuration/StandardSequences/Generator_cff')
process.load('Configuration/StandardSequences/VtxSmearedEarly10TeVCollision_cff')
process.load('Configuration/StandardSequences/Sim_cff')
process.load('Configuration/StandardSequences/Digi_cff')
process.load('Configuration/StandardSequences/SimL1Emulator_cff')
process.load('L1TriggerConfig/L1GtConfigProducers/Luminosity/lumi1030.L1Menu2008_2E30_Unprescaled_cff')
process.load('Configuration/StandardSequences/DigiToRaw_cff')
process.load('Configuration/StandardSequences/RawToDigi_cff')
process.load('Configuration/StandardSequences/Reconstruction_cff')
process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')
process.load('Configuration/EventContent/EventContent_cff')

process.configurationMetadata = cms.untracked.PSet(
    version = cms.untracked.string('$Revision: 1.1 $'),
    annotation = cms.untracked.string('Z/gamma* to mumu sample, M(mumu) gt40 GeV -- sqrt(s) = 10TeV'),
    name = cms.untracked.string('$Source: /cvs_server/repositories/CMSSW/UserCode/MitProd/TreeFiller/python/FullChainExample.py,v $')
)
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(10)
)
process.options = cms.untracked.PSet(
    Rethrow = cms.untracked.vstring('ProductNotFound')
)
# Input source
process.source = cms.Source("PythiaSource",
    pythiaPylistVerbosity = cms.untracked.int32(0),
    filterEfficiency = cms.untracked.double(0.509),
    pythiaHepMCVerbosity = cms.untracked.bool(False),
    comEnergy = cms.untracked.double(10000.0),
    crossSection = cms.untracked.double(1232.0),
    maxEventsToPrint = cms.untracked.int32(0),
    PythiaParameters = cms.PSet(
        pythiaUESettings = cms.vstring('MSTJ(11)=3     ! Choice of the fragmentation function', 
            'MSTJ(22)=2     ! Decay those unstable particles', 
            'PARJ(71)=10 .  ! for which ctau  10 mm', 
            'MSTP(2)=1      ! which order running alphaS', 
            'MSTP(33)=0     ! no K factors in hard cross sections', 
            'MSTP(51)=10042     ! CTEQ6L1 structure function chosen', 
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
            'MSTP(91)=1     !', 
            'PARP(91)=2.1   ! kt distribution', 
            'PARP(93)=15.0  ! '),
        processParameters = cms.vstring('MSEL=0            !User defined processes', 
            'MSUB(1)=1         !Incl Z0/gamma* production', 
            'MSTP(43)=3        !Both Z0 and gamma*', 
            'MDME(174,1)=0     !Z decay into d dbar', 
            'MDME(175,1)=0     !Z decay into u ubar', 
            'MDME(176,1)=0     !Z decay into s sbar', 
            'MDME(177,1)=0     !Z decay into c cbar', 
            'MDME(178,1)=0     !Z decay into b bbar', 
            'MDME(179,1)=0     !Z decay into t tbar', 
            'MDME(182,1)=0     !Z decay into e- e+', 
            'MDME(183,1)=0     !Z decay into nu_e nu_ebar', 
            'MDME(184,1)=1     !Z decay into mu- mu+', 
            'MDME(185,1)=0     !Z decay into nu_mu nu_mubar', 
            'MDME(186,1)=0     !Z decay into tau- tau+', 
            'MDME(187,1)=0     !Z decay into nu_tau nu_taubar', 
            'CKIN(1)=40.       !Minimum sqrt(s_hat) value (=Z mass)'),
        parameterSets = cms.vstring('pythiaUESettings', 
            'processParameters')
    )
)

# Output definition
process.output = cms.OutputModule("PoolOutputModule",
    outputCommands = process.FEVTSIMEventContent.outputCommands,
    fileName = cms.untracked.string('PYTHIA6_EWK_Zmumu_10TeV_cff_py_GEN_SIM_DIGI_L1_DIGI2RAW_RAW2DIGI_RECO.root'),
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('GEN-SIM-RAW-RECO'),
        filterName = cms.untracked.string('')
    ),
    SelectEvents = cms.untracked.PSet(
        SelectEvents = cms.vstring('generation_step')
    )
)

# Additional output definition

# Other statements
process.GlobalTag.globaltag = 'IDEAL_V9::All'

#Load MitTreeFiller 
process.TreeService = cms.Service("TreeService",
    fileNames = cms.untracked.vstring('mit-full')
)
process.add_(cms.Service("ObjectService"))
process.load("MitProd.TreeFiller.MitTreeFiller_cfi")
#disable HLT filler since the full chain example does not (and cannot at the moment) contain HLT info
process.MitTreeFiller.MetaInfos.hltActive = False

#Load Mit vProducer
process.load("MitEdm.Producers.vProducer_cff")

#Load Mit Mvf Conversion producer
process.load("MitEdm.Producers.conversionProducer_cff")

# compute ECAL shower shape variables
process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Geometry.CaloEventSetup.CaloGeometry_cfi")
process.load("Geometry.CMSCommonData.cmsIdealGeometryXML_cfi")
process.load("Geometry.CaloEventSetup.CaloTopology_cfi")

process.load("FWCore.MessageLogger.MessageLogger_cfi")

#Load ElectronID information
process.load("MitProd.TreeFiller.ElectronID_cfi")
#Load Electron Isolation. For Jurassic isolation veto fix.
process.load("RecoEgamma.EgammaIsolationAlgos.eleIsolationSequence_cff")


#For Jet Corrections (Summer08 Jet corrections)
process.load("JetMETCorrections.Configuration.L2L3Corrections_Summer08_cff")
process.prefer("L3JetCorrectorIC5Calo")

#enable Jet Corrections for all of our Jet collections
process.MitTreeFiller.CaloJets.jetCorrectionsActive = cms.untracked.bool(True)
process.MitTreeFiller.ItrCone5Jets.jetCorrectionsActive = cms.untracked.bool(True)
process.MitTreeFiller.SisCone5Jets.jetCorrectionsActive = cms.untracked.bool(True)
process.MitTreeFiller.SisCone7Jets.jetCorrectionsActive = cms.untracked.bool(True)
process.MitTreeFiller.Kt4Jets.jetCorrectionsActive = cms.untracked.bool(True)
process.MitTreeFiller.Kt6Jets.jetCorrectionsActive = cms.untracked.bool(True)

#For JetPlusTracks
process.load("JetMETCorrections.Configuration.JetPlusTrackCorrections_cff")
process.load("JetMETCorrections.Configuration.ZSPJetCorrections152_cff")
process.MitTreeFiller.IC5JetPlusTrack.active = cms.untracked.bool(True)

#Load Met Corrections
process.load("MitProd.TreeFiller.MetCorrections_cfi")

#Load Flavor Matching Information
process.load("MitProd.TreeFiller.JetsMCFlavourMatching_cfi")
#Enable Flavor matching for Reco Jets and GenJets
process.MitTreeFiller.CaloJets.flavorMatchingActive = cms.untracked.bool(True)
process.MitTreeFiller.ItrCone5Jets.flavorMatchingActive = cms.untracked.bool(True)
process.MitTreeFiller.SisCone5Jets.flavorMatchingActive = cms.untracked.bool(True)
process.MitTreeFiller.SisCone7Jets.flavorMatchingActive = cms.untracked.bool(True)
process.MitTreeFiller.Kt4Jets.flavorMatchingActive = cms.untracked.bool(True)
process.MitTreeFiller.Kt6Jets.flavorMatchingActive = cms.untracked.bool(True)
process.MitTreeFiller.IC5GenJets.flavorMatchingActive = cms.untracked.bool(True)
process.MitTreeFiller.SC5GenJets.flavorMatchingActive = cms.untracked.bool(True)
process.MitTreeFiller.SC7GenJets.flavorMatchingActive = cms.untracked.bool(True)
process.MitTreeFiller.KT4GenJets.flavorMatchingActive = cms.untracked.bool(True)
process.MitTreeFiller.KT6GenJets.flavorMatchingActive = cms.untracked.bool(True)

#produce jet vertex association information
process.load("MitProd.TreeFiller.JetVertexAssociation_cfi")
#Enable Jet Vertex association for Reco Jet collections
process.MitTreeFiller.CaloJets.jetToVertexActive = cms.untracked.bool(True)
process.MitTreeFiller.ItrCone5Jets.jetToVertexActive = cms.untracked.bool(True)
process.MitTreeFiller.SisCone5Jets.jetToVertexActive = cms.untracked.bool(True)
process.MitTreeFiller.SisCone7Jets.jetToVertexActive = cms.untracked.bool(True)
process.MitTreeFiller.Kt4Jets.jetToVertexActive = cms.untracked.bool(True)
process.MitTreeFiller.Kt6Jets.jetToVertexActive = cms.untracked.bool(True)

#Load track detector associator for Track-ECal association
process.load("MitProd.TreeFiller.TrackEcalAssociation_cfi")
process.MitTreeFiller.TrackAssociatorParameters = cms.untracked.PSet(process.TrackAssociatorParameters)
#enable Track-Ecal assocation in fillers
process.MitTreeFiller.GeneralTracks.ecalAssocActive                       = True
process.MitTreeFiller.StandaloneMuonTracks.ecalAssocActive                = True
process.MitTreeFiller.StandaloneMuonTracksWVtxConstraint.ecalAssocActive  = True
process.MitTreeFiller.GlobalMuonTracks.ecalAssocActive                    = True
process.MitTreeFiller.ConversionInOutTracks.ecalAssocActive               = True
process.MitTreeFiller.ConversionOutInTracks.ecalAssocActive               = True
process.MitTreeFiller.GsfTracks.ecalAssocActive                           = True

#load gsf track to general track associator
process.load("MitEdm.Producers.gsfTrackAssociator_cff")
process.MitTreeFiller.Electrons.gsfTrackAssocName = 'gsfTrackAssociator'

# Path and EndPath definitions
process.generation_step = cms.Path(process.pgen)
process.simulation_step = cms.Path(process.psim)
process.digitisation_step = cms.Path(process.pdigi)
process.L1simulation_step = cms.Path(process.SimL1Emulator)
process.digi2raw_step = cms.Path(process.DigiToRaw)
process.raw2digi_step = cms.Path(process.RawToDigi)
process.reconstruction_step = cms.Path(process.reconstruction)
process.mitTreeFiller_step = cms.Path(
    process.eleIsolationSequence *
    process.gsfTrackAssociator *
    process.vProducer *
    process.conversionProducer *
    (  process.MitEIdSequence
     + process.MitMetCorrections
     + process.caloJetMCFlavour
     + process.jetvertexAssociationSequence
     + process.ZSPJetCorrections*process.JetPlusTrackCorrections
     )
    *process.MitTreeFiller
    *process.vFiller
    *process.conversionFiller
     )
#process.out_step = cms.EndPath(process.output)

# Schedule definition
process.schedule = cms.Schedule(process.generation_step,process.simulation_step,process.digitisation_step,process.L1simulation_step,process.digi2raw_step,process.raw2digi_step,process.reconstruction_step,process.mitTreeFiller_step)
