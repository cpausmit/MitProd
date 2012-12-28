# $Id: FastJetCorrection_cff.py,v 1.13 2012/06/03 20:35:00 paus Exp $

import FWCore.ParameterSet.Config as cms

from RecoJets.Configuration.RecoPFJets_cff import *
kt6PFJetsForRhoComputationVoronoiMIT      = kt6PFJets.clone(doRhoFastjet = True, voronoiRfact=0.9)
kt6PFJetsForRhoComputationVoronoi25MIT    = kt6PFJets.clone(doRhoFastjet = True, voronoiRfact=0.9, Rho_EtaMax=2.5)
kt6PFJetsForRhoComputationRandomMIT       = kt6PFJets.clone(doRhoFastjet = True, voronoiRfact=-0.9)
kt6PFJetsForRhoComputationRandom25MIT     = kt6PFJets.clone(doRhoFastjet = True, voronoiRfact=-0.9, Rho_EtaMax=2.5)

kt6PFJetsForRhoComputationVoronoiCHSMIT   = kt6PFJets.clone(doAreaFastjet = True,doRhoFastjet = True, voronoiRfact=0.9,
                                                            rParam=cms.double(0.6),src=cms.InputTag('pfNoElectron'))
kt6PFJetsForRhoComputationVoronoi25CHSMIT = kt6PFJets.clone(doAreaFastjet = True,doRhoFastjet = True, voronoiRfact=0.9,Rho_EtaMax=2.5,
                                                            rParam=cms.double(0.6),src=cms.InputTag('pfNoElectron'))
kt6PFJetsForRhoComputationRandomCHSMIT    = kt6PFJets.clone(doAreaFastjet = True,doRhoFastjet = True, voronoiRfact=-0.9,
                                                            rParam=cms.double(0.6),src=cms.InputTag('pfNoElectron'))
kt6PFJetsForRhoComputationRandom25CHSMIT  = kt6PFJets.clone(doAreaFastjet = True,doRhoFastjet = True, voronoiRfact=-0.9,Rho_EtaMax=2.5,
                                                            rParam=cms.double(0.6),src=cms.InputTag('pfNoElectron'))
ak5PFJetsForL1Correction               = ak5PFJets.clone(doAreaFastjet = True)
kt4PFJetsForL1Correction               = kt4PFJets.clone(doAreaFastjet = True)

#ak5PFJetsForL1CorrectionCHS = ak5PFJets.clone(doAreaFastjet = True)
#kt4PFJetsForL1CorrectionCHS = kt4PFJets.clone(doAreaFastjet = True) 

#kt6PFJetsForRhoComputationRandom equivalent is stored in aod in 44x

# sequences to compute jet areas and rho
l1FastJetSequence    = cms.Sequence(kt6PFJetsForRhoComputationVoronoiMIT    * kt6PFJetsForRhoComputationVoronoi25MIT    * kt6PFJetsForRhoComputationRandomMIT    * kt6PFJetsForRhoComputationRandom25MIT)
l1FastJetSequenceCHS = cms.Sequence(kt6PFJetsForRhoComputationVoronoiCHSMIT * kt6PFJetsForRhoComputationVoronoi25CHSMIT * kt6PFJetsForRhoComputationRandom25CHSMIT)

# legacy sequence for 42x
l1FastJetSequence42  = cms.Sequence(kt6PFJetsForRhoComputationVoronoiMIT * kt6PFJetsForRhoComputationVoronoi25MIT * kt6PFJetsForRhoComputationRandomMIT * kt6PFJetsForRhoComputationRandom25MIT * ak5PFJetsForL1Correction * kt4PFJetsForL1Correction)
l1FastJetSequence42CHS  = cms.Sequence(kt6PFJetsForRhoComputationVoronoiCHSMIT * kt6PFJetsForRhoComputationVoronoi25CHSMIT * kt6PFJetsForRhoComputationRandomCHSMIT * kt6PFJetsForRhoComputationRandom25CHSMIT)
                                                    
