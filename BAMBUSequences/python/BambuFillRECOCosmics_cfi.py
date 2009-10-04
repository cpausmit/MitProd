# $Id: BambuFillRECO_cfi.py,v 1.6 2009/08/11 15:31:29 loizides Exp $

import FWCore.ParameterSet.Config as cms

from MitProd.TreeFiller.MitTreeFiller_CRAFT_cfi import *

# Load stablePart producers
from MitEdm.Producers.conversionElectronsStable_cfi import *
MitTreeFiller.ConversionInOutElectronsStable.active = True
MitTreeFiller.ConversionOutInElectronsStable.active = True

BambuRecoSequence = cms.Sequence(ckfInOutElectronsStable*
                                 ckfOutInElectronsStable
                                 )

BambuRecoFillSequence = cms.Sequence(MitTreeFiller)

BambuFillRECOCosmics = cms.Sequence(BambuRecoSequence*BambuRecoFillSequence)
