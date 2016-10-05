import FWCore.ParameterSet.Config as cms

from MitProd.TreeFiller.Puppi_cff import puppi
#from CommonTools.PileupAlgos.PhotonPuppi_cff import puppiPhoton

# photon ID used for puppiPhoton
def photonIdForPuppi(process):
    # copied from PhotonPupppi_cff to run on AOD
    from PhysicsTools.SelectorUtils.tools.vid_id_tools import switchOnVIDPhotonIdProducer, DataFormat, setupVIDPhotonSelection
    from RecoEgamma.PhotonIdentification.Identification.cutBasedPhotonID_Spring15_25ns_V1_cff import cutBasedPhotonID_Spring15_25ns_V1_standalone_loose

    switchOnVIDPhotonIdProducer(process, DataFormat.AOD)
    setupVIDPhotonSelection(process, cutBasedPhotonID_Spring15_25ns_V1_standalone_loose, patProducer = None, addUserData = False)

    return process.egmPhotonIDSequence


pfNoLep = cms.EDFilter("PdgIdCandViewSelector",
    src = cms.InputTag("particleFlow"), 
    pdgId = cms.vint32(1,2,22,111,130,310,2112,211,-211,321,-321,999211,2212,-2212)
)

pfLeptons = cms.EDFilter("PdgIdCandViewSelector",
    src = cms.InputTag("particleFlow"),
    pdgId = cms.vint32(-11,11,-13,13),
)

puppiNoLep = puppi.clone(
    candName = cms.InputTag('pfNoLep')
)

puppiMerged = cms.EDProducer("CandViewMerger",
    src = cms.VInputTag('puppiNoLep', 'pfLeptons')
)

# original class adapted from CommonTools.PileupAlgos.PhotonPuppi_cff
puppiForMET = cms.EDProducer('PuppiPhotonScaler',
    candName = cms.InputTag('particleFlow'),
    puppiCandName = cms.InputTag('puppiMerged'),
    photonName = cms.InputTag('gedPhotons'),
    footprintsName = cms.InputTag('particleBasedIsolation:gedPhotons'),
    photonId = cms.InputTag('egmPhotonIDs:cutBasedPhotonID-Spring15-25ns-V1-standalone-loose'),
    defaultWeight = cms.double(1.),
    pt = cms.double(10.),
    eta = cms.double(2.5),
    # if set, match by dR instead of ref
    dRMatch = cms.vdouble(0.005, 0.005, 0.005, 0.005),
    pdgids = cms.vuint32(22, 11, 211, 130)
)

puppiSequence = cms.Sequence(
    puppi +
    pfNoLep +
    pfLeptons +
    puppiNoLep +
    puppiMerged +
    puppiForMET
)
