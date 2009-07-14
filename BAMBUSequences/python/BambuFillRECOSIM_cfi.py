
import FWCore.ParameterSet.Config as cms

from MitProd.BAMBUSequences.BambuFillRECO_cfi import *

#enable fillers of MC Truth information
MitTreeFiller.MCParticles.active = True
MitTreeFiller.MCEventInfo.active = True
MitTreeFiller.IC5GenJets.active  = True
MitTreeFiller.SC5GenJets.active  = True
MitTreeFiller.SC7GenJets.active  = True
MitTreeFiller.KT4GenJets.active  = True
MitTreeFiller.KT6GenJets.active  = True
MitTreeFiller.GenMet.active      = True

#Load flavor history
from PhysicsTools.HepMCCandAlgos.flavorHistoryProducer_cfi import *
from PhysicsTools.HepMCCandAlgos.flavorHistoryFilter_cfi import *

#Load Flavor Matching Information
from MitProd.TreeFiller.JetsMCFlavourMatching_cfi import *
#Enable Flavor matching for Reco Jets and GenJets
MitTreeFiller.ItrCone5Jets.flavorMatchingActive   = True
MitTreeFiller.SisCone5Jets.flavorMatchingActive   = True
MitTreeFiller.SisCone7Jets.flavorMatchingActive   = True
MitTreeFiller.Kt4Jets.flavorMatchingActive        = True
MitTreeFiller.Kt6Jets.flavorMatchingActive        = True
MitTreeFiller.ItrCone5PFJets.flavorMatchingActive = True
MitTreeFiller.SisCone5PFJets.flavorMatchingActive = True
MitTreeFiller.SisCone7PFJets.flavorMatchingActive = True
MitTreeFiller.Kt4PFJets.flavorMatchingActive      = True
MitTreeFiller.Kt6PFJets.flavorMatchingActive      = True
MitTreeFiller.IC5GenJets.flavorMatchingActive     = True
MitTreeFiller.SC5GenJets.flavorMatchingActive     = True
MitTreeFiller.SC7GenJets.flavorMatchingActive     = True
MitTreeFiller.KT4GenJets.flavorMatchingActive     = True
MitTreeFiller.KT6GenJets.flavorMatchingActive     = True

BambuGenSequence = cms.Sequence(
    bFlavorHistoryProducer*
    cFlavorHistoryProducer*
    flavorHistoryFilter*
    caloJetMCFlavour*
    pfJetMCFlavour*
    extraPFJetMCFlavour
)

BambuFillRECOSIM = cms.Sequence(BambuGenSequence*BambuFillRECO)
