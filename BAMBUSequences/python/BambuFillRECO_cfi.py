# $Id: BambuFillRECO_cfi.py,v 1.53 2011/05/20 16:53:39 bendavid Exp $

import FWCore.ParameterSet.Config as cms

from MitProd.TreeFiller.MitTreeFiller_cfi import *

# Load stablePart producers
from MitEdm.Producers.conversionElectronsStable_cfi import *
MitTreeFiller.ElectronsStable.active                = True
MitTreeFiller.ConversionInOutElectronsStable.active = True
MitTreeFiller.ConversionOutInElectronsStable.active = True
MitTreeFiller.GsfElectronsStable.active             = True
MitTreeFiller.MergedElectronsStable.active          = True

# Load Mit vProducer
from MitProd.TreeFiller.vProducer_cff import PisStable,Ksh2PiPi,kShProducer,addKshFiller
addKshFiller(MitTreeFiller)

# Load Mit Mvf Conversion producer
from MitProd.TreeFiller.conversionProducer_cff import *
addConversionFiller(MitTreeFiller)

# For JetPlusTracks

MitTreeFiller.IC5JetPlusTrack.active = True
MitTreeFiller.AK5JetPlusTrack.active = True

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
MitTreeFiller.Kt4PFJetsNoArea.active = True
MitTreeFiller.AKt5PFJetsNoArea.active = True

MitTreeFiller.Kt4PFJets.edmName = 'kt4PFJetsForL1Correction'
MitTreeFiller.AKt5PFJets.edmName = 'ak5PFJetsForL1Correction'


from MitProd.TreeFiller.DAPrimaryVertex_cff import *
MitTreeFiller.DAPrimaryVertexes.active = True
MitTreeFiller.DAPrimaryVertexesBS.active = True
MitTreeFiller.Electrons.pvEdmName = 'offlinePrimaryVerticesDA'
MitTreeFiller.Electrons.pvBSEdmName = 'offlinePrimaryVerticesDABS'
MitTreeFiller.Muons.pvEdmName = 'offlinePrimaryVerticesDA'
MitTreeFiller.Muons.pvBSEdmName = 'offlinePrimaryVerticesDABS'


from MitProd.TreeFiller.newbtagging_cff import *
newJetTracksAssociatorAtVertex.jets = "ak5PFJetsForL1Correction"
newSoftElectronTagInfos.jets = "ak5PFJetsForL1Correction"
newSoftMuonTagInfos.jets = "ak5PFJetsForL1Correction"
MitTreeFiller.AKt5PFJets.bTaggingActive = True

BambuRecoSequence = cms.Sequence(#siPixelRecHits*
                                 #trackletVertices* 
                                 #clusterVertices*
                                 #evtSelData*
                                 electronsStable*
                                 kShProducer*
                                 conversionProducer*
                                 #JetPlusTrackCorrectionsIcone5*
                                 #JetPlusTrackCorrectionsAntiKt5*
                                 #jetvertexAssociationSequence*
                                 eidLikelihoodExt*
                                 l1FastJetSequence *
                                 daPrimaryVertexSequence *
                                 newBtaggingAll
                                 )

BambuRecoFillSequence = cms.Sequence(MitTreeFiller)

BambuFillRECO = cms.Sequence(BambuRecoSequence*BambuRecoFillSequence)
