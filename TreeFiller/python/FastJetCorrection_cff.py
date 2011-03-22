# $Id: FastJetCorrection_cff.py,v 1.4 2011/03/11 18:46:43 mzanetti Exp $

import FWCore.ParameterSet.Config as cms

# rereco ak5PFJets (computing area)
from RecoJets.JetProducers.ak5PFJets_cfi import *
ak5PFJetsForL1Correction = ak5PFJets.clone(doAreaFastjet = True)
ak5PFJetsForL1Correction.Rho_EtaMax = cms.double(2.5)
ak5PFJetsForL1Correction.Ghost_EtaMax = 2.5

# rereco kt4PFJets (computing area)
from RecoJets.JetProducers.kt4PFJets_cfi import *
kt4PFJetsForL1Correction = kt4PFJets.clone(doAreaFastjet = True) 
kt4PFJetsForL1Correction.Rho_EtaMax = cms.double(2.5)
kt4PFJetsForL1Correction.Ghost_EtaMax = 2.5

# compute energy density up to 2.5 for L1 fastjet correction (common to kt4 and atk5)
from RecoJets.JetProducers.kt4PFJets_cfi import *
kt6PFJetsForRhoComputation = kt4PFJets.clone( rParam = 0.6, doRhoFastjet = True ) 
kt6PFJetsForRhoComputation.Rho_EtaMax = cms.double(2.5)
kt6PFJetsForRhoComputation.Ghost_EtaMax = 2.5

# compute energy density up to 4.5 for comparison (to be stored in the bambu event)
kt6PFJetsForRhoComputationHighEta = kt4PFJets.clone( rParam = 0.6, doRhoFastjet = True ) 
kt6PFJetsForRhoComputationHighEta.Rho_EtaMax = cms.double(4.4)
kt6PFJetsForRhoComputationHighEta.Ghost_EtaMax = 5.0

# Jet Correction ESSource (only one, based on rho computation up to eta=2.5)
ak5PFL1Fastjet = cms.ESSource(
    'L1FastjetCorrectionService',
    era         = cms.string('Jec10V1'),
    level       = cms.string('L1FastJet'),
    algorithm   = cms.string('AK5PF'),
    section     = cms.string(''),
    srcRho      = cms.InputTag('kt6PFJetsForRhoComputation','rho'),
    useCondDB = cms.untracked.bool(True)
    )

# the actual corrected ak5PFJets collection
ak5PFJetsL1 = cms.EDProducer(
    'PFJetCorrectionProducer',
    src        = cms.InputTag('ak5PFJetsForL1Correction'),
    correctors = cms.vstring('ak5PFL1Fastjet')
    )


# the actual corrected kt4PFJets collection
kt4PFJetsL1 = cms.EDProducer(
    'PFJetCorrectionProducer',
    src        = cms.InputTag('kt4PFJetsForL1Correction'),
    correctors = cms.vstring('ak5PFL1Fastjet')
    )


# the fastjet correction sequence
l1FastJetAKt5PFCorrectionSequence = cms.Sequence( ak5PFJetsForL1Correction * kt6PFJetsForRhoComputation * ak5PFJetsL1 * kt6PFJetsForRhoComputationHighEta ) 
l1FastJetAKt5PFCorrectionSequence = cms.Sequence( kt4PFJetsForL1Correction * kt6PFJetsForRhoComputation * kt4PFJetsL1 * kt6PFJetsForRhoComputationHighEta ) 
l1FastJetCorrectionSequence =       cms.Sequence( ak5PFJetsForL1Correction * kt6PFJetsForRhoComputation * ak5PFJetsL1 *
                                                  kt4PFJetsForL1Correction * kt6PFJetsForRhoComputation * kt4PFJetsL1 * 
                                                  kt6PFJetsForRhoComputationHighEta )

