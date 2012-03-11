# $Id: FastJetCorrection_cff.py,v 1.10 2011/10/08 18:54:48 bendavid Exp $

import FWCore.ParameterSet.Config as cms

from RecoJets.Configuration.RecoPFJets_cff import *
kt6PFJetsForRhoComputationVoronoi    = kt6PFJets.clone(doRhoFastjet = True, voronoiRfact=0.9)
kt6PFJetsForRhoComputationVoronoi25  = kt6PFJets.clone(doRhoFastjet = True, voronoiRfact=0.9, Rho_EtaMax=2.5)
kt6PFJetsForRhoComputationRandom     = kt6PFJets.clone(doRhoFastjet = True, voronoiRfact=-0.9)
kt6PFJetsForRhoComputationRandom25   = kt6PFJets.clone(doRhoFastjet = True, voronoiRfact=-0.9, Rho_EtaMax=2.5)

kt6PFJetsForRhoComputationVoronoiCHS   = kt6PFJets.clone(doAreaFastjet = True,doRhoFastjet = True, voronoiRfact=0.9,
                                                            rParam=cms.double(0.6),src=cms.InputTag('pfNoElectron'))
kt6PFJetsForRhoComputationVoronoi25CHS = kt6PFJets.clone(doAreaFastjet = True,doRhoFastjet = True, voronoiRfact=0.9,Rho_EtaMax=2.5,
                                                            rParam=cms.double(0.6),src=cms.InputTag('pfNoElectron'))
kt6PFJetsForRhoComputationRandomCHS   = kt6PFJets.clone(doAreaFastjet = True,doRhoFastjet = True, voronoiRfact=-0.9,
                                                            rParam=cms.double(0.6),src=cms.InputTag('pfNoElectron'))
kt6PFJetsForRhoComputationRandom25CHS = kt6PFJets.clone(doAreaFastjet = True,doRhoFastjet = True, voronoiRfact=-0.9,Rho_EtaMax=2.5,
                                                            rParam=cms.double(0.6),src=cms.InputTag('pfNoElectron'))

ak5PFJetsForL1Correction = ak5PFJets.clone(doAreaFastjet = True)
kt4PFJetsForL1Correction = kt4PFJets.clone(doAreaFastjet = True)
#ak5PFJetsForL1CorrectionCHS = ak5PFJets.clone(doAreaFastjet = True)
#kt4PFJetsForL1CorrectionCHS = kt4PFJets.clone(doAreaFastjet = True) 

#kt6PFJetsForRhoComputationRandom equivalent is stored in aod in 44x

# sequences to compute jet areas and rho
l1FastJetSequence     = cms.Sequence(kt6PFJetsForRhoComputationVoronoi * kt6PFJetsForRhoComputationVoronoi25 * kt6PFJetsForRhoComputationRandom25)
l1FastJetSequenceCHS  = cms.Sequence(kt6PFJetsForRhoComputationVoronoiCHS * kt6PFJetsForRhoComputationVoronoi25CHS * kt6PFJetsForRhoComputationRandom25CHS)

#legacy sequence for 42x
l1FastJetSequence42  = cms.Sequence(kt6PFJetsForRhoComputationVoronoi * kt6PFJetsForRhoComputationVoronoi25 * kt6PFJetsForRhoComputationRandom * kt6PFJetsForRhoComputationRandom25 * ak5PFJetsForL1Correction * kt4PFJetsForL1Correction)
l1FastJetSequence42CHS  = cms.Sequence(kt6PFJetsForRhoComputationVoronoiCHS * kt6PFJetsForRhoComputationVoronoi25CHS * kt6PFJetsForRhoComputationRandomCHS * kt6PFJetsForRhoComputationRandom25CHS)
                                                    
