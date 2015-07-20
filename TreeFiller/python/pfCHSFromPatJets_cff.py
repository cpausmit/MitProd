from RecoJets.JetProducers.ak4PFJets_cfi import ak4PFJets
from RecoJets.JetProducers.ak4GenJets_cfi import ak4GenJets
from PhysicsTools.PatAlgos.tools.pfTools import *
from PhysicsTools.PatAlgos.tools.jetTools import *
from RecoJets.JetProducers.SubJetParameters_cfi import SubJetParameters
from RecoJets.JetProducers.ak4PFJetsPruned_cfi import ak4PFJetsPruned
from RecoJets.JetProducers.ak4PFJetsSoftDrop_cfi import ak4PFJetsSoftDrop
from RecoJets.JetProducers.nJettinessAdder_cfi import Njettiness
from RecoJets.JetProducers.qjetsadder_cfi import QJetsAdder

def makeFatJets(process,isMC):

    ########################################
    ##         INITIAL SETUP              ##
    ########################################
    jetCorrectionsAK4 = ('AK4PFchs', ['L1FastJet', 'L2Relative', 'L3Absolute'], 'None')
    jetCorrectionsAK8 = ('AK8PFchs', ['L1FastJet', 'L2Relative', 'L3Absolute'], 'None')

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

    usePF2PAT(process,runPF2PAT=True, jetAlgo=PFjetAlgo, runOnMC=isMC, postfix=postfix,
          jetCorrections=jetCorrectionsAK4, pvCollection=cms.InputTag(pvSource))

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
        ,'pfPositiveOnlyJetBProbabilityBJetTags'
        ,'pfPositiveOnlyJetProbabilityBJetTags'
        ,'pfNegativeOnlyJetBProbabilityBJetTags'
        ,'pfNegativeOnlyJetProbabilityBJetTags'
        ,'pfTrackCountingHighPurBJetTags'
        ,'pfTrackCountingHighEffBJetTags'
        ,'pfNegativeTrackCountingHighPurBJetTags'
        ,'pfNegativeTrackCountingHighEffBJetTags'
        ,'pfSimpleSecondaryVertexHighEffBJetTags'
        ,'pfSimpleSecondaryVertexHighPurBJetTags'
        ,'pfNegativeSimpleSecondaryVertexHighEffBJetTags'
        ,'pfNegativeSimpleSecondaryVertexHighPurBJetTags'
        ,'pfCombinedSecondaryVertexV2BJetTags'
        ,'pfPositiveCombinedSecondaryVertexV2BJetTags'
        ,'pfNegativeCombinedSecondaryVertexV2BJetTags'
        ,'pfCombinedInclusiveSecondaryVertexV2BJetTags'
        ,'pfPositiveCombinedInclusiveSecondaryVertexV2BJetTags'
        ,'pfNegativeCombinedInclusiveSecondaryVertexV2BJetTags'
        ,'softPFMuonBJetTags'
        ,'positiveSoftPFMuonBJetTags'
        ,'negativeSoftPFMuonBJetTags'
        ,'softPFElectronBJetTags'
        ,'positiveSoftPFElectronBJetTags'
        ,'negativeSoftPFElectronBJetTags'
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
    process.genJetsNoNuPruned8 = ak4GenJets.clone(
        SubJetParameters,
        jetAlgorithm = cms.string(jetAlgo),
        rParam = cms.double(.8),
        src = cms.InputTag("genParticlesForJetsNoNu"+postfix),
        usePruning = cms.bool(True),
        writeCompound = cms.bool(True),
        jetCollInstanceName=cms.string("SubJets")
    )
    process.PFJetsCHSPruned8 = ak4PFJetsPruned.clone(
        jetAlgorithm = cms.string(jetAlgo),
        rParam = cms.double(.8),
        src = getattr(process,"pfJetsPFBRECO"+postfix).src,
        srcPVs = getattr(process,"pfJetsPFBRECO"+postfix).srcPVs,
        doAreaFastjet = cms.bool(True),
        writeCompound = cms.bool(True),
        jetCollInstanceName=cms.string("SubJets"),
        jetPtMin = cms.double(250)
    )
    process.genJetsNoNuTrimmed8 = process.genJetsNoNu8.clone(
        useTrimming         = cms.bool(True),
        rFilt               = cms.double(0.2),
        trimPtFracMin       = cms.double(0.06),
        useExplicitGhosts   = cms.bool(True),
        writeCompound       = cms.bool(True),
        jetCollInstanceName = cms.string("SubJets")
    )
    process.PFJetsCHSTrimmed8 = process.PFJetsCHS8.clone(
        useTrimming         = cms.bool(True),
        rFilt               = cms.double(0.2),
        trimPtFracMin       = cms.double(0.06),
        useExplicitGhosts   = cms.bool(True),
        writeCompound       = cms.bool(True),
        jetCollInstanceName = cms.string("SubJets")
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
    process.genJetsNoNuPruned15 = ak4GenJets.clone(
      SubJetParameters,
      jetAlgorithm = cms.string(jetAlgo),
      rParam = cms.double(1.5),
      src = cms.InputTag("genParticlesForJetsNoNu"+postfix),
      usePruning = cms.bool(True),
      writeCompound = cms.bool(True),
      jetCollInstanceName=cms.string("SubJets")
    )
    process.PFJetsCHSPruned15 = ak4PFJetsPruned.clone(
      jetAlgorithm = cms.string(jetAlgo),
      rParam = cms.double(1.5),
      src = getattr(process,"pfJetsPFBRECO"+postfix).src,
      srcPVs = getattr(process,"pfJetsPFBRECO"+postfix).srcPVs,
      doAreaFastjet = cms.bool(True),
      writeCompound = cms.bool(True),
      jetCollInstanceName=cms.string("SubJets"),
      jetPtMin = cms.double(250)
    )
    process.genJetsNoNuTrimmed15 = process.genJetsNoNu15.clone(
      useTrimming         = cms.bool(True),
      rFilt               = cms.double(0.2),
      trimPtFracMin       = cms.double(0.06),
      useExplicitGhosts   = cms.bool(True),
      writeCompound       = cms.bool(True),
      jetCollInstanceName = cms.string("SubJets")
    )
    process.PFJetsCHSTrimmed15 = process.PFJetsCHS15.clone(
      useTrimming         = cms.bool(True),
      rFilt               = cms.double(0.2),
      trimPtFracMin       = cms.double(0.06),
      useExplicitGhosts   = cms.bool(True),
      writeCompound       = cms.bool(True),
      jetCollInstanceName = cms.string("SubJets")
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
    ## TRIMMED ##
    addJetCollection(
        process,
        labelName='TrimmedPFCHS8',
        jetSource=cms.InputTag('PFJetsCHSTrimmed8'),
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
        labelName='TrimmedSubjetsPFCHS8',
        jetSource=cms.InputTag('PFJetsCHSTrimmed8','SubJets'),
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
        genJetCollection = cms.InputTag('genJetsNoNuTrimmed8','SubJets'),
        genParticles = cms.InputTag(genParticles),
        explicitJTA = True,  # needed for subjet b tagging
        svClustering = True, # needed for subjet b tagging
        fatJets = cms.InputTag('PFJetsCHS8'),              # needed for subjet flavor clustering
        groomedFatJets = cms.InputTag('PFJetsCHSTrimmed8'), # needed for subjet flavor clustering
        runIVF = False,
        postfix = postfix
    )
    process.selectedPatJetsTrimmedPFCHSPacked8 = cms.EDProducer("BoostedJetMerger",
        jetSrc=cms.InputTag("selectedPatJetsTrimmedPFCHS8"+postfix),
        subjetSrc=cms.InputTag("selectedPatJetsTrimmedSubjetsPFCHS8"+postfix)
    )
    ## PRUNED ##
    addJetCollection(
        process,
        labelName='PrunedPFCHS8',
        jetSource=cms.InputTag('PFJetsCHSPruned8'),
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
        labelName='PrunedSubjetsPFCHS8',
        jetSource=cms.InputTag('PFJetsCHSPruned8','SubJets'),
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
        genJetCollection = cms.InputTag('genJetsNoNuPruned8','SubJets'),
        genParticles = cms.InputTag(genParticles),
        explicitJTA = True,  # needed for subjet b tagging
        svClustering = True, # needed for subjet b tagging
        fatJets = cms.InputTag('PFJetsCHS8'),              # needed for subjet flavor clustering
        groomedFatJets = cms.InputTag('PFJetsCHSPruned8'), # needed for subjet flavor clustering
        runIVF = False,
        postfix = postfix
    )
    process.selectedPatJetsPrunedPFCHSPacked8 = cms.EDProducer("BoostedJetMerger",
        jetSrc=cms.InputTag("selectedPatJetsPrunedPFCHS8"+postfix),
        subjetSrc=cms.InputTag("selectedPatJetsPrunedSubjetsPFCHS8"+postfix)
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
    process.packedPatJetsPFCHS8.algoTags.append( cms.InputTag('selectedPatJetsPrunedPFCHSPacked8') )
    process.packedPatJetsPFCHS8.algoLabels.append( 'Pruned' )
    process.packedPatJetsPFCHS8.algoTags.append( cms.InputTag('selectedPatJetsTrimmedPFCHSPacked8') )
    process.packedPatJetsPFCHS8.algoLabels.append( 'Trimmed' )
    ## USER FLOATS ##
    process.SoftDrop8 = cms.EDProducer("RecoJetDeltaRValueMapProducer",
        src = cms.InputTag("PFJetsCHS8"),
        matched = cms.InputTag("selectedPatJetsSoftDropPFCHSPacked8"),
        distMax = cms.double(.8),
        values = cms.vstring('mass','pt','eta','phi'),
        valueLabels = cms.vstring('Mass','Pt','Eta','Phi'),
        lazyParser = cms.bool(True)
    )
    process.Trimmed8 = cms.EDProducer("RecoJetDeltaRValueMapProducer",
        src = cms.InputTag("PFJetsCHS8"),
        matched = cms.InputTag("selectedPatJetsTrimmedPFCHSPacked8"),
        distMax = cms.double(.8),
        values = cms.vstring('mass','pt','eta','phi'),
        valueLabels = cms.vstring('Mass','Pt','Eta','Phi'),
        lazyParser = cms.bool(True)
    )
    process.Pruned8 = cms.EDProducer("RecoJetDeltaRValueMapProducer",
        src = cms.InputTag("PFJetsCHS8"),
        matched = cms.InputTag("selectedPatJetsPrunedPFCHSPacked8"),
        distMax = cms.double(.8),
        values = cms.vstring('mass','pt','eta','phi'),
        valueLabels = cms.vstring('Mass','Pt','Eta','Phi'),
        lazyParser = cms.bool(True)
    )
    getattr(process,'patJetsPFCHS8'+postfix).userData.userFloats.src += ['SoftDrop8:Mass','SoftDrop8:Pt','SoftDrop8:Eta','SoftDrop8:Phi','SoftDrop8:jecFactor0',
                                                                        'Pruned8:Mass'  ,'Pruned8:Pt'  ,'Pruned8:Eta'  ,'Pruned8:Phi',
                                                                        'Trimmed8:Mass', 'Trimmed8:Pt', 'Trimmed8:Eta', 'Trimmed8:Phi']
    process.Njettiness8 = Njettiness.clone(
        src = cms.InputTag("PFJetsCHS8"),
        R0  = cms.double(.8)
    )
    getattr(process,'patJetsPFCHS8'+postfix).userData.userFloats.src += ['Njettiness8:tau1','Njettiness8:tau2','Njettiness8:tau3','Njettiness8:tau4']
    process.RandomNumberGeneratorService = cms.Service("RandomNumberGeneratorService")
    process.Qjets8 = QJetsAdder.clone(
        src = cms.InputTag("PFJetsCHS8"),
        jetRad = cms.double(.8),
        jetAlgo = cms.string(algoLabel),
    )
    getattr(process,'patJetsPFCHS8'+postfix).userData.userFloats.src += ['Qjets8:QjetsVolatility']
    setattr(process.RandomNumberGeneratorService, "Qjets", cms.PSet(initialSeed = cms.untracked.uint32(42),
                                                                    engineName = cms.untracked.string('TRandom3')))

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
    ## TRIMMED ##
    addJetCollection(
        process,
        labelName='TrimmedPFCHS15',
        jetSource=cms.InputTag('PFJetsCHSTrimmed15'),
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
        labelName='TrimmedSubjetsPFCHS15',
        jetSource=cms.InputTag('PFJetsCHSTrimmed15','SubJets'),
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
        genJetCollection = cms.InputTag('genJetsNoNuTrimmed15','SubJets'),
        genParticles = cms.InputTag(genParticles),
        explicitJTA = True,  # needed for subjet b tagging
        svClustering = True, # needed for subjet b tagging
        fatJets = cms.InputTag('PFJetsCHS15'),              # needed for subjet flavor clustering
        groomedFatJets = cms.InputTag('PFJetsCHSTrimmed15'), # needed for subjet flavor clustering
        runIVF = False,
        postfix = postfix
    )
    process.selectedPatJetsTrimmedPFCHSPacked15 = cms.EDProducer("BoostedJetMerger",
        jetSrc=cms.InputTag("selectedPatJetsTrimmedPFCHS15"+postfix),
        subjetSrc=cms.InputTag("selectedPatJetsTrimmedSubjetsPFCHS15"+postfix)
    )
    ## PRUNED ##
    addJetCollection(
        process,
        labelName='PrunedPFCHS15',
        jetSource=cms.InputTag('PFJetsCHSPruned15'),
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
        labelName='PrunedSubjetsPFCHS15',
        jetSource=cms.InputTag('PFJetsCHSPruned15','SubJets'),
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
        genJetCollection = cms.InputTag('genJetsNoNuPruned15','SubJets'),
        genParticles = cms.InputTag(genParticles),
        explicitJTA = True,  # needed for subjet b tagging
        svClustering = True, # needed for subjet b tagging
        fatJets = cms.InputTag('PFJetsCHS15'),              # needed for subjet flavor clustering
        groomedFatJets = cms.InputTag('PFJetsCHSPruned15'), # needed for subjet flavor clustering
        runIVF = False,
        postfix = postfix
    )
    process.selectedPatJetsPrunedPFCHSPacked15 = cms.EDProducer("BoostedJetMerger",
        jetSrc=cms.InputTag("selectedPatJetsPrunedPFCHS15"+postfix),
        subjetSrc=cms.InputTag("selectedPatJetsPrunedSubjetsPFCHS15"+postfix)
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
    process.packedPatJetsPFCHS15.algoTags.append( cms.InputTag('selectedPatJetsPrunedPFCHSPacked15') )
    process.packedPatJetsPFCHS15.algoLabels.append( 'Pruned' )
    process.packedPatJetsPFCHS15.algoTags.append( cms.InputTag('selectedPatJetsTrimmedPFCHSPacked15') )
    process.packedPatJetsPFCHS15.algoLabels.append( 'Trimmed' )
    ## USER FLOATS ##
    process.SoftDrop15 = cms.EDProducer("RecoJetDeltaRValueMapProducer",
        src = cms.InputTag("PFJetsCHS15"),
        matched = cms.InputTag("selectedPatJetsSoftDropPFCHSPacked15"),
        distMax = cms.double(1.5),
        values = cms.vstring('mass','pt','eta','phi'),
        valueLabels = cms.vstring('Mass','Pt','Eta','Phi'),
        lazyParser = cms.bool(True)
    )
    process.Trimmed15 = cms.EDProducer("RecoJetDeltaRValueMapProducer",
        src = cms.InputTag("PFJetsCHS15"),
        matched = cms.InputTag("selectedPatJetsTrimmedPFCHSPacked15"),
        distMax = cms.double(1.5),
        values = cms.vstring('mass','pt','eta','phi'),
        valueLabels = cms.vstring('Mass','Pt','Eta','Phi'),
        lazyParser = cms.bool(True)
    )
    process.Pruned15 = cms.EDProducer("RecoJetDeltaRValueMapProducer",
        src = cms.InputTag("PFJetsCHS15"),
        matched = cms.InputTag("selectedPatJetsPrunedPFCHSPacked15"),
        distMax = cms.double(1.5),
        values = cms.vstring('mass','pt','eta','phi'),
        valueLabels = cms.vstring('Mass','Pt','Eta','Phi'),
        lazyParser = cms.bool(True)
    )
    getattr(process,'patJetsPFCHS15'+postfix).userData.userFloats.src += ['SoftDrop15:Mass','SoftDrop15:Pt','SoftDrop15:Eta','SoftDrop15:Phi','SoftDrop15:jecFactor0',
                                                                        'Pruned15:Mass'  ,'Pruned15:Pt'  ,'Pruned15:Eta'  ,'Pruned15:Phi',
                                                                        'Trimmed15:Mass', 'Trimmed15:Pt', 'Trimmed15:Eta', 'Trimmed15:Phi']
    process.Njettiness15 = Njettiness.clone(
        src = cms.InputTag("PFJetsCHS15"),
        R0  = cms.double(1.5)
    )
    getattr(process,'patJetsPFCHS15'+postfix).userData.userFloats.src += ['Njettiness15:tau1','Njettiness15:tau2','Njettiness15:tau3','Njettiness15:tau4']
    process.RandomNumberGeneratorService = cms.Service("RandomNumberGeneratorService")
    process.Qjets15 = QJetsAdder.clone(
        src = cms.InputTag("PFJetsCHS15"),
        jetRad = cms.double(1.5),
        jetAlgo = cms.string(algoLabel),
    )
    getattr(process,'patJetsPFCHS15'+postfix).userData.userFloats.src += ['Qjets15:QjetsVolatility']
    setattr(process.RandomNumberGeneratorService, "Qjets", cms.PSet(initialSeed = cms.untracked.uint32(42),
                                                                    engineName = cms.untracked.string('TRandom3')))

    ## Add full JetFlavourInfo and TagInfos to PAT jets
    for r0 in ['8','15']:
        for m in ['patJetsPFCHS'+r0+postfix, 'patJetsSoftDropSubjetsPFCHS'+r0+postfix, 'patJetsPrunedSubjetsPFCHS'+r0+postfix]:
            if hasattr(process,m) and getattr( getattr(process,m), 'addBTagInfo' ):
                print "Switching 'addTagInfos' for " + m + " to 'True'"
                setattr( getattr(process,m), 'addTagInfos', cms.bool(True) )
            if hasattr(process,m):
                print "Switching 'addJetFlavourInfo' for " + m + " to 'True'"
                setattr( getattr(process,m), 'addJetFlavourInfo', cms.bool(True) )


    process.fatjetSequence = cms.Sequence(
                        # process.patJetCorrFactorsPFCHS8PFlow*
                        # process.patJetsPFCHS8PFlow*
                        # process.selectedPatJetsPFCHS8PFlow*
                        #
                        # process.selectedPatJetsSoftDropPFCHS8PFlow*
                        # process.selectedPatJetsSoftDropSubjetsPFCHS8PFlow*
                        # process.selectedPatJetsSoftDropPFCHSPacked8*
                        #
                        # process.selectedPatJetsPrunedPFCHS8PFlow*
                        # process.selectedPatJetsPrunedSubjetsPFCHS8PFlow*
                        # process.selectedPatJetsPrunedPFCHSPacked8*
                        #
                        # process.selectedPatJetsTrimmedPFCHS8PFlow*
                        # process.selectedPatJetsTrimmedSubjetsPFCHS8PFlow*
                        # process.selectedPatJetsTrimmedPFCHSPacked8*

                        process.packedPatJetsPFCHS8*

                        # process.patJetsPFCHS15PFlow*
                        # process.selectedPatJetsPFCHS15PFlow*
                        #
                        # process.selectedPatJetsSoftDropPFCHS15PFlow*
                        # process.selectedPatJetsSoftDropSubjetsPFCHS15PFlow*
                        # process.selectedPatJetsSoftDropPFCHSPacked15*
                        #
                        # process.selectedPatJetsPrunedPFCHS15PFlow*
                        # process.selectedPatJetsPrunedSubjetsPFCHS15PFlow*
                        # process.selectedPatJetsPrunedPFCHSPacked15*
                        #
                        # process.selectedPatJetsTrimmedPFCHS15PFlow*
                        # process.selectedPatJetsTrimmedSubjetsPFCHS15PFlow*
                        # process.selectedPatJetsTrimmedPFCHSPacked15*

                        process.packedPatJetsPFCHS15
    )
    return process.fatjetSequence
