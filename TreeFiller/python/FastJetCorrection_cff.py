# $Id: FastJetCorrection_cff.py,v 1.3 2011/03/11 17:15:59 mzanetti Exp $

import FWCore.ParameterSet.Config as cms

from RecoJets.JetProducers.ak5PFJets_cfi import *
ak5PFJetsForL1Correction = ak5PFJets.clone(doAreaFastjet = True)
#ak5PFJets.doAreaFastjet = cms.bool(True)
ak5PFJetsForL1Correction.Rho_EtaMax = cms.double(3.0)
from RecoJets.JetProducers.kt4PFJets_cfi import *
kt6PFJets = kt4PFJets.clone( rParam = 0.6, doRhoFastjet = True ) 
kt6PFJets.Rho_EtaMax = cms.double(3.0)

from JetMETCorrections.Configuration.JetCorrectionServices_cff import *
ak5PFJetsL1 = cms.EDProducer(
    'PFJetCorrectionProducer',
    src        = cms.InputTag('ak5PFJetsForL1Correction'),
    correctors = cms.vstring('ak5PFL1Fastjet')
    )

l1FastJetCorrectionSequence = cms.Sequence( ak5PFJetsForL1Correction * kt6PFJets * ak5PFJetsL1 )

