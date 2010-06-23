# $Id: conversionProducer_cff.py,v 1.4 2009/11/03 15:13:18 bendavid Exp $

import FWCore.ParameterSet.Config as cms

from MitEdm.Producers.HitDropper_cfi import *

from MitEdm.Producers.conversions_cfi import *

conversionProducer = cms.Sequence(mvfConversions*mvfTrackerConversions*mvfConversionRemoval)

def addConversionFiller(filler):

    filler.fillers.extend(('MvfConversions',
                          'MvfTrackerConversions',
                          'MvfConversionRemoval',
                          'KinematicConversions'))

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
    
    filler.MvfConversionRemoval = cms.untracked.PSet(
      active       = cms.untracked.bool(True),
      mitName      = cms.untracked.string('MvfConversionRemoval'),
      edmName      = cms.untracked.string('mvfConversionRemoval'),
      vertexMap    = cms.untracked.string('PrimaryVertexBSMap'),
      basePartMaps = cms.untracked.vstring('ElectronsStableGsfMapName','MergedElectronsStableMapName'),
      fillerType   = cms.untracked.string('FillerDecayParts')
    )

    filler.KinematicConversions = cms.untracked.PSet(
      active       = cms.untracked.bool(False),
      mitName      = cms.untracked.string('KinematicConversions'),
      edmName      = cms.untracked.string('kinematicConversions'),
      vertexMap    = cms.untracked.string('PrimaryVertexBSMap'),
      basePartMaps = cms.untracked.vstring('MergedElectronsStableMapName'),
      fillerType   = cms.untracked.string('FillerDecayParts')
    )
    