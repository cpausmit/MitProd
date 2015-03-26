import FWCore.ParameterSet.Config as cms

# To run and store the decisions of the MET filters

#> The iso-based HBHE noise filter
from CommonTools.RecoAlgos.HBHENoiseFilterResultProducer_cfi import *
#> The ECAL dead cell trigger primitive filter
from RecoMET.METFilters.EcalDeadCellTriggerPrimitiveFilter_cfi import *
EcalDeadCellTriggerPrimitiveFilter.taggingMode = cms.bool(True)

#> The EE bad SuperCrystal filter
from RecoMET.METFilters.eeBadScFilter_cfi import *
eeBadScFilter.taggingMode = cms.bool(True)

#> The ECAL laser correction filter
from RecoMET.METFilters.ecalLaserCorrFilter_cfi import *
ecalLaserCorrFilter.taggingMode = cms.bool(True)

#> The Good vertices collection needed by the tracking failure filter
from MitProd.TreeFiller.goodVertices_cff import *

#> The tracking failure filter
from RecoMET.METFilters.trackingFailureFilter_cfi import *
trackingFailureFilter.taggingMode = cms.bool(True)

#> The tracking POG filters: NB for these three false means good event
from RecoMET.METFilters.trackingPOGFilters_cff import *
manystripclus53X.taggedMode = cms.untracked.bool(True)
manystripclus53X.forcedValue = cms.untracked.bool(False)
toomanystripclus53X.taggedMode = cms.untracked.bool(True)
toomanystripclus53X.forcedValue = cms.untracked.bool(False)
logErrorTooManyClusters.taggedMode = cms.untracked.bool(True)
logErrorTooManyClusters.forcedValue = cms.untracked.bool(False)

#> Finally define the met filter sequence
metFilters = cms.Sequence(
  HBHENoiseFilterResultProducer *
  EcalDeadCellTriggerPrimitiveFilter *
  goodVertices *
  trackingFailureFilter *
  eeBadScFilter *
  ecalLaserCorrFilter *
  trkPOGFilters
)
