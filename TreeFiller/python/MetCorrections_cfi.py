import FWCore.ParameterSet.Config as cms

from JetMETCorrections.Type1MET.MetMuonCorrections_cff import *
from Configuration.StandardSequences.MagneticField_cff import *
from Geometry.CommonDetUnit.globalTrackingGeometry_cfi import *
from Configuration.StandardSequences.FrontierConditions_GlobalTag_cff import *
GlobalTag.globaltag="IDEAL_V9::All" 
from Geometry.CommonDetUnit.bareGlobalTrackingGeometry_cfi import *
from TrackingTools.TrackAssociator.default_cfi    import *
from TrackingTools.TrackAssociator.DetIdAssociatorESProducer_cff   import *


#
# Iterative Cone
#
corMetType1Icone5 = cms.EDFilter("Type1MET",
    inputUncorJetsLabel = cms.string('iterativeCone5CaloJets'),
    jetEMfracLimit = cms.double(0.9),
    metType = cms.string('CaloMET'),
    jetPTthreshold = cms.double(20.0),
    inputUncorMetLabel = cms.string('corMetGlobalMuons'),
    corrector = cms.string('L2L3JetCorrectorIcone5')
)

#
# SIS Cone
#
corMetType1Scone5 = cms.EDFilter("Type1MET",
    inputUncorJetsLabel = cms.string('sisCone5CaloJets'),
    jetEMfracLimit = cms.double(0.9),
    metType = cms.string('CaloMET'),
    jetPTthreshold = cms.double(20.0),
    inputUncorMetLabel = cms.string('corMetGlobalMuons'),
    corrector = cms.string('L2L3JetCorrectorScone5')
)
corMetType1Scone7 = cms.EDFilter("Type1MET",
    inputUncorJetsLabel = cms.string('sisCone7CaloJets'),
    jetEMfracLimit = cms.double(0.9),
    metType = cms.string('CaloMET'),
    jetPTthreshold = cms.double(20.0),
    inputUncorMetLabel = cms.string('corMetGlobalMuons'),
    corrector = cms.string('L2L3JetCorrectorScone7')
)

#
# kT Cone
#
corMetType1Kt4 = cms.EDFilter("Type1MET",
    inputUncorJetsLabel = cms.string('kt4CaloJets'),
    jetEMfracLimit = cms.double(0.9),
    metType = cms.string('CaloMET'),
    jetPTthreshold = cms.double(20.0),
    inputUncorMetLabel = cms.string('corMetGlobalMuons'),
    corrector = cms.string('L2L3JetCorrectorKt4')
)
corMetType1Kt6 = cms.EDFilter("Type1MET",
    inputUncorJetsLabel = cms.string('kt6CaloJets'),
    jetEMfracLimit = cms.double(0.9),
    metType = cms.string('CaloMET'),
    jetPTthreshold = cms.double(20.0),
    inputUncorMetLabel = cms.string('corMetGlobalMuons'),
    corrector = cms.string('L2L3JetCorrectorKt6')
)

MitMetCorrections = cms.Sequence( MetMuonCorrections
                                   *corMetType1Icone5
				   *corMetType1Scone5
                                   *corMetType1Scone7
				   *corMetType1Kt4
                                   *corMetType1Kt6)

