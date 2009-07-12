
import FWCore.ParameterSet.Config as cms

from MitProd.BAMBUSequences.BambuFillRECO_cfi import *

MitTreeFiller.MCParticles.active = True
MitTreeFiller.MCEventInfo.active = True

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
