// $Id: FillerStableParts.cc,v 1.8 2008/07/14 21:01:00 loizides Exp $

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticleFwd.h"
#include "DataFormats/RecoCandidate/interface/TrackAssociation.h"

#include "MitEdm/AnalysisDataFormats/interface/CollectionsEdm.h"
#include "MitEdm/AnalysisDataFormats/interface/StablePart.h"
#include "MitProd/TreeFiller/interface/FillerStableParts.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataTree/interface/StablePart.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerStableParts::FillerStableParts(const ParameterSet &cfg, const char *name, bool active) : 
  BaseFiller(cfg,name,active),
  edmName_  (Conf().getUntrackedParameter<string>("edmName","")),
  mitName_  (Conf().getUntrackedParameter<string>("mitName","")),
  stables_  (new mithep::StablePartObjArr(250))
{
  // Constructor
}

//--------------------------------------------------------------------------------------------------
FillerStableParts::~FillerStableParts()
{
  // Destructor
  delete stables_;
}

//--------------------------------------------------------------------------------------------------
void FillerStableParts::BookDataBlock(TreeWriter &tws)
{
  // Add tracks branch to tree
  tws.AddBranch(mitName_.c_str(),&stables_);
}

//--------------------------------------------------------------------------------------------------
void FillerStableParts::FillDataBlock(const edm::Event      &evt, 
				      const edm::EventSetup &setup)
{
  // -----------------------------------------------------------------------------------------------
  // fill edm StablePart collection into the MIT StablePart collection
  // -----------------------------------------------------------------------------------------------
  stables_->Reset();
  // access the edm StablePart collection
  Handle<mitedm::BasePartObjArr> hParts;
  GetProduct(edmName_, hParts, evt);  
  const mitedm::BasePartObjArr *iParts = hParts.product();
  
  // -----------------------------------------------------------------------------------------------
  // loop through all StableParts and fill the information
  // -----------------------------------------------------------------------------------------------
  for (UInt_t i=0; i<iParts->Entries(); ++i) {
    mitedm::StablePart *p = (mitedm::StablePart*) iParts->At(i);                  // for convenience
    //cout << "MITEDM...\n";p->print();
    StablePart *d = new StablePart((const mithep::StablePart&) *p);
    stables_->Add(d);
    //cout << "MITHEP...\n";d->print();
  }
  stables_->Trim();
}
