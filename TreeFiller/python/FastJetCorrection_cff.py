# $Id: FastJetCorrection_cff.py,v 1.6 2011/03/23 19:02:51 mzanetti Exp $

import FWCore.ParameterSet.Config as cms

from RecoJets.Configuration.RecoPFJets_cff import *
kt6PFJetsForRhoComputation = kt6PFJets.clone(doRhoFastjet = True)
ak5PFJetsForL1Correction = ak5PFJets.clone(doAreaFastjet = True)
kt4PFJetsForL1Correction = kt4PFJets.clone(doAreaFastjet = True) 

# sequences to compute jet areas and rho
l1FastJetSequence  = cms.Sequence( kt6PFJetsForRhoComputation * ak5PFJetsForL1Correction * kt4PFJetsForL1Correction)
                                                    