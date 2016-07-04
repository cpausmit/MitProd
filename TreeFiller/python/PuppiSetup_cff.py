import FWCore.ParameterSet.Config as cms

from MitProd.TreeFiller.Puppi_cff import puppi
#from CommonTools.PileupAlgos.PhotonPuppi_cff import puppiPhoton

# photon ID used for puppiPhoton
def photonIdForPuppi(process):
    # copied from PhotonPupppi_cff to run on AOD
    from PhysicsTools.SelectorUtils.tools.vid_id_tools import switchOnVIDPhotonIdProducer, DataFormat, setupVIDPhotonSelection
    from RecoEgamma.PhotonIdentification.Identification.cutBasedPhotonID_PHYS14_PU20bx25_V2_cff import cutBasedPhotonID_PHYS14_PU20bx25_V2_standalone_loose

    switchOnVIDPhotonIdProducer(process, DataFormat.AOD)
    setupVIDPhotonSelection(process, cutBasedPhotonID_PHYS14_PU20bx25_V2_standalone_loose, patProducer = None, addUserData = False)

    return process.egmPhotonIDSequence


puppi.useExistingWeights = False

# As of 8_0_12, puppiNoLep is identical to puppi because useWeightsNoLep is commented out in PuppiProducer.
puppiNoLep = puppi.clone(
    useWeightsNoLep = True
)

# original class adapted from CommonTools.PileupAlgos.PhotonPuppi_cff
puppiPhoton = cms.EDProducer('PuppiPhotonScaler',
    candName = cms.InputTag('particleFlow'),
    puppiCandName = cms.InputTag('puppi'),
    photonName = cms.InputTag('gedPhotons'),
    footprintsName = cms.InputTag('particleBasedIsolation:gedPhotons'),
    photonId = cms.InputTag('egmPhotonIDs:cutBasedPhotonID-PHYS14-PU20bx25-V2-standalone-loose'),
    pt = cms.double(10.),
    dRMatch = cms.vdouble(10., 10., 10., 10.),
    pdgids = cms.vuint32(22, 11, 211, 130)
)

puppiForMET = cms.EDProducer('CandViewMerger',
    src = cms.VInputTag('puppiPhoton')
)

puppiSequence = cms.Sequence(
    puppi +
#    puppiNoLep +
    puppiPhoton +
    puppiForMET
)
