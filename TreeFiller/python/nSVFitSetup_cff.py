import FWCore.ParameterSet.Config as cms

# load what is needed to get the nSVfit stuff run
from PhysicsTools.PatAlgos.producersLayer1.metProducer_cfi import *
patMETs.metSource = "pfMet"
patMETs.addGenMET = False
patMETs.addMuonCorrections = False

from PhysicsTools.PatAlgos.recoLayer0.tauIsolation_cff import *
from PhysicsTools.PatAlgos.producersLayer1.tauProducer_cff import *
from PhysicsTools.PatAlgos.selectionLayer1.tauSelector_cfi import *
from PhysicsTools.PatAlgos.recoLayer0.pfCandidateIsoDepositSelection_cff import *

tauIsoDepositPFCandidates.src     = "hpsPFTauProducer"
tauIsoDepositPFCandidates.ExtractorPSet.tauSource     = "hpsPFTauProducer"
tauIsoDepositPFChargedHadrons.src = "hpsPFTauProducer"
tauIsoDepositPFChargedHadrons.ExtractorPSet.tauSource = "hpsPFTauProducer"
tauIsoDepositPFNeutralHadrons.src = "hpsPFTauProducer"
tauIsoDepositPFNeutralHadrons.ExtractorPSet.tauSource = "hpsPFTauProducer"
tauIsoDepositPFGammas.src         = "hpsPFTauProducer"
tauIsoDepositPFGammas.ExtractorPSet.tauSource         = "hpsPFTauProducer"

patTaus.tauSource = "hpsPFTauProducer"
patTaus.tauIDSources  = cms.PSet(
    decayModeFinding  = cms.InputTag("hpsPFTauDiscriminationByDecayModeFinding"),
    byVLooseIsolation = cms.InputTag("hpsPFTauDiscriminationByVLooseIsolation"),
    byLooseIsolation  = cms.InputTag("hpsPFTauDiscriminationByLooseIsolation"),
    byMediumIsolation = cms.InputTag("hpsPFTauDiscriminationByMediumIsolation"),
    byTightIsolation  = cms.InputTag("hpsPFTauDiscriminationByTightIsolation"),
    againstElectronLoose  = cms.InputTag("hpsPFTauDiscriminationByLooseElectronRejection"),
    againstElectronMedium = cms.InputTag("hpsPFTauDiscriminationByMediumElectronRejection"),
    againstElectronTight  = cms.InputTag("hpsPFTauDiscriminationByTightElectronRejection"),
    againstMuonLoose = cms.InputTag("hpsPFTauDiscriminationByLooseMuonRejection"),
    againstMuonTight = cms.InputTag("hpsPFTauDiscriminationByTightMuonRejection")
)

from PhysicsTools.PatAlgos.producersLayer1.muonProducer_cfi import *
from PhysicsTools.PatAlgos.selectionLayer1.muonSelector_cfi import *
from PhysicsTools.PatAlgos.producersLayer1.electronProducer_cfi import *
from PhysicsTools.PatAlgos.selectionLayer1.electronSelector_cfi import *

patTaus.addGenMatch      = False
patTaus.addGenJetMatch   = False
patMuons.addGenMatch     = False
patElectrons.addGenMatch = False

preselectedTaus  = selectedPatTaus.clone(cut = 'pt > 15. & abs(eta) < 2.3 & tauID("decayModeFinding") > 0.5 & tauID("byLooseIsolation") > 0.5 & tauID("againstMuonLoose") > 0.5 & tauID("againstElectronLoose") > 0.5')
preselectedMuons = selectedPatMuons.clone(cut = 'isGlobalMuon & isTrackerMuon & pt > 10. & abs(eta) < 2.1 & (trackIso+caloIso)/pt <  0.2')
preselectedElecs = selectedPatElectrons.clone(cut = 'et > 15. & abs(eta) < 2.5 & (trackIso+caloIso)/et <  0.15')

# run the nSVfit algorithm
from TauAnalysis.CandidateTools.nSVfitAlgorithmDiTau_cfi import *

## mu-tau version
nSVfitConfigMuTau   = nSVfitConfig_template.clone()
nSVfitConfigMuTau.event.resonances.A.daughters.leg1.src = "preselectedMuons"
nSVfitConfigMuTau.event.resonances.A.daughters.leg2.src = "preselectedTaus"
nSVfitConfigMuTau.event.srcMEt = "patMETs"
nSVfitProducerMuTau = nSVfitProducerByLikelihoodMaximization.clone()
nSVfitProducerMuTau.config = nSVfitConfigMuTau
#nSVfitProducerMuTau.algorithm.maxObjFunctionCalls = 1000.

## e-tau version
nSVfitConfigETau   = nSVfitConfig_template.clone()
nSVfitConfigETau.event.resonances.A.daughters.leg1.src = "preselectedElecs"
nSVfitConfigETau.event.resonances.A.daughters.leg1.likelihoodFunctions = cms.VPSet(nSVfitElectronLikelihoodPhaseSpace)
nSVfitConfigETau.event.resonances.A.daughters.leg1.builder = nSVfitTauToElecBuilder
nSVfitConfigETau.event.resonances.A.daughters.leg2.src = "preselectedTaus"
nSVfitConfigETau.event.resonances.A.daughters.leg2.likelihoodFunctions = cms.VPSet(nSVfitTauLikelihoodPhaseSpace)
nSVfitConfigETau.event.resonances.A.daughters.leg2.builder = nSVfitTauToHadBuilder
nSVfitConfigETau.event.srcMEt = "patMETs"
nSVfitProducerETau = nSVfitProducerByLikelihoodMaximization.clone()
nSVfitProducerETau.config = nSVfitConfigETau
#nSVfitProducerETau.algorithm.maxObjFunctionCalls = 1000.

##e-mu version
nSVfitConfigEMu   = nSVfitConfig_template.clone()
nSVfitConfigEMu.event.resonances.A.daughters.leg1.src = "preselectedMuons"
nSVfitConfigEMu.event.resonances.A.daughters.leg1.likelihoodFunctions = cms.VPSet(nSVfitMuonLikelihoodPhaseSpace)
nSVfitConfigEMu.event.resonances.A.daughters.leg1.builder = nSVfitTauToMuBuilder
nSVfitConfigEMu.event.resonances.A.daughters.leg2.src = "preselectedElecs"
nSVfitConfigEMu.event.resonances.A.daughters.leg2.likelihoodFunctions = cms.VPSet(nSVfitElectronLikelihoodPhaseSpace)
nSVfitConfigEMu.event.resonances.A.daughters.leg2.builder = nSVfitTauToElecBuilder
nSVfitConfigEMu.event.srcMEt = "patMETs"
nSVfitProducerEMu = nSVfitProducerByLikelihoodMaximization.clone()
nSVfitProducerEMu.config = nSVfitConfigEMu
#nSVfitProducerEMu.algorithm.maxObjFunctionCalls = 1000.

##mu-mu version
nSVfitConfigMuMu   = nSVfitConfig_template.clone()
nSVfitConfigMuMu.event.resonances.A.daughters.leg1.src = "preselectedMuons"
nSVfitConfigMuMu.event.resonances.A.daughters.leg1.likelihoodFunctions = cms.VPSet(nSVfitMuonLikelihoodPhaseSpace)
nSVfitConfigMuMu.event.resonances.A.daughters.leg1.builder = nSVfitTauToMuBuilder
nSVfitConfigMuMu.event.resonances.A.daughters.leg2.src = "preselectedMuons"
nSVfitConfigMuMu.event.resonances.A.daughters.leg2.likelihoodFunctions = cms.VPSet(nSVfitMuonLikelihoodPhaseSpace)
nSVfitConfigMuMu.event.resonances.A.daughters.leg2.builder = nSVfitTauToMuBuilder
nSVfitConfigMuMu.event.srcMEt = "patMETs"
nSVfitProducerMuMu = nSVfitProducerByLikelihoodMaximization.clone()
nSVfitProducerMuMu.config = nSVfitConfigMuMu
#nSVfitProducerMuMu.algorithm.maxObjFunctionCalls = 1000.


nSVFitSetup = cms.Sequence(
    patMETs
  * patPFCandidateIsoDepositSelection
  * patPFTauIsolation
  * patTaus          
  * preselectedTaus      
  * patMuons
  * preselectedMuons
  * patElectrons
  * preselectedElecs 
  * nSVfitProducerMuTau
  * nSVfitProducerETau
  * nSVfitProducerEMu
  * nSVfitProducerMuMu
)
