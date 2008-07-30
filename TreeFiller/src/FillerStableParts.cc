// $Id: FillerStableParts.cc,v 1.2 2008/07/29 22:54:37 bendavid Exp $

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticleFwd.h"
#include "DataFormats/RecoCandidate/interface/TrackAssociation.h"

#include "MitEdm/DataFormats/interface/CollectionsEdm.h"
#include "MitEdm/DataFormats/interface/BasePartFwd.h"
#include "MitEdm/DataFormats/interface/StablePartEdm.h"
#include "MitProd/TreeFiller/interface/FillerStableParts.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataTree/interface/StableParticle.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerStableParts::FillerStableParts(const ParameterSet &cfg, const char *name, bool active,
                                     const TrackMap *trackMap) : 
  BaseFiller(cfg,name,active),
  edmName_  (Conf().getUntrackedParameter<string>("edmName","")),
  mitName_  (Conf().getUntrackedParameter<string>("mitName","")),
  particleMap_(new mithep::BasePartMap),
  trackMap_(trackMap),
  stables_  (new mithep::StableParticleArr(250))
{
  // Constructor
}

//--------------------------------------------------------------------------------------------------
FillerStableParts::~FillerStableParts()
{
  // Destructor
  delete stables_;
  delete particleMap_;
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
  particleMap_->Reset();
  // access the edm StablePart collection
  Handle<mitedm::StablePartCol> hParts;
  GetProduct(edmName_, hParts, evt);  
  const mitedm::StablePartCol *iParts = hParts.product();
  
  // -----------------------------------------------------------------------------------------------
  // loop through all StableParts and fill the information
  // -----------------------------------------------------------------------------------------------
  for (UInt_t i=0; i<iParts->size(); ++i) {
    const mitedm::StablePartEdm &p = iParts->at(i);                  // for convenience
    //edm::RefToBaseProd<mitedm::BasePart> baseRef(hParts);
    //mitedm::BasePartBaseRef theRef(baseRef,i);
    PairIntKey theKey(hParts.id().id(),i);
    //cout << "MITEDM...\n";p->print();
    mithep::StableParticle *d = stables_->Allocate();
    new (d) mithep::StableParticle(p.pid());
    particleMap_->Add(theKey,d);
    if (trackMap_)
      d->SetTrk(trackMap_->GetMit(p.trackRef()));
    //cout << "MITHEP...\n";d->print();
  }
  stables_->Trim();
}
