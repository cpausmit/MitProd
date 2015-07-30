import FWCore.ParameterSet.Config as cms

def makeBtagSequence(process,btagDiscriminators,btagInfos,label):
    process.btagSequence = cms.Sequence()
    for info in btagInfos:
        process.btagSequence += getattr(process,info+label)
    for disc in btagDiscriminators:
        process.btagSequence += getattr(process,disc+label)
    return process.btagSequence
