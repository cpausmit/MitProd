# $Id: BambuFillRECO_cfi.py,v 1.57 2011/09/12 15:21:37 bendavid Exp $

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

#ctvmft stuff temporarily disabled pending debugging
# Load Mit vProducer
from MitProd.TreeFiller.vProducer_cff import PisStable,Ksh2PiPi,kShProducer,addKshFiller
#addKshFiller(MitTreeFiller)

## Load Mit Mvf Conversion producer
from MitProd.TreeFiller.conversionProducer_cff import *
#addConversionFiller(MitTreeFiller)

#Load NSVFit sequences
from MitProd.TreeFiller.nSVFitSetup_cff import *
from MitProd.TreeFiller.nSVFitResults_cff import *
addNSVFitResults(MitTreeFiller)


# Load track detector associator for Track-ECal association
from MitProd.TreeFiller.TrackEcalAssociation_cfi import *
MitTreeFiller.TrackAssociatorParameters = cms.untracked.PSet(TrackAssociatorParameterBlock.TrackAssociatorParameters)

# Enable Track-Ecal assocation in fillers
#MitTreeFiller.GeneralTracks.ecalAssocActive                      = True
#MitTreeFiller.StandaloneMuonTracks.ecalAssocActive               = True
#MitTreeFiller.StandaloneMuonTracksWVtxConstraint.ecalAssocActive = True
#MitTreeFiller.GlobalMuonTracks.ecalAssocActive                   = True
#MitTreeFiller.ConversionInOutTracks.ecalAssocActive              = True
#MitTreeFiller.ConversionOutInTracks.ecalAssocActive              = True
#MitTreeFiller.GsfTracks.ecalAssocActive                          = True

#electron likelihood-based id
from RecoEgamma.ElectronIdentification.electronIdLikelihoodExt_cfi import *
MitTreeFiller.Electrons.eIDLikelihoodName = 'eidLikelihoodExt'


# Load FastJet L1 corrections
from MitProd.TreeFiller.FastJetCorrection_cff import *

# Load the pileup energy density computed by fastjet 
MitTreeFiller.PileupEnergyDensity.active = True

# Enable the pileup energy density correction by fastjet 
#MitTreeFiller.Kt4PFJetsNoArea.active = True
#MitTreeFiller.AKt5PFJetsNoArea.active = True

#MitTreeFiller.Kt4PFJets.edmName = 'kt4PFJetsForL1Correction'
#MitTreeFiller.AKt5PFJets.edmName = 'ak5PFJetsForL1Correction'


from MitProd.TreeFiller.newbtagging_cff import *
#newJetTracksAssociatorAtVertex.jets = "ak5PFJetsForL1Correction"
#newSoftElectronTagInfos.jets = "ak5PFJetsForL1Correction"
#newSoftMuonTagInfos.jets = "ak5PFJetsForL1Correction"
MitTreeFiller.AKt5PFJets.bTaggingActive = True

#to re-run pftau reco/id (since final tags never made it into the 42x release)
from RecoTauTag.Configuration.RecoPFTauTag_cff import *

BambuRecoSequence = cms.Sequence(#siPixelRecHits*
                                 #trackletVertices* 
                                 #clusterVertices*
                                 #evtSelData*
                                 electronsStable*
                                 #kShProducer*
                                 #conversionProducer*
                                 #JetPlusTrackCorrectionsIcone5*
                                 #JetPlusTrackCorrectionsAntiKt5*
                                 #jetvertexAssociationSequence*
                                 eidLikelihoodExt*
                                 l1FastJetSequence*
                                 newBtaggingAll*
				 PFTau*
                                 nSVFitSetup
                                 )

BambuRecoFillSequence = cms.Sequence(MitTreeFiller)

BambuFillRECO = cms.Sequence(BambuRecoSequence*BambuRecoFillSequence)
