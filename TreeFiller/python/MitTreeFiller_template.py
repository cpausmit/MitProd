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
process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring(
   'file:/server/02a/sixie/RECO/001EA63A-DF60-DD11-9D5A-001A92810AA6.root'
                             ),
   secondaryFileNames = cms.untracked.vstring()
)

# compute ECAL shower shape variables
process.load("Configuration.StandardSequences.Geometry_cff")
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

process.load("MitProd.TreeFiller.JetsMCFlavourMatching_cfi")

process.TreeService = cms.Service("TreeService",
    fileNames = cms.untracked.vstring('XX-MITDATASET-XX')
)

process.add_(cms.Service("ObjectService"))

process.load("MitProd.TreeFiller.MitTreeFiller_cfi")

process.p1 = cms.Path((process.caloJetMCFlavour + process.eIdSequence)*process.MitTreeFiller)