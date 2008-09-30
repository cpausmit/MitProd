#---------------------------------------------------------------------------------------------------
# This template config file is intended to be a reference for the "HEAD" OAK tree version.
# This config file will be used by the mitprod account to do production on CRAB. It must
# be ensured that this config file is always working with the production CMSSW release
#---------------------------------------------------------------------------------------------------
# List of paramters to be properly replaced
#
#  - XX-MITDATASET-XX - MIT type dataset name (ex. csa08-1ipb-jpsi)
#
#---------------------------------------------------------------------------------------------------

import FWCore.ParameterSet.Config as cms
process = cms.Process("FILLER")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(20) )
process.options = cms.untracked.PSet(
    Rethrow = cms.untracked.vstring('ProductNotFound')
)
# Example of how to run on RAW and RECO simultaneously (files listed below do not correspond!)
#process.source = cms.Source("PoolSource",
                            #fileNames = cms.untracked.vstring(
   #'file:/server/02a/bendavid/RECO/0079F3C0-AF75-DD11-8EF5-003048772390.root'
                             #),
                            #secondaryFileNames = cms.untracked.vstring(
   #'file:/server/02a/bendavid/RAW/10A05094-446F-DD11-B8F3-00304865C360.root'
                             #),
#)

process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring(
   'file:/server/02a/sixie/RECO/001EA63A-DF60-DD11-9D5A-001A92810AA6.root'
                             ),
                            secondaryFileNames = cms.untracked.vstring()
)

# compute ECAL shower shape variables
process.load('Configuration/StandardSequences/MagneticField_38T_cff')
process.load("Configuration.StandardSequences.Geometry_cff")

process.load('Configuration/StandardSequences/Services_cff')
process.load('Configuration/StandardSequences/Digi_cff')
process.load('Configuration/StandardSequences/MixingNoPileUp_cff')
process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')

process.GlobalTag.globaltag = 'IDEAL_V9::All'


process.load("Geometry.CaloEventSetup.CaloGeometry_cfi")
process.load("Geometry.CMSCommonData.cmsIdealGeometryXML_cfi")
process.load("Geometry.CaloEventSetup.CaloTopology_cfi")

process.load("FWCore.MessageLogger.MessageLogger_cfi")

process.load( "RecoEgamma.ElectronIdentification.electronIdCutBased_cfi")
process.load( "RecoEgamma.ElectronIdentification.electronIdCutBasedClasses_cfi")
process.load( "RecoEgamma.ElectronIdentification.electronIdLikelihood_cfi")
process.load( "RecoEgamma.ElectronIdentification.electronIdNeuralNet_cfi")
process.load( "RecoEgamma.ElectronIdentification.electronIdCutBasedExt_cfi")
process.load( "RecoEgamma.ElectronIdentification.electronIdCutBasedClassesExt_cfi")
process.load( "RecoEgamma.ElectronIdentification.electronIdLikelihoodExt_cfi")
process.load( "RecoEgamma.ElectronIdentification.electronIdNeuralNetExt_cfi")


process.eIdSequence = cms.Sequence( process.eidCutBased +
                                    process.eidCutBasedExt +
                                    process.eidCutBasedClasses +
                                    process.eidCutBasedClassesExt +
                                    process.eidLikelihood +
                                    process.eidLikelihoodExt + 
                                    process.eidNeuralNet +
                                    process.eidNeuralNetExt)

#Load Flavor Matching Information
process.load("MitProd.TreeFiller.JetsMCFlavourMatching_cfi")

# setup MET muon corrections
process.load("JetMETCorrections.Type1MET.MetMuonCorrections_cff")                                          
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Geometry.CommonDetUnit.globalTrackingGeometry_cfi")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.GlobalTag.globaltag="IDEAL_V9::All"
process.load("Geometry.CommonDetUnit.bareGlobalTrackingGeometry_cfi")
process.load("TrackingTools.TrackAssociator.default_cfi")                                                   
process.load("TrackingTools.TrackAssociator.DetIdAssociatorESProducer_cff")  

# setup Type1 MET corrections
process.load("MitProd.TreeFiller.MetType1Corrections_cfi")
# Iterative Cone 0.5
process.corMetType1Icone5.inputUncorMetLabel  = cms.string('corMetGlobalMuons')
process.corMetType1Icone5.corrector           = cms.string('L2L3JetCorrectorIcone5')
# SIS Cone 0.5
process.corMetType1Scone5.inputUncorMetLabel = cms.string('corMetGlobalMuons')
process.corMetType1Scone5.corrector          = cms.string('L2L3JetCorrectorScone5') 
# SIS Cone 0.7
process.corMetType1Scone7.inputUncorMetLabel = cms.string('corMetGlobalMuons')
process.corMetType1Scone7.corrector          = cms.string('L2L3JetCorrectorScone7') 
# kT 0.4
process.corMetType1Kt4.inputUncorMetLabel = cms.string('corMetGlobalMuons')
process.corMetType1Kt4.corrector          = cms.string('L2L3JetCorrectorKt4') 
# kT 0.6
process.corMetType1Kt6.inputUncorMetLabel = cms.string('corMetGlobalMuons')
process.corMetType1Kt6.corrector          = cms.string('L2L3JetCorrectorKt6') 


#produce jet vertex association information
process.load("MitProd.TreeFiller.JetVertexAssociation_cfi")

#For Jet Corrections
process.load("MitProd.TreeFiller.JetCorrections_cfi")
process.prefer("L3JetCorrectorIcone5") 

process.TreeService = cms.Service("TreeService",
    fileNames = cms.untracked.vstring('mit-match')
)
process.add_(cms.Service("ObjectService"))

process.load("MitProd.TreeFiller.MitPostRecoGenerator_cff")
process.load("MitProd.TreeFiller.MitTreeFiller_cfi")
process.MitTreeFiller.MCParticles.trackingActive=True

process.p1 = cms.Path(
     process.pdigi
    *(process.caloJetMCFlavour
     + process.eIdSequence
     + (process.MetMuonCorrections * process.MetType1Corrections)
     + process.jetvertexAssociationSequence
     )
    *process.mit_postreco_generator
    *process.MitTreeFiller)