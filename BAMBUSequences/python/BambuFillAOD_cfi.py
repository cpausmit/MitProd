import FWCore.ParameterSet.Config as cms

# first load everyhting that can be done even on RECO
from MitProd.BAMBUSequences.BambuFillRECO_cfi import *

# now remove what cannot be done on AOD
try:
  BambuRecoSequence.remove(mergedConversionsStable)
except:
  pass
try:
  BambuRecoSequence.remove(mergedConversionsGeneralStable)
except:
  pass
try:
  BambuRecoSequence.remove(mergedElectronsStable)
except:
  pass
try:
  BambuRecoSequence.remove(kShProducer)
except:
  pass
try:
  BambuRecoSequence.remove(conversionProducer)
except:
  pass

MitTreeFiller.IC5JetPlusTrack.active = False
MitTreeFiller.AK5JetPlusTrack.active = False
MitTreeFiller.ItrCone5Jets.active    = False
MitTreeFiller.ItrCone5PFJets.active  = False
MitTreeFiller.Kt4TrackJets.active    = False
MitTreeFiller.PFTaus.active          = False

try:
  MitTreeFiller.MergedElectronsStable.active = False
except:
  pass 
try:
  MitTreeFiller.PisStable.active = False
except:
  pass
try:
  MitTreeFiller.Ksh2PiPi.active = False
except:
  pass
try:
  MitTreeFiller.MvfConversions.active = False
except:
  pass
try:
  MitTreeFiller.MvfTrackerConversions.active = False
except:
  pass
try:
  MitTreeFiller.MvfConversionRemoval.active = False
except:
  pass

MitTreeFiller.Electrons.expectedHitsName = ''
MitTreeFiller.Muons.expectedHitsName = ''

BambuFillAOD = cms.Sequence(BambuRecoSequence*BambuRecoFillSequence)
