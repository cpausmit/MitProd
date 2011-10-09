# $Id: BambuFillAODSIM_cfi.py,v 1.1 2009/10/04 12:53:19 bendavid Exp $

import FWCore.ParameterSet.Config as cms

from MitProd.BAMBUSequences42.BambuFillAOD_cfi import *

from MitProd.BAMBUSequences.BambuSIM_cfi import *

#Perform required tree filler reconfiguration for SIM info
enableSIM(MitTreeFiller)

#Disable filling of geant4 simulation truth, not present in AODSIM
MitTreeFiller.MCParticles.simActive = False

BambuFillAODSIM = cms.Sequence(BambuRecoSequence*BambuGenSequence*BambuRecoFillSequence)
