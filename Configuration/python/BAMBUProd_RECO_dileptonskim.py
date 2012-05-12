# $Id: BAMBUProd_RECO_dileptonskim.py,v 1.3 2012/04/11 20:18:27 paus Exp $

import FWCore.ParameterSet.Config as cms

process = cms.Process('FIFSKM')

# import of standard configurations
process.load('Configuration/StandardSequences/Services_cff')
process.load('FWCore/MessageService/MessageLogger_cfi')
process.load('Configuration.StandardSequences.Reconstruction_cff')
process.load('Configuration.StandardSequences.GeometryDB_cff')
process.load('Configuration/StandardSequences/MagneticField_AutoFromDBCurrent_cff')
process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')
process.load('Configuration/EventContent/EventContent_cff')

process.configurationMetadata = cms.untracked.PSet(
  version    = cms.untracked.string('Mit_028'),
  annotation = cms.untracked.string('RECO'),
  name       = cms.untracked.string('BambuProduction')
)

process.maxEvents = cms.untracked.PSet(
  input = cms.untracked.int32(-1)
)

process.options = cms.untracked.PSet(
  Rethrow  = cms.untracked.vstring('ProductNotFound'),
  fileMode =  cms.untracked.string('NOMERGE'),
)

# input source
process.source = cms.Source("PoolSource",
  fileNames = cms.untracked.vstring('/store/data/Run2010B/Electron/RECO/PromptReco-v2/000/147/222/5E50236B-38D1-DF11-9889-003048F1110E.root')
)
process.source.inputCommands = cms.untracked.vstring("keep *",
                                                     "drop *_MEtoEDMConverter_*_*",
                                                     "drop L1GlobalTriggerObjectMapRecord_hltL1GtObjectMap__HLT")

# other statements
process.GlobalTag.globaltag = 'GR_R_52_V7::All'

process.add_(cms.Service("ObjectService"))

process.load("MitProd.BAMBUSequences.BambuFillRECO_cfi")

process.MitTreeFiller.TreeWriter.fileName = 'XX-MITDATASET-XX'

# skimming definitions
TM_ARBITRATION = "numberOfMatches('SegmentAndTrackArbitration')>0";
MUON_CUT       = "pt > 10 && (isGlobalMuon || (isTrackerMuon && "+TM_ARBITRATION+"))"

process.goodMuons = cms.EDFilter("MuonRefSelector",
                                 src = cms.InputTag("muons"),
                                 cut = cms.string(MUON_CUT),
)
process.goodElectrons = cms.EDFilter("GsfElectronRefSelector",
                                     src = cms.InputTag("gsfElectrons"),
                                     cut = cms.string("pt > 10"),
)
process.diMuons = cms.EDProducer("CandViewShallowCloneCombiner",
                                 decay       = cms.string("goodMuons goodMuons"),
                                 checkCharge = cms.bool(False),
                                 cut         = cms.string("mass > 0"),
)
process.diElectrons = cms.EDProducer("CandViewShallowCloneCombiner",
                                     decay       = cms.string("goodElectrons goodElectrons"),
                                     checkCharge = cms.bool(False),
                                     cut         = cms.string("mass > 0"),
)
process.crossLeptons  = cms.EDProducer("CandViewShallowCloneCombiner",
                                       decay       = cms.string("goodMuons goodElectrons"),
                                       checkCharge = cms.bool(False),
                                       cut         = cms.string("mass > 0"),
)

process.objects = cms.Sequence(
    ( process.goodMuons     +
      process.goodElectrons ) *
    ( process.diMuons       +
      process.diElectrons   +
      process.crossLeptons  )
)

process.diMuonsFilter      = cms.EDFilter("CandViewCountFilter", src = cms.InputTag('diMuons'), minNumber = cms.uint32(1));
process.diElectronsFilter  = cms.EDFilter("CandViewCountFilter", src = cms.InputTag('diElectrons'), minNumber = cms.uint32(1));
process.crossLeptonsFilter = cms.EDFilter("CandViewCountFilter", src = cms.InputTag('crossLeptons'), minNumber = cms.uint32(1));

process.bambu_stepa  = cms.Path(process.objects*process.diMuonsFilter*process.BambuFillRECO)
process.bambu_stepb  = cms.Path(process.objects*process.diElectronsFilter*process.BambuFillRECO)
process.bambu_stepc  = cms.Path(process.objects*process.crossLeptonsFilter*process.BambuFillRECO)

# schedule definition
process.schedule = cms.Schedule(process.bambu_stepa,process.bambu_stepb,process.bambu_stepc)
