# $Id: FastJetCorrection_cff.py,v 1.2 2011/03/09 14:36:14 bendavid Exp $

import FWCore.ParameterSet.Config as cms

from RecoJets.JetProducers.ak5PFJets_cfi import *
ak5PFJets.doAreaFastjet = cms.bool(True)
ak5PFJets.Rho_EtaMax = cms.double(4.5)
from RecoJets.JetProducers.kt4PFJets_cfi import *
kt6PFJets = kt4PFJets.clone( rParam = 0.6, doRhoFastjet = True ) 
kt6PFJets.Rho_EtaMax = cms.double(4.5)

from JetMETCorrections.Configuration.JetCorrectionServices_cff import *
ak5PFJetsL1 = cms.EDProducer(
    'PFJetCorrectionProducer',
    src        = cms.InputTag('ak5PFJets'),
    correctors = cms.vstring('ak5PFL1Fastjet')
    )

l1FastJetCorrectionSequence = cms.Sequence( ak5PFJets * kt6PFJets * ak5PFJetsL1 )

