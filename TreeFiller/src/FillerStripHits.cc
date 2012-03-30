// $Id: FillerStripHits.cc,v 1.2 2010/03/18 20:21:01 bendavid Exp $

#include "MitProd/TreeFiller/interface/FillerStripHits.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "DataFormats/GeometryVector/interface/LocalPoint.h"
#include "DataFormats/SiStripDetId/interface/StripSubdetector.h"
#include "DataFormats/SiStripDetId/interface/TECDetId.h"
#include "DataFormats/SiStripDetId/interface/TIBDetId.h"
#include "DataFormats/SiStripDetId/interface/TIDDetId.h"
#include "DataFormats/SiStripDetId/interface/TOBDetId.h"
#include "DataFormats/TrackerRecHit2D/interface/SiStripMatchedRecHit2DCollection.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "Geometry/TrackerGeometryBuilder/interface/StripGeomDetUnit.h"
#include "Geometry/CommonTopologies/interface/StripTopology.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataTree/interface/StripHitCol.h"
#include "MitProd/ObjectService/interface/ObjectService.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerStripHits::FillerStripHits(const ParameterSet &cfg, const char *name, bool active) :
  BaseFiller(cfg,name,active),
  edmName_(Conf().getUntrackedParameter<string>("edmName","siStripMatchedRecHits:stereoRecHit")),
  mitName_(Conf().getUntrackedParameter<string>("mitName",Names::gkStripHitBrn)),
  shits_(new mithep::StripHitArr(1000))
{
  // Constructor.
}

//--------------------------------------------------------------------------------------------------
FillerStripHits::~FillerStripHits()
{
  // Destructor.

  delete shits_;
}

//--------------------------------------------------------------------------------------------------
void FillerStripHits::BookDataBlock(TreeWriter &tws)
{

  // Add tracks branch to tree, publish and get tracker geometry.

  tws.AddBranch(mitName_,&shits_);
  OS()->add<StripHitArr>(shits_,mitName_);
}

//--------------------------------------------------------------------------------------------------
void FillerStripHits::FillDataBlock(const edm::Event      &event, 
                                    const edm::EventSetup &setup)
{
  // Fill pixel hits from edm collection into our collection.

  shits_->Delete();

  // initialize handle and get product
  Handle<SiStripMatchedRecHit2DCollection> hRecHits;
  GetProduct(edmName_, hRecHits, event);  

  const SiStripMatchedRecHit2DCollection *hits = hRecHits.product();
  if (!hits->size())
    return;

  // get tracker geometry
  edm::ESHandle<TrackerGeometry> trackerHandle;
  setup.get<TrackerDigiGeometryRecord>().get(trackerHandle);
  const TrackerGeometry *tgeo = trackerHandle.product();

  for(SiStripMatchedRecHit2DCollection::DataContainer::const_iterator hit = hits->data().begin(), 
        end = hits->data().end(); hit != end; ++hit) {

    const SiStripRecHit2D &shit = hit->stereoHit();

    if (!shit.isValid())
      continue;

    int type = 0;
    int gtyp = 0;
    DetId id(shit.geographicalId());
    if(id.subdetId() == int(StripSubdetector::TIB)) {
      type = 1;
    } else if (id.subdetId() == int(StripSubdetector::TID)) {
      type = 2;
    } else if (id.subdetId() == int(StripSubdetector::TOB)) {
      type = 3;
      TOBDetId pid(id);
      gtyp = pid.layerNumber();
      if (pid.isZMinusSide())
        gtyp = -gtyp;
    } else if (id.subdetId() == int(StripSubdetector::TEC)) {
      type = 4;
    } else {
      continue;
    }

    const StripGeomDetUnit *tgdu = 
      static_cast<const StripGeomDetUnit*>(tgeo->idToDetUnit(id));

    LocalPoint lpos = LocalPoint(shit.localPosition().x(),
                                 shit.localPosition().y(),
                                 shit.localPosition().z());
    GlobalPoint gpos = tgdu->toGlobal(lpos);
    mithep::StripHit *newhit = shits_->Allocate();
    new (newhit) mithep::StripHit(gpos.x(),gpos.y(),gpos.z());
    newhit->SetType(type);
    newhit->SetGeoType(gtyp);
  }
  shits_->Trim();
}
