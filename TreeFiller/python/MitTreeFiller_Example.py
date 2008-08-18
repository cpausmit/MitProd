import FWCore.ParameterSet.Config as cms

process = cms.Process("MitFillerTest")

#this is used to compute ECAL shower shape variables
process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Geometry.CaloEventSetup.CaloGeometry_cfi")
process.load("Geometry.CMSCommonData.cmsIdealGeometryXML_cfi")
process.load("Geometry.CaloEventSetup.CaloTopology_cfi")

process.source = cms.Source("PoolSource",   
    fileNames = cms.untracked.vstring('file:testfile.root')
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(100)
)

process.load("FWCore.MessageLogger.MessageLogger_cfi")

process.TreeService = cms.Service("TreeService",
    fileNames = cms.untracked.vstring('mit-filler-RelVal210_Zee')
)

process.add_(cms.Service("ObjectService"))

process.load("MitProd.TreeFiller.MitTreeFiller_cfi")

process.p1 = cms.Path(process.MitTreeFiller)
