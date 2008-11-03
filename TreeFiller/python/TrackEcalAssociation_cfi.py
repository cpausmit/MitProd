# $Id: JetVertexAssociation_cfi.py,v 1.3 2008/09/30 15:57:32 sixie Exp $

import FWCore.ParameterSet.Config as cms

from TrackingTools.TrackAssociator.DetIdAssociatorESProducer_cff import *
from TrackingTools.TrackAssociator.default_cfi import *

TrackAssociatorParameters.useEcal = False
TrackAssociatorParameters.useHO   = False
TrackAssociatorParameters.useMuon = False
TrackAssociatorParameters.useHcal = False
TrackAssociatorParameters.useCalo = True
