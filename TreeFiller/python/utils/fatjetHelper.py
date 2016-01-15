import FWCore.ParameterSet.Config as cms

def makeBtagSequence(process,btagDiscriminators,btagInfos,label):
    setattr(process,'btagSequence'+label,cms.Sequence())
    seq = getattr(process,'btagSequence'+label)
    for info in btagInfos:
        seq += getattr(process,info+label)
    for disc in btagDiscriminators:
        seq += getattr(process,disc+label)
    return seq
