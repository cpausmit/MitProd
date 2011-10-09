# $Id: BambuFillRECOSIM_cfi.py,v 1.7 2009/10/04 12:53:19 bendavid Exp $

import FWCore.ParameterSet.Config as cms

from MitProd.BAMBUSequences42.BambuFillRECO_cfi import *

from MitProd.BAMBUSequences.BambuSIM_cfi import *

#Perform required tree filler reconfiguration for SIM info
enableSIM(MitTreeFiller)

BambuFillRECOSIM = cms.Sequence(BambuRecoSequence*BambuGenSequence*BambuRecoFillSequence)
