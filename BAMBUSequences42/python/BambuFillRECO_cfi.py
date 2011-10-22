# $Id: BambuFillRECO_cfi.py,v 1.1 2011/10/09 14:15:04 bendavid Exp $

import FWCore.ParameterSet.Config as cms

from MitProd.BAMBUSequences.BambuFillRECO_cfi import *

# Load FastJet L1 corrections
from MitProd.TreeFiller.FastJetCorrection_cff import *

#old collection name for endcap basic clusters
MitTreeFiller.EndcapBasicClusters.edmName = 'multi5x5BasicClusters:multi5x5EndcapBasicClusters'

from MitEdm.Producers.dummytrack_cfi import *

conversionStepTracks = dummyTracks.clone()

#42x variation of fastjet sequence
BambuRecoSequence.replace(l1FastJetSequence,l1FastJetSequence42)
MitTreeFiller.PileupEnergyDensity.edmNameRandom = cms.untracked.InputTag('kt6PFJetsForRhoComputationRandom','rho')
MitTreeFiller.Kt4PFJetsNoArea.active = True
MitTreeFiller.AKt5PFJetsNoArea.active = True

MitTreeFiller.Kt4PFJets.edmName = 'kt4PFJetsForL1Correction'
MitTreeFiller.AKt5PFJets.edmName = 'ak5PFJetsForL1Correction'

newJetTracksAssociatorAtVertex.jets = "ak5PFJetsForL1Correction"
newSoftElectronTagInfos.jets = "ak5PFJetsForL1Correction"
newSoftMuonTagInfos.jets = "ak5PFJetsForL1Correction"

#produce empty conversionStepTracks (not present in 42x)
BambuRecoSequence.insert(0,conversionStepTracks)
