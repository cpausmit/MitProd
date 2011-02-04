# $Id: BambuFillRECO_cfi.py,v 1.36 2010/11/22 16:56:12 bendavid Exp $

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

#corrected nexpectedhitsinner information for electrons and muons
#from MitProd.TreeFiller.correctedExpectedHits_cff import *
#MitTreeFiller.Electrons.expectedHitsName = 'expectedHitsEle'
#MitTreeFiller.Muons.expectedHitsName = 'expectedHitsMu'

# Produce pixel hit information
#from MitEdm.Producers.pixelRecHits_cfi import *
#from MitEdm.Producers.vertexz_cfi import *

## Enable pixel filling
#MitTreeFiller.PixelHits.active            = True
#MitTreeFiller.PixelVertexes.active        = True
#MitTreeFiller.TrackletVertexes.active     = True
#MitTreeFiller.ClusterVertexes.active      = True
#MitTreeFiller.PixelTracks.active          = True
##MitTreeFiller.PixelTracks.ecalAssocActive = True

## Produce event selection data
#from MitEdm.Producers.evtSelData_cfi import *
#MitTreeFiller.EvtSelData.active = True

# Load FastJet L1 corrections
from MitProd.TreeFiller.FastJetCorrection_cff import *
MitTreeFiller.AKt5PFJetsL1.active = True

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
                                 l1FastJetCorrection
                                 )

BambuRecoFillSequence = cms.Sequence(MitTreeFiller)

BambuFillRECO = cms.Sequence(BambuRecoSequence*BambuRecoFillSequence)
