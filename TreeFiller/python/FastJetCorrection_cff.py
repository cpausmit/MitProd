# $Id: FastJetCorrection_cff.py,v 1.6 2009/11/03 15:13:18 mzanetti Exp $

import FWCore.ParameterSet.Config as cms

from RecoJets.JetProducers.ak5PFJets_cfi import *
ak5PFJets.doAreaFastjet = cms.bool(True)
ak5PFJets.Rho_EtaMax = cms.double(4.5)
from RecoJets.JetProducers.kt4PFJets_cfi import *
kt6PFJets = kt4PFJets.clone( rParam = 0.6, doRhoFastjet = True ) 
kt6PFJets.Rho_EtaMax = cms.double(4.5)
from JetMETCorrections.Configuration.JetCorrectionProducersAllAlgos_cff import *
L1Fastjet.algorithm = cms.string('AK5Calo') #DUMMY needs to read an existing file 
L1Fastjet.era = cms.string('Spring10')      #DUMMY needs to read an existing file 
L1Fastjet.level = cms.string('L2Relative')  #DUMMY needs to read an existing file 
L1Fastjet.useCondDB = cms.untracked.bool(False) 
L1Fastjet.srcMedianPt = cms.InputTag('kt6PFJets','rho') 

l1FastJetCorrection = cms.Sequence( ak5PFJets * kt6PFJets * ak5PFJetsL1 )




