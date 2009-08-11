# $Id: BambuFillRECO_cfi.py,v 1.5 2009/07/22 11:26:19 loizides Exp $

import FWCore.ParameterSet.Config as cms

from MitProd.TreeFiller.MitTreeFiller_cfi import *

# Load antikt jet producers
from MitProd.TreeFiller.antiktJets_cfi import *
MitTreeFiller.AKt5Jets.active = True
MitTreeFiller.AKt7Jets.active = True
MitTreeFiller.AKt5PFJets.active = True
MitTreeFiller.AKt7PFJets.active = True

# Load stablePart producers
from MitEdm.Producers.conversionElectronsStable_cfi import *
MitTreeFiller.ElectronsStable.active                = True
MitTreeFiller.ConversionInOutElectronsStable.active = True
MitTreeFiller.ConversionOutInElectronsStable.active = True
MitTreeFiller.GsfElectronsStable.active             = True
MitTreeFiller.MergedElectronsStable.active          = True

# Load Mit vProducer
from MitProd.TreeFiller.vProducer_cff import PisStable,Ksh2PiPi,kShProducer,FillKsh

# Load Mit Mvf Conversion producer
from MitProd.TreeFiller.conversionProducer_cff import *

# For JetPlusTracks
from JetMETCorrections.Configuration.JetPlusTrackCorrections_cff import *
from JetMETCorrections.Configuration.ZSPJetCorrections219_cff import *
MitTreeFiller.IC5JetPlusTrack.active = True

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
MitTreeFiller.TrackAssociatorParameters = cms.untracked.PSet(TrackAssociatorParameters)
#Enable Track-Ecal assocation in fillers
MitTreeFiller.GeneralTracks.ecalAssocActive                       = True
MitTreeFiller.StandaloneMuonTracks.ecalAssocActive                = True
MitTreeFiller.StandaloneMuonTracksWVtxConstraint.ecalAssocActive  = True
MitTreeFiller.GlobalMuonTracks.ecalAssocActive                    = True
MitTreeFiller.ConversionInOutTracks.ecalAssocActive               = True
MitTreeFiller.ConversionOutInTracks.ecalAssocActive               = True
MitTreeFiller.GsfTracks.ecalAssocActive                           = True

# Additional output definition
MitTreeFiller.MetaInfosE29.active = True


BambuRecoSequence = cms.Sequence(antiktJets*
                                 conversionElectronsStable*
                                 mvfConversionElectronsStable*
                                 kShProducer*
                                 conversionProducer*
                                 ZSPJetCorrections*JetPlusTrackCorrections*
                                 jetvertexAssociationSequence*
                                 antiktJetVertexAssociationSequence)

BambuRecoFillSequence = cms.Sequence(MitTreeFiller*
                                     FillKsh*
                                     conversionFiller)

BambuFillRECO = cms.Sequence(BambuRecoSequence*BambuRecoFillSequence)
