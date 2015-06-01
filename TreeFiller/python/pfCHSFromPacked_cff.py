import FWCore.ParameterSet.Config as cms

# cuts taken from CommonTools/ParticleFlow/python/Isolation
# muon iso does not follow the POG recommendation, but that's what it is..
pfIsolatedMuons = cms.EDFilter("CandPtrSelector",
    src = cms.InputTag("slimmedMuons"),
    cut = cms.string('''
    (
     (pfCandidateRef.isNonnull() && sourceCandidatePtr(1).isNonnull() && sourceCandidatePtr(1).fromPV() > 0) ||
     (!pfCandidateRef.isNonnull() && sourceCandidatePtr(0).isNonnull() && sourceCandidatePtr(0).fromPV() > 0)
    ) &&
    abs(eta) < 2.4 && pt > 5. &&
    (
     pfIsolationR04().sumChargedHadronPt + 
     pfIsolationR04().sumNeutralHadronEt +
     pfIsolationR04().sumPhotonEt
    ) / pt < 0.15 &&
    (isPFMuon && (isGlobalMuon || isTrackerMuon))
    ''')
)
pfIsolatedElectrons = cms.EDFilter("CandPtrSelector",
    src = cms.InputTag("slimmedElectrons"),
    cut = cms.string('''
    (
     (pfCandidateRef.isNonnull() && sourceCandidatePtr(1).isNonnull() && sourceCandidatePtr(1).fromPV() > 0) ||
     (!pfCandidateRef.isNonnull() && sourceCandidatePtr(0).isNonnull() && sourceCandidatePtr(0).fromPV() > 0)
    ) &&
    abs(eta) < 2.5 && pt > 5. &&
    gsfTrack.isAvailable() &&
    gsfTrack.hitPattern().numberOfLostHits(\'MISSING_INNER_HITS\') < 2 &&
    (
     pfIsolationVariables().sumChargedHadronPt +
     pfIsolationVariables().sumNeutralHadronEt + 
     pfIsolationVariables().sumPhotonEt
    ) / pt < 0.2
    ''')
)

## Do projections
pfNoPileUp = cms.EDFilter("CandPtrSelector",
    src = cms.InputTag("packedPFCandidates"),
    cut = cms.string("fromPV")
)
pfNoMuon = cms.EDProducer("CandPtrProjector",
    src = cms.InputTag("pfNoPileUp"),
    veto = cms.InputTag("pfIsolatedMuons")
)
pfNoElectron = cms.EDProducer("CandPtrProjector",
    src = cms.InputTag("pfNoMuon"),
    veto = cms.InputTag("pfIsolatedElectrons")
)

pfCHSSequence = cms.Sequence(
    pfIsolatedMuons *
    pfIsolatedElectrons *
    pfNoPileUp *
    pfNoMuon *
    pfNoElectron
)
