# Auto generated configuration file
# using: 
# Revision: 1.77.2.1 
# Source: /cvs_server/repositories/CMSSW/CMSSW/Configuration/PyReleaseValidation/python/ConfigBuilder.py,v 
# with command line options: MitTreeFiller_template_RAW --step=DIGI,RAW2DIGI,RECO --conditions=FrontierConditions_GlobalTag,IDEAL_V9::All --filein=/server/02a/bendavid/RAW/QCD_BCtoE_Pt30to80/002EE608-8682-DD11-AFD3-001CC47B1118.root --no_exec --eventcontent=RECOSIM
import FWCore.ParameterSet.Config as cms

process = cms.Process('RECO')

# import of standard configurations
process.load('Configuration/StandardSequences/Services_cff')
process.load('FWCore/MessageService/MessageLogger_cfi')
process.load('Configuration/StandardSequences/MixingNoPileUp_cff')
process.load('Configuration/StandardSequences/GeometryPilot2_cff')
process.load('Configuration/StandardSequences/MagneticField_38T_cff')
process.load('Configuration/StandardSequences/Digi_cff')
process.load('Configuration/StandardSequences/RawToDigi_cff')
process.load('Configuration/StandardSequences/Reconstruction_cff')
process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')
process.load('Configuration/EventContent/EventContent_cff')

process.configurationMetadata = cms.untracked.PSet(
    version = cms.untracked.string('$Revision: 1.77.2.1 $'),
    annotation = cms.untracked.string('MitTreeFiller_template_RAW nevts:1'),
    name = cms.untracked.string('PyReleaseValidation')
)
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(500)
)
process.options = cms.untracked.PSet(
    Rethrow = cms.untracked.vstring('ProductNotFound')
)
# Input source
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring('file:/server/02a/bendavid/RAW/QCD_BCtoE_Pt30to80/002EE608-8682-DD11-AFD3-001CC47B1118.root')
)

# Output definition
process.output = cms.OutputModule("PoolOutputModule",
    outputCommands = process.RECOSIMEventContent.outputCommands,
    fileName = cms.untracked.string('MitTreeFiller_template_RAW_DIGI_RAW2DIGI_RECO.root'),
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string(''),
        filterName = cms.untracked.string('')
    )
)

# Additional output definition

# Other statements
process.GlobalTag.globaltag = 'IDEAL_V9::All'

# Path and EndPath definitions
process.digitisation_step = cms.Path(process.pdigi)
process.raw2digi_step = cms.Path(process.RawToDigi)
process.reconstruction_step = cms.Path(process.reconstruction)
process.out_step = cms.EndPath(process.output)

#Load MitTreeFiller 
process.TreeService = cms.Service("TreeService",
    fileNames = cms.untracked.vstring('XX-MITDATASET-XX')
)
process.add_(cms.Service("ObjectService"))
process.load("MitProd.TreeFiller.MitTreeFiller_cfi")

#Load Mit vProducer
process.load("MitEdm.Producers.vProducer_cff")

#Load Mit Mvf Conversion producer
process.load("MitEdm.Producers.conversionProducer_cff")
#Load additional producer for conversions without conversion constraint
process.load("MitEdm.Producers.conversionProducerUnconstrained_cff")
#Load additional producer for conversions with 2d conversion constraint only
process.load("MitEdm.Producers.conversionProducer2d_cff")

# compute ECAL shower shape variables
process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Geometry.CaloEventSetup.CaloGeometry_cfi")
process.load("Geometry.CMSCommonData.cmsIdealGeometryXML_cfi")
process.load("Geometry.CaloEventSetup.CaloTopology_cfi")

process.load("FWCore.MessageLogger.MessageLogger_cfi")

#Load ElectronID information
process.load("MitProd.TreeFiller.ElectronID_cfi")

#For Jet Corrections
process.load("MitProd.TreeFiller.JetCorrections_cfi")
process.prefer("L3JetCorrectorIcone5")
#enable Jet Corrections for all of our Jet collections
process.MitTreeFiller.CaloJets.jetCorrectionsActive = cms.untracked.bool(True)
process.MitTreeFiller.ItrCone5Jets.jetCorrectionsActive = cms.untracked.bool(True)
process.MitTreeFiller.SisCone5Jets.jetCorrectionsActive = cms.untracked.bool(True)
process.MitTreeFiller.SisCone7Jets.jetCorrectionsActive = cms.untracked.bool(True)
process.MitTreeFiller.Kt4Jets.jetCorrectionsActive = cms.untracked.bool(True)
process.MitTreeFiller.Kt6Jets.jetCorrectionsActive = cms.untracked.bool(True)

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

#hit based track-simulation matching
process.load("MitProd.TreeFiller.MitPostRecoGenerator_cff")
process.MitTreeFiller.MCParticles.trackingActive=True

process.p1 = cms.Path(
     process.vProducer *
     process.conversionProducer *
     process.conversionProducerUnconstrained *
     process.conversionProducer2d *
    (  process.MitEIdSequence
     + process.MitMetCorrections
     + process.caloJetMCFlavour
     + process.jetvertexAssociationSequence
     )
    *process.mit_postreco_generator
    *process.MitTreeFiller
    *process.vFiller
    *process.conversionFiller
    *process.conversionFillerUnconstrained
    *process.conversionFiller2d
     )

# Schedule definition
process.schedule = cms.Schedule(process.digitisation_step,process.raw2digi_step,process.reconstruction_step,process.p1)
