# $Id: ElectronID_cfi.py,v 1.1 2008/09/30 15:55:45 sixie Exp $

import FWCore.ParameterSet.Config as cms

from RecoEgamma.ElectronIdentification.electronIdLikelihood_cfi import *
from RecoEgamma.ElectronIdentification.electronIdLikelihoodExt_cfi import *

MitEIdSequence = cms.Sequence(eidLikelihood    +
                              eidLikelihoodExt)
