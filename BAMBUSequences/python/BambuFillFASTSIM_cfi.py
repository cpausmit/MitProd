# $Id: BambuFillAODSIM_cfi.py,v 1.1 2009/10/04 12:53:19 bendavid Exp $

import FWCore.ParameterSet.Config as cms

from MitProd.BAMBUSequences.BambuFillAOD_cfi import *

from MitProd.BAMBUSequences.BambuSIM_cfi import *

#Perform required tree filler reconfiguration for SIM info
enableSIM(MitTreeFiller)

#Disable filling of geant4 simulation truth, not present in AODSIM
MitTreeFiller.MCParticles.simActive = False

#replace conversion sequence with fastsim friendly sequence omitting ecal-seeded tracks
BambuRecoSequence.replace(electronsStable,electronsStableFastSim)
mvfConversions.iStables1 = 'mergedGeneralGsfStable'
mvfConversions.iStables2 = 'mergedGeneralGsfStable'
MitTreeFiller.MergedElectronsStable.edmName = 'mergedGeneralGsfStable'

#Disable filling of ecal-seeded conversions, not present in fastsim
MitTreeFiller.ConversionInOutTracks.active = False
MitTreeFiller.ConversionOutInTracks.active = False
MitTreeFiller.ConversionInOutElectronsStable.active = False
MitTreeFiller.ConversionOutInElectronsStable.active = False
MitTreeFiller.Conversions.active = False

#rerun trackjet reconstruction since it is not included by default in the fastsim sequence
from RecoJets.Configuration.RecoTrackJets_cff import *

BambuFillFASTSIM = cms.Sequence(BambuRecoSequence*recoTrackJets*BambuGenSequence*BambuRecoFillSequence)
