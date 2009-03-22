# $Id: MitMHT_cfi.py,v 1.2 2009/03/16 08:29:30 loizides Exp $

import FWCore.ParameterSet.Config as cms

htMetSC5 = cms.EDProducer("METProducer",
    src             = cms.InputTag("L2L3CorJetSC5Calo"),
    METType         = cms.string('MET'),
    alias           = cms.string('HTMETSC5'),
    noHF            = cms.bool(False),
    globalThreshold = cms.double(5.0),
    InputType       = cms.string('CaloJetCollection')
)

htMetSC7 = cms.EDProducer("METProducer",
    src             = cms.InputTag("L2L3CorJetSC7Calo"),
    METType         = cms.string('MET'),
    alias           = cms.string('HTMETSC7'),
    noHF            = cms.bool(False),
    globalThreshold = cms.double(5.0),
    InputType       = cms.string('CaloJetCollection')
)

htMetIC5 = cms.EDProducer("METProducer",
    src             = cms.InputTag("L2L3CorJetIC5Calo"),
    METType         = cms.string('MET'),
    alias           = cms.string('HTMETIC5'),
    noHF            = cms.bool(False),
    globalThreshold = cms.double(5.0),
    InputType       = cms.string('CaloJetCollection')
)

htMetKT4 = cms.EDProducer("METProducer",
    src             = cms.InputTag("L2L3CorJetKT4Calo"),
    METType         = cms.string('MET'),
    alias           = cms.string('HTMETKT4'),
    noHF            = cms.bool(False),
    globalThreshold = cms.double(5.0),
    InputType       = cms.string('CaloJetCollection')
)

htMetKT6 = cms.EDProducer("METProducer",
    src             = cms.InputTag("L2L3CorJetKT6Calo"),
    METType         = cms.string('MET'),
    alias           = cms.string('HTMETKT6'),
    noHF            = cms.bool(False),
    globalThreshold = cms.double(5.0),
    InputType       = cms.string('CaloJetCollection')
)

htMetIC5JPT = cms.EDProducer("METProducer",
    src             = cms.InputTag("L2L3CorJetIC5JPT"),
    METType         = cms.string('MET'),
    alias           = cms.string('HTMETIC5JPT'),
    noHF            = cms.bool(False),
    globalThreshold = cms.double(5.0),
    InputType       = cms.string('CaloJetCollection')
)

import JetMETCorrections.Type1MET.MetMuonCorrections_cff

corMHTGlobalMuonsSC5 = JetMETCorrections.Type1MET.MetMuonCorrections_cff.corMetGlobalMuons.clone()
JetMETCorrections.Type1MET.uncorMETInputTag = htMetSC5

corMHTGlobalMuonsSC7 = JetMETCorrections.Type1MET.MetMuonCorrections_cff.corMetGlobalMuons.clone()
JetMETCorrections.Type1MET.uncorMETInputTag = htMetSC7

corMHTGlobalMuonsIC5 = JetMETCorrections.Type1MET.MetMuonCorrections_cff.corMetGlobalMuons.clone()
JetMETCorrections.Type1MET.uncorMETInputTag = htMetIC5

corMHTGlobalMuonsKT4 = JetMETCorrections.Type1MET.MetMuonCorrections_cff.corMetGlobalMuons.clone()
JetMETCorrections.Type1MET.uncorMETInputTag = htMetKT4

corMHTGlobalMuonsKT6 = JetMETCorrections.Type1MET.MetMuonCorrections_cff.corMetGlobalMuons.clone()
JetMETCorrections.Type1MET.uncorMETInputTag = htMetKT6

corMHTGlobalMuonsIC5JPT = JetMETCorrections.Type1MET.MetMuonCorrections_cff.corMetGlobalMuons.clone()
JetMETCorrections.Type1MET.uncorMETInputTag = htMetIC5JPT

MitMHT = cms.Sequence(htMetSC5*
                      htMetSC7*
                      htMetIC5*
                      htMetKT4*
                      htMetKT6*
                      htMetIC5JPT*
                      corMHTGlobalMuonsSC5*
                      corMHTGlobalMuonsSC7*
                      corMHTGlobalMuonsIC5*
                      corMHTGlobalMuonsKT4*
                      corMHTGlobalMuonsKT6*
                      corMHTGlobalMuonsIC5JPT)
