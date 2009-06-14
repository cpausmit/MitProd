# Auto generated configuration file
# using: 
# Revision: 1.99.2.8 
# Source: /cvs_server/repositories/CMSSW/CMSSW/Configuration/PyReleaseValidation/python/ConfigBuilder.py,v 
# with command line options: Configuration/GenProduction/python/PYTHIA6_TTbar_10TeV_cff.py -s GEN,FASTSIM --pileup=NoPileUp --conditions FrontierConditions_GlobalTag,IDEAL_V12::All --beamspot=Early10TeVCollision --datatier GEN-SIM-DIGI-RECO --eventcontent AODSIM -n 10000 --no_exec
import FWCore.ParameterSet.Config as cms

process = cms.Process('HLT')

# import of standard configurations
process.load('FastSimulation/Configuration/RandomServiceInitialization_cff')
process.load('FastSimulation.PileUpProducer.PileUpSimulator10TeV_cfi')
process.load('Configuration/StandardSequences/MagneticField_38T_cff')
process.load('Configuration/StandardSequences/Generator_cff')
process.load('FastSimulation/Configuration/FamosSequences_cff')
process.load('FastSimulation/Configuration/HLT_cff')
process.load('Configuration.StandardSequences.L1TriggerDefaultMenu_cff')
process.load('IOMC.EventVertexGenerators.VtxSmearedParameters_cfi')
process.load('FastSimulation/Configuration/CommonInputs_cff')
process.load('FastSimulation/Configuration/EventContent_cff')

process.configurationMetadata = cms.untracked.PSet(
    version = cms.untracked.string('$Revision: 1.1 $'),
    annotation = cms.untracked.string('PYTHIA6-MinBias at 10TeV'),
    name = cms.untracked.string('$Source: /cvs_server/repositories/CMSSW/UserCode/MitProd/TreeFiller/python/FastChainExample.py,v $')
)
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(50)
)
process.options = cms.untracked.PSet(
    Rethrow = cms.untracked.vstring('ProductNotFound')
)
# Input source
process.source = cms.Source("PythiaSource",
    pythiaPylistVerbosity = cms.untracked.int32(1),
    filterEfficiency = cms.untracked.double(1.0),
    pythiaHepMCVerbosity = cms.untracked.bool(False),
    comEnergy = cms.untracked.double(10000.0),
    crossSection = cms.untracked.double(55000000000.0),
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
        processParameters = cms.vstring('MSEL=0         ! User defined processes', 
            'MSUB(81)  = 1     ! qqbar to QQbar', 
            'MSUB(82)  = 1     ! gg to QQbar', 
            'MSTP(7)   = 6     ! flavor = top', 
            'PMAS(6,1) = 172.4  ! top quark mass'),
        parameterSets = cms.vstring('pythiaUESettings', 
            'processParameters')
    )
)

# Output definition
#process.output = cms.OutputModule("PoolOutputModule",
    #outputCommands = process.AODSIMEventContent.outputCommands,
    #fileName = cms.untracked.string('PYTHIA6_TTbar_10TeV_cff_py_GEN_FASTSIM.root'),
    #dataset = cms.untracked.PSet(
        #dataTier = cms.untracked.string('GEN-SIM-DIGI-RECO'),
        #filterName = cms.untracked.string('')
    #),
    #SelectEvents = cms.untracked.PSet(
        #SelectEvents = cms.vstring('generation_step')
    #)
#)

# Additional output definition

# Other statements
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
# Apply Tracker misalignment
process.famosSimHits.ApplyAlignment = True
process.misalignedTrackerGeometry.applyAlignment = True

process.GlobalTag.globaltag = 'IDEAL_V12::All'

try:
  process.metreco.remove(process.tcMet)
except:
  pass


# Path and EndPath definitions
process.generation_step = cms.Path(cms.SequencePlaceholder("randomEngineStateProducer")+process.GeneInfo+process.genJetMET)
process.reconstruction = cms.Path(process.reconstructionWithFamos)
#process.out_step = cms.EndPath(process.output)

#Pre-load gsf tracks
process.load("MitEdm.TrackerElectrons.LoadTracks_cfi")

#Load MitTreeFiller 
process.TreeService = cms.Service("TreeService",
    fileNames = cms.untracked.vstring('mit-full'),
)
process.add_(cms.Service("ObjectService"))
process.load("MitProd.TreeFiller.MitTreeFiller_cfi")

#Reconfigure treefiller defaults for AOD content:
process.MitTreeFiller.MCParticles.simActive        = False
process.MitTreeFiller.ConversionInOutTracks.active = False
process.MitTreeFiller.ConversionOutInTracks.active = False
process.MitTreeFiller.ConversionElectrons.active   = False
process.MitTreeFiller.Conversions.active           = False
process.MitTreeFiller.ItrCone5PFJets.active        = False
process.MitTreeFiller.SisCone5PFJets.active        = False
process.MitTreeFiller.SisCone7PFJets.active        = False
process.MitTreeFiller.Kt4PFJets.active             = False
process.MitTreeFiller.Kt6PFJets.active             = False
process.MitTreeFiller.PFGsfTracks.active           = False
process.MitTreeFiller.TCMet.active                 = False
process.MitTreeFiller.MetaInfos.hltActive          = False
process.MitTreeFiller.ItrCone5Jets.bTaggingActive  = False
process.MitTreeFiller.PFMet.active                 = False
process.MitTreeFiller.CaloTaus.active              = False
process.MitTreeFiller.PFTaus.active                = False


#Load Mit vProducer
process.load("MitProd.TreeFiller.vProducerNoRefit_cff")

process.load("Configuration.StandardSequences.Geometry_cff")

#Load ElectronID information
process.load("MitProd.TreeFiller.ElectronID_cfi")

#Load electron isolation value map producers
process.load("RecoEgamma.EgammaIsolationAlgos.eleIsoFromDeposits_cff")

#For Jet Corrections (Winter09 FastSim-specific Jet corrections)
process.load("JetMETCorrections.Configuration.L2L3Corrections_Winter09_cff")
process.prefer("L3JetCorrectorIC5Calo")
process.correctedJets = cms.Sequence(process.L2L3CorJetIC5Calo *
                                     process.L2L3CorJetSC5Calo *
                                     process.L2L3CorJetSC7Calo *
                                     process.L2L3CorJetKT4Calo *
                                     process.L2L3CorJetKT6Calo *
                                     process.L2L3CorJetIC5JPT)
                                     
#Enable Jet Corrections for all of our Jet collections
process.MitTreeFiller.ItrCone5Jets.jetCorrectionsActive    = True
process.MitTreeFiller.SisCone5Jets.jetCorrectionsActive    = True
process.MitTreeFiller.SisCone7Jets.jetCorrectionsActive    = True
process.MitTreeFiller.Kt4Jets.jetCorrectionsActive         = True
process.MitTreeFiller.Kt6Jets.jetCorrectionsActive         = True
process.MitTreeFiller.IC5JetPlusTrack.jetCorrectionsActive = True

#For JetPlusTracks
process.load("JetMETCorrections.Configuration.JetPlusTrackCorrections_cff")
process.load("JetMETCorrections.Configuration.ZSPJetCorrections152_cff")
process.MitTreeFiller.IC5JetPlusTrack.active = True

#Load Met Corrections
process.load("MitProd.TreeFiller.MetCorrections_cfi")

#Load MHT sequence
process.load("MitProd.TreeFiller.MitMHT_cfi")

#Load track-corrected MET module
process.load("RecoMET.METProducers.TCMET_cfi")

#Load Flavor Matching Information
process.load("MitProd.TreeFiller.JetsMCFlavourMatching_cfi")

#Enable Flavor matching for Reco Jets and GenJets
#process.MitTreeFiller.ItrCone5Jets.flavorMatchingActive = True
#process.MitTreeFiller.SisCone5Jets.flavorMatchingActive = True
#process.MitTreeFiller.SisCone7Jets.flavorMatchingActive = True
#process.MitTreeFiller.Kt4Jets.flavorMatchingActive      = True
#process.MitTreeFiller.Kt6Jets.flavorMatchingActive      = True
#process.MitTreeFiller.IC5GenJets.flavorMatchingActive   = True
#process.MitTreeFiller.SC5GenJets.flavorMatchingActive   = True
#process.MitTreeFiller.SC7GenJets.flavorMatchingActive   = True
#process.MitTreeFiller.KT4GenJets.flavorMatchingActive   = True
#process.MitTreeFiller.KT6GenJets.flavorMatchingActive   = True

#Produce jet vertex association information
process.load("MitProd.TreeFiller.JetVertexAssociation_cfi")

#Enable Jet Vertex association for Reco Jet collections
process.MitTreeFiller.ItrCone5Jets.jetToVertexActive = True
process.MitTreeFiller.SisCone5Jets.jetToVertexActive = True
process.MitTreeFiller.SisCone7Jets.jetToVertexActive = True
process.MitTreeFiller.Kt4Jets.jetToVertexActive      = True
process.MitTreeFiller.Kt6Jets.jetToVertexActive      = True

#Load track detector associator for Track-ECal association
process.load("MitProd.TreeFiller.TrackEcalAssociation_cfi")
process.MitTreeFiller.TrackAssociatorParameters = cms.untracked.PSet(process.TrackAssociatorParameters)

#Enable Track-Ecal assocation in fillers
process.MitTreeFiller.GeneralTracks.ecalAssocActive                       = True
process.MitTreeFiller.StandaloneMuonTracks.ecalAssocActive                = True
process.MitTreeFiller.StandaloneMuonTracksWVtxConstraint.ecalAssocActive  = True
process.MitTreeFiller.GlobalMuonTracks.ecalAssocActive                    = True
process.MitTreeFiller.ConversionInOutTracks.ecalAssocActive               = True
process.MitTreeFiller.ConversionOutInTracks.ecalAssocActive               = True
process.MitTreeFiller.GsfTracks.ecalAssocActive                           = True

#process.p1 = cms.Path(
    #process.LoadTracks
#)

process.p2 = cms.Path(
    process.vProducer *
    process.eleIsoFromDeposits *
    (  process.MitEIdSequence
     + process.MitMetCorrections
     #+ process.caloJetMCFlavour
     #+ process.pfJetMCFlavour
     + process.jetvertexAssociationSequence
     + process.ZSPJetCorrections*process.JetPlusTrackCorrections
     + process.correctedJets
     + process.MitMHT
#     + process.tcMet
    ) *
    process.MitTreeFiller 
  * process.vFiller
)

# Schedule definition
#process.schedule = cms.Schedule(process.generation_step,process.HLTriggerFirstPath,process.HLT_L1Jet15,process.HLT_Jet30,process.HLT_Jet50,process.HLT_Jet80,process.HLT_Jet110,process.HLT_Jet180,process.HLT_Jet250,process.HLT_FwdJet20,process.HLT_DoubleJet150,process.HLT_DoubleJet125_Aco,process.HLT_DoubleFwdJet50,process.HLT_DiJetAve15,process.HLT_DiJetAve30,process.HLT_DiJetAve50,process.HLT_DiJetAve70,process.HLT_DiJetAve130,process.HLT_DiJetAve220,process.HLT_TripleJet85,process.HLT_QuadJet30,process.HLT_QuadJet60,process.HLT_SumET120,process.HLT_L1MET20,process.HLT_MET25,process.HLT_MET35,process.HLT_MET50,process.HLT_MET65,process.HLT_MET75,process.HLT_MET65_HT350,process.HLT_Jet180_MET60,process.HLT_Jet60_MET70_Aco,process.HLT_Jet100_MET60_Aco,process.HLT_DoubleJet125_MET60,process.HLT_DoubleFwdJet40_MET60,process.HLT_DoubleJet60_MET60_Aco,process.HLT_DoubleJet50_MET70_Aco,process.HLT_DoubleJet40_MET70_Aco,process.HLT_TripleJet60_MET60,process.HLT_QuadJet35_MET60,process.HLT_IsoEle15_L1I,process.HLT_IsoEle18_L1R,process.HLT_IsoEle15_LW_L1I,process.HLT_LooseIsoEle15_LW_L1R,process.HLT_Ele10_SW_L1R,process.HLT_Ele15_SW_L1R,process.HLT_Ele15_LW_L1R,process.HLT_EM80,process.HLT_EM200,process.HLT_DoubleIsoEle10_L1I,process.HLT_DoubleIsoEle12_L1R,process.HLT_DoubleIsoEle10_LW_L1I,process.HLT_DoubleIsoEle12_LW_L1R,process.HLT_DoubleEle5_SW_L1R,process.HLT_DoubleEle10_LW_OnlyPixelM_L1R,process.HLT_DoubleEle10_Z,process.HLT_DoubleEle6_Exclusive,process.HLT_IsoPhoton30_L1I,process.HLT_IsoPhoton10_L1R,process.HLT_IsoPhoton15_L1R,process.HLT_IsoPhoton20_L1R,process.HLT_IsoPhoton25_L1R,process.HLT_IsoPhoton40_L1R,process.HLT_Photon15_L1R,process.HLT_Photon25_L1R,process.HLT_DoubleIsoPhoton20_L1I,process.HLT_DoubleIsoPhoton20_L1R,process.HLT_DoublePhoton10_Exclusive,process.HLT_L1Mu,process.HLT_L1MuOpen,process.HLT_L2Mu9,process.HLT_IsoMu9,process.HLT_IsoMu11,process.HLT_IsoMu13,process.HLT_IsoMu15,process.HLT_Mu3,process.HLT_Mu5,process.HLT_Mu7,process.HLT_Mu9,process.HLT_Mu11,process.HLT_Mu13,process.HLT_Mu15,process.HLT_Mu15_Vtx2mm,process.HLT_DoubleIsoMu3,process.HLT_DoubleMu3,process.HLT_DoubleMu3_Vtx2mm,process.HLT_DoubleMu3_JPsi,process.HLT_DoubleMu3_Upsilon,process.HLT_DoubleMu7_Z,process.HLT_DoubleMu3_SameSign,process.HLT_DoubleMu3_Psi2S,process.HLT_BTagIP_Jet180,process.HLT_BTagIP_Jet120_Relaxed,process.HLT_BTagIP_DoubleJet120,process.HLT_BTagIP_DoubleJet60_Relaxed,process.HLT_BTagIP_TripleJet70,process.HLT_BTagIP_TripleJet40_Relaxed,process.HLT_BTagIP_QuadJet40,process.HLT_BTagIP_QuadJet30_Relaxed,process.HLT_BTagIP_HT470,process.HLT_BTagIP_HT320_Relaxed,process.HLT_BTagMu_DoubleJet120,process.HLT_BTagMu_DoubleJet60_Relaxed,process.HLT_BTagMu_TripleJet70,process.HLT_BTagMu_TripleJet40_Relaxed,process.HLT_BTagMu_QuadJet40,process.HLT_BTagMu_QuadJet30_Relaxed,process.HLT_BTagMu_HT370,process.HLT_BTagMu_HT250_Relaxed,process.HLT_DoubleMu3_BJPsi,process.HLT_DoubleMu4_BJPsi,process.HLT_TripleMu3_TauTo3Mu,process.HLT_IsoTau_MET65_Trk20,process.HLT_IsoTau_MET35_Trk15_L1MET,process.HLT_LooseIsoTau_MET30,process.HLT_LooseIsoTau_MET30_L1MET,process.HLT_DoubleIsoTau_Trk3,process.HLT_DoubleLooseIsoTau,process.HLT_IsoEle8_IsoMu7,process.HLT_IsoEle10_Mu10_L1R,process.HLT_IsoEle12_IsoTau_Trk3,process.HLT_IsoEle10_BTagIP_Jet35,process.HLT_IsoEle12_Jet40,process.HLT_IsoEle12_DoubleJet80,process.HLT_IsoEle5_TripleJet30,process.HLT_IsoEle12_TripleJet60,process.HLT_IsoEle12_QuadJet35,process.HLT_IsoMu14_IsoTau_Trk3,process.HLT_IsoMu7_BTagIP_Jet35,process.HLT_IsoMu7_BTagMu_Jet20,process.HLT_IsoMu7_Jet40,process.HLT_NoL2IsoMu8_Jet40,process.HLT_Mu14_Jet50,process.HLT_Mu5_TripleJet30,process.HLT_BTagMu_Jet20_Calib,process.HLT_ZeroBias,process.HLT_MinBias,process.HLT_MinBiasHcal,process.HLT_MinBiasEcal,process.HLT_MinBiasPixel,process.HLT_MinBiasPixel_Trk5,process.HLT_BackwardBSC,process.HLT_ForwardBSC,process.HLT_CSCBeamHalo,process.HLT_CSCBeamHaloOverlapRing1,process.HLT_CSCBeamHaloOverlapRing2,process.HLT_CSCBeamHaloRing2or3,process.HLT_TrackerCosmics,process.AlCa_IsoTrack,process.AlCa_EcalPhiSym,process.AlCa_EcalPi0,process.HLTriggerFinalPath,process.reconstruction,process.out_step)

process.schedule = cms.Schedule(process.generation_step,process.HLTriggerFirstPath,process.HLT_L1Jet15,process.HLT_Jet30,process.HLT_Jet50,process.HLT_Jet80,process.HLT_Jet110,process.HLT_Jet180,process.HLT_Jet250,process.HLT_FwdJet20,process.HLT_DoubleJet150,process.HLT_DoubleJet125_Aco,process.HLT_DoubleFwdJet50,process.HLT_DiJetAve15,process.HLT_DiJetAve30,process.HLT_DiJetAve50,process.HLT_DiJetAve70,process.HLT_DiJetAve130,process.HLT_DiJetAve220,process.HLT_TripleJet85,process.HLT_QuadJet30,process.HLT_QuadJet60,process.HLT_SumET120,process.HLT_L1MET20,process.HLT_MET25,process.HLT_MET35,process.HLT_MET50,process.HLT_MET65,process.HLT_MET75,process.HLT_MET65_HT350,process.HLT_Jet180_MET60,process.HLT_Jet60_MET70_Aco,process.HLT_Jet100_MET60_Aco,process.HLT_DoubleJet125_MET60,process.HLT_DoubleFwdJet40_MET60,process.HLT_DoubleJet60_MET60_Aco,process.HLT_DoubleJet50_MET70_Aco,process.HLT_DoubleJet40_MET70_Aco,process.HLT_TripleJet60_MET60,process.HLT_QuadJet35_MET60,process.HLT_IsoEle15_L1I,process.HLT_IsoEle18_L1R,process.HLT_IsoEle15_LW_L1I,process.HLT_LooseIsoEle15_LW_L1R,process.HLT_Ele10_SW_L1R,process.HLT_Ele15_SW_L1R,process.HLT_Ele15_LW_L1R,process.HLT_EM80,process.HLT_EM200,process.HLT_DoubleIsoEle10_L1I,process.HLT_DoubleIsoEle12_L1R,process.HLT_DoubleIsoEle10_LW_L1I,process.HLT_DoubleIsoEle12_LW_L1R,process.HLT_DoubleEle5_SW_L1R,process.HLT_DoubleEle10_LW_OnlyPixelM_L1R,process.HLT_DoubleEle10_Z,process.HLT_DoubleEle6_Exclusive,process.HLT_IsoPhoton30_L1I,process.HLT_IsoPhoton10_L1R,process.HLT_IsoPhoton15_L1R,process.HLT_IsoPhoton20_L1R,process.HLT_IsoPhoton25_L1R,process.HLT_IsoPhoton40_L1R,process.HLT_Photon15_L1R,process.HLT_Photon25_L1R,process.HLT_DoubleIsoPhoton20_L1I,process.HLT_DoubleIsoPhoton20_L1R,process.HLT_DoublePhoton10_Exclusive,process.HLT_L1Mu,process.HLT_L1MuOpen,process.HLT_L2Mu9,process.HLT_IsoMu9,process.HLT_IsoMu11,process.HLT_IsoMu13,process.HLT_IsoMu15,process.HLT_Mu3,process.HLT_Mu5,process.HLT_Mu7,process.HLT_Mu9,process.HLT_Mu11,process.HLT_Mu13,process.HLT_Mu15,process.HLT_Mu15_Vtx2mm,process.HLT_DoubleIsoMu3,process.HLT_DoubleMu3,process.HLT_DoubleMu3_Vtx2mm,process.HLT_DoubleMu3_JPsi,process.HLT_DoubleMu3_Upsilon,process.HLT_DoubleMu7_Z,process.HLT_DoubleMu3_SameSign,process.HLT_DoubleMu3_Psi2S,process.HLT_BTagIP_Jet180,process.HLT_BTagIP_Jet120_Relaxed,process.HLT_BTagIP_DoubleJet120,process.HLT_BTagIP_DoubleJet60_Relaxed,process.HLT_BTagIP_TripleJet70,process.HLT_BTagIP_TripleJet40_Relaxed,process.HLT_BTagIP_QuadJet40,process.HLT_BTagIP_QuadJet30_Relaxed,process.HLT_BTagIP_HT470,process.HLT_BTagIP_HT320_Relaxed,process.HLT_BTagMu_DoubleJet120,process.HLT_BTagMu_DoubleJet60_Relaxed,process.HLT_BTagMu_TripleJet70,process.HLT_BTagMu_TripleJet40_Relaxed,process.HLT_BTagMu_QuadJet40,process.HLT_BTagMu_QuadJet30_Relaxed,process.HLT_BTagMu_HT370,process.HLT_BTagMu_HT250_Relaxed,process.HLT_DoubleMu3_BJPsi,process.HLT_DoubleMu4_BJPsi,process.HLT_TripleMu3_TauTo3Mu,process.HLT_IsoTau_MET65_Trk20,process.HLT_IsoTau_MET35_Trk15_L1MET,process.HLT_LooseIsoTau_MET30,process.HLT_LooseIsoTau_MET30_L1MET,process.HLT_DoubleIsoTau_Trk3,process.HLT_DoubleLooseIsoTau,process.HLT_IsoEle8_IsoMu7,process.HLT_IsoEle10_Mu10_L1R,process.HLT_IsoEle12_IsoTau_Trk3,process.HLT_IsoEle10_BTagIP_Jet35,process.HLT_IsoEle12_Jet40,process.HLT_IsoEle12_DoubleJet80,process.HLT_IsoEle5_TripleJet30,process.HLT_IsoEle12_TripleJet60,process.HLT_IsoEle12_QuadJet35,process.HLT_IsoMu14_IsoTau_Trk3,process.HLT_IsoMu7_BTagIP_Jet35,process.HLT_IsoMu7_BTagMu_Jet20,process.HLT_IsoMu7_Jet40,process.HLT_NoL2IsoMu8_Jet40,process.HLT_Mu14_Jet50,process.HLT_Mu5_TripleJet30,process.HLT_BTagMu_Jet20_Calib,process.HLT_ZeroBias,process.HLT_MinBias,process.HLT_MinBiasHcal,process.HLT_MinBiasEcal,process.HLT_MinBiasPixel,process.HLT_MinBiasPixel_Trk5,process.HLT_BackwardBSC,process.HLT_ForwardBSC,process.HLT_CSCBeamHalo,process.HLT_CSCBeamHaloOverlapRing1,process.HLT_CSCBeamHaloOverlapRing2,process.HLT_CSCBeamHaloRing2or3,process.HLT_TrackerCosmics,process.AlCa_IsoTrack,process.AlCa_EcalPhiSym,process.AlCa_EcalPi0,process.HLTriggerFinalPath,process.reconstruction,process.p2)
