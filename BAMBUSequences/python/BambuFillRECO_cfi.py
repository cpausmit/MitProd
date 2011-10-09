# $Id: BambuFillRECO_cfi.py,v 1.59 2011/10/03 16:16:56 ksung Exp $

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

BambuRecoSequence = cms.Sequence(electronsStable*
                                 eidLikelihoodExt*
                                 l1FastJetSequence*
                                 newBtaggingAll*
				 ClusteredPFMetProducer
                                 )

BambuRecoFillSequence = cms.Sequence(MitTreeFiller)

BambuFillRECO = cms.Sequence(BambuRecoSequence*BambuRecoFillSequence)
