# $Id: BambuFillRECO_cfi.py,v 1.3 2011/11/22 15:09:47 bendavid Exp $

import FWCore.ParameterSet.Config as cms

from MitProd.BAMBUSequences.BambuFillRECO_cfi import *

# Load FastJet L1 corrections
from MitProd.TreeFiller.FastJetCorrection_cff import *

#old collection name for endcap basic clusters
MitTreeFiller.EndcapBasicClusters.edmName = 'multi5x5BasicClusters:multi5x5EndcapBasicClusters'

from MitEdm.Producers.dummytrack_cfi import *

conversionStepTracks = dummyTracks.clone()

from RecoMET.Configuration.RecoPFMET_cff import *

#42x variation of fastjet sequence
BambuRecoSequence.replace(l1FastJetSequence,l1FastJetSequence42)
MitTreeFiller.PileupEnergyDensity.edmNameRandom = cms.untracked.InputTag('kt6PFJetsForRhoComputationRandom','rho')
MitTreeFiller.Kt4PFJetsNoArea.active = True
MitTreeFiller.AKt5PFJetsNoArea.active = True

MitTreeFiller.Kt4PFJets.edmName = 'kt4PFJetsForL1Correction'
MitTreeFiller.AKt5PFJets.edmName = 'ak5PFJetsForL1Correction'

MitTreeFiller.Kt4PFJets.flavorMatchingActive      = False
MitTreeFiller.AKt5PFJets.flavorMatchingActive     = False

newJetTracksAssociatorAtVertex.jets = "ak5PFJetsForL1Correction"
newSoftElectronTagInfos.jets = "ak5PFJetsForL1Correction"
newSoftMuonTagInfos.jets = "ak5PFJetsForL1Correction"

#produce empty conversionStepTracks (not present in 42x)
BambuRecoSequence.insert(0,conversionStepTracks)
BambuRecoSequence.insert(4,recoPFMET)
