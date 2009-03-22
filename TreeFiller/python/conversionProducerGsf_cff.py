# $Id: conversionProducerGsf_cff.py,v 1.5 2009/03/20 20:42:19 bendavid Exp $

import FWCore.ParameterSet.Config as cms

from MitEdm.Producers.HitDropper_cfi import *

import MitEdm.Producers.stableParts_cfi
stablePartsGeneralGsf = MitEdm.Producers.stableParts_cfi.stableParts.clone()

from MitEdm.Producers.conversions_cfi import *
mvfConversionsGsf = MitEdm.Producers.conversions_cfi.mvfConversions.clone()

#stableParts.iTracks = 'ckfOutInTracksFromConversions'
stablePartsGeneralGsf.iTracks = 'generalTracksGsf'
stablePartsGeneralGsf.oPid = 11
mvfConversionsGsf.rhoMin = 0.9
mvfConversionsGsf.iStables1 = 'stablePartsGsf'
mvfConversionsGsf.iStables2 = 'stablePartsGsf'

conversionProducerGsf = cms.Sequence(stablePartsGeneralGsf*mvfConversionsGsf)

conversionFillerGsf = cms.EDAnalyzer("FillMitTree",
    fillers = cms.untracked.vstring('StableParts',
                                    'DecayParts'),
    
    StableParts = cms.untracked.PSet(
      active       = cms.untracked.bool(True),
      mitName      = cms.untracked.string('ElectronsStableGeneralGsf'),
      edmName      = cms.untracked.string('stablePartsGeneralGsf'),
      trackMapName = cms.untracked.string('GeneralGsfTracksMapName'),
      basePartMap  = cms.untracked.string('ElectronsStableGeneralGsfMapName'),
      fillerType   = cms.untracked.string('FillerStableParts')
    ),

    DecayParts = cms.untracked.PSet(
      active       = cms.untracked.bool(True),
      mitName      = cms.untracked.string('MvfConversionsGsf'),
      edmName      = cms.untracked.string('mvfConversionsGsf'),
      vertexMap    = cms.untracked.string('PrimaryVertexBSMap'),
      basePartMaps = cms.untracked.vstring('ElectronsStableGeneralGsfMapName'),
      fillerType   = cms.untracked.string('FillerDecayParts')
    )
)
