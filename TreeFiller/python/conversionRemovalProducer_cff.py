# $Id: conversionRemovalProducer_cff.py,v 1.4 2009/07/14 13:47:05 bendavid Exp $

import FWCore.ParameterSet.Config as cms

from MitEdm.Producers.HitDropper_cfi import *

from MitEdm.Producers.conversions_cfi import *

import MitEdm.Producers.conversions_cfi

mvfConversionsRemoval = MitEdm.Producers.conversions_cfi.mvfConversions.clone()

mvfConversionsRemoval.rhoMin = 0.9
mvfConversionsRemoval.iStables1 = 'generalElectronsStable'
mvfConversionsRemoval.iStables2 = 'stablePartsGsf'

conversionRemovalProducer = cms.Sequence(mvfConversionsRemoval)

conversionRemovalFiller = cms.EDAnalyzer("FillMitTree",
    fillers = cms.untracked.vstring('StableParts',
                                    'DecayParts'),
    
    StableParts = cms.untracked.PSet(
      active        = cms.untracked.bool(True),
      mitName       = cms.untracked.string('ElectronsStableGsf'),
      edmName       = cms.untracked.string('gsfElectronsStable'),
      trackMapNames = cms.untracked.vstring('GsfTracksMapName'),
      basePartMap   = cms.untracked.string('ElectronsStableGsfMapName'),
      fillerType    = cms.untracked.string('FillerStableParts')
    ),

    DecayParts = cms.untracked.PSet(
      active       = cms.untracked.bool(True),
      mitName      = cms.untracked.string('MvfConversionsRemoval'),
      edmName      = cms.untracked.string('mvfConversionsRemoval'),
      vertexMap    = cms.untracked.string('PrimaryVertexBSMap'),
      basePartMaps = cms.untracked.vstring('ElectronsStableMapName','ElectronsStableGsfMapName'),
      fillerType   = cms.untracked.string('FillerDecayParts')
    )
)
