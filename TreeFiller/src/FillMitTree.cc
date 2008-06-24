// $Id: FillMitTree.cc,v 1.4 2008/06/20 17:52:57 loizides Exp $

#include "MitProd/TreeFiller/interface/FillMitTree.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "MitProd/TreeService/interface/TreeService.h"
#include "MitProd/TreeFiller/interface/FillerMetaInfos.h"
#include "MitProd/TreeFiller/interface/FillerGlobalMuons.h"
#include "MitProd/TreeFiller/interface/FillerGenParts.h"

using namespace std;
using namespace edm;
using namespace mithep;

//-------------------------------------------------------------------------------------------------
FillMitTree::FillMitTree(const edm::ParameterSet &cfg)
{
  // Constructor: initialize fillers

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

  FillerGlobalMuons *fillerGlobalMuons = new FillerGlobalMuons(cfg);
  if (fillerGlobalMuons->Active())
    fillers_.push_back(fillerGlobalMuons);
  else 
    delete fillerGlobalMuons;

  FillerGenParts *fillerGenParts = new FillerGenParts(cfg);
  if (fillerGenParts->Active())
    fillers_.push_back(fillerGenParts);
  else 
    delete fillerGenParts;

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
