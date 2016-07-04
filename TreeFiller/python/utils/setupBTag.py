import FWCore.ParameterSet.Config as cms
import RecoBTag.Configuration.RecoBTag_cff as btag
import RecoBTag.CTagging.RecoCTagging_cff as ctag
import RecoVertex.AdaptiveVertexFinder.inclusiveVertexing_cff as vertexing

vertexingConfig = {} # {suffix: (candidates, primaryVertex)}

# Can call for each candidates & PV collection pair to construct secondary vertices from.
# Seems like CMS only uses particleFlow & offlinePrimaryVertices (or their MINIAOD equivalents) pair regardless of what types of jets they are b-tagging.
# So in the end we only call this function once.
def initBTag(process, suffix, candidates = 'particleFlow', primaryVertex = 'offlinePrimaryVertices'):
    if suffix in vertexingConfig:
        if (candidates, primaryVertex) != vertexingConfig[suffix]:
            raise RuntimeError('Duplicate vertexing configuration name')

        return

    if len(vertexingConfig) == 0:
        # ESProducers and vertexing sequences
        process.load('RecoBTag.ImpactParameter.impactParameter_EventSetup_cff')
        process.load('RecoBTag.CTagging.cTagging_EventSetup_cff')
        process.load('RecoBTag.SecondaryVertex.secondaryVertex_EventSetup_cff')
        process.load('RecoBTag.Combined.combinedMVA_EventSetup_cff')
        process.load('RecoBTag.SoftLepton.softLepton_EventSetup_cff')

    vertexingConfig[suffix] = (candidates, primaryVertex)

    inclusiveCandidateVertexFinder = vertexing.inclusiveCandidateVertexFinder.clone(
        primaryVertices = cms.InputTag(primaryVertex),
        tracks = cms.InputTag(candidates)
    )
    candidateVertexMerger = vertexing.candidateVertexMerger.clone(
        secondaryVertices = cms.InputTag('inclusiveCandidateVertexFinder' + suffix)
    )
    candidateVertexArbitrator = vertexing.candidateVertexArbitrator.clone(
        primaryVertices = cms.InputTag(primaryVertex),
        tracks = cms.InputTag(candidates),
        secondaryVertices = cms.InputTag('candidateVertexMerger' + suffix)
    )
    inclusiveCandidateSecondaryVertices = vertexing.inclusiveCandidateSecondaryVertices.clone(
        secondaryVertices = cms.InputTag('candidateVertexArbitrator' + suffix)
    )

    inclusiveCandidateVertexFinderCvsL = vertexing.inclusiveCandidateVertexFinderCvsL.clone(
        primaryVertices = cms.InputTag(primaryVertex),
        tracks = cms.InputTag(candidates)
    )
    candidateVertexMergerCvsL = vertexing.candidateVertexMergerCvsL.clone(
        secondaryVertices = cms.InputTag('inclusiveCandidateVertexFinderCvsL' + suffix)
    )
    candidateVertexArbitratorCvsL = vertexing.candidateVertexArbitratorCvsL.clone(
        primaryVertices = cms.InputTag(primaryVertex),
        tracks = cms.InputTag(candidates),
        secondaryVertices = cms.InputTag('candidateVertexMergerCvsL' + suffix)
    )
    inclusiveCandidateSecondaryVerticesCvsL = vertexing.inclusiveCandidateSecondaryVerticesCvsL.clone(
        secondaryVertices = cms.InputTag('candidateVertexArbitratorCvsL' + suffix)
    )

    sequence = cms.Sequence(
        inclusiveCandidateVertexFinder *
        candidateVertexMerger *
        candidateVertexArbitrator *
        inclusiveCandidateSecondaryVertices *
        inclusiveCandidateVertexFinderCvsL *
        candidateVertexMergerCvsL *
        candidateVertexArbitratorCvsL *
        inclusiveCandidateSecondaryVerticesCvsL
    )

    names = [
        'inclusiveCandidateVertexFinder',
        'candidateVertexMerger',
        'candidateVertexArbitrator',
        'inclusiveCandidateSecondaryVertices',
        'inclusiveCandidateVertexFinderCvsL',
        'candidateVertexMergerCvsL',
        'candidateVertexArbitratorCvsL',
        'inclusiveCandidateSecondaryVerticesCvsL'
    ]
    for name in names:
        setattr(process, name + suffix, eval(name))

    setattr(process, 'inclusiveVertexingSequence' + suffix, sequence)

    return sequence
    

# Give the list of btag discriminators (see below for names) in tags to run only a part of the full menu.
# vsuffix is the suffix given to initBTag that defines the secondary vertexing sequence.
# The optional argument addedTagInfos can be used to retrieve back the TagInfo modules added in order to compute the specified btag discriminators.
def setupBTag(process, jetCollection, suffix, vsuffix, muons = 'muons', electrons = 'gedGsfElectrons', tags = [], addedTagInfos = []):
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
        candidates = cms.InputTag(vertexingConfig[vsuffix][0]),
        primaryVertex = cms.InputTag(vertexingConfig[vsuffix][1])
    )
    secondaryVertexTagInfos = btag.pfSecondaryVertexTagInfos.clone(
        trackIPTagInfos = cms.InputTag(ipTagInfosName)
    )
    inclusiveSecondaryVertexFinderTagInfos = btag.pfInclusiveSecondaryVertexFinderTagInfos.clone(
        extSVCollection = cms.InputTag('inclusiveCandidateSecondaryVertices' + vsuffix),
        trackIPTagInfos = cms.InputTag(ipTagInfosName)
    )
    softPFMuonsTagInfos = btag.softPFMuonsTagInfos.clone(
        jets = jetCollection,
        muons = muons,
        primaryVertex = cms.InputTag(vertexingConfig[vsuffix][1])
    )
    softPFElectronsTagInfos = btag.softPFElectronsTagInfos.clone(
        jets = jetCollection,
        electrons = electrons,
        primaryVertex = vertexingConfig[vsuffix][1]
    )
    inclusiveSecondaryVertexFinderCvsLTagInfos = btag.pfInclusiveSecondaryVertexFinderCvsLTagInfos.clone(
        extSVCollection = cms.InputTag('inclusiveCandidateSecondaryVerticesCvsL' + vsuffix),
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

    # create sequence. Use a recursive function to resolve the dependencies
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
        'pfCombinedCvsBJetTags': (combinedCvsBJetTags, [ipTagInfosName, ivfcvslTagInfosName, smTagInfosName, seTagInfosName])
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
