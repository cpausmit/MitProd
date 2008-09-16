import FWCore.ParameterSet.Config as cms

jetVertexAlphaIC5 = cms.EDProducer("JetVertexAssociation", 
    JV_sigmaZ          = cms.double(9.5),
    JV_deltaZ          = cms.double(0.3),
    JV_alpha_threshold = cms.double(0.2),
    JV_cone_size       = cms.double(0.5),
    JV_type_Algo       = cms.int32(1),
    JET_ALGO = cms.string('iterativeCone5CaloJets'),
    TRACK_ALGO = cms.string('generalTracks'),
    VERTEX_ALGO = cms.string('offlinePrimaryVertices'),
    JV_cutType = cms.string('delta'),
)

jetVertexBetaIC5 = cms.EDProducer("JetVertexAssociation", 
    JV_sigmaZ          = cms.double(9.5),
    JV_deltaZ          = cms.double(0.3),
    JV_alpha_threshold = cms.double(0.2),
    JV_cone_size       = cms.double(0.5),
    JV_type_Algo       = cms.int32(2),
    JET_ALGO = cms.string('iterativeCone5CaloJets'),
    TRACK_ALGO = cms.string('generalTracks'),
    VERTEX_ALGO = cms.string('offlinePrimaryVertices'),
    JV_cutType = cms.string('delta'),
)

jetVertexAlphaSC5 = cms.EDProducer("JetVertexAssociation", 
    JV_sigmaZ          = cms.double(9.5),
    JV_deltaZ          = cms.double(0.3),
    JV_alpha_threshold = cms.double(0.2),
    JV_cone_size       = cms.double(0.5),
    JV_type_Algo       = cms.int32(1),
    JET_ALGO = cms.string('sisCone5CaloJets'),
    TRACK_ALGO = cms.string('generalTracks'),
    VERTEX_ALGO = cms.string('offlinePrimaryVertices'),
    JV_cutType = cms.string('delta'),
)

jetVertexBetaSC5 = cms.EDProducer("JetVertexAssociation", 
    JV_sigmaZ          = cms.double(9.5),
    JV_deltaZ          = cms.double(0.3),
    JV_alpha_threshold = cms.double(0.2),
    JV_cone_size       = cms.double(0.5),
    JV_type_Algo       = cms.int32(2),
    JET_ALGO = cms.string('sisCone5CaloJets'),
    TRACK_ALGO = cms.string('generalTracks'),
    VERTEX_ALGO = cms.string('offlinePrimaryVertices'),
    JV_cutType = cms.string('delta'),
)

jetvertexAssociationSequence = cms.Sequence( jetVertexAlphaIC5
                                          + jetVertexBetaIC5
                                          + jetVertexAlphaSC5
                                          + jetVertexBetaSC5
                                          )
