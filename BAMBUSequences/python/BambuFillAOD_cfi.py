# $Id: BambuFillAOD_cfi.py,v 1.1 2009/10/04 12:53:19 bendavid Exp $

import FWCore.ParameterSet.Config as cms

from MitProd.BAMBUSequences.BambuFillRECO_cfi import *

#Disable duplicate removal in StableParticle merging for conversions,
#since this requires tracking RecHits which aren't available in AOD
mergedConversionsStable.removeDuplicates = False
mergedConversionsGeneralStable.removeDuplicates = False
mergedElectronsStable.removeDuplicates = False

BambuFillAOD = cms.Sequence(BambuRecoSequence*BambuRecoFillSequence)
