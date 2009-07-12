# $Id: conversionProducer_cff.py,v 1.2 2009/06/18 23:10:39 bendavid Exp $

import FWCore.ParameterSet.Config as cms

from MitEdm.Producers.HitDropper_cfi import *

from MitEdm.Producers.conversions_cfi import *

conversionProducer = cms.Sequence(mvfConversions*mvfTrackerConversions)

conversionFiller = cms.EDAnalyzer("FillMitTree",
    fillers = cms.untracked.vstring('MvfConversions',
                                    'MvfTrackerConversions'),

    MvfConversions = cms.untracked.PSet(
      active       = cms.untracked.bool(True),
      mitName      = cms.untracked.string('MvfConversions'),
      edmName      = cms.untracked.string('mvfConversions'),
      vertexMap    = cms.untracked.string('PrimaryVertexBSMap'),
      basePartMaps = cms.untracked.vstring('MergedElectronsStableMapName'),
      fillerType   = cms.untracked.string('FillerDecayParts')
    ),

    MvfTrackerConversions = cms.untracked.PSet(
      active       = cms.untracked.bool(True),
      mitName      = cms.untracked.string('MvfTrackerConversions'),
      edmName      = cms.untracked.string('mvfTrackerConversions'),
      vertexMap    = cms.untracked.string('PrimaryVertexBSMap'),
      basePartMaps = cms.untracked.vstring('ElectronsStableMapName'),
      fillerType   = cms.untracked.string('FillerDecayParts')
    ),

)
