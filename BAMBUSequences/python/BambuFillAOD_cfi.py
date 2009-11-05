# $Id: BambuFillAOD_cfi.py,v 1.2 2009/10/19 18:44:51 bendavid Exp $

import FWCore.ParameterSet.Config as cms

from MitProd.BAMBUSequences.BambuFillRECO_cfi import *

#disable pixel hit/track producers/fillers
try:
  BambuRecoSequence.remove(siPixelRecHits)
except:
  pass
  
MitTreeFiller.PixelHits.active            = False
MitTreeFiller.PixelVertexes.active        = False
MitTreeFiller.PixelTracks.active          = False

#Disable duplicate removal in StableParticle merging for conversions,
#since this requires tracking RecHits which aren't available in AOD
mergedConversionsStable.removeDuplicates = False
mergedConversionsGeneralStable.removeDuplicates = False
mergedElectronsStable.removeDuplicates = False

BambuFillAOD = cms.Sequence(BambuRecoSequence*BambuRecoFillSequence)
