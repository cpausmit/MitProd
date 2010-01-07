# $Id: BambuFillRECODEBUG_cfi.py,v 1.2 2009/07/22 11:26:19 loizides Exp $

import FWCore.ParameterSet.Config as cms

from MitProd.BAMBUSequences.BambuFillRECOSIM_cfi import *

from MitProd.TreeFiller.MitPostRecoGenerator_cff import *

# Enable TrackingParticle association to MCParticles
MitTreeFiller.MCParticles.trackingActive = True
MitTreeFiller.MCParticles.fillTracking = True

BambuFillRECODEBUG = cms.Sequence(mit_postreco_generator*BambuFillRECOSIM)
