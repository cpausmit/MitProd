# $Id: BambuFillRECO_cfi.py,v 1.66 2012/12/28 17:36:20 pharris Exp $

import FWCore.ParameterSet.Config as cms

from MitProd.TreeFiller.MitTreeFiller_cfi import *

# Load stablePart producers
from MitEdm.Producers.conversionElectronsStable_cfi import *
MitTreeFiller.ElectronsStable.active                = True
MitTreeFiller.ConversionInOutElectronsStable.active = True
MitTreeFiller.ConversionOutInElectronsStable.active = True
MitTreeFiller.GsfElectronsStable.active             = True
MitTreeFiller.ConversionStepElectronsStable.active  = True
MitTreeFiller.MergedElectronsStable.active          = True

## Load Mit Mvf Conversion producer
from MitProd.TreeFiller.conversionProducer_cff import *

#electron likelihood-based id
from RecoEgamma.ElectronIdentification.electronIdLikelihoodExt_cfi import *
MitTreeFiller.Electrons.eIDLikelihoodName = 'eidLikelihoodExt'

# Load FastJet L1 corrections
from MitProd.TreeFiller.FastJetCorrection_cff import *

# Load the pileup energy density computed by fastjet 
MitTreeFiller.PileupEnergyDensity.active = True


from MitProd.TreeFiller.newbtagging_cff import *
MitTreeFiller.AKt5PFJets.bTaggingActive = True

# Load Clustered PF-MET producer
#from CommonTools.ClusteredPFMetProducer.ClusteredPFMetProducer_cfi import *
#MitTreeFiller.AssocPFMet.active         = True
#MitTreeFiller.TrkPFMet.active           = True
#MitTreeFiller.AssocOtherVtxPFMet.active = True
#MitTreeFiller.CentralPFMet.active       = True
#MitTreeFiller.CleanPFMet.active         = True

#to re-run pftau reco/id (since final tags never made it into the 42x nor 44x release)
from RecoTauTag.Configuration.RecoPFTauTag_cff import *

from PhysicsTools.SelectorUtils.pvSelector_cfi import pvSelector
goodOfflinePrimaryVertices = cms.EDFilter(
    "PrimaryVertexObjectFilter",
    filterParams = pvSelector.clone( minNdof = cms.double(4.0),maxZ = cms.double(24.0) ),
    src = cms.InputTag('offlinePrimaryVertices')
    )

from CommonTools.ParticleFlow.pfNoPileUp_cff                 import *
from CommonTools.ParticleFlow.pfParticleSelection_cff        import *
from CommonTools.ParticleFlow.pfPhotons_cff                  import *
from CommonTools.ParticleFlow.pfElectrons_cff                import *
from CommonTools.ParticleFlow.pfMuons_cff                    import *
from CommonTools.ParticleFlow.pfJets_cff                     import *
from CommonTools.ParticleFlow.TopProjectors.pfNoMuon_cfi     import *
from CommonTools.ParticleFlow.TopProjectors.pfNoElectron_cfi import *
from CommonTools.ParticleFlow.TopProjectors.pfNoJet_cfi      import *

pfPileUp.PFCandidates          = 'particleFlow'
pfNoPileUp.bottomCollection    = 'particleFlow'
pfPileUpIso.PFCandidates       = 'particleFlow'
pfNoPileUpIso.bottomCollection = 'particleFlow'
pfPileUp.Enable                = True
pfPileUp.Vertices              = 'goodOfflinePrimaryVertices'
pfPileUp.checkClosestZVertex   = cms.bool(False)
pfJets.doAreaFastjet           = True
pfJets.doRhoFastjet            = False

#to run and store the decisions of the MET filters
#> The iso-based HBHE noise filter
from CommonTools.RecoAlgos.HBHENoiseFilterResultProducer_cfi import *
#> The ECAL dead cell trigger primitive filter
from RecoMET.METFilters.EcalDeadCellTriggerPrimitiveFilter_cfi import *
EcalDeadCellTriggerPrimitiveFilter.taggingMode = cms.bool(True)
#> The EE bad SuperCrystal filter
from RecoMET.METFilters.eeBadScFilter_cfi import *
eeBadScFilter.taggingMode = cms.bool(True)
#> The ECAL laser correction filter
from RecoMET.METFilters.ecalLaserCorrFilter_cfi import *
ecalLaserCorrFilter.taggingMode = cms.bool(True)
#> The Good vertices collection needed by the tracking failure filter
goodVertices = cms.EDFilter(
  "VertexSelector",
  filter = cms.bool(False),
  src = cms.InputTag("offlinePrimaryVertices"),
  cut = cms.string("!isFake && ndof > 4 && abs(z) <= 24 && position.rho < 2")
)
#> The tracking failure filter
from RecoMET.METFilters.trackingFailureFilter_cfi import *
trackingFailureFilter.taggingMode = cms.bool(True)
#> The tracking POG filters: NB for these three false means good event
from RecoMET.METFilters.trackingPOGFilters_cff import *
manystripclus53X.taggedMode = cms.untracked.bool(True)
manystripclus53X.forcedValue = cms.untracked.bool(False)
toomanystripclus53X.taggedMode = cms.untracked.bool(True)
toomanystripclus53X.forcedValue = cms.untracked.bool(False)
logErrorTooManyClusters.taggedMode = cms.untracked.bool(True)
logErrorTooManyClusters.forcedValue = cms.untracked.bool(False)
#> The MET filter sequence
metFilters = cms.Sequence(
   HBHENoiseFilterResultProducer *
   EcalDeadCellTriggerPrimitiveFilter *
   goodVertices * trackingFailureFilter *
   eeBadScFilter *
   ecalLaserCorrFilter *
   trkPOGFilters
)

BambuRecoSequence = cms.Sequence(electronsStable*
                                 eidLikelihoodExt*
                                 l1FastJetSequence*
                                 newBtaggingAll*
				 #ClusteredPFMetProducer*
                                 goodOfflinePrimaryVertices*
                                 pfNoPileUpSequence*
                                 pfParticleSelectionSequence*
                                 pfPhotonSequence*
                                 pfMuonSequence*
                                 pfNoMuon*
                                 pfElectronSequence*
                                 pfNoElectron*
                                 pfJetSequence*
                                 pfNoJet*
                                 l1FastJetSequenceCHS*
                                 PFTau*
                                 metFilters
                                 )

BambuRecoFillSequence = cms.Sequence(MitTreeFiller)

BambuFillRECO = cms.Sequence(BambuRecoSequence*BambuRecoFillSequence)
