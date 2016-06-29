import string

import FWCore.ParameterSet.Config as cms
from RecoJets.JetProducers.ak4PFJets_cfi import ak4PFJets
from RecoJets.JetProducers.nJettinessAdder_cfi import Njettiness
import RecoBTag.Configuration.RecoBTag_cff as btag
from PhysicsTools.PatAlgos.producersLayer1.jetProducer_cfi import _patJets
from PhysicsTools.PatAlgos.selectionLayer1.jetSelector_cfi import selectedPatJets
from MitProd.TreeFiller.utils.setupBTag import initBTag, setupBTag

def makeFatJets(process, src, algoLabel, jetRadius, colLabel, jetPtMin = 150.):

  rLabel = algoLabel + '%d' % (int(jetRadius * 10)) # 'ak8'

  if algoLabel.lower() == 'ca':
    jetAlgo = 'CambridgeAachen'
  elif algoLabel.lower() == 'ak':
    jetAlgo = 'AntiKt'
  else:
    raise RuntimeError('Unknown jet algo ' + algoLabel)

  if jetRadius < 1.:
    sdZcut = 0.1
    sdBeta = 0.0
  else:
    sdZcut = 0.2
    sdBeta = 1.0

  ## Various collection names
  pfSource = 'particleFlow'
  pvSource = 'offlinePrimaryVertices'
  svSource = 'inclusiveCandidateSecondaryVertices'
  muSource = 'muons'
  elSource = 'gedGsfElectrons'

  ########################################
  ##           CLUSTER JETS             ##
  ########################################

  jets = ak4PFJets.clone(
    jetAlgorithm = cms.string(jetAlgo),
    rParam = cms.double(jetRadius),
    src = cms.InputTag(src),
    srcPVs = cms.InputTag(pvSource),
    doAreaFastjet = cms.bool(True),
    jetPtMin = cms.double(jetPtMin)
  )

  jetsName = rLabel + colLabel # 'ak8PFJetsCHS'
  setattr(process, jetsName, jets)

  softDropJets = jets.clone(
    useSoftDrop = cms.bool(True),
    R0 = cms.double(jetRadius),
    zcut = cms.double(sdZcut),
    beta = cms.double(sdBeta),
    writeCompound = cms.bool(True),
    useExplicitGhosts = cms.bool(True),
    jetCollInstanceName = cms.string("SubJets")
  )

  softDropJetsName = jetsName + 'SoftDrop'
  setattr(process, softDropJetsName, softDropJets)

  softDropSubjetsName = softDropJetsName + 'Subjets'

  clustering = cms.Sequence(
    jets +
    softDropJets
  )
  
  ########################################
  ##           SUBSTRUCTURE             ##
  ########################################

  njettiness = Njettiness.clone(
    src = cms.InputTag(jetsName),
    R0 = cms.double(jetRadius),
    Njets = cms.vuint32(1,2,3,4)
  )

  njettinessName = 'njettiness' + jetsName
  setattr(process, njettinessName, njettiness)

  sdKinematics = cms.EDProducer('RecoJetDeltaRValueMapProducer',
    src = cms.InputTag(jetsName),
    matched = cms.InputTag(softDropJetsName),
    distMax = cms.double(1.5),
    values = cms.vstring('mass'),
    valueLabels = cms.vstring('Mass'),
  )

  sdKinematicsName = 'sdKinematics' + jetsName
  setattr(process, sdKinematicsName, sdKinematics)

  substructure = cms.Sequence(
    njettiness +
    sdKinematics
  )

  #######################################
  ##         FATJET BTAGGING           ##
  #######################################

  initBTag(process)

  btags = [
    'pfJetProbabilityBJetTags',
    'pfJetBProbabilityBJetTags',
    'pfCombinedSecondaryVertexV2BJetTags',
    'pfCombinedInclusiveSecondaryVertexV2BJetTags',
    'pfCombinedMVAV2BJetTags'
  ]

  btagInfos = []
  btagging = setupBTag(process, jetsName, jetsName, tags = btags, addedTagInfos = btagInfos)

  #######################################
  ##         SUBJET BTAGGING           ##
  #######################################

  # Product: std::vector<reco::CandIPTagInfo>
  # IPTagInfo object contains a reference to jet
  subjetIpTagInfos = btag.pfImpactParameterTagInfos.clone(
    jets = cms.InputTag(softDropJetsName, 'SubJets'),
    maxDeltaR = cms.double(jetRadius),
    primaryVertex = cms.InputTag(pvSource),
    candidates = cms.InputTag(pfSource),
    explicitJTA = cms.bool(True)
  )

  subjetIpTagInfosName = 'pfImpactParameterTagInfos' + softDropSubjetsName
  setattr(process, subjetIpTagInfosName, subjetIpTagInfos)

  # Product: std::vector<TemplatedSecondaryVertexTagInfo<reco::CandIPTagInfo, reco::VertexCompositePtrCandidate>>
  subjetInclSVFinderTagInfos = btag.pfInclusiveSecondaryVertexFinderTagInfos.clone(
    trackIPTagInfos = cms.InputTag(subjetIpTagInfosName),
    extSVCollection = cms.InputTag(svSource),
    useSVClustering = cms.bool(True),
    jetAlgorithm = cms.string(jetAlgo),
    rParam = cms.double(jetRadius),
    fatJets = cms.InputTag(jetsName),
    groomedFatJets = cms.InputTag(softDropJetsName)
  )

  subjetInclSVFinderTagInfosName = 'pfInclusiveSecondaryVertexFinderTagInfos' + softDropSubjetsName
  setattr(process, subjetInclSVFinderTagInfosName, subjetInclSVFinderTagInfos)

  # Product: JetTagCollection (aka reco::JetFloatAssociation::Container aka edm::AssociationVector<reco::JetRefBaseProd, std::vector<float>>)
  # Uses the internal reference to the jet in the TagInfos object to create an AssociationVector
  subjetBjetTags = btag.pfCombinedInclusiveSecondaryVertexV2BJetTags.clone(
    tagInfos = cms.VInputTag( 
      cms.InputTag(subjetIpTagInfosName), 
      cms.InputTag(subjetInclSVFinderTagInfosName)
    )
  )

  subjetBjetTagsName = 'pfCombinedInclusiveSecondaryVertexV2BJetTags' + softDropSubjetsName
  setattr(process, subjetBjetTagsName, subjetBjetTags)

  subjetBTagging = cms.Sequence(
    subjetIpTagInfos +
    subjetInclSVFinderTagInfos +
    subjetBjetTags
  )

  ########################################
  ##          MAKE PAT JETS             ##
  ########################################

  patJets = _patJets.clone(
    jetSource = cms.InputTag(jetsName),
    getJetMCFlavour = cms.bool(False),
    addGenJetMatch = cms.bool(False),
    addGenPartonMatch = cms.bool(False),
    addJetFlavourInfo = cms.bool(False),
    addAssociatedTracks = cms.bool(False),
    addJetCharge = cms.bool(False),
    addJetCorrFactors = cms.bool(False),
    addBTagInfo = cms.bool(True),
    addTagInfos = cms.bool(True),
    addDiscriminators = cms.bool(True),
    tagInfoSources = cms.VInputTag([cms.InputTag(name) for name in btagInfos]),
    discriminatorSources = cms.VInputTag([cms.InputTag(name + jetsName) for name in btags])
  )
  patJets.userData.userFloats.src = [
    cms.InputTag(njettinessName + ':tau1'),
    cms.InputTag(njettinessName + ':tau2'),
    cms.InputTag(njettinessName + ':tau3'),
    cms.InputTag(njettinessName + ':tau4'),
    cms.InputTag(sdKinematicsName + ':Mass')
  ]

  patJetsName = 'pat' + jetsName
  setattr(process, patJetsName, patJets)

  selectedJets = selectedPatJets.clone(
    src = cms.InputTag(patJetsName)
  )

  selectedJetsName = 'selectedPat' + jetsName
  setattr(process, selectedJetsName, selectedJets)

  patSoftDropJets = _patJets.clone(
    jetSource = cms.InputTag(softDropJetsName),
    getJetMCFlavour = cms.bool(False),
    addGenJetMatch = cms.bool(False),
    addGenPartonMatch = cms.bool(False),
    addJetFlavourInfo = cms.bool(False),
    addTagInfos = cms.bool(False),
    addBTagInfo = cms.bool(False),
    addDiscriminators = cms.bool(False),
    addAssociatedTracks = cms.bool(False),
    addJetCharge = cms.bool(False),
    addJetCorrFactors = cms.bool(False)
  )

  patSoftDropJetsName = 'pat' + softDropJetsName
  setattr(process, patSoftDropJetsName, patSoftDropJets)

  selectedSoftDropJets = selectedPatJets.clone(
    src = cms.InputTag(patSoftDropJetsName)
  )

  selectedSoftDropJetsName = 'selectedPat' + softDropJetsName
  setattr(process, selectedSoftDropJetsName, selectedSoftDropJets)

  patSoftDropSubjets = _patJets.clone(
    jetSource = cms.InputTag(softDropJetsName, 'SubJets'),
    getJetMCFlavour = cms.bool(False),
    addGenJetMatch = cms.bool(False),
    addGenPartonMatch = cms.bool(False),
    addJetFlavourInfo = cms.bool(False),
    addAssociatedTracks = cms.bool(False),
    addJetCharge = cms.bool(False),
    addJetCorrFactors = cms.bool(False),
    addBTagInfo = cms.bool(True),
    addDiscriminators = cms.bool(True),
    addTagInfos = cms.bool(False),
    discriminatorSources = cms.VInputTag(
      cms.InputTag(subjetBjetTagsName)
    )
  )

  patSoftDropSubjetsName = 'pat' + softDropJetsName + 'Subjets'
  setattr(process, patSoftDropSubjetsName, patSoftDropSubjets)

  selectedSoftDropSubjets = selectedPatJets.clone(
    src = cms.InputTag(patSoftDropSubjetsName)
  )

  selectedSoftDropSubjetsName = 'selectedPat' + softDropSubjetsName
  setattr(process, selectedSoftDropSubjetsName, selectedSoftDropSubjets)

  pat = cms.Sequence(
    patJets +
    selectedJets +
    patSoftDropJets +
    selectedSoftDropJets +
    patSoftDropSubjets +
    selectedSoftDropSubjets
  )

  ###############################################
  ##     Pack subjets back into fat jets       ##
  ###############################################

  packedSoftDropJets = cms.EDProducer('BoostedJetMerger',
    jetSrc = cms.InputTag(selectedSoftDropJetsName),
    subjetSrc = cms.InputTag(selectedSoftDropSubjetsName)
  )

  packedSoftDropJetsName = 'packed' + softDropJetsName
  setattr(process, packedSoftDropJetsName, packedSoftDropJets)

  packedPatJets = cms.EDProducer('JetSubstructurePacker',
    jetSrc = cms.InputTag(selectedJetsName),
    distMax = cms.double(jetRadius),
    algoTags = cms.VInputTag(cms.InputTag(packedSoftDropJetsName)),
    algoLabels = cms.vstring('SoftDrop'),
    fixDaughters = cms.bool(False)
  )

  packedPatJetsName = 'packed' + jetsName
  setattr(process, packedPatJetsName, packedPatJets)

  packing = cms.Sequence(
    packedSoftDropJets +
    packedPatJets
  )

  ##############################
  ##     Return a sequence    ##
  ##############################

  sequence = cms.Sequence(
    clustering + 
    substructure +
    btagging +
    subjetBTagging +
    pat +
    packing
  )

  return sequence
