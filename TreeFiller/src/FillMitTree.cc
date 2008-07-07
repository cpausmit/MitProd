// $Id: FillMitTree.cc,v 1.10 2008/07/07 13:29:05 bendavid Exp $

#include "MitProd/TreeFiller/interface/FillMitTree.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "MitProd/TreeService/interface/TreeService.h"
#include "MitProd/TreeFiller/interface/AssociationMaps.h"
#include "MitProd/TreeFiller/interface/FillerMetaInfos.h"
#include "MitProd/TreeFiller/interface/FillerTracks.h"
#include "MitProd/TreeFiller/interface/FillerGsfTracks.h"
#include "MitProd/TreeFiller/interface/FillerMuons.h"
#include "MitProd/TreeFiller/interface/FillerElectrons.h"
#include "MitProd/TreeFiller/interface/FillerElectrons.h"
#include "MitProd/TreeFiller/interface/FillerCaloJets.h"
#include "MitProd/TreeFiller/interface/FillerCaloMet.h"
#include "MitProd/TreeFiller/interface/FillerConversions.h"
#include "MitProd/TreeFiller/interface/FillerConversionElectrons.h"
#include "MitProd/TreeFiller/interface/FillerPhotons.h"
#include "MitProd/TreeFiller/interface/FillerGenParts.h"
#include "MitProd/TreeFiller/interface/FillerSimParticles.h"

using namespace std;
using namespace edm;
using namespace mithep;

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
      << "Could not get pointer to tree." << "\n";
    return;
  }
  
  // Loop over the various components and book the branches
  for (std::vector<BaseFiller*>::iterator iF = fillers_.begin(); iF != fillers_.end(); ++iF) {
    (*iF)->BookDataBlock(*tws);
  }
}

//--------------------------------------------------------------------------------------------------
bool FillMitTree::configure(const edm::ParameterSet &cfg)
{
  // Configure our fillers.

  FillerMetaInfos *fillerMetaInfos = new FillerMetaInfos(cfg);
  if (fillerMetaInfos->Active())
    fillers_.push_back(fillerMetaInfos);
  else {
    delete fillerMetaInfos;
    fillerMetaInfos = 0;
  }

  FillerGenParts *fillerGenParts = new FillerGenParts(cfg,defactive_);
  const GenParticleMap* genParticleMap = 0;
  if (fillerGenParts->Active()) {
    fillers_.push_back(fillerGenParts);
    genParticleMap = fillerGenParts->GetGenParticleMap();
  }
  else {
    delete fillerGenParts;  
    fillerGenParts = 0;
  }

  FillerSimParticles *fillerSimParticles = new FillerSimParticles(cfg,defactive_,genParticleMap);
  const SimParticleMap *simParticleMap = 0;
  if (fillerSimParticles->Active()) {
    fillers_.push_back(fillerSimParticles);
    simParticleMap = fillerSimParticles->GetSimParticleMap();
  }
  else {
    delete fillerSimParticles;  
    fillerSimParticles = 0;
  }

  FillerCaloJets *fillerCaloJets = new FillerCaloJets(cfg,"CaloJets",defactive_);
  if (fillerCaloJets->Active()) {
    fillers_.push_back(fillerCaloJets);
  }
  else {
    delete fillerCaloJets;
    fillerCaloJets = 0;
  }

  FillerCaloMet *fillerCaloMet = new FillerCaloMet(cfg,"CaloMet",defactive_);
  if (fillerCaloMet->Active()) {
    fillers_.push_back(fillerCaloMet);
  }
  else {
    delete fillerCaloMet;
    fillerCaloMet = 0;
  }

  FillerTracks *fillerGeneralTracks = 
    new FillerTracks(cfg,"GeneralTracks",defactive_,simParticleMap);
  const TrackMap *generalTrackMap=0;
  if (fillerGeneralTracks->Active()) {
    fillers_.push_back(fillerGeneralTracks);
    generalTrackMap = fillerGeneralTracks->GetTrackMap();
  }
  else {
    delete fillerGeneralTracks;
    fillerGeneralTracks = 0;
  }

  FillerTracks *fillerStandaloneMuonTracks = 
    new FillerTracks(cfg,"StandaloneMuonTracks",defactive_);
  const TrackMap *standaloneMuonTrackMap=0;
  if (fillerStandaloneMuonTracks->Active()) {
    fillers_.push_back(fillerStandaloneMuonTracks);
    standaloneMuonTrackMap = fillerStandaloneMuonTracks->GetTrackMap();
  }
  else {
    delete fillerStandaloneMuonTracks;
    fillerStandaloneMuonTracks = 0;
  }

  FillerTracks *fillerStandaloneMuonTracksVtx = 
    new FillerTracks(cfg,"StandaloneMuonTracksWVtxConstraint",defactive_);
  const TrackMap *standaloneMuonTrackVtxMap=0;
  if (fillerStandaloneMuonTracksVtx->Active()) {
    fillers_.push_back(fillerStandaloneMuonTracksVtx);
    standaloneMuonTrackVtxMap = fillerStandaloneMuonTracksVtx->GetTrackMap();
  }
  else {
    delete fillerStandaloneMuonTracksVtx;
    fillerStandaloneMuonTracksVtx = 0;
  }

  FillerTracks *fillerGlobalMuonTracks = new FillerTracks(cfg,"GlobalMuonTracks",defactive_);
  const TrackMap *globalMuonTrackMap=0;
  if (fillerGlobalMuonTracks->Active()) {
    fillers_.push_back(fillerGlobalMuonTracks);
    globalMuonTrackMap = fillerGlobalMuonTracks->GetTrackMap();
  }
  else {
    delete fillerGlobalMuonTracks;  
    fillerGlobalMuonTracks = 0;
  }
    
  FillerTracks *fillerConversionInOutTracks = 
    new FillerTracks(cfg,"ConversionInOutTracks",defactive_,simParticleMap);
  const TrackMap *conversionInOutTrackMap=0;
  const TrackCol *conversionInOutTracks=0;
  if (fillerConversionInOutTracks->Active()) {
    fillers_.push_back(fillerConversionInOutTracks);
    conversionInOutTrackMap = fillerConversionInOutTracks->GetTrackMap();
    conversionInOutTracks = fillerConversionInOutTracks->GetTrackCol();
  }
  else {
    delete fillerConversionInOutTracks;
    fillerConversionInOutTracks = 0;
  }

  FillerTracks *fillerConversionOutInTracks = 
    new FillerTracks(cfg,"ConversionOutInTracks",defactive_,simParticleMap);
  const TrackMap *conversionOutInTrackMap=0;
  const TrackCol *conversionOutInTracks=0;
  if (fillerConversionOutInTracks->Active()) {
    fillers_.push_back(fillerConversionOutInTracks);
    conversionOutInTrackMap = fillerConversionOutInTracks->GetTrackMap();
    conversionOutInTracks = fillerConversionOutInTracks->GetTrackCol();
  }
  else {
    delete fillerConversionOutInTracks;
    fillerConversionOutInTracks = 0;
  }

  FillerGsfTracks *fillerGsfTracks = 
    new FillerGsfTracks(cfg,"GsfTracks",defactive_,simParticleMap);
  const GsfTrackMap *gsfTrackMap=0;
  if (fillerGsfTracks->Active()) {
    fillers_.push_back(fillerGsfTracks);
    gsfTrackMap = (GsfTrackMap*)fillerGsfTracks->GetTrackMap();
  }
  else {
    delete fillerGsfTracks;
    fillerGsfTracks = 0;
  }

  FillerMuons *fillerMuons = 
    new FillerMuons(cfg,defactive_,globalMuonTrackMap,standaloneMuonTrackMap, 
                    standaloneMuonTrackVtxMap,generalTrackMap);
  if (fillerMuons->Active())
    fillers_.push_back(fillerMuons);
  else {
    delete fillerMuons;
    fillerMuons = 0;
  }

  FillerElectrons *fillerElectrons = 
    new FillerElectrons(cfg,defactive_,gsfTrackMap,generalTrackMap);
  if (fillerElectrons->Active())
    fillers_.push_back(fillerElectrons);
  else {
    delete fillerElectrons;
      fillerElectrons = 0;
  }

  FillerConversionElectrons *fillerConversionElectrons = 
    new FillerConversionElectrons(cfg, defactive_, conversionInOutTracks, conversionOutInTracks, 
                                  conversionInOutTrackMap, conversionOutInTrackMap);
  const ConversionElectronMap *convElectronMap=0;
  if (fillerConversionElectrons->Active()) {
    fillers_.push_back(fillerConversionElectrons);
    convElectronMap = fillerConversionElectrons->GetConversionElectronMap();
  }
  else {
    delete fillerConversionElectrons;
    fillerConversionElectrons = 0;
  }

  FillerConversions *fillerConversions = new FillerConversions(cfg, defactive_, convElectronMap);
  const ConversionMap *conversionMap=0;
  if (fillerConversions->Active()) {
    fillers_.push_back(fillerConversions);
    conversionMap = fillerConversions->GetConversionMap();
  }
  else {
    delete fillerConversions;
    fillerConversions = 0;
  }

  FillerPhotons *fillerPhotons = new FillerPhotons(cfg, defactive_, conversionMap);
  if (fillerPhotons->Active())
    fillers_.push_back(fillerPhotons);
  else {
    delete fillerPhotons;
    fillerPhotons = 0;
  }

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
