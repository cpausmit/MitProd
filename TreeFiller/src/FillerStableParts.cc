#include "MitProd/TreeFiller/interface/FillerStableParts.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/Common/interface/RefToPtr.h"
#include "DataFormats/RecoCandidate/interface/TrackAssociation.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticleFwd.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataTree/interface/StableParticleCol.h"
#include "MitProd/ObjectService/interface/ObjectService.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerStableParts::FillerStableParts(const ParameterSet &cfg, edm::ConsumesCollector& collector, ObjectService* os, const char *name, bool active) :
  BaseFiller(cfg,os,name,active),
  edmToken_(GetToken<mitedm::StablePartCol>(collector, cfg, "edmName","")),
  mitName_(cfg.getUntrackedParameter<string>("mitName","")),
  trackMapNames_(cfg.exists("trackMapNames") ? 
                    cfg.getUntrackedParameter<vector<string> >("trackMapNames") : 
                    vector<string>()),
  basePartMapName_(cfg.getUntrackedParameter<string>("basePartMap",
                                                        Form("%sMapName",mitName_.c_str()))),
  trackPartMapName_(cfg.getUntrackedParameter<string>("trackPartMap",
                                                        Form("%sTrackMapName",mitName_.c_str()))),
  stables_(new mithep::StableParticleArr(250)),
  basePartMap_(new mithep::BasePartMap),
  trackPartMap_(new mithep::TrackPartMap)
{
  // Constructor.
}

//--------------------------------------------------------------------------------------------------
FillerStableParts::~FillerStableParts()
{
  // Destructor.

  delete stables_;
  delete basePartMap_;
  delete trackPartMap_;
}

//--------------------------------------------------------------------------------------------------
void FillerStableParts::BookDataBlock(TreeWriter &tws)
{
  // Add tracks branch to tree, get and publish our maps.

  tws.AddBranch(mitName_,&stables_);
  OS()->add<mithep::StableParticleArr>(stables_,mitName_);

  if (!basePartMapName_.empty()) {
    basePartMap_->SetBrName(mitName_);
    OS()->add(basePartMap_,basePartMapName_);
  }
  if (!trackPartMapName_.empty()) {
    trackPartMap_->SetBrName(mitName_);
    OS()->add(trackPartMap_,trackPartMapName_);
  }

  for (std::vector<std::string>::const_iterator tmapName = trackMapNames_.begin();
        tmapName!=trackMapNames_.end(); ++tmapName) {
    if (!tmapName->empty()) {
      const TrackMap *map = OS()->get<TrackMap>(*tmapName);
      if (map) {
        trackMaps_.push_back(map);
        AddBranchDep(mitName_,map->GetBrName());
      }
    }
  }
}

//--------------------------------------------------------------------------------------------------
void FillerStableParts::FillDataBlock(const edm::Event      &evt, 
				      const edm::EventSetup &setup)
{
  // Fill our EDM StablePart collection into the MIT StableParticle collection.

  stables_->Delete();
  basePartMap_->Reset();

  Handle<mitedm::StablePartCol> hParts;
  GetProduct(edmToken_, hParts, evt);  
  const mitedm::StablePartCol *iParts = hParts.product();
  
  // loop through all StableParts and fill the information
  for (UInt_t i=0; i<iParts->size(); ++i) {
    const mitedm::StablePart &p = iParts->at(i); 
    mitedm::BasePartPtr thePtr(hParts,i);
    mithep::StableParticle *d = stables_->Allocate();
    new (d) mithep::StableParticle(p.pid());
    basePartMap_->Add(thePtr,d);
    trackPartMap_->Add(p.trackPtr(),d);
    if (trackMaps_.size())
      d->SetTrk(GetMitTrack(p.trackPtr()));
  }
  stables_->Trim();
}

//--------------------------------------------------------------------------------------------------
mithep::Track *FillerStableParts::GetMitTrack(const mitedm::TrackPtr &ptr) const
{
  // Return our particle referenced by the edm pointer.

  mithep::Track *mitTrack = 0;
  for (std::vector<const mithep::TrackMap*>::const_iterator tmap = trackMaps_.begin();
        tmap!=trackMaps_.end(); ++tmap) {
    const mithep::TrackMap *theMap = *tmap;
    if (theMap->HasMit(ptr)) {
      mitTrack = theMap->GetMit(ptr);
      return mitTrack;
    }
  }
  
  if (!mitTrack)
    throw edm::Exception(edm::errors::Configuration, "FillerStableParts::FillDataBlock()\n")
    << "Error! MITHEP Object " 
    << "not found in AssociationMaps (" << typeid(*this).name() << ")." << std::endl;
    
  return mitTrack;
}

DEFINE_MITHEP_TREEFILLER(FillerStableParts);
