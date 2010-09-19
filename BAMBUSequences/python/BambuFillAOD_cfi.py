# $Id: BambuFillAOD_cfi.py,v 1.4 2010/05/30 15:28:58 bendavid Exp $

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

try:
  BambuRecoSequence.remove(jetVertexAlphaIC5)
except:
  pass

try:
  BambuRecoSequence.remove(jetVertexBetaIC5)
except:
  pass

MitTreeFiller.IC5JetPlusTrack.active = False
MitTreeFiller.AK5JetPlusTrack.active = False
MitTreeFiller.ItrCone5Jets.active = False
MitTreeFiller.ItrCone5PFJets.active = False
MitTreeFiller.Kt4TrackJets.active = False
MitTreeFiller.BarrelBasicClusters.active = False
MitTreeFiller.EndcapBasicClusters.active = False
MitTreeFiller.PFTaus.active = False
  
MitTreeFiller.PixelHits.active            = False
MitTreeFiller.PixelVertexes.active        = False
MitTreeFiller.PixelTracks.active          = False

#Disable duplicate removal in StableParticle merging for conversions,
#since this requires tracking RecHits which aren't available in AOD
mergedConversionsStable.removeDuplicates = False
mergedConversionsGeneralStable.removeDuplicates = False
mergedElectronsStable.removeDuplicates = False

BambuFillAOD = cms.Sequence(BambuRecoSequence*BambuRecoFillSequence)
