# $Id: MitPostRecoGenerator_cff.py,v 1.7 2009/07/20 05:04:20 loizides Exp $

import FWCore.ParameterSet.Config as cms

from RecoJets.JetProducers.antikt5CaloJets_cff import *
from RecoJets.JetProducers.antikt7CaloJets_cff import *

from RecoJets.Configuration.RecoPFJets_cff import *

antikt7PFJets = antikt5PFJets.clone()
antikt7PFJets.alias = 'ANTIKT7PFJet'
antikt7PFJets.FJ_ktRParam = 0.7

antiktJets = cms.Sequence(antikt5CaloJets*antikt7CaloJets*antikt5PFJets*antikt7PFJets)
