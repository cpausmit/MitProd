# $Id: conversionRemovalProducer_cff.py,v 1.1 2009/03/22 08:49:00 loizides Exp $

import FWCore.ParameterSet.Config as cms

from MitEdm.Producers.HitDropper_cfi import *

import MitEdm.Producers.stableParts_cfi
stablePartsGsf = MitEdm.Producers.stableParts_cfi.stableParts.clone()

from MitEdm.Producers.conversions_cfi import *
mvfConversionsRemoval = MitEdm.Producers.conversions_cfi.mvfConversions.clone()

#stableParts.iTracks = 'ckfOutInTracksFromConversions'
stablePartsGsf.iTracks = 'electronGsfTracks'
stablePartsGsf.oPid = 11
mvfConversionsRemoval.rhoMin = 0.9
mvfConversionsRemoval.iStables1 = 'generalElectronsStable'
mvfConversionsRemoval.iStables2 = 'stablePartsGsf'

conversionRemovalProducer = cms.Sequence(stablePartsGsf*mvfConversionsRemoval)

conversionRemovalFiller = cms.EDAnalyzer("FillMitTree",
    fillers = cms.untracked.vstring('StableParts',
                                    'DecayParts'),
    
    StableParts = cms.untracked.PSet(
      active       = cms.untracked.bool(True),
      mitName      = cms.untracked.string('ElectronsStableGsf'),
      edmName      = cms.untracked.string('stablePartsGsf'),
      trackMapName = cms.untracked.string('GsfTracksMapName'),
      basePartMap  = cms.untracked.string('ElectronsStableGsfMapName'),
      fillerType   = cms.untracked.string('FillerStableParts')
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
