# $Id: MitTreeFiller_template.py,v 1.18 2009/03/03 21:34:18 bendavid Exp $
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

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1000) )
process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring(
   'file:/server/02a/bendavid/RECO/Zmumu-IDEAL_V11_redigi_V1/3A9FC4B3-7FCC-DD11-BB45-003048673F9E.root'
                             ),
   secondaryFileNames = cms.untracked.vstring()
)

#Load MitTreeFiller 
process.TreeService = cms.Service("TreeService",
    fileNames = cms.untracked.vstring('XX-MITDATASET-XX'),
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

#For Jet Corrections (Summer08 redigi-rereco Jet corrections)
process.load("JetMETCorrections.Configuration.L2L3Corrections_Summer08Redigi_cff")
process.prefer("L3JetCorrectorIC5Calo")

#enable Jet Corrections for all of our Jet collections
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

#Load Flavor Matching Information
process.load("MitProd.TreeFiller.JetsMCFlavourMatching_cfi")
#Enable Flavor matching for Reco Jets and GenJets
process.MitTreeFiller.ItrCone5Jets.flavorMatchingActive = True
process.MitTreeFiller.SisCone5Jets.flavorMatchingActive = True
process.MitTreeFiller.SisCone7Jets.flavorMatchingActive = True
process.MitTreeFiller.Kt4Jets.flavorMatchingActive      = True
process.MitTreeFiller.Kt6Jets.flavorMatchingActive      = True
process.MitTreeFiller.IC5GenJets.flavorMatchingActive   = True
process.MitTreeFiller.SC5GenJets.flavorMatchingActive   = True
process.MitTreeFiller.SC7GenJets.flavorMatchingActive   = True
process.MitTreeFiller.KT4GenJets.flavorMatchingActive   = True
process.MitTreeFiller.KT6GenJets.flavorMatchingActive   = True

#produce jet vertex association information
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

process.p1 = cms.Path(
    process.gsfTrackAssociator *
    #process.vProducer *
    #process.conversionProducer *
    (  process.MitEIdSequence
     + process.MitMetCorrections
     + process.caloJetMCFlavour
     + process.jetvertexAssociationSequence
     + process.ZSPJetCorrections*process.JetPlusTrackCorrections
     )
    *process.MitTreeFiller
    #*process.vFiller
    #*process.conversionFiller
     )
