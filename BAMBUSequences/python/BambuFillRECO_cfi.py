# $Id: BambuFillRECO_cfi.py,v 1.18 2009/12/01 10:15:33 bendavid Exp $

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
#addKshFiller(MitTreeFiller)

# Load Mit Mvf Conversion producer
from MitProd.TreeFiller.conversionProducer_cff import *
#addConversionFiller(MitTreeFiller)

# For JetPlusTracks
from JetMETCorrections.Configuration.JetPlusTrackCorrections_cff import *
from JetMETCorrections.Configuration.ZSPJetCorrections219_cff import *

MitTreeFiller.IC5JetPlusTrack.active = True
MitTreeFiller.SC5JetPlusTrack.active = True
MitTreeFiller.AK5JetPlusTrack.active = True

# Add track jets producers and enable fillers, disabled for now
#from RecoJets.JetProducers.TracksForJets_cff import *
#from RecoJets.JetProducers.ic5TrackJets_cfi import *
#from RecoJets.JetProducers.sc5TrackJets_cfi import *
#from RecoJets.JetProducers.ak5TrackJets_cfi import *

#MitTreeFiller.IC5TrackJets.active    = True
#MitTreeFiller.SC5TrackJets.active    = True
#MitTreeFiller.AK5TrackJets.active    = True

# Produce jet vertex association information
from MitProd.TreeFiller.JetVertexAssociation_cfi import *
# Enable Jet Vertex association for Reco Jet collections
MitTreeFiller.ItrCone5Jets.jetToVertexActive = True
MitTreeFiller.SisCone5Jets.jetToVertexActive = True
MitTreeFiller.SisCone7Jets.jetToVertexActive = True
MitTreeFiller.Kt4Jets.jetToVertexActive      = True
MitTreeFiller.Kt6Jets.jetToVertexActive      = True
MitTreeFiller.AKt5Jets.jetToVertexActive     = True
MitTreeFiller.AKt7Jets.jetToVertexActive     = True

# Load track detector associator for Track-ECal association
from MitProd.TreeFiller.TrackEcalAssociation_cfi import *
MitTreeFiller.TrackAssociatorParameters = cms.untracked.PSet(TrackAssociatorParameterBlock.TrackAssociatorParameters)

# Enable Track-Ecal assocation in fillers
#MitTreeFiller.GeneralTracks.ecalAssocActive                       = True
#MitTreeFiller.StandaloneMuonTracks.ecalAssocActive                = True
#MitTreeFiller.StandaloneMuonTracksWVtxConstraint.ecalAssocActive  = True
#MitTreeFiller.GlobalMuonTracks.ecalAssocActive                    = True
#MitTreeFiller.ConversionInOutTracks.ecalAssocActive               = True
#MitTreeFiller.ConversionOutInTracks.ecalAssocActive               = True
#MitTreeFiller.GsfTracks.ecalAssocActive                           = True

# Produce pixel hit information
from MitEdm.Producers.pixelRecHits_cfi import *
from MitEdm.Producers.vertexz_cfi import *

# Enable pixel filling
MitTreeFiller.PixelHits.active            = True
MitTreeFiller.PixelVertexes.active        = True
MitTreeFiller.TrackletVertexes.active     = True
MitTreeFiller.ClusterVertexes.active      = True
MitTreeFiller.PixelTracks.active          = True
MitTreeFiller.PixelTracks.ecalAssocActive = True

# Add pixel-less tracks, needed at startup
MitTreeFiller.PixelLessTracks.active          = True
#MitTreeFiller.PixelLessTracks.ecalAssocActive = True

# Produce global trigger record
l1GtRecord = cms.EDProducer("L1GlobalTriggerRecordProducer",
    L1GtReadoutRecordTag = cms.InputTag("gtDigis")
)

BambuRecoSequence = cms.Sequence(l1GtRecord*
                                 siPixelRecHits*
                                 trackletVertices* 
                                 clusterVertices* 
                                 conversionElectronsStable*
                                 mvfConversionElectronsStable*
#                                 kShProducer*
#                                 conversionProducer*
                                 ZSPJetCorrectionsIcone5*
                                 JetPlusTrackCorrectionsIcone5*
                                 ZSPJetCorrectionsSisCone5*
                                 JetPlusTrackCorrectionsSisCone5*
                                 ZSPJetCorrectionsAntiKt5*
                                 JetPlusTrackCorrectionsAntiKt5*
                                 #tracksForJets*
                                 #iterativeCone5TrackJets*
                                 #sisCone5TrackJets*
                                 #ak5TrackJets*
                                 jetvertexAssociationSequence)

BambuRecoFillSequence = cms.Sequence(MitTreeFiller)

BambuFillRECO = cms.Sequence(BambuRecoSequence*BambuRecoFillSequence)
