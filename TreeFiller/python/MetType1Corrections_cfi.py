import FWCore.ParameterSet.Config as cms

#
# Iterative Cone
#
corMetType1Icone5 = cms.EDFilter("Type1MET",
    inputUncorJetsLabel = cms.string('iterativeCone5CaloJets'),
    jetEMfracLimit = cms.double(0.9),
    metType = cms.string('CaloMET'),
    jetPTthreshold = cms.double(20.0),
    inputUncorMetLabel = cms.string('met'),
    corrector = cms.string('MCJetCorrectorIcone5')
)

#
# SIS Cone
#
corMetType1Scone5 = cms.EDFilter("Type1MET",
    inputUncorJetsLabel = cms.string('sisCone5CaloJets'),
    jetEMfracLimit = cms.double(0.9),
    metType = cms.string('CaloMET'),
    jetPTthreshold = cms.double(20.0),
    inputUncorMetLabel = cms.string('met'),
    corrector = cms.string('MCJetCorrectorIcone5')
)
corMetType1Scone7 = cms.EDFilter("Type1MET",
    inputUncorJetsLabel = cms.string('sisCone7CaloJets'),
    jetEMfracLimit = cms.double(0.9),
    metType = cms.string('CaloMET'),
    jetPTthreshold = cms.double(20.0),
    inputUncorMetLabel = cms.string('met'),
    corrector = cms.string('MCJetCorrectorScone7')
)

#
# kT Cone
#
corMetType1Kt4 = cms.EDFilter("Type1MET",
    inputUncorJetsLabel = cms.string('kt4CaloJets'),
    jetEMfracLimit = cms.double(0.9),
    metType = cms.string('CaloMET'),
    jetPTthreshold = cms.double(20.0),
    inputUncorMetLabel = cms.string('met'),
    corrector = cms.string('MCJetCorrectorKt4')
)
corMetType1Kt6 = cms.EDFilter("Type1MET",
    inputUncorJetsLabel = cms.string('kt6CaloJets'),
    jetEMfracLimit = cms.double(0.9),
    metType = cms.string('CaloMET'),
    jetPTthreshold = cms.double(20.0),
    inputUncorMetLabel = cms.string('met'),
    corrector = cms.string('MCJetCorrectorKt6')
)

MetType1Corrections = cms.Sequence( corMetType1Icone5
				   *corMetType1Scone5*corMetType1Scone7
				   *corMetType1Kt4*corMetType1Kt6)

