// $Id: FillMitTree.cc,v 1.5 2008/06/24 14:25:46 loizides Exp $

#include "MitProd/TreeFiller/interface/FillMitTree.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "MitProd/TreeService/interface/TreeService.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"
#include "MitProd/TreeFiller/interface/FillerMetaInfos.h"
#include "MitProd/TreeFiller/interface/FillerSimParticles.h"
#include "MitProd/TreeFiller/interface/FillerTracks.h"
#include "MitProd/TreeFiller/interface/FillerGsfTracks.h"
#include "MitProd/TreeFiller/interface/FillerMuons.h"
#include "MitProd/TreeFiller/interface/FillerElectrons.h"
//#include "MitProd/TreeFiller/interface/FillerConvElectrons.h"
//#include "MitProd/TreeFiller/interface/FillerPhotons.h"
#include "MitProd/TreeFiller/interface/FillerGenParts.h"

using namespace std;
using namespace edm;
using namespace mithep;

//-------------------------------------------------------------------------------------------------
FillMitTree::FillMitTree(const edm::ParameterSet &cfg)
{
  // Constructor.
    
  if (!configure(cfg)) {
    throw edm::Exception(edm::errors::Configuration, "FillMitTree::FillMitTree()\n")
      << "Could not configure fillers." << "\n";
  }
}

//-------------------------------------------------------------------------------------------------
FillMitTree::~FillMitTree()
{
  // Destructor: nothing to be done here.
}

//-------------------------------------------------------------------------------------------------
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

//-------------------------------------------------------------------------------------------------
void FillMitTree::beginJob(const edm::EventSetup &event)
{
  // Access the tree and book branches.

  Service<TreeService> ts;
  TreeWriter *tws = ts->get();
  if (! tws) {
    throw edm::Exception(edm::errors::Configuration, "FillMitTree::beginJob()\n")
      << "Could not get pointer to tree." << "\n";
    return;
  }
  
  // Loop over the various components and book the branches
  for (std::vector<BaseFiller*>::iterator iF = fillers_.begin(); iF != fillers_.end(); ++iF) {
    (*iF)->BookDataBlock(*tws);
  }
}

//-------------------------------------------------------------------------------------------------
bool FillMitTree::configure(const edm::ParameterSet &cfg)
{
  // Configure our fillers.

  FillerMetaInfos *fillerMetaInfos = new FillerMetaInfos(cfg);
  if (fillerMetaInfos->Active())
    fillers_.push_back(fillerMetaInfos);
  else 
    delete fillerMetaInfos;

  FillerGenParts *fillerGenParts = new FillerGenParts(cfg);
  if (fillerGenParts->Active())
    fillers_.push_back(fillerGenParts);
  else 
    delete fillerGenParts;  
    
  FillerSimParticles *fillerSimParticles = new FillerSimParticles(cfg);
  const SimParticleMap* simParticleMap=0;
  if (fillerSimParticles->Active()) {
    fillers_.push_back(fillerSimParticles);
    simParticleMap = fillerSimParticles->GetSimParticleMap();
  }
  else 
    delete fillerSimParticles;  
  
  FillerTracks *fillerGeneralTracks = new FillerTracks(cfg,"GeneralTracks", simParticleMap);
  const TrackMap* generalTrackMap=0;
  if (fillerGeneralTracks->Active()) {
    fillers_.push_back(fillerGeneralTracks);
    generalTrackMap = fillerGeneralTracks->GetTrackMap();
  }
  else 
    delete fillerGeneralTracks;
    
  FillerTracks *fillerStandaloneMuonTracks = new FillerTracks(cfg,"StandaloneMuonTracks");
  const TrackMap* standaloneMuonTrackMap=0;
  if (fillerStandaloneMuonTracks->Active()) {
    fillers_.push_back(fillerStandaloneMuonTracks);
    standaloneMuonTrackMap = fillerStandaloneMuonTracks->GetTrackMap();
  }
  else 
    delete fillerStandaloneMuonTracks;

  FillerTracks *fillerStandaloneMuonTracksVtx = 
    new FillerTracks(cfg,"StandaloneMuonTracksWVtxConstraint");
  const TrackMap* standaloneMuonTrackVtxMap=0;
  if (fillerStandaloneMuonTracksVtx->Active()) {
    fillers_.push_back(fillerStandaloneMuonTracksVtx);
    standaloneMuonTrackVtxMap = fillerStandaloneMuonTracksVtx->GetTrackMap();
  }
  else 
    delete fillerStandaloneMuonTracksVtx;
    
  FillerTracks *fillerGlobalMuonTracks = new FillerTracks(cfg,"GlobalMuonTracks");
  const TrackMap* globalMuonTrackMap=0;
  if (fillerGlobalMuonTracks->Active()) {
    fillers_.push_back(fillerGlobalMuonTracks);
    globalMuonTrackMap = fillerGlobalMuonTracks->GetTrackMap();
  }
  else 
    delete fillerGlobalMuonTracks;  
    
  FillerTracks *fillerConversionInOutTracks = 
    new FillerTracks(cfg,"ConversionInOutTracks", simParticleMap);
  const TrackMap* conversionInOutTrackMap=0;
  const TrackCol* conversionInOutTracks=0;
  if (fillerConversionInOutTracks->Active()) {
    fillers_.push_back(fillerConversionInOutTracks);
    conversionInOutTrackMap = fillerConversionInOutTracks->GetTrackMap();
    conversionInOutTracks = fillerConversionInOutTracks->GetTrackCol();
  }
  else 
    delete fillerConversionInOutTracks;
    
  FillerTracks *fillerConversionOutInTracks = 
    new FillerTracks(cfg,"ConversionOutInTracks", simParticleMap);
  const TrackMap* conversionOutInTrackMap=0;
  const TrackCol* conversionOutInTracks=0;
  if (fillerConversionOutInTracks->Active()) {
    fillers_.push_back(fillerConversionOutInTracks);
    conversionOutInTrackMap = fillerConversionOutInTracks->GetTrackMap();
    conversionOutInTracks = fillerConversionOutInTracks->GetTrackCol();
  }
  else 
    delete fillerConversionOutInTracks;
    
  FillerGsfTracks *fillerGsfTracks = new FillerGsfTracks(cfg,"GsfTracks", simParticleMap);
  const GsfTrackMap* gsfTrackMap=0;
  if (fillerGsfTracks->Active()) {
    fillers_.push_back(fillerGsfTracks);
    gsfTrackMap = (GsfTrackMap*)fillerGsfTracks->GetTrackMap();
  }
  else 
    delete fillerGsfTracks;
    
  FillerMuons *fillerMuons = 
    new FillerMuons(cfg, globalMuonTrackMap, standaloneMuonTrackMap, 
                    standaloneMuonTrackVtxMap, generalTrackMap);
  if (fillerMuons->Active())
    fillers_.push_back(fillerMuons);
  else 
    delete fillerMuons;
    
  FillerElectrons *fillerElectrons = new FillerElectrons(cfg, gsfTrackMap, generalTrackMap);
  if (fillerElectrons->Active())
    fillers_.push_back(fillerElectrons);
  else 
    delete fillerElectrons;
  
#if 0
  FillerConversionElectrons *fillerConversionElectrons = 
    new FillerConversionElectrons(cfg, conversionInOutTracks, conversionOutInTracks, 
                                  conversionInOutTrackMap, conversionOutInTrackMap);

  const ConversionElectronMap* convElectronMap=0;
  if (fillerConversionElectrons->Active()) {
    fillers_.push_back(fillerConversionElectrons);
    convElectronMap = fillerConversionElectrons->GetConversionElectronMap();
  }
  else 
    delete fillerConversionElectrons;
    
  FillerPhotons *fillerPhotons = new FillerPhotons(cfg, convElectronMap);
  if (fillerPhotons->Active())
    fillers_.push_back(fillerPhotons);
  else 
    delete fillerPhotons;
#endif

  return 1;
}

//-------------------------------------------------------------------------------------------------
void FillMitTree::endJob()
{
  // Delete fillers.

  for (std::vector<BaseFiller*>::iterator iF = fillers_.begin(); iF != fillers_.end(); ++iF) {
    delete *iF;
  }

  edm::LogInfo("FillMitTree::endJob") << "Ending Job" << endl;
}
