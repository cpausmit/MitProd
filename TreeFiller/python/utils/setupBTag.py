import FWCore.ParameterSet.Config as cms
from RecoBTag.Configuration.RecoBTag_cff import *
from RecoBTag.CTagging.RecoCTagging_cff import *

def setupBTag(process, jetCollection, suffix, candidates = 'particleFlow', primaryVertex = 'offlinePrimaryVertices', muons = 'muons', electrons = 'gedGsfElectrons'):
    """
    Configure the BTag sequence for the given jet collection.
    The suffix will be appended to the CMSSW module names to
    distinguish sequences for multiple collections.
    """

    # btag info
    ipTagInfosName = 'pfImpactParameterTagInfos' + suffix
    svTagInfosName = 'pfSecondaryVertexTagInfos' + suffix
    ivfTagInfosName = 'pfInclusiveSecondaryVertexFinderTagInfos' + suffix
    smTagInfosName = 'softPFMuonsTagInfos' + suffix
    seTagInfosName = 'softPFElectronsTagInfos' + suffix
    # ctag info
    ivfcvslTagInfosName = 'pfInclusiveSecondaryVertexFinderCvsLTagInfos' + suffix

    # modules below will be later setattr'ed to process using the names above
    newImpactParameterTagInfos = pfImpactParameterTagInfos.clone(
        jets = jetCollection,
        candidates = candidates,
        primaryVertex = primaryVertex
    )
    newSecondaryVertexTagInfos = pfSecondaryVertexTagInfos.clone(
        trackIPTagInfos = ipTagInfosName
    )
    newInclusiveSecondaryVertexFinderTagInfos = pfInclusiveSecondaryVertexFinderTagInfos.clone(
        trackIPTagInfos = ipTagInfosName
    )
    newSoftPFMuonsTagInfos = softPFMuonsTagInfos.clone(
        jets = jetCollection,
        muons = muons,
        primaryVertex = primaryVertex
    )
    newSoftPFElectronsTagInfos = softPFElectronsTagInfos.clone(
        jets = jetCollection,
        electrons = electrons,
        primaryVertex = primaryVertex
    )
    newInclusiveSecondaryVertexFinderCvsLTagInfos = pfInclusiveSecondaryVertexFinderCvsLTagInfos.clone(
        trackIPTagInfos = ipTagInfosName
    )

    inclusiveCandidateVertexFinder.primaryVertices = primaryVertex
    inclusiveCandidateVertexFinder.tracks = candidates
    candidateVertexArbitrator.primaryVertices = primaryVertex
    candidateVertexArbitrator.tracks = candidates

    # impact parameter b-tags
    newTrackCountingHighEffBJetTags = pfTrackCountingHighEffBJetTags.clone(
        tagInfos = cms.VInputTag(cms.InputTag(ipTagInfosName))
    )

    # jet probability b-tags
    newJetProbabilityBJetTags = pfJetProbabilityBJetTags.clone(
        tagInfos = cms.VInputTag(cms.InputTag(ipTagInfosName))
    )
    newJetBProbabilityBJetTags = pfJetBProbabilityBJetTags.clone(
        tagInfos = cms.VInputTag(cms.InputTag(ipTagInfosName))
    )

    # secondary vertex b-tags
    newSimpleSecondaryVertexHighEffBJetTags = pfSimpleSecondaryVertexHighEffBJetTags.clone(
        tagInfos = cms.VInputTag(cms.InputTag(svTagInfosName))
    )

    # inclusive secondary vertex b-tags
    newSimpleInclusiveSecondaryVertexHighEffBJetTags = pfSimpleInclusiveSecondaryVertexHighEffBJetTags.clone(
        tagInfos = cms.VInputTag(cms.InputTag(ivfTagInfosName))
    )

    # combined secondary vertex b-tags
    newCombinedSecondaryVertexV2BJetTags = pfCombinedSecondaryVertexV2BJetTags.clone(
        tagInfos = cms.VInputTag(
            cms.InputTag(ipTagInfosName),
            cms.InputTag(svTagInfosName)
        )
    )

    # inclusive combined secondary vertex b-tags
    newCombinedInclusiveSecondaryVertexV2BJetTags = pfCombinedInclusiveSecondaryVertexV2BJetTags.clone(
        tagInfos = cms.VInputTag(
            cms.InputTag(ipTagInfosName),
            cms.InputTag(ivfTagInfosName)
        )
    )

    # soft lepton b-tags
    newSoftPFMuonBJetTags = softPFMuonBJetTags.clone(
        tagInfos = cms.VInputTag(cms.InputTag(smTagInfosName))
    )
    newSoftPFElectronBJetTags = softPFElectronBJetTags.clone(
        tagInfos = cms.VInputTag(cms.InputTag(seTagInfosName))
    )

    # super-combined b-tags
    newCombinedMVAV2BJetTags = pfCombinedMVAV2BJetTags.clone(
        tagInfos = cms.VInputTag(
            cms.InputTag(ipTagInfosName),
            cms.InputTag(svTagInfosName),
            cms.InputTag(ivfTagInfosName),
            cms.InputTag(smTagInfosName),
            cms.InputTag(seTagInfosName)
        )
    )

    # ctags
    newCombinedCvsLJetTags = pfCombinedCvsLJetTags.clone(
        tagInfos = cms.VInputTag(
            cms.InputTag(ipTagInfosName),
            cms.InputTag(ivfcvslTagInfosName),
            cms.InputTag(smTagInfosName),
            cms.InputTag(seTagInfosName)
        )
    )
    newCombinedCvsBJetTags = pfCombinedCvsBJetTags.clone(
        tagInfos = cms.VInputTag(
            cms.InputTag(ipTagInfosName),
            cms.InputTag(ivfcvslTagInfosName),
            cms.InputTag(smTagInfosName),
            cms.InputTag(seTagInfosName)
        )
    )

    # sequence
    sequence = cms.Sequence(
        (
            newImpactParameterTagInfos *
            (
                newTrackCountingHighEffBJetTags +
                newJetProbabilityBJetTags +
                newJetBProbabilityBJetTags +
                newSecondaryVertexTagInfos *
                (
                    newSimpleSecondaryVertexHighEffBJetTags +
                    newCombinedSecondaryVertexV2BJetTags
                ) +
                inclusiveCandidateVertexing *
                newInclusiveSecondaryVertexFinderTagInfos *
                newSimpleInclusiveSecondaryVertexHighEffBJetTags *
                newCombinedInclusiveSecondaryVertexV2BJetTags +
                newInclusiveSecondaryVertexFinderCvsLTagInfos
            ) +
            newSoftPFMuonsTagInfos *
            newSoftPFMuonBJetTags +
            newSoftPFElectronsTagInfos *
            newSoftPFElectronBJetTags
        ) *
        newCombinedMVAV2BJetTags +
        newCombinedCvsLJetTags +
        newCombinedCvsBJetTags
    )

    # configure process

    # ESProducers and vertexing sequences
    # Will load as many times as the function is called but this is not a problem
    process.load('RecoBTag.Configuration.RecoBTag_cff')
    process.load('RecoVertex.AdaptiveVertexFinder.inclusiveVertexing_cff')

    # add tag info calculators
    setattr(process, ipTagInfosName, newImpactParameterTagInfos)
    setattr(process, svTagInfosName, newSecondaryVertexTagInfos)
    setattr(process, ivfTagInfosName, newInclusiveSecondaryVertexFinderTagInfos)
    setattr(process, smTagInfosName, newSoftPFMuonsTagInfos)
    setattr(process, seTagInfosName, newSoftPFElectronsTagInfos)
    setattr(process, ivfcvslTagInfosName, newInclusiveSecondaryVertexFinderCvsLTagInfos)

    # add btag calculators
    setattr(process, 'trackCountingHighEffBJetTags' + suffix, newTrackCountingHighEffBJetTags)
    setattr(process, 'jetProbabilityBJetTags' + suffix, newJetProbabilityBJetTags)
    setattr(process, 'jetBProbabilityBJetTags' + suffix, newJetBProbabilityBJetTags)
    setattr(process, 'simpleSecondaryVertexHighEffBJetTags' + suffix, newSimpleSecondaryVertexHighEffBJetTags)
    setattr(process, 'combinedSecondaryVertexV2BJetTags' + suffix, newCombinedSecondaryVertexV2BJetTags)
    setattr(process, 'simpleInclusiveSecondaryVertexHighEffBJetTags' + suffix, newSimpleInclusiveSecondaryVertexHighEffBJetTags)
    setattr(process, 'combinedInclusiveSecondaryVertexV2BJetTags' + suffix, newCombinedInclusiveSecondaryVertexV2BJetTags)
    setattr(process, 'softPFMuonBJetTags' + suffix, newSoftPFMuonBJetTags)
    setattr(process, 'softPFElectronBJetTags' + suffix, newSoftPFElectronBJetTags)
    setattr(process, 'combinedMVAV2BJetTags' + suffix, newCombinedMVAV2BJetTags)
    setattr(process, 'combinedCvsLJetTags' + suffix, newCombinedCvsLJetTags)
    setattr(process, 'combinedCvsBJetTags' + suffix, newCombinedCvsBJetTags)

    # finally add sequence
    setattr(process, 'pfBTagging' + suffix, sequence)
    
    return sequence
