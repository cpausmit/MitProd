import FWCore.ParameterSet.Config as cms

from MitProd.BAMBUSequences.BambuFillRECO_cfi import *
from MitProd.BAMBUSequences.BambuSIM_cfi import *

# Perform required tree filler reconfiguration for SIM info
enableSIM(MitTreeFiller)

BambuFillRECOSIM = cms.Sequence(BambuRecoSequence*
                                BambuGenSequence*
                                BambuRecoFillSequence)
