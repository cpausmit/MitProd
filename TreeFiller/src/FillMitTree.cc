// $Id: FillMitTree.cc,v 1.29 2009/02/23 20:53:35 bendavid Exp $

#include "MitProd/TreeFiller/interface/FillMitTree.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "MitProd/TreeService/interface/TreeService.h"
#include "MitProd/ObjectService/interface/ObjectService.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"
#include "MitProd/TreeFiller/interface/FillerMetaInfos.h"
#include "MitProd/TreeFiller/interface/FillerVertexes.h"
#include "MitProd/TreeFiller/interface/FillerBeamSpot.h"
#include "MitProd/TreeFiller/interface/FillerTracks.h"
#include "MitProd/TreeFiller/interface/FillerBasicClusters.h"
#include "MitProd/TreeFiller/interface/FillerSuperClusters.h"
#include "MitProd/TreeFiller/interface/FillerCaloTowers.h"
#include "MitProd/TreeFiller/interface/FillerMuons.h"
#include "MitProd/TreeFiller/interface/FillerElectrons.h"
#include "MitProd/TreeFiller/interface/FillerGenJets.h"
#include "MitProd/TreeFiller/interface/FillerCaloJets.h"
#include "MitProd/TreeFiller/interface/FillerCaloMet.h"
#include "MitProd/TreeFiller/interface/FillerConversions.h"
#include "MitProd/TreeFiller/interface/FillerConversionElectrons.h"
#include "MitProd/TreeFiller/interface/FillerPhotons.h"
#include "MitProd/TreeFiller/interface/FillerMCParticles.h"
#include "MitProd/TreeFiller/interface/FillerAODMCParticles.h"
#include "MitProd/TreeFiller/interface/FillerDecayParts.h"
#include "MitProd/TreeFiller/interface/FillerStableParts.h"
#include "MitProd/TreeFiller/interface/FillerPATMuons.h"
#include "MitProd/TreeFiller/interface/FillerPATElectrons.h"

using namespace std;
using namespace edm;
using namespace mithep;

mithep::ObjectService *mithep::FillMitTree::os_ = 0;

//--------------------------------------------------------------------------------------------------
FillMitTree::FillMitTree(const edm::ParameterSet &cfg) :
  defactive_(cfg.getUntrackedParameter<bool>("defactive",1))
{
  // Constructor.

  if (!configure(cfg)) {
    throw edm::Exception(edm::errors::Configuration, "FillMitTree::FillMitTree()\n")
      << "Could not configure fillers." << "\n";
  }
}

//--------------------------------------------------------------------------------------------------
FillMitTree::~FillMitTree()
{
  // Destructor: nothing to be done here.
}

//--------------------------------------------------------------------------------------------------
bool FillMitTree::addActiveFiller(BaseFiller *bf)
{
  // Check if filler is active and add it to list of fillers. Otherwise delete it.

  if (bf->Active()) {
    fillers_.push_back(bf);
    return 1;
  }

  delete bf;
  bf = 0;
  return 0;
}

//--------------------------------------------------------------------------------------------------
void FillMitTree::analyze(const edm::Event      &event, 
			  const edm::EventSetup &setup)
{
  // Access and copy event content.

  // First step: Loop over the data fillers of the various components
  for (std::vector<BaseFiller*>::const_iterator iF = fillers_.begin(); iF != fillers_.end(); ++iF) {
    (*iF)->FillDataBlock(event,setup);
  }

  // Second step: Loop over the link resolution of the various components
  for (std::vector<BaseFiller*>::const_iterator iF = fillers_.begin(); iF != fillers_.end(); ++iF) {
    (*iF)->ResolveLinks(event,setup);
  }
}

//--------------------------------------------------------------------------------------------------
void FillMitTree::beginJob(const edm::EventSetup &event)
{
  // Access the tree and book branches.

  Service<TreeService> ts;
  TreeWriter *tws = ts->get();
  if (! tws) {
    throw edm::Exception(edm::errors::Configuration, "FillMitTree::beginJob()\n")
      << "Could not get pointer to tree. " 
      << "Do you have the TreeServie define in your config?" << "\n";
    return;
  }

  if (os_==0) { 
    Service<ObjectService> os;
    if (!os.isAvailable()) {
      throw edm::Exception(edm::errors::Configuration, "FillMitTree::beginJob()\n")
        << "Could not get object service. " 
        << "Do you have the ObjectService defined in your config?" << "\n";
      return;
    }
    os_ = &(*os);
  }

  // Loop over the various components and book the branches
  for (std::vector<BaseFiller*>::iterator iF = fillers_.begin(); iF != fillers_.end(); ++iF) {
    edm::LogInfo("FillMitTree::beginJob") << "Booking for " << (*iF)->Name() << endl;
    (*iF)->BookDataBlock(*tws);
  }
}

//--------------------------------------------------------------------------------------------------
bool FillMitTree::configure(const edm::ParameterSet &cfg)
{
  // Configure our fillers.

  FillerMetaInfos *fillerMetaInfos = new FillerMetaInfos(cfg,defactive_);
  addActiveFiller(fillerMetaInfos);

  FillerMCParticles *fillerMCParticles = new FillerMCParticles(cfg,"MCParticles",defactive_);
  addActiveFiller(fillerMCParticles);
  
  FillerAODMCParticles *fillerAODMCParticles = new FillerAODMCParticles(cfg,"AODMCParticles",false);
  addActiveFiller(fillerAODMCParticles);

  FillerBeamSpot *fillerBeamSpot = 
    new FillerBeamSpot(cfg,"BeamSpot", defactive_);
  addActiveFiller(fillerBeamSpot);
  
  FillerVertexes *fillerPrimaryVertexes = 
    new FillerVertexes(cfg,"PrimaryVertexes", defactive_);
  addActiveFiller(fillerPrimaryVertexes);
  
  //primary vertexes with beamspot constraint
  FillerVertexes *fillerPrimaryVertexesBS = 
    new FillerVertexes(cfg,"PrimaryVertexesBS", defactive_);
  addActiveFiller(fillerPrimaryVertexesBS);  
  
  FillerCaloTowers *fillerCaloTowers = 
    new FillerCaloTowers(cfg, "CaloTowers", defactive_);
  addActiveFiller(fillerCaloTowers);

  FillerGenJets *fillerIC5GenJets = new FillerGenJets(cfg,"IC5GenJets",defactive_);
  addActiveFiller(fillerIC5GenJets);

  FillerGenJets *fillerSC5GenJets = new FillerGenJets(cfg,"SC5GenJets",defactive_);
  addActiveFiller(fillerSC5GenJets);
  
  FillerGenJets *fillerSC7GenJets = new FillerGenJets(cfg,"SC7GenJets",defactive_);
  addActiveFiller(fillerSC7GenJets);

  FillerGenJets *fillerKT4GenJets = new FillerGenJets(cfg,"KT4GenJets",defactive_);
  addActiveFiller(fillerKT4GenJets);
  
  FillerGenJets *fillerKT6GenJets = new FillerGenJets(cfg,"KT6GenJets",defactive_);
  addActiveFiller(fillerKT6GenJets);

  FillerCaloJets *fillerItrCone5Jets = new FillerCaloJets(cfg,"ItrCone5Jets",defactive_);
  addActiveFiller(fillerItrCone5Jets);

  FillerCaloJets *fillerSisCone5Jets = new FillerCaloJets(cfg,"SisCone5Jets",defactive_);
  addActiveFiller(fillerSisCone5Jets);

  FillerCaloJets *fillerSisCone7Jets = new FillerCaloJets(cfg,"SisCone7Jets",defactive_);
  addActiveFiller(fillerSisCone7Jets);

  FillerCaloJets *fillerKt4Jets = new FillerCaloJets(cfg,"Kt4Jets",defactive_);
  addActiveFiller(fillerKt4Jets);     
  
  FillerCaloJets *fillerKt6Jets = new FillerCaloJets(cfg,"Kt6Jets",defactive_);
  addActiveFiller(fillerKt6Jets);

  FillerCaloJets *fillerIC5JetPlusTrack = new FillerCaloJets(cfg,"IC5JetPlusTrack",defactive_);
  addActiveFiller(fillerIC5JetPlusTrack);

  FillerCaloMet *fillerItrCone5Met = new FillerCaloMet(cfg,"ItrCone5Met",defactive_);
  addActiveFiller(fillerItrCone5Met);
  
  FillerCaloMet *fillerSisCone5Met = new FillerCaloMet(cfg,"SisCone5Met",defactive_);
  addActiveFiller(fillerSisCone5Met);
  
  FillerCaloMet *fillerSisCone7Met = new FillerCaloMet(cfg,"SisCone7Met",defactive_);
  addActiveFiller(fillerSisCone7Met);

  FillerCaloMet *fillerKt4Met = new FillerCaloMet(cfg,"Kt4Met",defactive_);
  addActiveFiller(fillerKt4Met);
  
  FillerCaloMet *fillerKt6Met = new FillerCaloMet(cfg,"Kt6Met",defactive_);
  addActiveFiller(fillerKt6Met);  

  FillerBasicClusters *fillerBarrelBasicClusters = 
    new FillerBasicClusters(cfg, "BarrelBasicClusters", defactive_);
  addActiveFiller(fillerBarrelBasicClusters);

  FillerSuperClusters *fillerBarrelSuperClusters =  
    new FillerSuperClusters(cfg,"BarrelSuperClusters", defactive_);
  addActiveFiller(fillerBarrelSuperClusters);

  FillerBasicClusters *fillerEndcapBasicClusters = 
    new FillerBasicClusters(cfg,"EndcapBasicClusters", defactive_);
  addActiveFiller(fillerEndcapBasicClusters);
  
  FillerSuperClusters *fillerEndcapSuperClusters =  
    new FillerSuperClusters(cfg,"EndcapSuperClusters", defactive_);
  addActiveFiller(fillerEndcapSuperClusters);
  
  FillerTracks *fillerGeneralTracks = new FillerTracks(cfg,"GeneralTracks",defactive_);
  addActiveFiller(fillerGeneralTracks);

  FillerTracks *fillerStandaloneMuonTracks = 
    new FillerTracks(cfg,"StandaloneMuonTracks",defactive_);
  addActiveFiller(fillerStandaloneMuonTracks);

  FillerTracks *fillerStandaloneMuonTracksVtx = 
    new FillerTracks(cfg,"StandaloneMuonTracksWVtxConstraint",defactive_);
  addActiveFiller(fillerStandaloneMuonTracksVtx);

  FillerTracks *fillerGlobalMuonTracks = new FillerTracks(cfg,"GlobalMuonTracks",defactive_);
  addActiveFiller(fillerGlobalMuonTracks);
    
  FillerTracks *fillerConversionInOutTracks = 
    new FillerTracks(cfg,"ConversionInOutTracks",defactive_);
  addActiveFiller(fillerConversionInOutTracks);

  FillerTracks *fillerConversionOutInTracks = 
    new FillerTracks(cfg,"ConversionOutInTracks",defactive_);
  addActiveFiller(fillerConversionOutInTracks);

  FillerTracks *fillerGsfTracks = new FillerTracks(cfg,"GsfTracks",defactive_);
  addActiveFiller(fillerGsfTracks);

  FillerMuons *fillerMuons = new FillerMuons(cfg,defactive_);
  addActiveFiller(fillerMuons);
  
  FillerElectrons *fillerElectrons = new FillerElectrons(cfg,defactive_);
  addActiveFiller(fillerElectrons);

  FillerConversionElectrons *fillerConversionElectrons = 
    new FillerConversionElectrons(cfg,defactive_);
  addActiveFiller(fillerConversionElectrons);

  FillerConversions *fillerConversions = new FillerConversions(cfg,defactive_);
  addActiveFiller(fillerConversions);

  FillerPhotons *fillerPhotons = new FillerPhotons(cfg,defactive_);
  addActiveFiller(fillerPhotons);

  FillerStableParts *fillerStableParts = new FillerStableParts(cfg,"StableParts",defactive_);
  addActiveFiller(fillerStableParts);
  
  FillerDecayParts *fillerDecayParts = new FillerDecayParts(cfg,"DecayParts",defactive_);
  addActiveFiller(fillerDecayParts);

  return 1;
}

//--------------------------------------------------------------------------------------------------
void FillMitTree::endJob()
{
  // Delete fillers.

  for (std::vector<BaseFiller*>::iterator iF = fillers_.begin(); iF != fillers_.end(); ++iF) {
    delete *iF;
  }

  edm::LogInfo("FillMitTree::endJob") << "Ending Job" << endl;
}
