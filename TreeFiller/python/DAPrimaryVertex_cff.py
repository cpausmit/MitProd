# $Id: DAPrimaryVertex_cff.py,v 1.1 2011/03/02 10:26:31 mzanetti Exp $

import FWCore.ParameterSet.Config as cms

offlinePrimaryVerticesDA = cms.EDProducer("PrimaryVertexProducer",
    verbose = cms.untracked.bool(False),
    algorithm = cms.string('AdaptiveVertexFitter'),
    TrackLabel = cms.InputTag("generalTracks"),
    useBeamConstraint = cms.bool(False),
    beamSpotLabel = cms.InputTag("offlineBeamSpot"),
    minNdof  = cms.double(0.0),
    PVSelParameters = cms.PSet(
        maxDistanceToBeam = cms.double(1.0)
    ),
    TkFilterParameters = cms.PSet(
        algorithm=cms.string('filter'),
        maxNormalizedChi2 = cms.double(5.0),
        minPixelLayersWithHits=cms.int32(2),
        minSiliconLayersWithHits = cms.int32(5),
        maxD0Significance = cms.double(5.0), 
        minPt = cms.double(0.0),
        trackQuality = cms.string("any")
    ),

    TkClusParameters = cms.PSet(
        algorithm   = cms.string("DA"),
        TkDAClusParameters = cms.PSet( 
            coolingFactor = cms.double(0.6),  #  rather slow annealing for now
            Tmin = cms.double(4.),            #  end of annealing
            vertexSize = cms.double(0.01)     #  100 um !!
        )
    )
)

daPrimaryVertexSequence = cms.Sequence( offlinePrimaryVerticesDA )
