# $Id: BambuFillRECOSIM_cfi.py,v 1.6 2009/07/22 11:26:19 loizides Exp $

import FWCore.ParameterSet.Config as cms

from MitProd.BAMBUSequences.BambuFillAOD_cfi import *

from MitProd.BAMBUSequences.BambuSIM_cfi import *

#Perform required tree filler reconfiguration for SIM info
enableSIM(MitTreeFiller)

#Disable filling of geant4 simulation truth, not present in AODSIM
MitTreeFiller.MCParticles.simActive = False

BambuFillAODSIM = cms.Sequence(BambuRecoSequence*BambuGenSequence*BambuRecoFillSequence)
