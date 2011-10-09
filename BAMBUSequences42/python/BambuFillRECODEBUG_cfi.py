# $Id: BambuFillRECODEBUG_cfi.py,v 1.3 2010/01/07 11:05:06 bendavid Exp $

import FWCore.ParameterSet.Config as cms

from MitProd.BAMBUSequences42.BambuFillRECOSIM_cfi import *

from MitProd.TreeFiller.MitPostRecoGenerator_cff import *

# Enable TrackingParticle association to MCParticles
MitTreeFiller.MCParticles.trackingActive = True
MitTreeFiller.MCParticles.fillTracking = True

BambuFillRECODEBUG = cms.Sequence(mit_postreco_generator*BambuFillRECOSIM)
