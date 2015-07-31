from RecoJets.JetProducers.ak4PFJets_cfi import ak4PFJets
from RecoJets.JetProducers.ak4GenJets_cfi import ak4GenJets
from PhysicsTools.PatAlgos.tools.coreTools import removeMCMatching
from PhysicsTools.PatAlgos.tools.pfTools import *
from PhysicsTools.PatAlgos.tools.jetTools import *
from PhysicsTools.PatAlgos.patTemplate_cfg import *
from RecoJets.JetProducers.SubJetParameters_cfi import SubJetParameters
from RecoJets.JetProducers.ak4PFJetsPruned_cfi import ak4PFJetsPruned
from RecoJets.JetProducers.ak4PFJetsSoftDrop_cfi import ak4PFJetsSoftDrop
from RecoJets.JetProducers.nJettinessAdder_cfi import Njettiness
from RecoJets.JetProducers.qjetsadder_cfi import QJetsAdder
from MitProd.TreeFiller.utils.fatjetHelper import *

def makeFatJets(process,isData):

    isMC = not isData

    ########################################
    ##         INITIAL SETUP              ##
    ########################################
    jetCorrectionsAK4 = ('AK4PFchs', ['L1FastJet', 'L2Relative', 'L3Absolute'], 'None')
    jetCorrectionsAK8 = ('AK8PFchs', ['L1FastJet', 'L2Relative', 'L3Absolute'], 'None')
    if isData:
        jetCorrectionsAK4[1].append('L2L3Residual')
        jetCorrectionsAK8[1].append('L2L3Residual')

    algoLabel = 'CA'
    jetAlgo = 'CambridgeAachen'
    jetRadii = [0.8,1.5]

    postfix = "PFlow"
    ## Various collection names
    genParticles = 'genParticles'
    jetSource = 'pfJetsPFBRECO'+postfix
    genJetCollection = 'ak4GenJetsNoNu'+postfix
    pfCandidates = 'particleFlow'
    pvSource = 'offlinePrimaryVertices'
    svSource = 'inclusiveCandidateSecondaryVertices'
    muSource = 'muons'
    elSource = 'gedGsfElectrons'

    # from PhysicsTools.PatAlgos.patEventContent_cff import patEventContent
    PFjetAlgo="AK4"
    process.load("PhysicsTools.PatAlgos.patSequences_cff")

    usePF2PAT(process,runPF2PAT=True, jetAlgo=PFjetAlgo, runOnMC=isMC, postfix=postfix,
              jetCorrections=jetCorrectionsAK4, pvCollection=cms.InputTag(pvSource), outputModules = [])

    ## Top projections in PF2PAT
    getattr(process,"pfPileUpJME"+postfix).checkClosestZVertex = False
    getattr(process,"pfNoPileUpJME"+postfix).enable = True
    getattr(process,"pfNoMuonJMEPFBRECO"+postfix).enable = True
    getattr(process,"pfNoElectronJMEPFBRECO"+postfix).enable = True
    ## Load standard PAT objects (here we only need PAT muons but the framework will figure out what it needs to run using the unscheduled mode)
    process.load("PhysicsTools.PatAlgos.producersLayer1.patCandidates_cff")
    process.load("PhysicsTools.PatAlgos.selectionLayer1.selectedPatCandidates_cff")

    bTagInfos = [
        'pfImpactParameterTagInfos'
       ,'pfSecondaryVertexTagInfos'
       ,'pfInclusiveSecondaryVertexFinderTagInfos'
       ,'softPFMuonsTagInfos'
       ,'softPFElectronsTagInfos'
    ]
    ## b-tag discriminators
    bTagDiscriminators = [
         'pfJetBProbabilityBJetTags'
        ,'pfJetProbabilityBJetTags'
        # ,'pfPositiveOnlyJetBProbabilityBJetTags'
        # ,'pfPositiveOnlyJetProbabilityBJetTags'
        # ,'pfNegativeOnlyJetBProbabilityBJetTags'
        # ,'pfNegativeOnlyJetProbabilityBJetTags'
        ,'pfTrackCountingHighPurBJetTags'
        ,'pfTrackCountingHighEffBJetTags'
        # ,'pfNegativeTrackCountingHighPurBJetTags'
        # ,'pfNegativeTrackCountingHighEffBJetTags'
        # ,'pfSimpleSecondaryVertexHighEffBJetTags'
        # ,'pfSimpleSecondaryVertexHighPurBJetTags'
        # ,'pfNegativeSimpleSecondaryVertexHighEffBJetTags'
        # ,'pfNegativeSimpleSecondaryVertexHighPurBJetTags'
        ,'pfCombinedSecondaryVertexV2BJetTags'
        # ,'pfPositiveCombinedSecondaryVertexV2BJetTags'
        # ,'pfNegativeCombinedSecondaryVertexV2BJetTags'
        ,'pfCombinedInclusiveSecondaryVertexV2BJetTags'
        # ,'pfPositiveCombinedInclusiveSecondaryVertexV2BJetTags'
        # ,'pfNegativeCombinedInclusiveSecondaryVertexV2BJetTags'
        # ,'softPFMuonBJetTags'
        # ,'positiveSoftPFMuonBJetTags'
        # ,'negativeSoftPFMuonBJetTags'
        # ,'softPFElectronBJetTags'
        # ,'positiveSoftPFElectronBJetTags'
        # ,'negativeSoftPFElectronBJetTags'
    ]


    bTagInfosSubjets = [
        'pfImpactParameterTagInfos'
       ,'pfSecondaryVertexTagInfos'
       ,'pfInclusiveSecondaryVertexFinderTagInfos'
    ]
    ## b-tag discriminators
    bTagDiscriminatorsSubjets = [
        'pfCombinedSecondaryVertexV2BJetTags'
        ,'pfCombinedInclusiveSecondaryVertexV2BJetTags'
    ]


    ## Switch the default jet collection (done in order to use the above specified b-tag infos and discriminators)
    switchJetCollection(
        process,
        jetSource = cms.InputTag(jetSource),
        pfCandidates = cms.InputTag(pfCandidates),
        pvSource = cms.InputTag(pvSource),
        svSource = cms.InputTag(svSource),
        muSource = cms.InputTag(muSource),
        elSource = cms.InputTag(elSource),
        btagInfos = bTagInfos,
        btagDiscriminators = bTagDiscriminators,
        jetCorrections = jetCorrectionsAK4,
        genJetCollection = cms.InputTag(genJetCollection),
        genParticles = cms.InputTag(genParticles),
        postfix = postfix
    )

    ########################################
    ##           REMAKE JETS              ##
    ########################################

    ########### CA8 ############
    process.genJetsNoNu8 = ak4GenJets.clone(
        jetAlgorithm = cms.string(jetAlgo),
        rParam = cms.double(.8),
        src = cms.InputTag("genParticlesForJetsNoNu"+postfix)
    )
    process.PFJetsCHS8 = ak4PFJets.clone(
        jetAlgorithm = cms.string(jetAlgo),
        rParam = cms.double(.8),
        src = getattr(process,"pfJetsPFBRECO"+postfix).src,
        srcPVs = getattr(process,"pfJetsPFBRECO"+postfix).srcPVs,
        doAreaFastjet = cms.bool(True),
        jetPtMin = cms.double(250)
    )
    process.genJetsNoNuSoftDrop8 = ak4GenJets.clone(
        jetAlgorithm = cms.string(jetAlgo),
        rParam = cms.double(.8),
        R0 = cms.double(.8),
        src = cms.InputTag("genParticlesForJetsNoNu"+postfix),
        useSoftDrop = cms.bool(True),
        zcut = cms.double(0.1),
        beta = cms.double(0.0),
        writeCompound = cms.bool(True),
        jetCollInstanceName=cms.string("SubJets")
    )
    process.PFJetsCHSSoftDrop8 = ak4PFJetsSoftDrop.clone(
        jetAlgorithm = cms.string(jetAlgo),
        rParam = cms.double(.8),
        R0 = cms.double(.8),
        src = getattr(process,"pfJetsPFBRECO"+postfix).src,
        srcPVs = getattr(process,"pfJetsPFBRECO"+postfix).srcPVs,
        doAreaFastjet = cms.bool(True),
        writeCompound = cms.bool(True),
        jetCollInstanceName=cms.string("SubJets"),
        jetPtMin = cms.double(250)
    )

    ########### CA15 ############
    process.genJetsNoNu15 = ak4GenJets.clone(
        jetAlgorithm = cms.string(jetAlgo),
        rParam = cms.double(1.5),
        src = cms.InputTag("genParticlesForJetsNoNu"+postfix)
    )
    process.PFJetsCHS15 = ak4PFJets.clone(
        jetAlgorithm = cms.string(jetAlgo),
        rParam = cms.double(1.5),
        src = getattr(process,"pfJetsPFBRECO"+postfix).src,
        srcPVs = getattr(process,"pfJetsPFBRECO"+postfix).srcPVs,
        doAreaFastjet = cms.bool(True),
        jetPtMin = cms.double(250)
    )
    process.genJetsNoNuSoftDrop15 = ak4GenJets.clone(
        jetAlgorithm = cms.string(jetAlgo),
        rParam = cms.double(1.5),
        R0 = cms.double(1.5),
        src = cms.InputTag("genParticlesForJetsNoNu"+postfix),
        useSoftDrop = cms.bool(True),
        zcut = cms.double(0.1),
        beta = cms.double(0.0),
        writeCompound = cms.bool(True),
        jetCollInstanceName=cms.string("SubJets")
    )
    process.PFJetsCHSSoftDrop15 = ak4PFJetsSoftDrop.clone(
        jetAlgorithm = cms.string(jetAlgo),
        rParam = cms.double(1.5),
        R0 = cms.double(1.5),
        src = getattr(process,"pfJetsPFBRECO"+postfix).src,
        srcPVs = getattr(process,"pfJetsPFBRECO"+postfix).srcPVs,
        doAreaFastjet = cms.bool(True),
        writeCompound = cms.bool(True),
        jetCollInstanceName=cms.string("SubJets"),
        jetPtMin = cms.double(250)
    )

    ########################################
    ##          MAKE PAT JETS             ##
    ########################################

    ############# CA8 ################
    addJetCollection(
        process,
        labelName='PFCHS8',
        jetSource=cms.InputTag('PFJetsCHS8'),
        algo=algoLabel,           # needed for jet flavor clustering
        rParam=.8, # needed for jet flavor clustering
        pfCandidates = cms.InputTag(pfCandidates),
        pvSource = cms.InputTag(pvSource),
        svSource = cms.InputTag(svSource),
        muSource = cms.InputTag(muSource),
        elSource = cms.InputTag(elSource),
        btagInfos = bTagInfos,
        btagDiscriminators = bTagDiscriminators,
        jetCorrections = jetCorrectionsAK8,
        genJetCollection = cms.InputTag('genJetsNoNu8'),
        genParticles = cms.InputTag(genParticles),
        postfix = postfix
    )
    getattr(process,'selectedPatJetsPFCHS8'+postfix).cut = cms.string("abs(eta) < " + str(2.5))
    ## SOFT DROP ##
    addJetCollection(
        process,
        labelName='SoftDropPFCHS8',
        jetSource=cms.InputTag('PFJetsCHSSoftDrop8'),
        algo=algoLabel,
        btagInfos = ['None'],
        btagDiscriminators = ['None'],
        jetCorrections=jetCorrectionsAK8,
        genJetCollection = cms.InputTag('genJetsNoNu8'),
        genParticles = cms.InputTag(genParticles),
        getJetMCFlavour = False, # jet flavor disabled
        postfix = postfix
    )
    addJetCollection(
        process,
        labelName='SoftDropSubjetsPFCHS8',
        jetSource=cms.InputTag('PFJetsCHSSoftDrop8','SubJets'),
        algo=algoLabel,           # needed for subjet flavor clustering
        rParam=.8, # needed for subjet flavor clustering
        pfCandidates = cms.InputTag(pfCandidates),
        pvSource = cms.InputTag(pvSource),
        svSource = cms.InputTag(svSource),
        muSource = cms.InputTag(muSource),
        elSource = cms.InputTag(elSource),
        btagInfos = bTagInfos,
        btagDiscriminators = bTagDiscriminators,
        jetCorrections = jetCorrectionsAK4,
        genJetCollection = cms.InputTag('genJetsNoNuSoftDrop8','SubJets'),
        genParticles = cms.InputTag(genParticles),
        explicitJTA = True,  # needed for subjet b tagging
        svClustering = True, # needed for subjet b tagging
        fatJets = cms.InputTag('PFJetsCHS8'),              # needed for subjet flavor clustering
        groomedFatJets = cms.InputTag('PFJetsCHSSoftDrop8'), # needed for subjet flavor clustering
        runIVF = False,
        postfix = postfix
    )
    process.selectedPatJetsSoftDropPFCHSPacked8 = cms.EDProducer("BoostedJetMerger",
        jetSrc=cms.InputTag("selectedPatJetsSoftDropPFCHS8"+postfix),
        subjetSrc=cms.InputTag("selectedPatJetsSoftDropSubjetsPFCHS8"+postfix)
    )

    ## PACK ##
    process.packedPatJetsPFCHS8 = cms.EDProducer("JetSubstructurePacker",
        jetSrc = cms.InputTag('selectedPatJetsPFCHS8'+postfix),
        distMax = cms.double(.8),
        algoTags = cms.VInputTag(),
        algoLabels = cms.vstring(),
        fixDaughters = cms.bool(False)
    )
    process.packedPatJetsPFCHS8.algoTags.append( cms.InputTag('selectedPatJetsSoftDropPFCHSPacked8') )
    process.packedPatJetsPFCHS8.algoLabels.append( 'SoftDrop' )
    ## CA15
    addJetCollection(
        process,
        labelName='PFCHS15',
        jetSource=cms.InputTag('PFJetsCHS15'),
        algo=algoLabel,           # needed for jet flavor clustering
        rParam=1.5, # needed for jet flavor clustering
        pfCandidates = cms.InputTag(pfCandidates),
        pvSource = cms.InputTag(pvSource),
        svSource = cms.InputTag(svSource),
        muSource = cms.InputTag(muSource),
        elSource = cms.InputTag(elSource),
        btagInfos = bTagInfos,
        btagDiscriminators = bTagDiscriminators,
        jetCorrections = jetCorrectionsAK8,
        genJetCollection = cms.InputTag('genJetsNoNu15'),
        genParticles = cms.InputTag(genParticles),
        postfix = postfix
    )
    getattr(process,'selectedPatJetsPFCHS15'+postfix).cut = cms.string("abs(eta) < " + str(2.5))
    ## SOFT DROP ##
    addJetCollection(
        process,
        labelName='SoftDropPFCHS15',
        jetSource=cms.InputTag('PFJetsCHSSoftDrop15'),
        algo=algoLabel,
        btagInfos = ['None'],
        btagDiscriminators = ['None'],
        jetCorrections=jetCorrectionsAK8,
        genJetCollection = cms.InputTag('genJetsNoNu15'),
        genParticles = cms.InputTag(genParticles),
        getJetMCFlavour = False, # jet flavor disabled
        postfix = postfix
    )
    addJetCollection(
        process,
        labelName='SoftDropSubjetsPFCHS15',
        jetSource=cms.InputTag('PFJetsCHSSoftDrop15','SubJets'),
        algo=algoLabel,           # needed for subjet flavor clustering
        rParam=1.5, # needed for subjet flavor clustering
        pfCandidates = cms.InputTag(pfCandidates),
        pvSource = cms.InputTag(pvSource),
        svSource = cms.InputTag(svSource),
        muSource = cms.InputTag(muSource),
        elSource = cms.InputTag(elSource),
        btagInfos = bTagInfos,
        btagDiscriminators = bTagDiscriminators,
        jetCorrections = jetCorrectionsAK4,
        genJetCollection = cms.InputTag('genJetsNoNuSoftDrop15','SubJets'),
        genParticles = cms.InputTag(genParticles),
        explicitJTA = True,  # needed for subjet b tagging
        svClustering = True, # needed for subjet b tagging
        fatJets = cms.InputTag('PFJetsCHS15'),              # needed for subjet flavor clustering
        groomedFatJets = cms.InputTag('PFJetsCHSSoftDrop15'), # needed for subjet flavor clustering
        runIVF = False,
        postfix = postfix
    )
    process.selectedPatJetsSoftDropPFCHSPacked15 = cms.EDProducer("BoostedJetMerger",
        jetSrc=cms.InputTag("selectedPatJetsSoftDropPFCHS15"+postfix),
        subjetSrc=cms.InputTag("selectedPatJetsSoftDropSubjetsPFCHS15"+postfix)
    )

    ## PACK ##
    process.packedPatJetsPFCHS15 = cms.EDProducer("JetSubstructurePacker",
        jetSrc = cms.InputTag('selectedPatJetsPFCHS15'+postfix),
        distMax = cms.double(1.5),
        algoTags = cms.VInputTag(),
        algoLabels = cms.vstring(),
        fixDaughters = cms.bool(False)
    )
    process.packedPatJetsPFCHS15.algoTags.append( cms.InputTag('selectedPatJetsSoftDropPFCHSPacked15') )
    process.packedPatJetsPFCHS15.algoLabels.append( 'SoftDrop' )
    ## Add full JetFlavourInfo and TagInfos to PAT jets
    for r0 in ['8','15']:
        for m in ['patJetsPFCHS'+r0+postfix, 'patJetsSoftDropSubjetsPFCHS'+r0+postfix]:
            if hasattr(process,m) and getattr( getattr(process,m), 'addBTagInfo' ):
                # print "Switching 'addTagInfos' for " + m + " to 'True'"
                setattr( getattr(process,m), 'addTagInfos', cms.bool(True) )
            if hasattr(process,m):
                # print "Switching 'addJetFlavourInfo' for " + m + " to 'True'"
                setattr( getattr(process,m), 'addJetFlavourInfo', cms.bool(True) )


    btag8Seq = makeBtagSequence(process,bTagDiscriminators,bTagInfos,"PFCHS8"+postfix)
    btag8SoftDropSeq = makeBtagSequence(process,bTagDiscriminators,bTagInfos,"SoftDropSubjetsPFCHS8"+postfix)
    btag15Seq = makeBtagSequence(process,bTagDiscriminators,bTagInfos,"PFCHS15"+postfix)
    btag15SoftDropSeq = makeBtagSequence(process,bTagDiscriminators,bTagInfos,"SoftDropSubjetsPFCHS15"+postfix)

    pfbrecoSequence = cms.Sequence(
        process.particleFlowPtrsPFlow*
        process.pfPileUpIsoPFBRECOPFlow*
        process.pfNoPileUpIsoPFBRECOPFlow*
        process.pfPileUpPFBRECOPFlow*
        process.pfNoPileUpPFBRECOPFlow*
        process.pfPileUpJMEPFlow*
        process.pfNoPileUpJMEPFlow*
        process.pfAllMuonsPFBRECOPFlow*
        process.pfMuonsFromVertexPFBRECOPFlow*
        process.pfIsolatedMuonsPFBRECOPFlow*
        process.pfNoMuonJMEPFBRECOPFlow*
        process.pfNoMuonPFBRECOPFlow*
        process.pfAllElectronsPFBRECOPFlow*
        process.pfElectronsFromVertexPFBRECOPFlow*
        process.pfIsolatedElectronsPFBRECOPFlow*
        process.pfNoElectronJMEPFBRECOPFlow
    )

    pfJetsCHS8Sequence = cms.Sequence(
        process.PFJetsCHS8*
        process.patJetCorrFactorsPFCHS8PFlow*
        # gen sequence
        btag8Seq*
        process.PFJetsCHSSoftDrop8*
        process.patJetCorrFactorsSoftDropPFCHS8PFlow*
        # gen sequence
        process.patJetsSoftDropPFCHS8PFlow*
        process.selectedPatJetsSoftDropPFCHS8PFlow*
        process.patJetCorrFactorsSoftDropSubjetsPFCHS8PFlow*
        # gen sequence
        btag8SoftDropSeq*
        process.patJetsSoftDropSubjetsPFCHS8PFlow*
        process.selectedPatJetsSoftDropSubjetsPFCHS8PFlow*
        process.selectedPatJetsSoftDropPFCHSPacked8*
        process.patJetsPFCHS8PFlow*
        process.selectedPatJetsPFCHS8PFlow*
        process.packedPatJetsPFCHS8
    )

    pfJetsCHS15Sequence = cms.Sequence(
        process.PFJetsCHS15*
        process.patJetCorrFactorsPFCHS15PFlow*
        # gen sequence
        btag15Seq*
        process.PFJetsCHSSoftDrop15*
        process.patJetCorrFactorsSoftDropPFCHS15PFlow*
        # gen sequence
        process.patJetsSoftDropPFCHS15PFlow*
        process.selectedPatJetsSoftDropPFCHS15PFlow*
        process.patJetCorrFactorsSoftDropSubjetsPFCHS15PFlow*
        # gen sequence
        btag15SoftDropSeq*
        process.patJetsSoftDropSubjetsPFCHS15PFlow*
        process.selectedPatJetsSoftDropSubjetsPFCHS15PFlow*
        process.selectedPatJetsSoftDropPFCHSPacked15*
        process.patJetsPFCHS15PFlow*
        process.selectedPatJetsPFCHS15PFlow*
        process.packedPatJetsPFCHS15
    )

    process.fatjetSequence = cms.Sequence(
        pfbrecoSequence *
        pfJetsCHS8Sequence *
        pfJetsCHS15Sequence
    )

    if isMC:
        genSequence = cms.Sequence(
            process.genParticlesForJetsNoNuPFlow*
            process.genJetsNoNu8*
            process.genJetsNoNu15*
            process.genJetsNoNuSoftDrop8*
            process.genJetsNoNuSoftDrop15
        )

        # do these sequences have to be inserted in small chunks, or can we just do two insertions?
        
        pf8CHSGenSequence = cms.Sequence(
            process.patJetFlavourAssociationPFCHS8PFlow*
            process.patJetPartonMatchPFCHS8PFlow*
            process.genJetsNoNu8*
            process.patJetGenJetMatchPFCHS8PFlow
        )

        pf8SoftDropMatchSequence = cms.Sequence(
            process.patJetPartonMatchSoftDropPFCHS8PFlow*
            process.patJetGenJetMatchSoftDropPFCHS8PFlow
        )

        pf8SoftDropMatchPFlowSequence = cms.Sequence(
            process.patJetFlavourAssociationSoftDropSubjetsPFCHS8PFlow*
            process.patJetPartonMatchSoftDropSubjetsPFCHS8PFlow*
            process.patJetGenJetMatchSoftDropSubjetsPFCHS8PFlow
        )

        pf15CHSGenSequence = cms.Sequence(
            process.patJetFlavourAssociationPFCHS15PFlow*
            process.patJetPartonMatchPFCHS15PFlow*
            process.genJetsNoNu15*
            process.patJetGenJetMatchPFCHS15PFlow
        )

        pf15SoftDropMatchSequence = cms.Sequence(
            process.patJetPartonMatchSoftDropPFCHS15PFlow*
            process.patJetGenJetMatchSoftDropPFCHS15PFlow
        )

        pf15SoftDropMatchPFlowSequence = cms.Sequence(
            process.patJetFlavourAssociationSoftDropSubjetsPFCHS15PFlow*
            process.patJetPartonMatchSoftDropSubjetsPFCHS15PFlow*
            process.patJetGenJetMatchSoftDropSubjetsPFCHS15PFlow
        )

        process.fatjetSequence.insert(process.fatjetSequence.index(pfJetsCHS8Sequence), genSequence)

        # should this be in genSequence?
        pfJetsCHS8Sequence.insert(pfJetsCHS8Sequence.index(btag8Seq), process.patJetPartonsPFlow)

        pfJetsCHS8Sequence.insert(pfJetsCHS8Sequence.index(btag8Seq), pf8CHSGenSequence)
        pfJetsCHS8Sequence.insert(pfJetsCHS8Sequence.index(process.patJetsSoftDropPFCHS8PFlow), pf8SoftDropMatchSequence)
        pfJetsCHS8Sequence.insert(pfJetsCHS8Sequence.index(btag8SoftDropSeq), pf8SoftDropMatchPFlowSequence)

        pfJetsCHS15Sequence.insert(pfJetsCHS15Sequence.index(btag15Seq), pf15CHSGenSequence)
        pfJetsCHS15Sequence.insert(pfJetsCHS15Sequence.index(process.patJetsSoftDropPFCHS15PFlow), pf15SoftDropMatchSequence)
        pfJetsCHS15Sequence.insert(pfJetsCHS15Sequence.index(btag15SoftDropSeq), pf15SoftDropMatchPFlowSequence)

    if isData:
        removeMCMatching(process, ['All'], outputModules = [])

    return process.fatjetSequence
