// $Id: FillMitTree.cc,v 1.1 2008/06/18 13:23:22 paus Exp $

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "MitProd/TreeFiller/interface/FillMitTree.h"
#include "MitProd/TreeFiller/interface/FillerGlobalMuons.h"

using namespace std;
using namespace edm;
using namespace mithep;

//-------------------------------------------------------------------------------------------------
FillMitTree::FillMitTree(const edm::ParameterSet &cfg)
{
  // Constructor: initialize fillers

  FillerGlobalMuons *fillerGlobalMuons = new FillerGlobalMuons(cfg);
  if (fillerGlobalMuons->Active())
    fillers_.push_back(fillerGlobalMuons);
  else 
    delete fillerGlobalMuons;
}

//-------------------------------------------------------------------------------------------------
FillMitTree::~FillMitTree()
{
  // Destructor: nothing to be done here.
}

//-------------------------------------------------------------------------------------------------
void FillMitTree::beginJob(edm::EventSetup const &event)
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
    (*iF)->BookDataBlock(tws);
  }
  return;
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
void FillMitTree::endJob()
{
  // Delete fillers.

  for (std::vector<BaseFiller*>::iterator iF = fillers_.begin(); iF != fillers_.end(); ++iF) {
    delete *iF;
  }

  edm::LogInfo("FillMitTree::endJob") << "Ending Job" << endl;
}
