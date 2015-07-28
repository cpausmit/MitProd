import FWCore.ParameterSet.Config as cms

#---------------------------------------------------------------------------------------------------
#                                           M A I N
#---------------------------------------------------------------------------------------------------
# create the process
process = cms.Process('FILEFI')

# say how many events to process (-1 means no limit)
process.maxEvents = cms.untracked.PSet(
  input = cms.untracked.int32(-1)
)

#>> input source

process.source = cms.Source(
  "PoolSource",
  fileNames = cms.untracked.vstring('root://xrootd.unl.edu//store/data/Run2015A/DoubleMuon/AOD/PromptReco-v1/000/248/036/00000/62D24D3F-5714-E511-B9D7-02163E011913.root')
)
process.source.inputCommands = cms.untracked.vstring(
  "keep *",
  "drop *_MEtoEDMConverter_*_*",
  "drop L1GlobalTriggerObjectMapRecord_hltL1GtObjectMap__HLT"
)

#>> configurations

# determine the global tag to use
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
process.GlobalTag.globaltag = 'GR_P_V56'

# define meta data for this production
process.configurationMetadata = cms.untracked.PSet(
  name       = cms.untracked.string('BambuProd'),
  version    = cms.untracked.string('Mit_042'),
  annotation = cms.untracked.string('AOD')
)

#>> standard sequences

# load some standard sequences we will need
process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.GeometryDB_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.EventContent.EventContent_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('RecoVertex.PrimaryVertexProducer.OfflinePrimaryVertices_cfi')
process.load('TrackingTools.TransientTrack.TransientTrackBuilder_cfi')

# define sequence for ProductNotFound
process.options = cms.untracked.PSet(
  Rethrow = cms.untracked.vstring('ProductNotFound'),
  fileMode = cms.untracked.string('NOMERGE'),
  wantSummary = cms.untracked.bool(False),
  allowUnscheduled = cms.untracked.bool(True)
)

# Import/Load the filler so all is already available for config changes
from MitProd.TreeFiller.MitTreeFiller_cfi import MitTreeFiller
process.load('MitProd.TreeFiller.MitTreeFiller_cfi')

#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#
#
#                               R E C O  S E Q U E N C E
#
#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#

## Load stablePart producers
from MitEdm.Producers.conversionElectronsStable_cfi import electronsStable
process.load('MitEdm.Producers.conversionElectronsStable_cfi')

# Load Mit Mvf Conversion producer
# MultiVertexFitter is currently broken
#from MitProd.TreeFiller.conversionProducer_cff import conversionProducer, addConversionFiller
#process.load('MitProd.TreeFiller.conversionProducer_cff')
#addConversionFiller(MitTreeFiller)

# Electron likelihood-based id
from RecoEgamma.ElectronIdentification.electronIdLikelihoodExt_cfi import eidLikelihoodExt
process.load('RecoEgamma.ElectronIdentification.electronIdLikelihoodExt_cfi')
MitTreeFiller.Electrons.eIDLikelihoodName = 'eidLikelihoodExt'

# Load FastJet L1 corrections
from MitProd.TreeFiller.FastJetCorrection_cff import l1FastJetSequence, l1FastJetSequenceCHS
process.load('MitProd.TreeFiller.FastJetCorrection_cff')

# Load btagging
from MitProd.TreeFiller.utils.setupBTag import setupBTag
ak4PFBTagSequence = setupBTag(process, 'ak4PFJets', 'AKt4PF')
ak4PFCHSBTagSequence = setupBTag(process, 'ak4PFJetsCHS', 'AKt4PFCHS')

# Load basic particle flow collections
# Used for rho calculation
from CommonTools.ParticleFlow.goodOfflinePrimaryVertices_cfi import goodOfflinePrimaryVertices
from CommonTools.ParticleFlow.pfParticleSelection_cff import pfParticleSelectionSequence, pfPileUp, pfNoPileUp, pfPileUpIso, pfNoPileUpIso
from CommonTools.ParticleFlow.pfPhotons_cff import pfPhotonSequence
from CommonTools.ParticleFlow.pfElectrons_cff import pfElectronSequence
from CommonTools.ParticleFlow.pfMuons_cff import pfMuonSequence
from CommonTools.ParticleFlow.pfJets_cff import pfJets
from CommonTools.ParticleFlow.TopProjectors.pfNoMuon_cfi import pfNoMuon
from CommonTools.ParticleFlow.TopProjectors.pfNoElectron_cfi import pfNoElectron

process.load('CommonTools.ParticleFlow.goodOfflinePrimaryVertices_cfi')
process.load('CommonTools.ParticleFlow.pfParticleSelection_cff')
process.load('CommonTools.ParticleFlow.pfPhotons_cff')
process.load('CommonTools.ParticleFlow.pfElectrons_cff')
process.load('CommonTools.ParticleFlow.pfMuons_cff')
process.load('CommonTools.ParticleFlow.TopProjectors.pfNoMuon_cfi')
process.load('CommonTools.ParticleFlow.TopProjectors.pfNoElectron_cfi')

# Loading PFProducer to get the ptrs
from RecoParticleFlow.PFProducer.pfLinker_cff import particleFlowPtrs
process.load('RecoParticleFlow.PFProducer.pfLinker_cff')

# Load btagging
# recluster fat jets, subjets, btagging
from MitProd.TreeFiller.pfCHSFromPatJets_cff import makeFatJets
fatjetSequence = makeFatJets(process, True)

pfPileUp.PFCandidates = 'particleFlowPtrs'
pfNoPileUp.bottomCollection = 'particleFlowPtrs'
pfPileUpIso.PFCandidates = 'particleFlowPtrs'
pfNoPileUpIso.bottomCollection='particleFlowPtrs'

pfPileUp.Enable = True
pfPileUp.Vertices = 'goodOfflinePrimaryVertices'
pfPileUp.checkClosestZVertex = cms.bool(False)

#> Setup jet corrections
process.load('JetMETCorrections.Configuration.JetCorrectionServices_cff')

#> Setup the met filters
from MitProd.TreeFiller.metFilters_cff import metFilters
process.load('MitProd.TreeFiller.metFilters_cff')

# these are being loaded due to allowUnscheduled, but do not want
del(process.tobtecfakesfilter)
del(process.particleFlow)

#> The bambu reco sequence
recoSequence = cms.Sequence(
  electronsStable *
  eidLikelihoodExt *
#  conversionProducer *
  goodOfflinePrimaryVertices *
  inclusiveVertexing *
  inclusiveCandidateVertexing *
  particleFlowPtrs *
  pfParticleSelectionSequence *
  pfPhotonSequence *
  pfMuonSequence *
  pfNoMuon *
  pfElectronSequence *
  pfNoElectron *
  l1FastJetSequence *
  l1FastJetSequenceCHS *
  ak4PFBTagSequence *
  ak4PFCHSBTagSequence *
  fatjetSequence *
  metFilters
)

#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#
#
#                               G E N  S E Q U E N C E
#
#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#

# this is data, so nothing here

#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#
#
#                               B A M B U  S E Q U E N C E
#
#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#

# remember the bambu sequence has been imported and loaded already in the beginning

# configure the filler
MitTreeFiller.TreeWriter.fileName = 'bambu-output-file-tmp'
# remove Monte Carlo information
MitTreeFiller.PileupInfo.active = False
MitTreeFiller.MCParticles.active = False
MitTreeFiller.MCEventInfo.active = False
MitTreeFiller.MCVertexes.active = False

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
  bambuFillerSequence
)

process.schedule = cms.Schedule(process.path)
process.prune()
