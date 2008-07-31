// $Id: FillerStableParts.cc,v 1.4 2008/07/31 12:34:04 loizides Exp $

#include "MitAna/DataTree/interface/StableParticle.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticleFwd.h"
#include "DataFormats/RecoCandidate/interface/TrackAssociation.h"
#include "MitEdm/DataFormats/interface/CollectionsEdm.h"
#include "MitEdm/DataFormats/interface/BasePartFwd.h"
#include "MitEdm/DataFormats/interface/StablePart.h"
#include "MitProd/TreeFiller/interface/FillerStableParts.h"
#include "MitAna/DataTree/interface/Names.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerStableParts::FillerStableParts(const ParameterSet &cfg, const char *name, bool active) :
  BaseFiller(cfg,name,active),
  edmName_(Conf().getUntrackedParameter<string>("edmName","")),
  mitName_(Conf().getUntrackedParameter<string>("mitName","")),
  trackMapName_(Conf().getUntrackedParameter<string>("trackMapName","")),
  basePartMapName_(Conf().getUntrackedParameter<string>("basePartMap",
                                                        Form("%sMapName",mitName_.c_str()))),
  trackMap_(0),
  stables_(new mithep::StableParticleArr(250)),
  basePartMap_(new mithep::BasePartMap)
{
  // Constructor.
}

//--------------------------------------------------------------------------------------------------
FillerStableParts::~FillerStableParts()
{
  // Destructor.

  delete stables_;
  delete basePartMap_;
}

//--------------------------------------------------------------------------------------------------
void FillerStableParts::BookDataBlock(TreeWriter &tws)
{
  // Add tracks branch to tree, get and publish our maps.

  tws.AddBranch(mitName_.c_str(),&stables_);

  OS()->add(basePartMap_,basePartMapName_.c_str());
  if (!trackMapName_.empty()) 
    trackMap_ = OS()->get<TrackMap>(trackMapName_.c_str());
}

//--------------------------------------------------------------------------------------------------
void FillerStableParts::FillDataBlock(const edm::Event      &evt, 
				      const edm::EventSetup &setup)
{
  // Fill our EDM StablePart collection into the MIT StableParticle collection.

  stables_->Reset();
  basePartMap_->Reset();

  Handle<mitedm::StablePartCol> hParts;
  GetProduct(edmName_, hParts, evt);  
  const mitedm::StablePartCol *iParts = hParts.product();
  
  // loop through all StableParts and fill the information
  for (UInt_t i=0; i<iParts->size(); ++i) {
    const mitedm::StablePart &p = iParts->at(i);                  // for convenience
    //edm::RefToBaseProd<mitedm::BasePart> baseRef(hParts);
    //mitedm::BasePartBaseRef theRef(baseRef,i);
    PairIntKey theKey(hParts.id().id(),i);
    //cout << "MITEDM...\n";p->print();
    mithep::StableParticle *d = stables_->Allocate();
    new (d) mithep::StableParticle(p.pid());
    basePartMap_->Add(theKey,d);
    if (trackMap_)
      d->SetTrk(trackMap_->GetMit(p.trackRef()));
    //cout << "MITHEP...\n";d->print();
  }
  stables_->Trim();
}
