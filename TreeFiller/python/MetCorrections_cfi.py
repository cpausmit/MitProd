# $Id: MetCorrections_cfi.py,v 1.9 2009/07/20 05:04:20 loizides Exp $

import FWCore.ParameterSet.Config as cms

#deprecated

#from JetMETCorrections.Type1MET.MetMuonCorrections_cff           import *
#from TrackingTools.TrackAssociator.default_cfi                   import *
#from TrackingTools.TrackAssociator.DetIdAssociatorESProducer_cff import *

##
## Iterative Cone
##
#corMetType1Icone5 = cms.EDFilter("Type1MET",
    #inputUncorJetsLabel = cms.string('iterativeCone5CaloJets'),
    #jetEMfracLimit      = cms.double(0.9),
    #metType             = cms.string('CaloMET'),
    #jetPTthreshold      = cms.double(20.0),
    #inputUncorMetLabel  = cms.string('corMetGlobalMuons'),
    #corrector           = cms.string('L2L3JetCorrectorIC5Calo')
#)

##
## SIS Cone
##
#corMetType1Scone5 = cms.EDFilter("Type1MET",
    #inputUncorJetsLabel = cms.string('sisCone5CaloJets'),
    #jetEMfracLimit      = cms.double(0.9),
    #metType             = cms.string('CaloMET'),
    #jetPTthreshold      = cms.double(20.0),
    #inputUncorMetLabel  = cms.string('corMetGlobalMuons'),
    #corrector           = cms.string('L2L3JetCorrectorSC5Calo')
#)
#corMetType1Scone7 = cms.EDFilter("Type1MET",
    #inputUncorJetsLabel = cms.string('sisCone7CaloJets'),
    #jetEMfracLimit      = cms.double(0.9),
    #metType             = cms.string('CaloMET'),
    #jetPTthreshold      = cms.double(20.0),
    #inputUncorMetLabel  = cms.string('corMetGlobalMuons'),
    #corrector           = cms.string('L2L3JetCorrectorSC7Calo')
#)

##
## kT Cone
##
#corMetType1Kt4 = cms.EDFilter("Type1MET",
    #inputUncorJetsLabel = cms.string('kt4CaloJets'),
    #jetEMfracLimit      = cms.double(0.9),
    #metType             = cms.string('CaloMET'),
    #jetPTthreshold      = cms.double(20.0),
    #inputUncorMetLabel  = cms.string('corMetGlobalMuons'),
    #corrector           = cms.string('L2L3JetCorrectorKT4Calo')
#)
#corMetType1Kt6 = cms.EDFilter("Type1MET",
    #inputUncorJetsLabel = cms.string('kt6CaloJets'),
    #jetEMfracLimit      = cms.double(0.9),
    #metType             = cms.string('CaloMET'),
    #jetPTthreshold      = cms.double(20.0),
    #inputUncorMetLabel  = cms.string('corMetGlobalMuons'),
    #corrector           = cms.string('L2L3JetCorrectorKT6Calo')
#)

#MitMetCorrections = cms.Sequence(#MetMuonCorrections *
                                 #corMetType1Icone5 *
				 #corMetType1Scone5 *
                                 #corMetType1Scone7 *
				 #corMetType1Kt4 *
                                 #corMetType1Kt6)
