import FWCore.ParameterSet.Config as cms

#################### L2 Source definitions ##################################
L2JetCorrectorIcone5 = cms.ESSource("L2RelativeCorrectionService", 
    tagName = cms.string('iCSA08_S156_L2Relative_Icone5'),
    label = cms.string('L2RelativeJetCorrectorIcone5')
)
L2JetCorrectorPFIcone5 = cms.ESSource("L2RelativeCorrectionService", 
    tagName = cms.string('iCSA08_S156_L2Relative_PFIcone5'),
    label = cms.string('L2RelativeJetCorrectorPFIcone5')
)
L2JetCorrectorScone5 = cms.ESSource("L2RelativeCorrectionService", 
    tagName = cms.string('iCSA08_S156_L2Relative_Scone5'),
    label = cms.string('L2RelativeJetCorrectorScone5')
)
L2JetCorrectorScone7 = cms.ESSource("L2RelativeCorrectionService", 
    tagName = cms.string('iCSA08_S156_L2Relative_Scone7'),
    label = cms.string('L2RelativeJetCorrectorScone7')
)
L2JetCorrectorKt4 = cms.ESSource("L2RelativeCorrectionService", 
    tagName = cms.string('iCSA08_S156_L2Relative_Kt4'),
    label = cms.string('L2RelativeJetCorrectorKt4')
)
L2JetCorrectorKt6 = cms.ESSource("L2RelativeCorrectionService", 
    tagName = cms.string('iCSA08_S156_L2Relative_Kt6'),
    label = cms.string('L2RelativeJetCorrectorKt6')
)
#################### L3 Source definitions ##################################
L3JetCorrectorIcone5 = cms.ESSource("L3AbsoluteCorrectionService", 
    tagName = cms.string('iCSA08_S156_L3Absolute_Icone5'),
    label = cms.string('L3AbsoluteJetCorrectorIcone5')
)
L3JetCorrectorPFIcone5 = cms.ESSource("L3AbsoluteCorrectionService", 
    tagName = cms.string('iCSA08_S156_L3Absolute_PFIcone5'),
    label = cms.string('L3AbsoluteJetCorrectorPFIcone5')
)
L3JetCorrectorScone5 = cms.ESSource("L3AbsoluteCorrectionService", 
    tagName = cms.string('iCSA08_S156_L3Absolute_Scone5'),
    label = cms.string('L3AbsoluteJetCorrectorScone5')
)
L3JetCorrectorScone7 = cms.ESSource("L3AbsoluteCorrectionService", 
    tagName = cms.string('iCSA08_S156_L3Absolute_Scone7'),
    label = cms.string('L3AbsoluteJetCorrectorScone7')
)
L3JetCorrectorKt4 = cms.ESSource("L3AbsoluteCorrectionService", 
    tagName = cms.string('iCSA08_S156_L3Absolute_Kt4'),
    label = cms.string('L3AbsoluteJetCorrectorKt4')
)
L3JetCorrectorKt6 = cms.ESSource("L3AbsoluteCorrectionService", 
    tagName = cms.string('iCSA08_S156_L3Absolute_Kt6'),
    label = cms.string('L3AbsoluteJetCorrectorKt6')
)
