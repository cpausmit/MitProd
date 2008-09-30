# $Id: $

import FWCore.ParameterSet.Config as cms

from RecoEgamma.ElectronIdentification.electronIdCutBased_cfi import *
from RecoEgamma.ElectronIdentification.electronIdCutBasedClasses_cfi import *
from RecoEgamma.ElectronIdentification.electronIdLikelihood_cfi import *
from RecoEgamma.ElectronIdentification.electronIdNeuralNet_cfi import *
from RecoEgamma.ElectronIdentification.electronIdCutBasedExt_cfi import *
from RecoEgamma.ElectronIdentification.electronIdCutBasedClassesExt_cfi import *
from RecoEgamma.ElectronIdentification.electronIdLikelihoodExt_cfi import *
from RecoEgamma.ElectronIdentification.electronIdNeuralNetExt_cfi import *

MitEIdSequence = cms.Sequence( eidCutBased +
                            eidCutBasedExt +
                            eidCutBasedClasses +
                            eidCutBasedClassesExt +
                            eidLikelihood +
                            eidLikelihoodExt + 
                            eidNeuralNet +
                            eidNeuralNetExt)
