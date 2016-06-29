import FWCore.ParameterSet.Config as cms
import RecoBTag.Configuration.RecoBTag_cff as btag
import RecoBTag.CTagging.RecoCTagging_cff as ctag

_candidates = 'particleFlow'
_primaryVertex = 'offlinePrimaryVertices'


def initBTag(process, candidates = 'particleFlow', primaryVertex = 'offlinePrimaryVertices'):
    if hasattr(process, 'inclusiveCandidateVertexing'):
        return

    _candidates = candidates
    _primaryVertex = primaryVertex

    # ESProducers and vertexing sequences
    process.load('RecoBTag.ImpactParameter.impactParameter_EventSetup_cff')
    process.load('RecoBTag.CTagging.cTagging_EventSetup_cff')
    process.load('RecoBTag.SecondaryVertex.secondaryVertex_EventSetup_cff')
    process.load('RecoBTag.Combined.combinedMVA_EventSetup_cff')
    process.load('RecoBTag.SoftLepton.softLepton_EventSetup_cff')
    process.load('RecoVertex.AdaptiveVertexFinder.inclusiveVertexing_cff')

    # these modules are not bound to jets - no need to clone.
    btag.inclusiveCandidateVertexFinder.primaryVertices = primaryVertex
    btag.inclusiveCandidateVertexFinder.tracks = candidates
    btag.candidateVertexArbitrator.primaryVertices = primaryVertex
    btag.candidateVertexArbitrator.tracks = candidates

    process.inclusiveCandidateVertexing = btag.inclusiveCandidateVertexing


def setupBTag(process, jetCollection, suffix, muons = 'muons', electrons = 'gedGsfElectrons', tags = [], addedTagInfos = []):
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
    impactParameterTagInfos = btag.pfImpactParameterTagInfos.clone(
        jets = cms.InputTag(jetCollection),
        candidates = cms.InputTag(_candidates),
        primaryVertex = cms.InputTag(_primaryVertex)
    )
    secondaryVertexTagInfos = btag.pfSecondaryVertexTagInfos.clone(
        trackIPTagInfos = cms.InputTag(ipTagInfosName)
    )
    inclusiveSecondaryVertexFinderTagInfos = btag.pfInclusiveSecondaryVertexFinderTagInfos.clone(
        trackIPTagInfos = ipTagInfosName
    )
    softPFMuonsTagInfos = btag.softPFMuonsTagInfos.clone(
        jets = jetCollection,
        muons = muons,
        primaryVertex = cms.InputTag(_primaryVertex)
    )
    softPFElectronsTagInfos = btag.softPFElectronsTagInfos.clone(
        jets = jetCollection,
        electrons = electrons,
        primaryVertex = _primaryVertex
    )
    inclusiveSecondaryVertexFinderCvsLTagInfos = btag.pfInclusiveSecondaryVertexFinderCvsLTagInfos.clone(
        trackIPTagInfos = ipTagInfosName
    )

    # impact parameter b-tags
    trackCountingHighEffBJetTags = btag.pfTrackCountingHighEffBJetTags.clone(
        tagInfos = cms.VInputTag(cms.InputTag(ipTagInfosName))
    )

    # jet probability b-tags
    jetProbabilityBJetTags = btag.pfJetProbabilityBJetTags.clone(
        tagInfos = cms.VInputTag(cms.InputTag(ipTagInfosName))
    )
    jetBProbabilityBJetTags = btag.pfJetBProbabilityBJetTags.clone(
        tagInfos = cms.VInputTag(cms.InputTag(ipTagInfosName))
    )

    # secondary vertex b-tags
    simpleSecondaryVertexHighEffBJetTags = btag.pfSimpleSecondaryVertexHighEffBJetTags.clone(
        tagInfos = cms.VInputTag(cms.InputTag(svTagInfosName))
    )

    # inclusive secondary vertex b-tags
    simpleInclusiveSecondaryVertexHighEffBJetTags = btag.pfSimpleInclusiveSecondaryVertexHighEffBJetTags.clone(
        tagInfos = cms.VInputTag(cms.InputTag(ivfTagInfosName))
    )

    # combined secondary vertex b-tags
    combinedSecondaryVertexV2BJetTags = btag.pfCombinedSecondaryVertexV2BJetTags.clone(
        tagInfos = cms.VInputTag(
            cms.InputTag(ipTagInfosName),
            cms.InputTag(svTagInfosName)
        )
    )

    # inclusive combined secondary vertex b-tags
    combinedInclusiveSecondaryVertexV2BJetTags = btag.pfCombinedInclusiveSecondaryVertexV2BJetTags.clone(
        tagInfos = cms.VInputTag(
            cms.InputTag(ipTagInfosName),
            cms.InputTag(ivfTagInfosName)
        )
    )

    # soft lepton b-tags
    softPFMuonBJetTags = btag.softPFMuonBJetTags.clone(
        tagInfos = cms.VInputTag(cms.InputTag(smTagInfosName))
    )
    softPFElectronBJetTags = btag.softPFElectronBJetTags.clone(
        tagInfos = cms.VInputTag(cms.InputTag(seTagInfosName))
    )

    # super-combined b-tags
    combinedMVAV2BJetTags = btag.pfCombinedMVAV2BJetTags.clone(
        tagInfos = cms.VInputTag(
            cms.InputTag(ipTagInfosName),
            cms.InputTag(svTagInfosName),
            cms.InputTag(ivfTagInfosName),
            cms.InputTag(smTagInfosName),
            cms.InputTag(seTagInfosName)
        )
    )

    # ctags
    combinedCvsLJetTags = ctag.pfCombinedCvsLJetTags.clone(
        tagInfos = cms.VInputTag(
            cms.InputTag(ipTagInfosName),
            cms.InputTag(ivfcvslTagInfosName),
            cms.InputTag(smTagInfosName),
            cms.InputTag(seTagInfosName)
        )
    )
    combinedCvsBJetTags = ctag.pfCombinedCvsBJetTags.clone(
        tagInfos = cms.VInputTag(
            cms.InputTag(ipTagInfosName),
            cms.InputTag(ivfcvslTagInfosName),
            cms.InputTag(smTagInfosName),
            cms.InputTag(seTagInfosName)
        )
    )

    # sequence
    dependencies = {
        'pfTrackCountingHighEffBJetTags': (trackCountingHighEffBJetTags, [ipTagInfosName]),
        'pfJetProbabilityBJetTags': (jetProbabilityBJetTags, [ipTagInfosName]),
        'pfJetBProbabilityBJetTags': (jetBProbabilityBJetTags, [ipTagInfosName]),
        'pfSimpleSecondaryVertexHighEffBJetTags': (simpleSecondaryVertexHighEffBJetTags, [ipTagInfosName, svTagInfosName]),
        'pfCombinedSecondaryVertexV2BJetTags': (combinedSecondaryVertexV2BJetTags, [ipTagInfosName, svTagInfosName]),
        'pfSimpleInclusiveSecondaryVertexHighEffBJetTags': (simpleInclusiveSecondaryVertexHighEffBJetTags, [ipTagInfosName, ivfTagInfosName]),
        'pfCombinedInclusiveSecondaryVertexV2BJetTags': (combinedInclusiveSecondaryVertexV2BJetTags, [ipTagInfosName, ivfTagInfosName]),
        'softPFMuonBJetTags': (softPFMuonBJetTags, [smTagInfosName]),
        'softPFElectronBJetTags': (softPFElectronBJetTags, [seTagInfosName]),
        'pfCombinedMVAV2BJetTags': (combinedMVAV2BJetTags, [ipTagInfosName, svTagInfosName, ivfTagInfosName, smTagInfosName, seTagInfosName]),
        'pfCombinedCvsLJetTags': (combinedCvsLJetTags, [ipTagInfosName, ivfcvslTagInfosName, smTagInfosName, seTagInfosName]),
        'pvCombinedCvsBJetTags': (combinedCvsBJetTags, [ipTagInfosName, ivfcvslTagInfosName, smTagInfosName, seTagInfosName])
    }

    tagInfos = {
        ipTagInfosName: (impactParameterTagInfos, []),
        svTagInfosName: (secondaryVertexTagInfos, [ipTagInfosName]),
        ivfTagInfosName: (inclusiveSecondaryVertexFinderTagInfos, [ipTagInfosName]),
        ivfcvslTagInfosName: (inclusiveSecondaryVertexFinderCvsLTagInfos, [ipTagInfosName]),
        smTagInfosName: (softPFMuonsTagInfos, []),
        seTagInfosName: (softPFElectronsTagInfos, []),
    }

    def addTagInfo(name, sequence):
        tagInfo, dependency = tagInfos[name]

        for depName in dependency:
            addTagInfo(depName, sequence)

        try:
            sequence.index(tagInfo)
        except:            
            sequence += tagInfo
            setattr(process, name, tagInfo)
            addedTagInfos.append(name)

    sequence = cms.Sequence()

    if len(tags) == 0:
        tags = dependencies.keys()

    for tagName in tags:
        tag, dependency = dependencies[tagName]

        for tagInfoName in dependency:
            addTagInfo(tagInfoName, sequence)

        sequence += tag
        setattr(process, tagName + suffix, tag)

    return sequence
