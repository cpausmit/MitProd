# Auto generated configuration file
# using: 
# Revision: 1.120 
# Source: /cvs_server/repositories/CMSSW/CMSSW/Configuration/PyReleaseValidation/python/ConfigBuilder.py,v 
# with command line options: testingconf -s RECO --conditions FrontierConditions_GlobalTag,IDEAL_31X::All
import FWCore.ParameterSet.Config as cms

process = cms.Process('FILLER')

# import of standard configurations
process.load('Configuration/StandardSequences/Services_cff')
process.load('FWCore/MessageService/MessageLogger_cfi')
#process.load('Configuration/StandardSequences/MixingNoPileUp_cff')
process.load('Configuration/StandardSequences/GeometryIdeal_cff')
process.load('Configuration/StandardSequences/MagneticField_38T_cff')
#process.load('Configuration/StandardSequences/Reconstruction_cff')
process.load('Configuration/StandardSequences/EndOfProcess_cff')
process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')
process.load('Configuration/EventContent/EventContent_cff')

process.configurationMetadata = cms.untracked.PSet(
    version = cms.untracked.string('$Revision: 1.4 $'),
    annotation = cms.untracked.string('testingconf nevts:1'),
    name = cms.untracked.string('PyReleaseValidation')
)
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1000)
)
process.options = cms.untracked.PSet(
   Rethrow = cms.untracked.vstring('ProductNotFound')
)
# Input source
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring('/store/relval/CMSSW_3_1_1/RelValTTbar/GEN-SIM-RECO/MC_31X_V2-v1/0002/CEC0E4DD-5D6B-DE11-91DB-001D09F2545B.root')
)

# Other statements
process.GlobalTag.globaltag = 'MC_31X_V2::All'




#Load MitTreeFiller 
process.TreeService = cms.Service("TreeService",
    fileNames = cms.untracked.vstring('XX-MITDATASET-XX'),
)
process.add_(cms.Service("ObjectService"))
process.load("MitProd.TreeFiller.MitTreeFiller_cfi")

#Load flavor history
process.load("PhysicsTools.HepMCCandAlgos.flavorHistoryProducer_cfi")
process.load("PhysicsTools.HepMCCandAlgos.flavorHistoryFilter_cfi")

#Load stablePart producers
process.load("MitEdm.Producers.conversionElectronsStable_cfi")
process.MitTreeFiller.ElectronsStable.active                = True
process.MitTreeFiller.ConversionInOutElectronsStable.active = True
process.MitTreeFiller.ConversionOutInElectronsStable.active = True
process.MitTreeFiller.GsfElectronsStable.active             = True
process.MitTreeFiller.MergedElectronsStable.active          = True

#Load Mit vProducer
process.load("MitProd.TreeFiller.vProducer_cff")

#Load Mit Mvf Conversion producer
process.load("MitProd.TreeFiller.conversionProducer_cff")

#For JetPlusTracks
process.load("JetMETCorrections.Configuration.JetPlusTrackCorrections_cff")
process.load("JetMETCorrections.Configuration.ZSPJetCorrections219_cff")
process.MitTreeFiller.IC5JetPlusTrack.active = True

#Load Flavor Matching Information
process.load("MitProd.TreeFiller.JetsMCFlavourMatching_cfi")
#Enable Flavor matching for Reco Jets and GenJets
process.MitTreeFiller.ItrCone5Jets.flavorMatchingActive   = True
process.MitTreeFiller.SisCone5Jets.flavorMatchingActive   = True
process.MitTreeFiller.SisCone7Jets.flavorMatchingActive   = True
process.MitTreeFiller.Kt4Jets.flavorMatchingActive        = True
process.MitTreeFiller.Kt6Jets.flavorMatchingActive        = True
process.MitTreeFiller.ItrCone5PFJets.flavorMatchingActive = True
process.MitTreeFiller.IC5GenJets.flavorMatchingActive     = True
process.MitTreeFiller.SC5GenJets.flavorMatchingActive     = True
process.MitTreeFiller.SC7GenJets.flavorMatchingActive     = True
process.MitTreeFiller.KT4GenJets.flavorMatchingActive     = True
process.MitTreeFiller.KT6GenJets.flavorMatchingActive     = True

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

# Additional output definition



# Path and EndPath definitions
#process.reconstruction_step = cms.Path(process.reconstruction)

process.p1 = cms.Path(
    process.bFlavorHistoryProducer*
    process.cFlavorHistoryProducer*
    process.flavorHistoryFilter*
    process.conversionElectronsStable*
    process.mvfConversionElectronsStable*
    process.vProducer *
    process.conversionProducer *
    process.ZSPJetCorrections*process.JetPlusTrackCorrections*
    process.caloJetMCFlavour*
    process.pfJetMCFlavour*
    process.extraPFJetMCFlavour*
    process.jetvertexAssociationSequence*
    process.MitTreeFiller
    process.vFiller *
    process.conversionFiller
)

process.endjob_step = cms.Path(process.endOfProcess)
#process.out_step = cms.EndPath(process.output)

# Schedule definition
#process.schedule = cms.Schedule(process.reconstruction_step,process.endjob_step,process.out_step)
process.schedule = cms.Schedule(process.p1,process.endjob_step)
