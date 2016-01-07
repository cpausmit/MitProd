import FWCore.ParameterSet.Config as cms

# To run and store the decisions of the MET filters

#> The iso-based HBHE noise filter
from CommonTools.RecoAlgos.HBHENoiseFilterResultProducer_cfi import *
# no filtering on the largest number of zeros found in a single RBX in the event
HBHENoiseFilterResultProducer.minZeros = cms.int32(99999)
HBHENoiseFilterResultProducer.IgnoreTS4TS5ifJetInLowBVRegion = cms.bool(False) 
HBHENoiseFilterResultProducer.defaultDecision = cms.string("HBHENoiseFilterResultRun2Loose")

#> The ECAL dead cell trigger primitive filter
from RecoMET.METFilters.EcalDeadCellTriggerPrimitiveFilter_cfi import *
EcalDeadCellTriggerPrimitiveFilter.taggingMode = cms.bool(True)

#> The ECAL dead cell boundary energy filter
from RecoMET.METFilters.EcalDeadCellBoundaryEnergyFilter_cfi import *
EcalDeadCellBoundaryEnergyFilter.taggingMode = cms.bool(True)

#> The EE bad SuperCrystal filter
from RecoMET.METFilters.eeBadScFilter_cfi import *
eeBadScFilter.badscEE = [-1023023, 1048098, -1078063, -1043093]
eeBadScFilter.taggingMode = cms.bool(True)

#> The Good vertices collection needed by the tracking failure filter
from RecoMET.METFilters.metFilters_cff import goodVertices

from RecoMET.METFilters.CSCTightHaloFilter_cfi import *
CSCTightHaloFilter.taggingMode = cms.bool(True)

#> Finally define the met filter sequence
metFilters = cms.Sequence(
  HBHENoiseFilterResultProducer *
  EcalDeadCellTriggerPrimitiveFilter *
  EcalDeadCellBoundaryEnergyFilter *
#  goodVertices *
  eeBadScFilter *
  CSCTightHaloFilter
)
