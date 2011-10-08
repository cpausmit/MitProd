# $Id: FastJetCorrection_cff.py,v 1.9 2011/09/12 15:21:38 bendavid Exp $

import FWCore.ParameterSet.Config as cms

from RecoJets.Configuration.RecoPFJets_cff import *
kt6PFJetsForRhoComputationVoronoi = kt6PFJets.clone(doRhoFastjet = True, voronoiRfact=0.9)
kt6PFJetsForRhoComputationVoronoi25 = kt6PFJets.clone(doRhoFastjet = True, voronoiRfact=0.9, Rho_EtaMax=2.5)
kt6PFJetsForRhoComputationRandom = kt6PFJets.clone(doRhoFastjet = True, voronoiRfact=-0.9)
kt6PFJetsForRhoComputationRandom25 = kt6PFJets.clone(doRhoFastjet = True, voronoiRfact=-0.9, Rho_EtaMax=2.5)
ak5PFJetsForL1Correction = ak5PFJets.clone(doAreaFastjet = True)
kt4PFJetsForL1Correction = kt4PFJets.clone(doAreaFastjet = True) 

#kt6PFJetsForRhoComputationRandom equivalent is stored in aod in 44x

# sequences to compute jet areas and rho
l1FastJetSequence  = cms.Sequence(kt6PFJetsForRhoComputationVoronoi * kt6PFJetsForRhoComputationVoronoi25 * kt6PFJetsForRhoComputationRandom25)

#legacy sequence for 42x
l1FastJetSequence42  = cms.Sequence(kt6PFJetsForRhoComputationVoronoi * kt6PFJetsForRhoComputationVoronoi25 * kt6PFJetsForRhoComputationRandom * kt6PFJetsForRhoComputationRandom25 * ak5PFJetsForL1Correction * kt4PFJetsForL1Correction)

                                                    