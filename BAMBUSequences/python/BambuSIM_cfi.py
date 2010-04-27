# $Id: BambuSIM_cfi.py,v 1.10 2010/03/25 18:51:41 bendavid Exp $

import FWCore.ParameterSet.Config as cms

from MitProd.BAMBUSequences.BambuFillRECO_cfi import *

# Load extra gen jet producers
from MitProd.TreeFiller.antiktGenJets_cfi import *

# Load flavor history
from PhysicsTools.HepMCCandAlgos.flavorHistoryProducer_cfi import *
from PhysicsTools.HepMCCandAlgos.flavorHistoryFilter_cfi import *

# Load Flavor Matching Information
from MitProd.TreeFiller.JetsMCFlavourMatching_cfi import *

BambuGenSequence = cms.Sequence(
    antiktGenJets*
    bFlavorHistoryProducer*
    cFlavorHistoryProducer*
    flavorHistoryFilter*
    caloJetMCFlavour*
    pfJetMCFlavour
)

#function to enable sim filling for the given tree filler
#This avoids duplication of config sequences between RECOSIM and AODSIM
def enableSIM(filler):

  # Disable filling of out-of-time bx
  filler.MetaInfos.l1GtReadRecEdmName = ''

  # Disable pixelLess tracks, not in normal MC
  #filler.PixelLessTracks.active = False

  # Enable fillers of MC Truth information
  filler.MCParticles.active = True
  filler.MCEventInfo.active = True
  filler.MCVertexes.active  = True
  filler.IC5GenJets.active  = True
  filler.SC5GenJets.active  = True
  filler.SC7GenJets.active  = True
  filler.KT4GenJets.active  = True
  filler.KT6GenJets.active  = True
  filler.GenMet.active      = True
  
  #Enable filling antikt gen jets
  filler.AKT5GenJets.active = True
  filler.AKT7GenJets.active = True
  
  # Enable Flavor history filling
  filler.MCEventInfo.flavorHistoryActive = True
  
  # Enable Flavor matching for Reco Jets and GenJets
  filler.ItrCone5Jets.flavorMatchingActive   = True
  filler.SisCone5Jets.flavorMatchingActive   = True
  filler.SisCone7Jets.flavorMatchingActive   = True
  filler.Kt4Jets.flavorMatchingActive        = True
  filler.Kt6Jets.flavorMatchingActive        = True
  filler.AKt5Jets.flavorMatchingActive       = True
  filler.AKt7Jets.flavorMatchingActive       = True
  filler.ItrCone5PFJets.flavorMatchingActive = True
  filler.SisCone5PFJets.flavorMatchingActive = True
  filler.SisCone7PFJets.flavorMatchingActive = True
  filler.Kt4PFJets.flavorMatchingActive      = True
  filler.Kt6PFJets.flavorMatchingActive      = True
  filler.AKt5PFJets.flavorMatchingActive     = True
  filler.AKt7PFJets.flavorMatchingActive     = True
  filler.IC5GenJets.flavorMatchingActive     = True
  filler.SC5GenJets.flavorMatchingActive     = True
  filler.SC7GenJets.flavorMatchingActive     = True
  filler.KT4GenJets.flavorMatchingActive     = True
  filler.KT6GenJets.flavorMatchingActive     = True
  filler.AKT5GenJets.flavorMatchingActive    = True
  filler.AKT7GenJets.flavorMatchingActive    = True
