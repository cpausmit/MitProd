import FWCore.ParameterSet.Config as cms

cpFiller = cms.EDAnalyzer("FillMitTree",
  fillers = cms.untracked.vstring(
    'MetaInfos'
    #'MCParticles',
    #'MCEventInfo',
    #'MCVertexes',
    #'PileupInfo',
    #'PileupEnergyDensity',
    #'AKT5GenJets'
    #'Electrons',
    #'ElectronsStable',
  ),
  
  TreeWriter = cms.untracked.PSet(
    fileName  = cms.untracked.string('XX-MITDATASET-XX'),
    maxSize   = cms.untracked.uint32(20000),
    compLevel = cms.untracked.uint32(9)
  ),

  MetaInfos = cms.untracked.PSet(
    active             = cms.untracked.bool(True),
    hltActive          = cms.untracked.bool(True),
    hltTreeMitName     = cms.untracked.string('HLT'),
    hltBitsMitName     = cms.untracked.string('HLTBits'),
    hltObjsMitName     = cms.untracked.string('HLTObjects'),
    hltResEdmName      = cms.untracked.string('TriggerResults'),
    hltEvtEdmName      = cms.untracked.string('hltTriggerSummaryAOD'),
    hltProcName        = cms.untracked.string('HLT'),
    l1Active           = cms.untracked.bool(True),
    l1GtRecordEdmName  = cms.untracked.string('l1GtRecord'),
    l1GtReadRecEdmName = cms.untracked.string('gtDigis'),
    l1TechBitsMitName  = cms.untracked.string('L1TechBits'),
    l1AlgoBitsMitName  = cms.untracked.string('L1AlgoBits'),
    fillerType         = cms.untracked.string('FillerMetaInfos')
  )
)
