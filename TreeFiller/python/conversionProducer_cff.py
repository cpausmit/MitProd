# $Id: conversionProducer_cff.py,v 1.3 2009/07/12 13:12:17 bendavid Exp $

import FWCore.ParameterSet.Config as cms

from MitEdm.Producers.HitDropper_cfi import *

from MitEdm.Producers.conversions_cfi import *

conversionProducer = cms.Sequence(mvfConversions*mvfTrackerConversions)

def addConversionFiller(filler):

    filler.fillers.extend(('MvfConversions',
                          'MvfTrackerConversions'))

    filler.MvfConversions = cms.untracked.PSet(
      active       = cms.untracked.bool(True),
      mitName      = cms.untracked.string('MvfConversions'),
      edmName      = cms.untracked.string('mvfConversions'),
      vertexMap    = cms.untracked.string('PrimaryVertexBSMap'),
      basePartMaps = cms.untracked.vstring('MergedElectronsStableMapName'),
      fillerType   = cms.untracked.string('FillerDecayParts')
    )
  
    filler.MvfTrackerConversions = cms.untracked.PSet(
      active       = cms.untracked.bool(True),
      mitName      = cms.untracked.string('MvfTrackerConversions'),
      edmName      = cms.untracked.string('mvfTrackerConversions'),
      vertexMap    = cms.untracked.string('PrimaryVertexBSMap'),
      basePartMaps = cms.untracked.vstring('ElectronsStableMapName'),
      fillerType   = cms.untracked.string('FillerDecayParts')
    )
