import FWCore.ParameterSet.Config as cms

# To run and store the decisions of the MET filters

#> The iso-based HBHE noise filter
from CommonTools.RecoAlgos.HBHENoiseFilterResultProducer_cfi import HBHENoiseFilterResultProducer
# no filtering on the largest number of zeros found in a single RBX in the event
HBHENoiseFilterResultProducer.minZeros = cms.int32(99999)
HBHENoiseFilterResultProducer.IgnoreTS4TS5ifJetInLowBVRegion = cms.bool(False) 
HBHENoiseFilterResultProducer.defaultDecision = cms.string("HBHENoiseFilterResultRun2Loose")

#> The ECAL dead cell trigger primitive filter
from RecoMET.METFilters.EcalDeadCellTriggerPrimitiveFilter_cfi import EcalDeadCellTriggerPrimitiveFilter
EcalDeadCellTriggerPrimitiveFilter.taggingMode = cms.bool(True)

#> The ECAL dead cell boundary energy filter
from RecoMET.METFilters.EcalDeadCellBoundaryEnergyFilter_cfi import EcalDeadCellBoundaryEnergyFilter
EcalDeadCellBoundaryEnergyFilter.taggingMode = cms.bool(True)

#> The EE bad SuperCrystal filter
from RecoMET.METFilters.eeBadScFilter_cfi import eeBadScFilter
eeBadScFilter.badscEE = [-1023023, 1048098, -1078063, -1043093]
eeBadScFilter.taggingMode = cms.bool(True)

from RecoMET.METFilters.CSCTightHalo2015Filter_cfi import CSCTightHalo2015Filter
CSCTightHalo2015Filter.taggingMode = cms.bool(True)

from RecoMET.METFilters.globalTightHalo2016Filter_cfi import globalTightHalo2016Filter
globalTightHalo2016Filter.taggingMode = cms.bool(True)

from RecoMET.METFilters.globalSuperTightHalo2016Filter_cfi import globalSuperTightHalo2016Filter
globalSuperTightHalo2016Filter.taggingMode = cms.bool(True)

from RecoMET.METFilters.chargedHadronTrackResolutionFilter_cfi import chargedHadronTrackResolutionFilter
chargedHadronTrackResolutionFilter.taggingMode = cms.bool(True)

from RecoMET.METFilters.muonBadTrackFilter_cfi import muonBadTrackFilter
muonBadTrackFilter.taggingMode = cms.bool(True)

from RecoMET.METFilters.BadPFMuonFilter_cfi import BadPFMuonFilter
BadPFMuonFilter.taggingMode = cms.bool(True)

from RecoMET.METFilters.BadChargedCandidateFilter_cfi import BadChargedCandidateFilter
BadChargedCandidateFilter.taggingMode = cms.bool(True)

#> Finally define the met filter sequence
metFilters = cms.Sequence(
    HBHENoiseFilterResultProducer *
    EcalDeadCellTriggerPrimitiveFilter *
    EcalDeadCellBoundaryEnergyFilter *
    eeBadScFilter *
    CSCTightHalo2015Filter *
    globalTightHalo2016Filter *
    globalSuperTightHalo2016Filter *
    chargedHadronTrackResolutionFilter *
    muonBadTrackFilter *
    BadPFMuonFilter *
    BadChargedCandidateFilter
)
