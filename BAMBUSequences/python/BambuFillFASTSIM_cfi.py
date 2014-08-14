import FWCore.ParameterSet.Config as cms

from MitProd.BAMBUSequences.BambuFillAOD_cfi import *
from MitProd.BAMBUSequences.BambuSIM_cfi import *

# Perform required tree filler reconfiguration for SIM info
enableSIM(MitTreeFiller)

# Disable filling of geant4 simulation truth, not present in AODSIM
MitTreeFiller.MCParticles.simActive = False

# Disable filling of L1 trigger info (not consistently available in fastsim)
MitTreeFiller.MetaInfos.l1Active = False

# Disable pileup info filling
MitTreeFiller.PileupInfo.active = False

# Replace conversion sequence with fastsim friendly sequence omitting ecal-seeded tracks
BambuRecoSequence.replace(electronsStable,electronsStableFastSim)
mvfConversions.iStables1 = 'mergedGeneralGsfStable'
mvfConversions.iStables2 = 'mergedGeneralGsfStable'
MitTreeFiller.MergedElectronsStable.edmName = 'mergedGeneralGsfStable'

# Disable filling of ecal-seeded conversions, not present in fastsim
MitTreeFiller.ConversionInOutTracks.active = False
MitTreeFiller.ConversionOutInTracks.active = False
MitTreeFiller.ConversionInOutElectronsStable.active = False
MitTreeFiller.ConversionOutInElectronsStable.active = False
MitTreeFiller.Conversions.active = False

# Rerun trackjet reconstruction since it is not included by default in the fastsim sequence
from RecoJets.Configuration.RecoTrackJets_cff import *

BambuFillFASTSIM = cms.Sequence(BambuRecoSequence*
                                recoTrackJets*
                                BambuGenSequence*
                                BambuRecoFillSequence)
