# $Id: MitTreeFiller_CRAFT.py,v 1.4 2009/03/24 15:49:13 bendavid Exp $
#---------------------------------------------------------------------------------------------------
# This template config file is intended to be a reference for the "HEAD" bambu tree version.
# This config file will be used by the mitprod account to do production on CRAB. It must
# be ensured that this config file is always working with the production CMSSW release.
#---------------------------------------------------------------------------------------------------
# List of paramters to be properly replaced
#
#  - XX-MITDATASET-XX - MIT type dataset name (ex. csa08-1ipb-jpsi)
#
#---------------------------------------------------------------------------------------------------

import FWCore.ParameterSet.Config as cms
process = cms.Process("FILLER")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1000) )
process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring(
   'file:/server/02a/bendavid/RECO/CRAFT_225_FromSuperpointing/6E1D2117-280C-DE11-B00A-001731AF677D.root'
                             ),
   secondaryFileNames = cms.untracked.vstring()
)

#Load MitTreeFiller 
process.TreeService = cms.Service("TreeService",
    fileNames = cms.untracked.vstring('mit-craft')
)
process.add_(cms.Service("ObjectService"))
process.load("MitProd.TreeFiller.MitTreeFiller_CRAFT_cfi")

process.load("FWCore.MessageLogger.MessageLogger_cfi")

process.p1 = cms.Path(
    process.MitTreeFiller
)
