import FWCore.ParameterSet.Config as cms

process = cms.Process("MitFillerTest")

#this is used to compute ECAL shower shape variables
process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Geometry.CaloEventSetup.CaloGeometry_cfi")
process.load("Geometry.CMSCommonData.cmsIdealGeometryXML_cfi")
process.load("Geometry.CaloEventSetup.CaloTopology_cfi")


process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(100) )
process.source = cms.Source ("PoolSource",
    fileNames = cms.untracked.vstring (
        '/store/data/BeamCommissioning08/BeamHalo/RECO/v1/000/062/234/0C9980ED-6E80-DD11-BC8E-000423D985E4.root'
    ),
    secondaryFileNames = cms.untracked.vstring (
    )
)

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

#produce corrected MET objects
process.load("JetMETCorrections.Configuration.MCJetCorrections152_cff") 
process.load("JetMETCorrections.Type1MET.MetType1Corrections_cff")

process.load("MitProd.TreeFiller.JetCorrections_cfi")
process.prefer("L3JetCorrectorIcone5") 

process.TreeService = cms.Service("TreeService",
    fileNames = cms.untracked.vstring('BeamCommisioning_test')
)

process.add_(cms.Service("ObjectService"))

process.load("MitProd.TreeFiller.MitTreeFiller_BeamHalo_cfi")

process.p1 = cms.Path(
    (
     process.corMetType1Icone5
    )*process.MitTreeFiller)
