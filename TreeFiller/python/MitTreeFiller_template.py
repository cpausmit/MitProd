# $Id: MitTreeFiller_template.py,v 1.10 2008/09/30 15:59:42 sixie Exp $
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

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(50) )
process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring(
   'file:/server/02a/sixie/RECO/001EA63A-DF60-DD11-9D5A-001A92810AA6.root'
                             ),
   secondaryFileNames = cms.untracked.vstring()
)

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

process.p1 = cms.Path(
     process.vProducer *
     process.conversionProducer *
    (  process.MitEIdSequence
     + process.MitMetCorrections
     + process.caloJetMCFlavour
     + process.jetvertexAssociationSequence
     )
    *process.MitTreeFiller
    *process.vFiller
    *process.conversionFiller
     )
