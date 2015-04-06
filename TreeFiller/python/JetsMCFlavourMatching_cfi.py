# $Id: JetsMCFlavourMatching_cfi.py,v 1.12 2010/06/23 15:01:17 bendavid Exp $

import FWCore.ParameterSet.Config as cms

# select the partons for Jet MC Flavour
myPartons = cms.EDProducer("PartonSelector",
    withLeptons = cms.bool(False),
    src = cms.InputTag("genParticles")
)

#Iterative Cone
# Flavour byReference
IC5byRef = cms.EDProducer("JetPartonMatcher",
    jets                = cms.InputTag("iterativeCone5CaloJets"),
    coneSizeToAssociate = cms.double(0.3),
    partons             = cms.InputTag("myPartons")
)
 
# Flavour byValue PhysDef
IC5byValPhys = cms.EDProducer("JetFlavourIdentifier",
    srcByReference    = cms.InputTag("IC5byRef"),
    physicsDefinition = cms.bool(True)
)
 
# Flavour byValue AlgoDef
IC5byValAlgo = cms.EDProducer("JetFlavourIdentifier",
    srcByReference    = cms.InputTag("IC5byRef"),
    physicsDefinition = cms.bool(False)
)

#SisCone Jets
# Flavour byReference
SC5byRef = cms.EDProducer("JetPartonMatcher",
    jets                = cms.InputTag("sisCone5CaloJets"),
    coneSizeToAssociate = cms.double(0.3),
    partons             = cms.InputTag("myPartons")
)

# Flavour byValue PhysDef
SC5byValPhys = cms.EDProducer("JetFlavourIdentifier",
    srcByReference    = cms.InputTag("SC5byRef"),
    physicsDefinition = cms.bool(True)
)

# Flavour byValue AlgoDef
SC5byValAlgo = cms.EDProducer("JetFlavourIdentifier",
    srcByReference    = cms.InputTag("SC5byRef"),
    physicsDefinition = cms.bool(False)
)

# Flavour byReference
SC7byRef = cms.EDProducer("JetPartonMatcher",
    jets                = cms.InputTag("sisCone7CaloJets"),
    coneSizeToAssociate = cms.double(0.3),
    partons             = cms.InputTag("myPartons")
)

# Flavour byValue PhysDef
SC7byValPhys = cms.EDProducer("JetFlavourIdentifier",
    srcByReference    = cms.InputTag("SC7byRef"),
    physicsDefinition = cms.bool(True)
)

# Flavour byValue AlgoDef
SC7byValAlgo = cms.EDProducer("JetFlavourIdentifier",
    srcByReference    = cms.InputTag("SC7byRef"),
    physicsDefinition = cms.bool(False)
)

#Kt Jets
# Flavour byReference
KT4byRef = cms.EDProducer("JetPartonMatcher",
    jets                = cms.InputTag("kt4CaloJets"),
    coneSizeToAssociate = cms.double(0.3),
    partons             = cms.InputTag("myPartons")
)

# Flavour byValue PhysDef
KT4byValPhys = cms.EDProducer("JetFlavourIdentifier",
    srcByReference    = cms.InputTag("KT4byRef"),
    physicsDefinition = cms.bool(True)
)

# Flavour byValue AlgoDef
KT4byValAlgo = cms.EDProducer("JetFlavourIdentifier",
    srcByReference    = cms.InputTag("KT4byRef"),
    physicsDefinition = cms.bool(False)
)


# Flavour byReference
KT6byRef = cms.EDProducer("JetPartonMatcher",
    jets                = cms.InputTag("kt6CaloJets"),
    coneSizeToAssociate = cms.double(0.3),
    partons             = cms.InputTag("myPartons")
)

# Flavour byValue PhysDef
KT6byValPhys = cms.EDProducer("JetFlavourIdentifier",
    srcByReference    = cms.InputTag("KT6byRef"),
    physicsDefinition = cms.bool(True)
)

# Flavour byValue AlgoDef
KT6byValAlgo = cms.EDProducer("JetFlavourIdentifier",
    srcByReference    = cms.InputTag("KT6byRef"),
    physicsDefinition = cms.bool(False)
)

#AntiKt Jets
# Flavour byReference
AKT4byRef = cms.EDProducer("JetPartonMatcher",
    jets                = cms.InputTag("ak4CaloJets"),
    coneSizeToAssociate = cms.double(0.3),
    partons             = cms.InputTag("myPartons")
)

# Flavour byValue PhysDef
AKT4byValPhys = cms.EDProducer("JetFlavourIdentifier",
    srcByReference    = cms.InputTag("AKT4byRef"),
    physicsDefinition = cms.bool(True)
)

# Flavour byValue AlgoDef
AKT4byValAlgo = cms.EDProducer("JetFlavourIdentifier",
    srcByReference    = cms.InputTag("AKT4byRef"),
    physicsDefinition = cms.bool(False)
)

# Flavour byReference
AKT7byRef = cms.EDProducer("JetPartonMatcher",
    jets                = cms.InputTag("ak7CaloJets"),
    coneSizeToAssociate = cms.double(0.3),
    partons             = cms.InputTag("myPartons")
)

# Flavour byValue PhysDef
AKT7byValPhys = cms.EDProducer("JetFlavourIdentifier",
    srcByReference    = cms.InputTag("AKT7byRef"),
    physicsDefinition = cms.bool(True)
)

# Flavour byValue AlgoDef
AKT7byValAlgo = cms.EDProducer("JetFlavourIdentifier",
    srcByReference    = cms.InputTag("AKT7byRef"),
    physicsDefinition = cms.bool(False)
)

#IC5PF Jets
# Flavour byReference
IC5PFbyRef = cms.EDProducer("JetPartonMatcher",
    jets                = cms.InputTag("iterativeCone5PFJets"),
    coneSizeToAssociate = cms.double(0.3),
    partons             = cms.InputTag("myPartons")
)
 
# Flavour byValue PhysDef
IC5PFbyValPhys = cms.EDProducer("JetFlavourIdentifier",
    srcByReference    = cms.InputTag("IC5PFbyRef"),
    physicsDefinition = cms.bool(True)
)
 
# Flavour byValue AlgoDef
IC5PFbyValAlgo = cms.EDProducer("JetFlavourIdentifier",
    srcByReference    = cms.InputTag("IC5PFbyRef"),
    physicsDefinition = cms.bool(False)
)

#SisCone Jets
# Flavour byReference
SC5PFbyRef = cms.EDProducer("JetPartonMatcher",
    jets                = cms.InputTag("sisCone5PFJets"),
    coneSizeToAssociate = cms.double(0.3),
    partons             = cms.InputTag("myPartons")
)

# Flavour byValue PhysDef
SC5PFbyValPhys = cms.EDProducer("JetFlavourIdentifier",
    srcByReference    = cms.InputTag("SC5PFbyRef"),
    physicsDefinition = cms.bool(True)
)

# Flavour byValue AlgoDef
SC5PFbyValAlgo = cms.EDProducer("JetFlavourIdentifier",
    srcByReference    = cms.InputTag("SC5PFbyRef"),
    physicsDefinition = cms.bool(False)
)

# Flavour byReference
SC7PFbyRef = cms.EDProducer("JetPartonMatcher",
    jets                = cms.InputTag("sisCone7PFJets"),
    coneSizeToAssociate = cms.double(0.3),
    partons             = cms.InputTag("myPartons")
)

# Flavour byValue PhysDef
SC7PFbyValPhys = cms.EDProducer("JetFlavourIdentifier",
    srcByReference    = cms.InputTag("SC7PFbyRef"),
    physicsDefinition = cms.bool(True)
)

# Flavour byValue AlgoDef
SC7PFbyValAlgo = cms.EDProducer("JetFlavourIdentifier",
    srcByReference    = cms.InputTag("SC7PFbyRef"),
    physicsDefinition = cms.bool(False)
)

#Kt Jets
# Flavour byReference
KT4PFbyRef = cms.EDProducer("JetPartonMatcher",
    jets                = cms.InputTag("kt4PFJets"),
    coneSizeToAssociate = cms.double(0.3),
    partons             = cms.InputTag("myPartons")
)

# Flavour byValue PhysDef
KT4PFbyValPhys = cms.EDProducer("JetFlavourIdentifier",
    srcByReference    = cms.InputTag("KT4PFbyRef"),
    physicsDefinition = cms.bool(True)
)

# Flavour byValue AlgoDef
KT4PFbyValAlgo = cms.EDProducer("JetFlavourIdentifier",
    srcByReference    = cms.InputTag("KT4PFbyRef"),
    physicsDefinition = cms.bool(False)
)


# Flavour byReference
KT6PFbyRef = cms.EDProducer("JetPartonMatcher",
    jets                = cms.InputTag("kt6PFJets"),
    coneSizeToAssociate = cms.double(0.3),
    partons             = cms.InputTag("myPartons")
)

# Flavour byValue PhysDef
KT6PFbyValPhys = cms.EDProducer("JetFlavourIdentifier",
    srcByReference    = cms.InputTag("KT6PFbyRef"),
    physicsDefinition = cms.bool(True)
)

# Flavour byValue AlgoDef
KT6PFbyValAlgo = cms.EDProducer("JetFlavourIdentifier",
    srcByReference    = cms.InputTag("KT6PFbyRef"),
    physicsDefinition = cms.bool(False)
)

#AntiKt Jets
# Flavour byReference
AKT4PFbyRef = cms.EDProducer("JetPartonMatcher",
    jets                = cms.InputTag("ak4PFJets"),
    coneSizeToAssociate = cms.double(0.3),
    partons             = cms.InputTag("myPartons")
)

# Flavour byValue PhysDef
AKT4PFbyValPhys = cms.EDProducer("JetFlavourIdentifier",
    srcByReference    = cms.InputTag("AKT4PFbyRef"),
    physicsDefinition = cms.bool(True)
)

# Flavour byValue AlgoDef
AKT4PFbyValAlgo = cms.EDProducer("JetFlavourIdentifier",
    srcByReference    = cms.InputTag("AKT4PFbyRef"),
    physicsDefinition = cms.bool(False)
)

# Flavour byReference
AKT8PFbyRef = cms.EDProducer("JetPartonMatcher",
    jets                = cms.InputTag("ak8PFJetsCHS"),
    coneSizeToAssociate = cms.double(0.3),
    partons             = cms.InputTag("myPartons")
)

# Flavour byValue PhysDef
AKT8PFbyValPhys = cms.EDProducer("JetFlavourIdentifier",
    srcByReference    = cms.InputTag("AKT8PFbyRef"),
    physicsDefinition = cms.bool(True)
)

# Flavour byValue AlgoDef
AKT8PFbyValAlgo = cms.EDProducer("JetFlavourIdentifier",
    srcByReference    = cms.InputTag("AKT8PFbyRef"),
    physicsDefinition = cms.bool(False)
)

#Flavour by Reference
GenIC5byRef = cms.EDProducer("JetPartonMatcher",
    jets                = cms.InputTag("iterativeCone5GenJets"),
    coneSizeToAssociate = cms.double(0.3),
    partons             = cms.InputTag("myPartons")
)

#Flavour by Reference
GenSC5byRef = cms.EDProducer("JetPartonMatcher",
    jets                = cms.InputTag("sisCone5GenJets"),
    coneSizeToAssociate = cms.double(0.3),
    partons             = cms.InputTag("myPartons")
)

#Flavour by Reference
GenSC7byRef = cms.EDProducer("JetPartonMatcher",
    jets                = cms.InputTag("sisCone7GenJets"),
    coneSizeToAssociate = cms.double(0.3),
    partons             = cms.InputTag("myPartons")
)

#Flavour by Reference
GenKT4byRef = cms.EDProducer("JetPartonMatcher",
    jets                = cms.InputTag("kt4GenJets"),
    coneSizeToAssociate = cms.double(0.3),
    partons             = cms.InputTag("myPartons")
)

#Flavour by Reference
GenKT6byRef = cms.EDProducer("JetPartonMatcher",
    jets                = cms.InputTag("kt6GenJets"),
    coneSizeToAssociate = cms.double(0.3),
    partons             = cms.InputTag("myPartons")
)

#Flavour by Reference
GenAKT4byRef = cms.EDProducer("JetPartonMatcher",
    jets                = cms.InputTag("ak4GenJets"),
    coneSizeToAssociate = cms.double(0.3),
    partons             = cms.InputTag("myPartons")
)

#Flavour by Reference
GenAKT8byRef = cms.EDProducer("JetPartonMatcher",
    jets                = cms.InputTag("ak8GenJets"),
    coneSizeToAssociate = cms.double(0.3),
    partons             = cms.InputTag("myPartons")
)

iterativeCone5Flavour = cms.Sequence(IC5byRef*IC5byValPhys*IC5byValAlgo)
sisCone5Flavour = cms.Sequence(SC5byRef*SC5byValPhys*SC5byValAlgo)
sisCone7Flavour = cms.Sequence(SC7byRef*SC7byValPhys*SC7byValAlgo)
KT4Flavour = cms.Sequence(KT4byRef*KT4byValPhys*KT4byValAlgo)
KT6Flavour = cms.Sequence(KT6byRef*KT6byValPhys*KT6byValAlgo)
AKT4Flavour = cms.Sequence(AKT4byRef*AKT4byValPhys*AKT4byValAlgo)
AKT7Flavour = cms.Sequence(AKT7byRef*AKT7byValPhys*AKT7byValAlgo)
iterativeCone5PFFlavour = cms.Sequence(IC5PFbyRef*IC5PFbyValPhys*IC5PFbyValAlgo)
sisCone5PFFlavour = cms.Sequence(SC5PFbyRef*SC5PFbyValPhys*SC5PFbyValAlgo)
sisCone7PFFlavour = cms.Sequence(SC7PFbyRef*SC7PFbyValPhys*SC7PFbyValAlgo)
KT4PFFlavour = cms.Sequence(KT4PFbyRef*KT4PFbyValPhys*KT4PFbyValAlgo)
KT6PFFlavour = cms.Sequence(KT6PFbyRef*KT6PFbyValPhys*KT6PFbyValAlgo)
AKT4PFFlavour = cms.Sequence(AKT4PFbyRef*AKT4PFbyValPhys*AKT4PFbyValAlgo)
AKT8PFFlavour = cms.Sequence(AKT8PFbyRef*AKT8PFbyValPhys*AKT8PFbyValAlgo)

GenJetFlavour = cms.Sequence(GenKT4byRef*GenKT6byRef*GenAKT4byRef*GenAKT8byRef)

caloJetMCFlavour = cms.Sequence(myPartons
                                +KT4Flavour
                                +KT6Flavour
                                +AKT4Flavour
                                +AKT7Flavour
                                +GenJetFlavour)

pfJetMCFlavour = cms.Sequence(KT4PFFlavour+KT6PFFlavour+AKT4PFFlavour+AKT8PFFlavour)

