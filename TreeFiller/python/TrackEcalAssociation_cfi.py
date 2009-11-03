# $Id: TrackEcalAssociation_cfi.py,v 1.1 2008/11/03 11:23:07 bendavid Exp $

import FWCore.ParameterSet.Config as cms

from TrackingTools.TrackAssociator.DetIdAssociatorESProducer_cff import *
from TrackingTools.TrackAssociator.default_cfi import *

TrackAssociatorParameterBlock.TrackAssociatorParameters.useEcal = False
TrackAssociatorParameterBlock.TrackAssociatorParameters.useHO   = False
TrackAssociatorParameterBlock.TrackAssociatorParameters.useMuon = False
TrackAssociatorParameterBlock.TrackAssociatorParameters.useHcal = False
TrackAssociatorParameterBlock.TrackAssociatorParameters.useCalo = True
TrackAssociatorParameterBlock.TrackAssociatorParameters.usePreshower = False
