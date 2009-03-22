# $Id: MitTreeFiller_template_AOD.py,v 1.7 2009/03/22 08:49:00 loizides Exp $
#---------------------------------------------------------------------------------------------------
# This template config file is intended to be a reference for the "HEAD" bambu tree version.
# This config file will be used by the mitprod account to do production on CRAB. It must
# be ensured that this config file is always working with the production CMSSW release.
#---------------------------------------------------------------------------------------------------
# List of paramters to be properly replaced
#
#  - XX-MITDATASET-XX - MIT type dataset name (ex. csa08-1ipb-jpsi)
#
#---------------------------------------------------------------------------------------------------

import FWCore.ParameterSet.Config as cms
process = cms.Process("FILLER")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(390) )
process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring(
   'file:/server/02a/bendavid/RECO/Zjets-madgraph_Winter09_IDEAL_V11_FastSim_v1/0405D546-3ED1-DD11-8CF9-003048322C3A.root'
                             ),
   secondaryFileNames = cms.untracked.vstring()
)

#Load MitTreeFiller 
process.TreeService = cms.Service("TreeService",
    fileNames = cms.untracked.vstring('XX-MITDATASET-XX'),
)
process.add_(cms.Service("ObjectService"))
process.load("MitProd.TreeFiller.MitTreeFiller_cfi")

#Reconfigure treefiller defaults for AOD content:
process.MitTreeFiller.MCParticles.simActive        = False
process.MitTreeFiller.ConversionInOutTracks.active = False
process.MitTreeFiller.ConversionOutInTracks.active = False
process.MitTreeFiller.ConversionElectrons.active   = False
process.MitTreeFiller.Conversions.active           = False
process.MitTreeFiller.SisCone5PFJets.active        = False
process.MitTreeFiller.SisCone7PFJets.active        = False
process.MitTreeFiller.Kt4PFJets.active             = False
process.MitTreeFiller.Kt6PFJets.active             = False
process.MitTreeFiller.PFGsfTracks.active           = False

#Load Mit vProducer
process.load("MitProd.TreeFiller.vProducerNoRefit_cff")

process.load("FWCore.MessageLogger.MessageLogger_cfi")

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

#Load MHT sequence
process.load("MitProd.TreeFiller.MitMHT_cfi")

#load track-corrected MET module
process.load("RecoMET.METProducers.TCMET_cfi")

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

process.p1 = cms.Path(
    process.vProducer *
    process.eleIsoFromDeposits *
    (  process.MitEIdSequence
     + process.MitMetCorrections
     + process.caloJetMCFlavour
     + process.pfJetMCFlavour
     + process.jetvertexAssociationSequence
     + process.ZSPJetCorrections*process.JetPlusTrackCorrections
     + process.correctedJets
     + process.MitMHT
     + process.tcMet
    ) *
    process.MitTreeFiller *
    process.vFiller
)
