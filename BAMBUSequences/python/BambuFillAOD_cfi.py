# $Id: BambuFillRECO_cfi.py,v 1.6 2009/08/11 15:31:29 loizides Exp $

import FWCore.ParameterSet.Config as cms

from MitProd.BAMBUSequences.BambuFillRECO_cfi import *

#Disable duplicate removal in StableParticle merging for conversions,
#since this requires tracking RecHits which aren't available in AOD
mergedConversionsStable.removeDuplicates = False
mergedConversionsGeneralStable.removeDuplicates = False
mergedElectronsStable.removeDuplicates = False

#disable JPT in filler and sequence
#MitTreeFiller.IC5JetPlusTrack.active = False
#BambuRecoSequence.remove(ZSPJetCorrections)
#BambuRecoSequence.remove(JetPlusTrackCorrections)

BambuFillAOD = cms.Sequence(BambuRecoSequence*BambuRecoFillSequence)
