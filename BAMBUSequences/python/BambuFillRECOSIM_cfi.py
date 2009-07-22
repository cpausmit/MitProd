# $Id: $

import FWCore.ParameterSet.Config as cms

from MitProd.BAMBUSequences.BambuFillRECO_cfi import *

# Enable fillers of MC Truth information
MitTreeFiller.MCParticles.active = True
MitTreeFiller.MCEventInfo.active = True
MitTreeFiller.IC5GenJets.active  = True
MitTreeFiller.SC5GenJets.active  = True
MitTreeFiller.SC7GenJets.active  = True
MitTreeFiller.KT4GenJets.active  = True
MitTreeFiller.KT6GenJets.active  = True
MitTreeFiller.GenMet.active      = True

# Load antikt gen jet producers
from MitProd.TreeFiller.antiktGenJets_cfi import *
MitTreeFiller.AKT5GenJets.active = True
MitTreeFiller.AKT7GenJets.active = True

# Load flavor history
from PhysicsTools.HepMCCandAlgos.flavorHistoryProducer_cfi import *
from PhysicsTools.HepMCCandAlgos.flavorHistoryFilter_cfi import *
# Enable Flavor history filling
MitTreeFiller.MCEventInfo.flavorHistoryActive = True

# Load Flavor Matching Information
from MitProd.TreeFiller.JetsMCFlavourMatching_cfi import *
# Enable Flavor matching for Reco Jets and GenJets
MitTreeFiller.ItrCone5Jets.flavorMatchingActive   = True
MitTreeFiller.SisCone5Jets.flavorMatchingActive   = True
MitTreeFiller.SisCone7Jets.flavorMatchingActive   = True
MitTreeFiller.Kt4Jets.flavorMatchingActive        = True
MitTreeFiller.Kt6Jets.flavorMatchingActive        = True
MitTreeFiller.AKt5Jets.flavorMatchingActive       = True
MitTreeFiller.AKt7Jets.flavorMatchingActive       = True
MitTreeFiller.ItrCone5PFJets.flavorMatchingActive = True
MitTreeFiller.SisCone5PFJets.flavorMatchingActive = True
MitTreeFiller.SisCone7PFJets.flavorMatchingActive = True
MitTreeFiller.Kt4PFJets.flavorMatchingActive      = True
MitTreeFiller.Kt6PFJets.flavorMatchingActive      = True
MitTreeFiller.AKt5PFJets.flavorMatchingActive     = True
MitTreeFiller.AKt7PFJets.flavorMatchingActive     = True
MitTreeFiller.IC5GenJets.flavorMatchingActive     = True
MitTreeFiller.SC5GenJets.flavorMatchingActive     = True
MitTreeFiller.SC7GenJets.flavorMatchingActive     = True
MitTreeFiller.KT4GenJets.flavorMatchingActive     = True
MitTreeFiller.KT6GenJets.flavorMatchingActive     = True
MitTreeFiller.AKT5GenJets.flavorMatchingActive    = True
MitTreeFiller.AKT7GenJets.flavorMatchingActive    = True

BambuGenSequence = cms.Sequence(
    antiktGenJets*
    bFlavorHistoryProducer*
    cFlavorHistoryProducer*
    flavorHistoryFilter*
    caloJetMCFlavour*
    pfJetMCFlavour*
    antiKtJetMCFlavour
)

BambuFillRECOSIM = cms.Sequence(BambuRecoSequence*BambuGenSequence*BambuRecoFillSequence)
