
import FWCore.ParameterSet.Config as cms

from MitProd.BAMBUSequences.BambuFillRECOSIM_cfi import *

from MitProd.TreeFiller.MitPostRecoGenerator_cff import *

#Enable TrackingParticle association to MCParticles
MitTreeFiller.MCParticles.trackingActive = True

BambuFillRECODEBUG = cms.Sequence(mit_postreco_generator*BambuFillRECOSIM)
