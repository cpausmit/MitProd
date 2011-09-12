# $Id: FastJetCorrection_cff.py,v 1.8 2011/06/15 20:04:14 bendavid Exp $

import FWCore.ParameterSet.Config as cms

from RecoJets.Configuration.RecoPFJets_cff import *
kt6PFJetsForRhoComputation = kt6PFJets.clone(doRhoFastjet = True)
kt6PFJetsForRhoComputation25 = kt6PFJets.clone(doRhoFastjet = True, Rho_EtaMax=2.5)
kt6PFJetsForRhoComputationRandom = kt6PFJets.clone(doRhoFastjet = True, voronoiRfact=-0.9)
kt6PFJetsForRhoComputationRandom25 = kt6PFJets.clone(doRhoFastjet = True, voronoiRfact=-0.9, Rho_EtaMax=2.5)
ak5PFJetsForL1Correction = ak5PFJets.clone(doAreaFastjet = True)
kt4PFJetsForL1Correction = kt4PFJets.clone(doAreaFastjet = True) 

# sequences to compute jet areas and rho
l1FastJetSequence  = cms.Sequence(kt6PFJetsForRhoComputation25 * kt6PFJetsForRhoComputationRandom * kt6PFJetsForRhoComputationRandom25)
                                                    