// $Id: FillMitTree.cc,v 1.16 2008/07/30 16:39:58 loizides Exp $

#include "MitProd/TreeFiller/interface/FillMitTree.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "MitProd/TreeService/interface/TreeService.h"
#include "MitProd/ObjectService/interface/ObjectService.h"
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
#include "MitProd/TreeFiller/interface/FillerMCParticles.h"
#include "MitProd/TreeFiller/interface/FillerDecayParts.h"
#include "MitProd/TreeFiller/interface/FillerStableParts.h"

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

  FillerCaloJets *fillerCaloJets = new FillerCaloJets(cfg,"CaloJets",defactive_);
  addActiveFiller(fillerCaloJets);

  FillerCaloMet *fillerCaloMet = new FillerCaloMet(cfg,"CaloMet",defactive_);
  addActiveFiller(fillerCaloMet);

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

  FillerGsfTracks *fillerGsfTracks = new FillerGsfTracks(cfg,"GsfTracks",defactive_);
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
