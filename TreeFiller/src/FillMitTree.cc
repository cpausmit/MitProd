// $Id: FillMitTree.cc,v 1.2 2008/06/11 12:50:17 loizides Exp $

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "MitProd/TreeFiller/interface/FillerGlobalMuons.h"
#include "MitProd/TreeFiller/interface/FillMitTree.h"

using namespace std;
using namespace edm;
using namespace mithep;

//-------------------------------------------------------------------------------------------------
FillMitTree::FillMitTree(const edm::ParameterSet &cfg) :
  fillerGlobalMuons_(0)
{
  // initialize fillers
  fillerGlobalMuons_ = new FillerGlobalMuons(cfg);
  if (fillerGlobalMuons_->Active())
    fillers_.push_back(fillerGlobalMuons_);
}

//-------------------------------------------------------------------------------------------------
FillMitTree::~FillMitTree()
{
  // We own the fillers so we have to delete them
  delete fillerGlobalMuons_;
}

//-------------------------------------------------------------------------------------------------
void FillMitTree::beginJob(edm::EventSetup const &event)
{
  Service<TreeService> ts;
  TreeWriter *tws = ts->get();
  if (! tws) {
    throw edm::Exception(edm::errors::Configuration, "FillMitTree::beginJob()\n")
      << "Could not get pointer to Tree with name " << tws->GetName() << "\n";
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
  edm::LogInfo("FillMitTree::endJob") << "Ending Job" << endl;
}
