import FWCore.ParameterSet.Config as cms

# select the partons for Jet MC Flavour
myPartons = cms.EDFilter("PartonSelector",
    withLeptons = cms.bool(False)
)

#Iterative Cone
# Flavour byReference
IC5byRef = cms.EDFilter("JetPartonMatcher",
    jets = cms.InputTag("iterativeCone5CaloJets"),
    coneSizeToAssociate = cms.double(0.3),
    partons = cms.InputTag("myPartons")
)
 
# Flavour byValue PhysDef
IC5byValPhys = cms.EDFilter("JetFlavourIdentifier",
    srcByReference = cms.InputTag("IC5byRef"),
    physicsDefinition = cms.bool(True)
)
 
# Flavour byValue AlgoDef
IC5byValAlgo = cms.EDFilter("JetFlavourIdentifier",
    srcByReference = cms.InputTag("IC5byRef"),
    physicsDefinition = cms.bool(False)
)

#SisCone Jets
# Flavour byReference
SC5byRef = cms.EDFilter("JetPartonMatcher",
    jets = cms.InputTag("sisCone5CaloJets"),
    coneSizeToAssociate = cms.double(0.3),
    partons = cms.InputTag("myPartons")
)

# Flavour byValue PhysDef
SC5byValPhys = cms.EDFilter("JetFlavourIdentifier",
    srcByReference = cms.InputTag("SC5byRef"),
    physicsDefinition = cms.bool(True)
)

# Flavour byValue AlgoDef
SC5byValAlgo = cms.EDFilter("JetFlavourIdentifier",
    srcByReference = cms.InputTag("SC5byRef"),
    physicsDefinition = cms.bool(False)
)

# Flavour byReference
SC7byRef = cms.EDFilter("JetPartonMatcher",
    jets = cms.InputTag("sisCone7CaloJets"),
    coneSizeToAssociate = cms.double(0.3),
    partons = cms.InputTag("myPartons")
)

# Flavour byValue PhysDef
SC7byValPhys = cms.EDFilter("JetFlavourIdentifier",
    srcByReference = cms.InputTag("SC7byRef"),
    physicsDefinition = cms.bool(True)
)

# Flavour byValue AlgoDef
SC7byValAlgo = cms.EDFilter("JetFlavourIdentifier",
    srcByReference = cms.InputTag("SC7byRef"),
    physicsDefinition = cms.bool(False)
)

#Kt Jets
# Flavour byReference
KT4byRef = cms.EDFilter("JetPartonMatcher",
    jets = cms.InputTag("kt4CaloJets"),
    coneSizeToAssociate = cms.double(0.3),
    partons = cms.InputTag("myPartons")
)

# Flavour byValue PhysDef
KT4byValPhys = cms.EDFilter("JetFlavourIdentifier",
    srcByReference = cms.InputTag("KT4byRef"),
    physicsDefinition = cms.bool(True)
)

# Flavour byValue AlgoDef
KT4byValAlgo = cms.EDFilter("JetFlavourIdentifier",
    srcByReference = cms.InputTag("KT4byRef"),
    physicsDefinition = cms.bool(False)
)


# Flavour byReference
KT6byRef = cms.EDFilter("JetPartonMatcher",
    jets = cms.InputTag("kt6CaloJets"),
    coneSizeToAssociate = cms.double(0.3),
    partons = cms.InputTag("myPartons")
)

# Flavour byValue PhysDef
KT6byValPhys = cms.EDFilter("JetFlavourIdentifier",
    srcByReference = cms.InputTag("KT6byRef"),
    physicsDefinition = cms.bool(True)
)

# Flavour byValue AlgoDef
KT6byValAlgo = cms.EDFilter("JetFlavourIdentifier",
    srcByReference = cms.InputTag("KT6byRef"),
    physicsDefinition = cms.bool(False)
)

#Flavour by Reference
GenIC5byRef = cms.EDFilter("JetPartonMatcher",
    jets = cms.InputTag("iterativeCone5GenJets"),
    coneSizeToAssociate = cms.double(0.3),
    partons = cms.InputTag("myPartons")
)

#Flavour by Reference
GenSC5byRef = cms.EDFilter("JetPartonMatcher",
    jets = cms.InputTag("sisCone5GenJets"),
    coneSizeToAssociate = cms.double(0.3),
    partons = cms.InputTag("myPartons")
)

iterativeCone5Flavour = cms.Sequence(IC5byRef*IC5byValPhys*IC5byValAlgo)
sisCone5Flavour = cms.Sequence(SC5byRef*SC5byValPhys*SC5byValAlgo)
sisCone7Flavour = cms.Sequence(SC7byRef*SC7byValPhys*SC7byValAlgo)
KT4Flavour = cms.Sequence(KT4byRef*KT4byValPhys*KT4byValAlgo)
KT6Flavour = cms.Sequence(KT6byRef*KT6byValPhys*KT6byValAlgo)
GenJetFlavour = cms.Sequence(GenIC5byRef*GenSC5byRef)
caloJetMCFlavour = cms.Sequence(myPartons*iterativeCone5Flavour+sisCone5Flavour+sisCone7Flavour+KT4Flavour+KT6Flavour+GenJetFlavour)
