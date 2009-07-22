# $Id: MitPostRecoGenerator_cff.py,v 1.7 2009/07/20 05:04:20 loizides Exp $

import FWCore.ParameterSet.Config as cms

from RecoJets.Configuration.GenJetParticles_cff import *
from RecoJets.JetProducers.antikt5GenJets_cff import *
from RecoJets.JetProducers.antikt7GenJets_cff import *

antiktGenJets = cms.Sequence(genJetParticles*antikt5GenJets*antikt7GenJets)
