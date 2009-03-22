# $Id: ElectronID_cfi.py,v 1.2 2009/03/03 18:52:14 bendavid Exp $

import FWCore.ParameterSet.Config as cms

from RecoEgamma.ElectronIdentification.electronIdLikelihood_cfi import *
from RecoEgamma.ElectronIdentification.electronIdLikelihoodExt_cfi import *

MitEIdSequence = cms.Sequence(eidLikelihood +
                              eidLikelihoodExt)
