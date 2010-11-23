# $Id: BambuFillAOD_cfi.py,v 1.7 2010/11/22 16:56:12 bendavid Exp $

import FWCore.ParameterSet.Config as cms

from MitProd.BAMBUSequences.BambuFillRECO_cfi import *


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
try:
  BambuRecoSequence.remove(eidLikelihoodExt)
except:
  pass

MitTreeFiller.Electrons.eIDLikelihoodName = ''


MitTreeFiller.IC5JetPlusTrack.active = False
MitTreeFiller.AK5JetPlusTrack.active = False
MitTreeFiller.ItrCone5Jets.active = False
MitTreeFiller.ItrCone5PFJets.active = False
MitTreeFiller.Kt4TrackJets.active = False
MitTreeFiller.PFTaus.active = False
  
#MitTreeFiller.ElectronsStable.active = False
#MitTreeFiller.ConversionInOutElectronsStable.active = False
#MitTreeFiller.ConversionOutInElectronsStable.active = False
#MitTreeFiller.GsfElectronsStable.active = False
MitTreeFiller.MergedElectronsStable.active = False

MitTreeFiller.PisStable.active = False
MitTreeFiller.Ksh2PiPi.active = False
MitTreeFiller.MvfConversions.active = False
MitTreeFiller.MvfTrackerConversions.active = False
MitTreeFiller.MvfConversionRemoval.active = False


#Disable duplicate removal in StableParticle merging for conversions,
#since this requires tracking RecHits which aren't available in AOD
#mergedConversionsStable.removeDuplicates = False
#mergedConversionsGeneralStable.removeDuplicates = False
#mergedElectronsStable.removeDuplicates = False

MitTreeFiller.Electrons.expectedHitsName = ''
MitTreeFiller.Muons.expectedHitsName = ''

BambuFillAOD = cms.Sequence(BambuRecoSequence*BambuRecoFillSequence)
