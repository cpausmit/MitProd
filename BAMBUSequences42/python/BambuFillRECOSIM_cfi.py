# $Id: BambuFillRECOSIM_cfi.py,v 1.1 2011/10/09 14:15:04 bendavid Exp $

import FWCore.ParameterSet.Config as cms

from MitProd.BAMBUSequences42.BambuFillRECO_cfi import *

from MitProd.BAMBUSequences.BambuSIM_cfi import *

#Perform required tree filler reconfiguration for SIM info
enableSIM(MitTreeFiller)

MitTreeFiller.Kt4PFJets.flavorMatchingActive      = False
MitTreeFiller.AKt5PFJets.flavorMatchingActive     = False

BambuFillRECOSIM = cms.Sequence(BambuRecoSequence*BambuGenSequence*BambuRecoFillSequence)
