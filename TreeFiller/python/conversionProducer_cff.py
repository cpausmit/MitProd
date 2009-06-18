# $Id: conversionProducer_cff.py,v 1.1 2009/03/22 08:49:00 loizides Exp $

import FWCore.ParameterSet.Config as cms

from MitEdm.Producers.HitDropper_cfi import *

from MitEdm.Producers.conversions_cfi import *

#stableParts.iTracks = 'ckfOutInTracksFromConversions'
#stableParts.oPid = 11
mvfConversions.rhoMin = 0.9

conversionProducer = cms.Sequence(mvfConversions)

conversionFiller = cms.EDAnalyzer("FillMitTree",
    fillers = cms.untracked.vstring('DecayParts'),

    DecayParts = cms.untracked.PSet(
      active       = cms.untracked.bool(True),
      mitName      = cms.untracked.string('MvfConversions'),
      edmName      = cms.untracked.string('mvfConversions'),
      vertexMap    = cms.untracked.string('PrimaryVertexBSMap'),
      basePartMaps = cms.untracked.vstring('ElectronsStableMapName'),
      fillerType   = cms.untracked.string('FillerDecayParts')
    )
)
