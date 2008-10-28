# $Id: MitTreeFiller_template.py,v 1.12 2008/10/07 17:57:19 sixie Exp $
#---------------------------------------------------------------------------------------------------
# This template config file is intended to be a reference for the "HEAD" OAK tree version.
# This config file will be used by the mitprod account to do production on CRAB. It must
# be ensured that this config file is always working with the production CMSSW release
#---------------------------------------------------------------------------------------------------
# List of paramters to be properly replaced
#
#  - XX-MITDATASET-XX - MIT type dataset name (ex. csa08-1ipb-jpsi)
#
#---------------------------------------------------------------------------------------------------

import FWCore.ParameterSet.Config as cms
process = cms.Process("FILLER")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(99999999) )
process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring(
   #'file:/server/02a/sixie/RECO/001EA63A-DF60-DD11-9D5A-001A92810AA6.root'
   #'/store/data/Commissioning08/Cosmics/RECO/v1/000/067/818/785E3E2E-C0A4-DD11-88AE-001D09F23F2A.root'
   #'/store/data/Commissioning08/Cosmics/RECO/v1/000/067/818/AE974FB8-B0A4-DD11-86A3-001D09F2932B.root'
   #'/store/data/Commissioning08/Cosmics/RECO/v1/000/067/647/B67938C8-60A3-DD11-B865-001D09F23F2A.root'
   '/store/data/Commissioning08/Cosmics/RECO/v1/000/067/818/F861BA57-A6A4-DD11-BFF6-000423D98804.root'
                             ),
   secondaryFileNames = cms.untracked.vstring()
)

#Load MitTreeFiller 
process.TreeService = cms.Service("TreeService",
    fileNames = cms.untracked.vstring('XX-MITDATASET-XX')
)
process.add_(cms.Service("ObjectService"))
process.load("MitProd.TreeFiller.MitTreeFiller_CRAFT_cfi")

process.rsTrackFiller = cms.EDAnalyzer("FillMitTree",
    defactive = cms.untracked.bool(False),
    
    GeneralTracks  = cms.untracked.PSet(
        active                = cms.untracked.bool(True),
        mitName               = cms.untracked.string('RsWithMaterialTracks'),
        edmName               = cms.untracked.string('rsWithMaterialTracksP5'),
        trackingMapName       = cms.untracked.string('TrackingMap'),
        trackMapName          = cms.untracked.string('rsTracksMapName'),
        edmSimAssociationName = cms.untracked.string('trackingParticleRecoTrackAsssociation')
    )
)

process.cosmicTrackFiller = cms.EDAnalyzer("FillMitTree",
    defactive = cms.untracked.bool(False),
    
    GeneralTracks  = cms.untracked.PSet(
        active                = cms.untracked.bool(True),
        mitName               = cms.untracked.string('CosmicTracks'),
        edmName               = cms.untracked.string('cosmictrackfinderP5'),
        trackingMapName       = cms.untracked.string('TrackingMap'),
        trackMapName          = cms.untracked.string('cosmicTracksMapName'),
        edmSimAssociationName = cms.untracked.string('trackingParticleRecoTrackAsssociation')
    )
)

process.load("FWCore.MessageLogger.MessageLogger_cfi")


process.p1 = cms.Path(
     process.MitTreeFiller
    *process.rsTrackFiller
    *process.cosmicTrackFiller
     )
