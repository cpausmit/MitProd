
import FWCore.ParameterSet.Config as cms

import HiggsAnalysis.HiggsToWW2Leptons.expectedHitsComputer_cfi

expectedHitsEle = HiggsAnalysis.HiggsToWW2Leptons.expectedHitsComputer_cfi.expectedHitsComputer.clone()
expectedHitsEle.inputColl   = cms.InputTag("gsfElectrons")
expectedHitsEle.useGsfTrack = cms.bool(True)

expectedHitsMu  = HiggsAnalysis.HiggsToWW2Leptons.expectedHitsComputer_cfi.expectedHitsComputer.clone()
expectedHitsMu.inputColl   = cms.InputTag("muons")
expectedHitsMu.useGsfTrack = cms.bool(False)

correctedExpectedHits = cms.Sequence(expectedHitsEle*expectedHitsMu)
