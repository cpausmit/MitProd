# $Id: conversionProducer_cff.py,v 1.5 2010/06/23 13:14:15 bendavid Exp $

import FWCore.ParameterSet.Config as cms

def addNSVFitResults(filler):

    filler.fillers.extend(('NSVFitResultsEMu',
                           'NSVFitResultsETau',
                           'NSVFitResultsMuTau'))

    filler.NSVFitResultsEMu = cms.untracked.PSet(
	active = cms.untracked.bool(True),
	mitName = cms.untracked.string('NSVFitResultsEMu'),
	edmName = cms.untracked.string('nSVfitProducerEMu'),
        fillerType   = cms.untracked.string('FillerNSVFit')
    )                               

    filler.NSVFitResultsETau = cms.untracked.PSet(
	active = cms.untracked.bool(True),
	mitName = cms.untracked.string('NSVFitResultsETau'),
	edmName = cms.untracked.string('nSVfitProducerETau'),
        fillerType   = cms.untracked.string('FillerNSVFit')
    )

    filler.NSVFitResultsMuTau = cms.untracked.PSet(
	active = cms.untracked.bool(True),
	mitName = cms.untracked.string('NSVFitResultsMuTau'),
	edmName = cms.untracked.string('nSVfitProducerMuTau'),
        fillerType   = cms.untracked.string('FillerNSVFit')
    )                               

    
