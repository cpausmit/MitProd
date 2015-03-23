# $Id: antiktGenJets_cfi.py,v 1.1 2009/07/22 04:41:00 bendavid Exp $

import FWCore.ParameterSet.Config as cms

from RecoJets.Configuration.GenJetParticles_cff import *
from RecoJets.Configuration.RecoGenJets_cff import *

antiktGenJets = cms.Sequence(genJetParticles*ak5GenJets*ak8GenJets)
