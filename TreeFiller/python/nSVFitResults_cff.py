# $Id: nSVFitResults_cff.py,v 1.1 2011/05/05 12:43:19 rwolf Exp $

import FWCore.ParameterSet.Config as cms

def addNSVFitResults(filler):

    filler.fillers.extend(('NSVFitResultsEMu',
                           'NSVFitResultsMuMu',
                           'NSVFitResultsETau',
                           'NSVFitResultsMuTau'))

    filler.NSVFitResultsEMu = cms.untracked.PSet(
	active = cms.untracked.bool(True),
	mitName = cms.untracked.string('NSVFitResultsEMu'),
	edmName = cms.untracked.string('nSVfitProducerEMu'),
        fillerType   = cms.untracked.string('FillerNSVFit')
    )                               

    filler.NSVFitResultsMuMu = cms.untracked.PSet(
	active = cms.untracked.bool(True),
	mitName = cms.untracked.string('NSVFitResultsMuMuu'),
	edmName = cms.untracked.string('nSVfitProducerMuMu'),
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


    
