# $Id: BambuFillAOD_cfi.py,v 1.3 2009/11/05 21:40:14 bendavid Exp $

import FWCore.ParameterSet.Config as cms

from MitProd.BAMBUSequences.BambuFillRECO_cfi import *

#disable pixel hit/track producers/fillers
try:
  BambuRecoSequence.remove(siPixelRecHits)
except:
  pass
  
#disable jet plus tracks
try:
  BambuRecoSequence.remove(JetPlusTrackCorrectionsIcone5)
except:
  pass

try:
  BambuRecoSequence.remove(JetPlusTrackCorrectionsAntiKt5)
except:
  pass


MitTreeFiller.IC5JetPlusTrack.active = False
MitTreeFiller.AK5JetPlusTrack.active = False
  
MitTreeFiller.PixelHits.active            = False
MitTreeFiller.PixelVertexes.active        = False
MitTreeFiller.PixelTracks.active          = False

#Disable duplicate removal in StableParticle merging for conversions,
#since this requires tracking RecHits which aren't available in AOD
mergedConversionsStable.removeDuplicates = False
mergedConversionsGeneralStable.removeDuplicates = False
mergedElectronsStable.removeDuplicates = False

BambuFillAOD = cms.Sequence(BambuRecoSequence*BambuRecoFillSequence)
