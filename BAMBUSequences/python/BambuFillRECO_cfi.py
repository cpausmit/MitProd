# $Id: BambuFillRECO_cfi.py,v 1.61 2011/10/09 23:28:48 bendavid Exp $

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

#to re-run pftau reco/id (since final tags never made it into the 42x release)
from RecoTauTag.Configuration.RecoPFTauTag_cff import *

# Load Clustered PF-MET producer
from CommonTools.ClusteredPFMetProducer.ClusteredPFMetProducer_cfi import *
MitTreeFiller.AssocPFMet.active         = True
MitTreeFiller.TrkPFMet.active           = True
MitTreeFiller.AssocOtherVtxPFMet.active = True
MitTreeFiller.CentralPFMet.active       = True
MitTreeFiller.CleanPFMet.active         = True

#photonfix parameters
from HiggsAnalysis.HiggsToGammaGamma.PhotonFixParams4_2_cfi import *
MitTreeFiller.Photons.PFParameters = PhotonFixParameters
MitTreeFiller.Photons.enablePhotonFix = True

#to re-run pftau reco/id (since final tags never made it into the 42x nor 44x release)
from RecoTauTag.Configuration.RecoPFTauTag_cff import *

BambuRecoSequence = cms.Sequence(electronsStable*
                                 eidLikelihoodExt*
                                 l1FastJetSequence*
                                 newBtaggingAll*
				 ClusteredPFMetProducer*
				 PFTau
                                 )

BambuRecoFillSequence = cms.Sequence(MitTreeFiller)

BambuFillRECO = cms.Sequence(BambuRecoSequence*BambuRecoFillSequence)
