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

def initFatJets(process,isData):
    ## called once to set up b-tagging
    isMC = not isData

    ########################################
    ##         INITIAL SETUP              ##
    ########################################
    jetCorrectionsAK4 = ('AK4PFchs', ['L1FastJet', 'L2Relative', 'L3Absolute'], 'None')
    jetCorrectionsAK8 = ('AK8PFchs', ['L1FastJet', 'L2Relative', 'L3Absolute'], 'None')
    if isData:
        jetCorrectionsAK4[1].append('L2L3Residual')
        jetCorrectionsAK8[1].append('L2L3Residual')
    
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
    
    PFjetAlgo="AK4"
    process.load("PhysicsTools.PatAlgos.patSequences_cff")

    usePF2PAT(process,runPF2PAT=True, jetAlgo=PFjetAlgo, runOnMC=isMC, postfix=postfix,
              jetCorrections=jetCorrectionsAK4, pvCollection=cms.InputTag(pvSource), outputModules = [])

    ## Top projections in PF2PAT
    getattr(process,"pfPileUpJME"+postfix).checkClosestZVertex = False
    getattr(process,"pfNoPileUpJME"+postfix).enable = True
    getattr(process,"pfNoMuonJMEPFBRECO"+postfix).enable = True
    getattr(process,"pfNoElectronJMEPFBRECO"+postfix).enable = True
    ## Load standard PAT objects 
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
        ,'pfTrackCountingHighPurBJetTags'
        ,'pfTrackCountingHighEffBJetTags'
        ,'pfCombinedSecondaryVertexV2BJetTags'
        ,'pfCombinedInclusiveSecondaryVertexV2BJetTags'
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
    
    process.pfbrecoSequence = cms.Sequence(
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

    return process.pfbrecoSequence



def makeFatJets(process,isData,algoLabel,jetRadius,pfCandidates='particleFlow'):

    postfix = "PFlow"
    isMC = not isData

    ########################################
    ##         INITIAL SETUP              ##
    ########################################
    jetCorrectionsAK4 = ('AK4PFchs', ['L1FastJet', 'L2Relative', 'L3Absolute'], 'None')
    jetCorrectionsAK8 = ('AK8PFchs', ['L1FastJet', 'L2Relative', 'L3Absolute'], 'None')
    if isData:
        jetCorrectionsAK4[1].append('L2L3Residual')
        jetCorrectionsAK8[1].append('L2L3Residual')

    if algoLabel=='CA':
      jetAlgo = 'CambridgeAachen'
    else:
      jetAlgo = 'AntiKt'

    rLabel = str(int(jetRadius*10))
    
    if pfCandidates=='particleFlow':
      puMethod='CHS'
    else:
      puMethod='Puppi'
    
    if jetRadius<1:
      sdZcut = 0.1
      sdBeta = 0.0
    else:
      sdZcut = 0.2
      sdBeta = 1.0

    ## Various collection names
    genParticles = 'genParticles'
    genJetCollection = 'ak4GenJetsNoNu'+postfix
    pvSource = 'offlinePrimaryVertices'
    svSource = 'inclusiveCandidateSecondaryVertices'
    muSource = 'muons'
    elSource = 'gedGsfElectrons'

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
        ,'pfTrackCountingHighPurBJetTags'
        ,'pfTrackCountingHighEffBJetTags'
        ,'pfCombinedSecondaryVertexV2BJetTags'
        ,'pfCombinedInclusiveSecondaryVertexV2BJetTags'
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


    ########################################
    ##           REMAKE JETS              ##
    ########################################
  
    if not(hasattr(process,"genJetsNoNu"+rLabel)):
      setattr(process,"genJetsNoNu"+rLabel, ak4GenJets.clone(
                                             jetAlgorithm = cms.string(jetAlgo),
                                             rParam = cms.double(jetRadius),
                                             src = cms.InputTag("genParticlesForJetsNoNu"+postfix)
                                           )
      )
    setattr(process,"PFJets"+puMethod+rLabel, ak4PFJets.clone(
                                                  jetAlgorithm = cms.string(jetAlgo),
                                                  rParam = cms.double(jetRadius),
                                                  src = cms.InputTag(pfCandidates),
                                                  srcPVs = cms.InputTag(pvSource),
                                                  doAreaFastjet = cms.bool(True),
                                                  jetPtMin = cms.double(150)
                                              )
    )
    if not(hasattr(process,"genJetsNoNuSoftDrop"+rLabel)):
      setattr(process,"genJetsNoNuSoftDrop"+rLabel, ak4GenJets.clone(
																										        jetAlgorithm = cms.string(jetAlgo),
																										        rParam = cms.double(jetRadius),
																										        R0 = cms.double(jetRadius),
																										        src = cms.InputTag("genParticlesForJetsNoNu"+postfix),
																										        useSoftDrop = cms.bool(True),
																										        zcut = cms.double(sdZcut),
																										        beta = cms.double(sdBeta),
																										        writeCompound = cms.bool(True),
																										        jetCollInstanceName=cms.string("SubJets")
																									)
      )
    setattr(process,"PFJets"+puMethod+"SoftDrop"+rLabel, ak4PFJetsSoftDrop.clone(
																										        jetAlgorithm = cms.string(jetAlgo),
																										        rParam = cms.double(jetRadius),
																										        R0 = cms.double(jetRadius),
																										        zcut = cms.double(sdZcut),
																										        beta = cms.double(sdBeta),
                                                            src = cms.InputTag(pfCandidates),
                                                            srcPVs = cms.InputTag(pvSource),
																										        doAreaFastjet = cms.bool(True),
																										        writeCompound = cms.bool(True),
																										        jetCollInstanceName=cms.string("SubJets"),
																										        jetPtMin = cms.double(150)
																										    )
    )


    ########################################
    ##          MAKE PAT JETS             ##
    ########################################

    addJetCollection(
        process,
        labelName='PF'+puMethod+rLabel,
        jetSource=cms.InputTag('PFJets'+puMethod+rLabel),
        algo=algoLabel,           # needed for jet flavor clustering
        rParam=jetRadius, # needed for jet flavor clustering
        pfCandidates = cms.InputTag(pfCandidates),
        pvSource = cms.InputTag(pvSource),
        svSource = cms.InputTag(svSource),
        muSource = cms.InputTag(muSource),
        elSource = cms.InputTag(elSource),
        btagInfos = bTagInfos,
        btagDiscriminators = bTagDiscriminators,
        jetCorrections = ['None'],
        genJetCollection = cms.InputTag('genJetsNoNu'+rLabel),
        genParticles = cms.InputTag(genParticles),
        postfix = postfix
    )
    getattr(process,'selectedPatJetsPF'+puMethod+rLabel+postfix).cut = cms.string("abs(eta) < " + str(2.5))
    ## SOFT DROP ##
    addJetCollection(
        process,
        labelName='SoftDropPF'+puMethod+rLabel,
        jetSource=cms.InputTag('PFJets'+puMethod+'SoftDrop'+rLabel),
        algo=algoLabel,
        btagInfos = ['None'],
        btagDiscriminators = ['None'],
        jetCorrections=['None'],
        genJetCollection = cms.InputTag('genJetsNoNu'+rLabel),
        genParticles = cms.InputTag(genParticles),
        getJetMCFlavour = False, # jet flavor disabled
        postfix = postfix
    )
    addJetCollection(
        process,
        labelName='SoftDropSubjetsPF'+puMethod+rLabel,
        jetSource=cms.InputTag('PFJets'+puMethod+'SoftDrop'+rLabel,'SubJets'),
        algo=algoLabel,           # needed for subjet flavor clustering
        rParam=jetRadius, # needed for subjet flavor clustering
        pfCandidates = cms.InputTag(pfCandidates),
        pvSource = cms.InputTag(pvSource),
        svSource = cms.InputTag(svSource),
        muSource = cms.InputTag(muSource),
        elSource = cms.InputTag(elSource),
        btagInfos = bTagInfos,
        btagDiscriminators = bTagDiscriminators,
        jetCorrections = ['None'],
        genJetCollection = cms.InputTag('genJetsNoNuSoftDrop'+rLabel,'SubJets'),
        genParticles = cms.InputTag(genParticles),
        explicitJTA = True,  # needed for subjet b tagging
        svClustering = True, # needed for subjet b tagging
        fatJets = cms.InputTag('PFJets'+puMethod+rLabel),              # needed for subjet flavor clustering
        groomedFatJets = cms.InputTag('PFJets'+puMethod+'SoftDrop'+rLabel), # needed for subjet flavor clustering
        runIVF = False,
        postfix = postfix
    )
    setattr(process,"selectedPatJetsSoftDropPF"+puMethod+"Packed"+rLabel, cms.EDProducer("BoostedJetMerger",
                                                                            jetSrc=cms.InputTag("selectedPatJetsSoftDropPF"+puMethod+rLabel+postfix),
                                                                            subjetSrc=cms.InputTag("selectedPatJetsSoftDropSubjetsPF"+puMethod+rLabel+postfix)
                                                                          )
    )                                                                          

    ## PACK ##
    setattr(process,"packedPatJetsPF"+puMethod+rLabel, cms.EDProducer("JetSubstructurePacker",
                                                          jetSrc = cms.InputTag('selectedPatJetsPF'+puMethod+rLabel+postfix),
                                                          distMax = cms.double(jetRadius),
                                                          algoTags = cms.VInputTag(),
                                                          algoLabels = cms.vstring(),
                                                          fixDaughters = cms.bool(False)
                                                        )
    )
    getattr(process,"packedPatJetsPF"+puMethod+rLabel).algoTags.append(cms.InputTag('selectedPatJetsSoftDropPF'+puMethod+'Packed'+rLabel))
    getattr(process,"packedPatJetsPF"+puMethod+rLabel).algoLabels.append('SoftDrop')
    
    btagSeq = makeBtagSequence(process,bTagDiscriminators,bTagInfos,"PF"+puMethod+rLabel+postfix)
    btagSoftDropSeq = makeBtagSequence(process,bTagDiscriminators,bTagInfos,"SoftDropSubjetsPF"+puMethod+rLabel+postfix)

    pfJetsSequence = cms.Sequence(
        # TODO
        
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
