# $Id: JetsMCFlavourMatching_cfi.py,v 1.8 2009/07/22 04:41:00 bendavid Exp $

import FWCore.ParameterSet.Config as cms

# select the partons for Jet MC Flavour
myPartons = cms.EDFilter("PartonSelector",
    withLeptons = cms.bool(False),
    src = cms.InputTag("genParticles")
)

#Iterative Cone
# Flavour byReference
IC5byRef = cms.EDFilter("JetPartonMatcher",
    jets                = cms.InputTag("iterativeCone5CaloJets"),
    coneSizeToAssociate = cms.double(0.3),
    partons             = cms.InputTag("myPartons")
)
 
# Flavour byValue PhysDef
IC5byValPhys = cms.EDFilter("JetFlavourIdentifier",
    srcByReference    = cms.InputTag("IC5byRef"),
    physicsDefinition = cms.bool(True)
)
 
# Flavour byValue AlgoDef
IC5byValAlgo = cms.EDFilter("JetFlavourIdentifier",
    srcByReference    = cms.InputTag("IC5byRef"),
    physicsDefinition = cms.bool(False)
)

#SisCone Jets
# Flavour byReference
SC5byRef = cms.EDFilter("JetPartonMatcher",
    jets                = cms.InputTag("sisCone5CaloJets"),
    coneSizeToAssociate = cms.double(0.3),
    partons             = cms.InputTag("myPartons")
)

# Flavour byValue PhysDef
SC5byValPhys = cms.EDFilter("JetFlavourIdentifier",
    srcByReference    = cms.InputTag("SC5byRef"),
    physicsDefinition = cms.bool(True)
)

# Flavour byValue AlgoDef
SC5byValAlgo = cms.EDFilter("JetFlavourIdentifier",
    srcByReference    = cms.InputTag("SC5byRef"),
    physicsDefinition = cms.bool(False)
)

# Flavour byReference
SC7byRef = cms.EDFilter("JetPartonMatcher",
    jets                = cms.InputTag("sisCone7CaloJets"),
    coneSizeToAssociate = cms.double(0.3),
    partons             = cms.InputTag("myPartons")
)

# Flavour byValue PhysDef
SC7byValPhys = cms.EDFilter("JetFlavourIdentifier",
    srcByReference    = cms.InputTag("SC7byRef"),
    physicsDefinition = cms.bool(True)
)

# Flavour byValue AlgoDef
SC7byValAlgo = cms.EDFilter("JetFlavourIdentifier",
    srcByReference    = cms.InputTag("SC7byRef"),
    physicsDefinition = cms.bool(False)
)

#Kt Jets
# Flavour byReference
KT4byRef = cms.EDFilter("JetPartonMatcher",
    jets                = cms.InputTag("kt4CaloJets"),
    coneSizeToAssociate = cms.double(0.3),
    partons             = cms.InputTag("myPartons")
)

# Flavour byValue PhysDef
KT4byValPhys = cms.EDFilter("JetFlavourIdentifier",
    srcByReference    = cms.InputTag("KT4byRef"),
    physicsDefinition = cms.bool(True)
)

# Flavour byValue AlgoDef
KT4byValAlgo = cms.EDFilter("JetFlavourIdentifier",
    srcByReference    = cms.InputTag("KT4byRef"),
    physicsDefinition = cms.bool(False)
)


# Flavour byReference
KT6byRef = cms.EDFilter("JetPartonMatcher",
    jets                = cms.InputTag("kt6CaloJets"),
    coneSizeToAssociate = cms.double(0.3),
    partons             = cms.InputTag("myPartons")
)

# Flavour byValue PhysDef
KT6byValPhys = cms.EDFilter("JetFlavourIdentifier",
    srcByReference    = cms.InputTag("KT6byRef"),
    physicsDefinition = cms.bool(True)
)

# Flavour byValue AlgoDef
KT6byValAlgo = cms.EDFilter("JetFlavourIdentifier",
    srcByReference    = cms.InputTag("KT6byRef"),
    physicsDefinition = cms.bool(False)
)

#AntiKt Jets
# Flavour byReference
AKT5byRef = cms.EDFilter("JetPartonMatcher",
    jets                = cms.InputTag("ak5CaloJets"),
    coneSizeToAssociate = cms.double(0.3),
    partons             = cms.InputTag("myPartons")
)

# Flavour byValue PhysDef
AKT5byValPhys = cms.EDFilter("JetFlavourIdentifier",
    srcByReference    = cms.InputTag("AKT5byRef"),
    physicsDefinition = cms.bool(True)
)

# Flavour byValue AlgoDef
AKT5byValAlgo = cms.EDFilter("JetFlavourIdentifier",
    srcByReference    = cms.InputTag("AKT5byRef"),
    physicsDefinition = cms.bool(False)
)

# Flavour byReference
AKT7byRef = cms.EDFilter("JetPartonMatcher",
    jets                = cms.InputTag("ak7CaloJets"),
    coneSizeToAssociate = cms.double(0.3),
    partons             = cms.InputTag("myPartons")
)

# Flavour byValue PhysDef
AKT7byValPhys = cms.EDFilter("JetFlavourIdentifier",
    srcByReference    = cms.InputTag("AKT7byRef"),
    physicsDefinition = cms.bool(True)
)

# Flavour byValue AlgoDef
AKT7byValAlgo = cms.EDFilter("JetFlavourIdentifier",
    srcByReference    = cms.InputTag("AKT7byRef"),
    physicsDefinition = cms.bool(False)
)

#IC5PF Jets
# Flavour byReference
IC5PFbyRef = cms.EDFilter("JetPartonMatcher",
    jets                = cms.InputTag("iterativeCone5PFJets"),
    coneSizeToAssociate = cms.double(0.3),
    partons             = cms.InputTag("myPartons")
)
 
# Flavour byValue PhysDef
IC5PFbyValPhys = cms.EDFilter("JetFlavourIdentifier",
    srcByReference    = cms.InputTag("IC5PFbyRef"),
    physicsDefinition = cms.bool(True)
)
 
# Flavour byValue AlgoDef
IC5PFbyValAlgo = cms.EDFilter("JetFlavourIdentifier",
    srcByReference    = cms.InputTag("IC5PFbyRef"),
    physicsDefinition = cms.bool(False)
)

#SisCone Jets
# Flavour byReference
SC5PFbyRef = cms.EDFilter("JetPartonMatcher",
    jets                = cms.InputTag("sisCone5PFJets"),
    coneSizeToAssociate = cms.double(0.3),
    partons             = cms.InputTag("myPartons")
)

# Flavour byValue PhysDef
SC5PFbyValPhys = cms.EDFilter("JetFlavourIdentifier",
    srcByReference    = cms.InputTag("SC5PFbyRef"),
    physicsDefinition = cms.bool(True)
)

# Flavour byValue AlgoDef
SC5PFbyValAlgo = cms.EDFilter("JetFlavourIdentifier",
    srcByReference    = cms.InputTag("SC5PFbyRef"),
    physicsDefinition = cms.bool(False)
)

# Flavour byReference
SC7PFbyRef = cms.EDFilter("JetPartonMatcher",
    jets                = cms.InputTag("sisCone7PFJets"),
    coneSizeToAssociate = cms.double(0.3),
    partons             = cms.InputTag("myPartons")
)

# Flavour byValue PhysDef
SC7PFbyValPhys = cms.EDFilter("JetFlavourIdentifier",
    srcByReference    = cms.InputTag("SC7PFbyRef"),
    physicsDefinition = cms.bool(True)
)

# Flavour byValue AlgoDef
SC7PFbyValAlgo = cms.EDFilter("JetFlavourIdentifier",
    srcByReference    = cms.InputTag("SC7PFbyRef"),
    physicsDefinition = cms.bool(False)
)

#Kt Jets
# Flavour byReference
KT4PFbyRef = cms.EDFilter("JetPartonMatcher",
    jets                = cms.InputTag("kt4PFJets"),
    coneSizeToAssociate = cms.double(0.3),
    partons             = cms.InputTag("myPartons")
)

# Flavour byValue PhysDef
KT4PFbyValPhys = cms.EDFilter("JetFlavourIdentifier",
    srcByReference    = cms.InputTag("KT4PFbyRef"),
    physicsDefinition = cms.bool(True)
)

# Flavour byValue AlgoDef
KT4PFbyValAlgo = cms.EDFilter("JetFlavourIdentifier",
    srcByReference    = cms.InputTag("KT4PFbyRef"),
    physicsDefinition = cms.bool(False)
)


# Flavour byReference
KT6PFbyRef = cms.EDFilter("JetPartonMatcher",
    jets                = cms.InputTag("kt6PFJets"),
    coneSizeToAssociate = cms.double(0.3),
    partons             = cms.InputTag("myPartons")
)

# Flavour byValue PhysDef
KT6PFbyValPhys = cms.EDFilter("JetFlavourIdentifier",
    srcByReference    = cms.InputTag("KT6PFbyRef"),
    physicsDefinition = cms.bool(True)
)

# Flavour byValue AlgoDef
KT6PFbyValAlgo = cms.EDFilter("JetFlavourIdentifier",
    srcByReference    = cms.InputTag("KT6PFbyRef"),
    physicsDefinition = cms.bool(False)
)

#AntiKt Jets
# Flavour byReference
AKT5PFbyRef = cms.EDFilter("JetPartonMatcher",
    jets                = cms.InputTag("ak5PFJets"),
    coneSizeToAssociate = cms.double(0.3),
    partons             = cms.InputTag("myPartons")
)

# Flavour byValue PhysDef
AKT5PFbyValPhys = cms.EDFilter("JetFlavourIdentifier",
    srcByReference    = cms.InputTag("AKT5PFbyRef"),
    physicsDefinition = cms.bool(True)
)

# Flavour byValue AlgoDef
AKT5PFbyValAlgo = cms.EDFilter("JetFlavourIdentifier",
    srcByReference    = cms.InputTag("AKT5PFbyRef"),
    physicsDefinition = cms.bool(False)
)

# Flavour byReference
AKT7PFbyRef = cms.EDFilter("JetPartonMatcher",
    jets                = cms.InputTag("ak7PFJets"),
    coneSizeToAssociate = cms.double(0.3),
    partons             = cms.InputTag("myPartons")
)

# Flavour byValue PhysDef
AKT7PFbyValPhys = cms.EDFilter("JetFlavourIdentifier",
    srcByReference    = cms.InputTag("AKT7PFbyRef"),
    physicsDefinition = cms.bool(True)
)

# Flavour byValue AlgoDef
AKT7PFbyValAlgo = cms.EDFilter("JetFlavourIdentifier",
    srcByReference    = cms.InputTag("AKT7PFbyRef"),
    physicsDefinition = cms.bool(False)
)

#Flavour by Reference
GenIC5byRef = cms.EDFilter("JetPartonMatcher",
    jets                = cms.InputTag("iterativeCone5GenJets"),
    coneSizeToAssociate = cms.double(0.3),
    partons             = cms.InputTag("myPartons")
)

#Flavour by Reference
GenSC5byRef = cms.EDFilter("JetPartonMatcher",
    jets                = cms.InputTag("sisCone5GenJets"),
    coneSizeToAssociate = cms.double(0.3),
    partons             = cms.InputTag("myPartons")
)

#Flavour by Reference
GenSC7byRef = cms.EDFilter("JetPartonMatcher",
    jets                = cms.InputTag("sisCone7GenJets"),
    coneSizeToAssociate = cms.double(0.3),
    partons             = cms.InputTag("myPartons")
)

#Flavour by Reference
GenKT4byRef = cms.EDFilter("JetPartonMatcher",
    jets                = cms.InputTag("kt4GenJets"),
    coneSizeToAssociate = cms.double(0.3),
    partons             = cms.InputTag("myPartons")
)

#Flavour by Reference
GenKT6byRef = cms.EDFilter("JetPartonMatcher",
    jets                = cms.InputTag("kt6GenJets"),
    coneSizeToAssociate = cms.double(0.3),
    partons             = cms.InputTag("myPartons")
)

#Flavour by Reference
GenAKT5byRef = cms.EDFilter("JetPartonMatcher",
    jets                = cms.InputTag("ak5GenJets"),
    coneSizeToAssociate = cms.double(0.3),
    partons             = cms.InputTag("myPartons")
)

#Flavour by Reference
GenAKT7byRef = cms.EDFilter("JetPartonMatcher",
    jets                = cms.InputTag("ak7GenJets"),
    coneSizeToAssociate = cms.double(0.3),
    partons             = cms.InputTag("myPartons")
)

iterativeCone5Flavour = cms.Sequence(IC5byRef*IC5byValPhys*IC5byValAlgo)
sisCone5Flavour = cms.Sequence(SC5byRef*SC5byValPhys*SC5byValAlgo)
sisCone7Flavour = cms.Sequence(SC7byRef*SC7byValPhys*SC7byValAlgo)
KT4Flavour = cms.Sequence(KT4byRef*KT4byValPhys*KT4byValAlgo)
KT6Flavour = cms.Sequence(KT6byRef*KT6byValPhys*KT6byValAlgo)
AKT5Flavour = cms.Sequence(AKT5byRef*AKT5byValPhys*AKT5byValAlgo)
AKT7Flavour = cms.Sequence(AKT7byRef*AKT7byValPhys*AKT7byValAlgo)
iterativeCone5PFFlavour = cms.Sequence(IC5PFbyRef*IC5PFbyValPhys*IC5PFbyValAlgo)
sisCone5PFFlavour = cms.Sequence(SC5PFbyRef*SC5PFbyValPhys*SC5PFbyValAlgo)
sisCone7PFFlavour = cms.Sequence(SC7PFbyRef*SC7PFbyValPhys*SC7PFbyValAlgo)
KT4PFFlavour = cms.Sequence(KT4PFbyRef*KT4PFbyValPhys*KT4PFbyValAlgo)
KT6PFFlavour = cms.Sequence(KT6PFbyRef*KT6PFbyValPhys*KT6PFbyValAlgo)
AKT5PFFlavour = cms.Sequence(AKT5PFbyRef*AKT5PFbyValPhys*AKT5PFbyValAlgo)
AKT7PFFlavour = cms.Sequence(AKT7PFbyRef*AKT7PFbyValPhys*AKT7PFbyValAlgo)
GenJetFlavour = cms.Sequence(GenIC5byRef*GenSC5byRef*GenSC7byRef*GenKT4byRef*GenKT6byRef*GenAKT5byRef*GenAKT7byRef)
caloJetMCFlavour = cms.Sequence(myPartons*iterativeCone5Flavour
                                +sisCone5Flavour
                                +sisCone7Flavour
                                +KT4Flavour
                                +KT6Flavour
                                +AKT5Flavour
                                +AKT7Flavour
                                +GenJetFlavour)

pfJetMCFlavour = cms.Sequence(iterativeCone5PFFlavour+sisCone5PFFlavour+sisCone7PFFlavour+KT4PFFlavour+KT6PFFlavour+AKT5PFFlavour+AKT7PFFlavour)

