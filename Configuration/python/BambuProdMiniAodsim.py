import FWCore.ParameterSet.Config as cms

#---------------------------------------------------------------------------------------------------
#                                           M A I N
#---------------------------------------------------------------------------------------------------
# create the process
process = cms.Process('FILEFI')

# say how many events to process (-1 means no limit)
process.maxEvents = cms.untracked.PSet(
  #input = cms.untracked.int32(100)
  input = cms.untracked.int32(-1)
)

#>> input source

process.source = cms.Source(
  "PoolSource",
  fileNames = cms.untracked.vstring()
)
process.source.inputCommands = cms.untracked.vstring(
  "keep *"
)

#>> configurations

# determine the global tag to use
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.GlobalTag.globaltag = 'MCRUN2_74_V9::All'

# define meta data for this production
process.configurationMetadata = cms.untracked.PSet(
  name       = cms.untracked.string('BambuProd'),
  version    = cms.untracked.string('Mit_040'),
  annotation = cms.untracked.string('AODSIM')
)

#>> standard sequences

# load some standard sequences we will need
process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.GeometryDB_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('TrackingTools.TransientTrack.TransientTrackBuilder_cfi')

# define sequence for ProductNotFound
process.options = cms.untracked.PSet(
  Rethrow = cms.untracked.vstring('ProductNotFound'),
  fileMode = cms.untracked.string('NOMERGE'),
)

# Import/Load the filler so all is already available for config changes
from MitProd.TreeFiller.MitTreeFiller_cfi import *
process.load('MitProd.TreeFiller.MitTreeFiller_cfi')

from MitProd.TreeFiller.MitTreeFiller_MiniAOD_cff import configureForMiniAOD
configureForMiniAOD(MitTreeFiller)

#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#
#
#                               R E C O  S E Q U E N C E
#
#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#

## Load jet reco producers
from RecoJets.Configuration.RecoJets_cff import *
process.load('RecoJets.Configuration.RecoJets_cff')

## Load particle flow jet reco producers
from RecoJets.Configuration.RecoPFJets_cff import *
process.load('RecoJets.Configuration.RecoPFJets_cff')

# must come before loading l1FastJetSequence
kt6PFJets.src = 'packedPFCandidates'
kt6PFJetsCentralChargedPileUp.src = 'packedPFCandidates'
kt6PFJetsCentralNeutral.src = 'packedPFCandidates'
kt6PFJetsCentralNeutralTight.src = 'packedPFCandidates'

# Load FastJet L1 corrections
from MitProd.TreeFiller.FastJetCorrection_cff import *
process.load('MitProd.TreeFiller.FastJetCorrection_cff')

# Load PF CHS using PackedCandidates
from MitProd.TreeFiller.pfCHSFromPacked_cff import *
process.load('MitProd.TreeFiller.pfCHSFromPacked_cff')

# Load tracking off packedCandidates
from PhysicsTools.PatAlgos.slimming.unpackedTracksAndVertices_cfi import *
process.load('PhysicsTools.PatAlgos.slimming.unpackedTracksAndVertices_cfi')

#> The bambu reco sequence
recoSequence = cms.Sequence(
  unpackedTracksAndVertices *
  pfCHSSequence *
  kt6PFJets *
  kt6PFJetsCentralChargedPileUp *
  kt6PFJetsCentralNeutral *
  kt6PFJetsCentralNeutralTight *
  l1FastJetSequence
)

#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#
#
#                               G E N  S E Q U E N C E
#
#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#

# Import/Load genjets
from RecoJets.Configuration.GenJetParticles_cff import *
process.load('RecoJets.Configuration.GenJetParticles_cff')
from RecoJets.Configuration.RecoGenJets_cff import *
process.load('RecoJets.Configuration.RecoGenJets_cff')

genParticlesForJets.src = 'packedGenParticles'
genParticlesForJets.isMiniAOD = cms.bool(True)

genSequence = cms.Sequence(
  genParticlesForJets *
  ak4GenJets *
  ak8GenJets
)

#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#
#
#                               B A M B U  S E Q U E N C E
#
#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#

# remember the bambu sequence has been imported and loaded already in the beginning

# configure the filler
MitTreeFiller.TreeWriter.fileName = 'XX-MITDATASET-XX'
MitTreeFiller.PileupInfo.active = True
MitTreeFiller.MCParticles.active = True
MitTreeFiller.MCEventInfo.active = True
MitTreeFiller.MCVertexes.active = True

# define fill bambu filler sequence

bambuFillerSequence = cms.Sequence(
  MitTreeFiller
)

#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#
#
#                               C M S S W  P A T H
#
#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#

process.path = cms.Path(
  recoSequence *
  genSequence *
  bambuFillerSequence
)

process.schedule = cms.Schedule(process.path)
