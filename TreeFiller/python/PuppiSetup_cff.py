import FWCore.ParameterSet.Config as cms

from CommonTools.PileupAlgos.Puppi_cff import puppi
from RecoMET.METProducers.PFMET_cfi import pfMet

pfCandNoLep = cms.EDFilter("CandPtrSelector", 
                          src = cms.InputTag("particleFlow"), 
                          cut = cms.string("abs(pdgId) != 13 && abs(pdgId) != 11 && abs(pdgId) != 15")
                          )
pfCandLep   = cms.EDFilter("CandPtrSelector", 
                          src = cms.InputTag("particleFlow"), 
                          cut = cms.string("abs(pdgId) == 13 || abs(pdgId) == 11 || abs(pdgId) == 15")
                        )
puppiNoLep = puppi.clone()
puppiNoLep.candName = 'pfCandNoLep'

puppiNoLepPlusLep = cms.EDProducer('CandViewMerger',
                                    src=cms.VInputTag('puppiNoLep','pfCandLep')
                                  )

puppiMet = pfMet.clone()
puppiMet.src = cms.InputTag('puppiNoLepPlusLep')
puppiMet.calculateSignificance = False

puppiSequence = cms.Sequence( puppi*
                              pfCandNoLep*
                              pfCandLep*
                              puppiNoLep*
                              puppiNoLepPlusLep*
                              puppiMet
                            )