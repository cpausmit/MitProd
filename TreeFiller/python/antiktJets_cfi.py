# $Id: antiktJets_cfi.py,v 1.1 2009/07/22 04:41:00 bendavid Exp $

import FWCore.ParameterSet.Config as cms

from RecoJets.JetProducers.antikt7CaloJets_cff import *

from RecoJets.Configuration.RecoPFJets_cff import *

antikt7PFJets = antikt5PFJets.clone()
antikt7PFJets.alias = 'ANTIKT7PFJet'
antikt7PFJets.FJ_ktRParam = 0.7

antiktJets = cms.Sequence(antikt7CaloJets*antikt7PFJets)
