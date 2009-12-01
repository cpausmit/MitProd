// $Id: FillerPixelHits.cc,v 1.5 2009/11/25 14:45:40 loizides Exp $

#include "MitProd/TreeFiller/interface/FillerPixelHits.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "DataFormats/GeometryVector/interface/LocalPoint.h"
#include "DataFormats/SiPixelDetId/interface/PXBDetId.h"
#include "DataFormats/SiPixelDetId/interface/PXFDetId.h"
#include "DataFormats/TrackerRecHit2D/interface/SiPixelRecHitCollection.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "Geometry/CommonDetUnit/interface/GeomDet.h"
#include "Geometry/TrackerTopology/interface/RectangularPixelTopology.h" 
#include "Geometry/TrackerGeometryBuilder/interface/PixelGeomDetUnit.h"
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataTree/interface/PixelHitCol.h"
#include "MitProd/ObjectService/interface/ObjectService.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerPixelHits::FillerPixelHits(const ParameterSet &cfg, const char *name, bool active) :
  BaseFiller(cfg,name,active),
  edmName_(Conf().getUntrackedParameter<string>("edmName","siPixelRecHits")),
  mitName_(Conf().getUntrackedParameter<string>("mitName",Names::gkPixelHitBrn)),
  phits_(new mithep::PixelHitArr(1000))
{
  // Constructor.
}

//--------------------------------------------------------------------------------------------------
FillerPixelHits::~FillerPixelHits()
{
  // Destructor.

  delete phits_;
}

//--------------------------------------------------------------------------------------------------
void FillerPixelHits::BookDataBlock(TreeWriter &tws, const edm::EventSetup &es)
{
  // Add tracks branch to tree, publish and get tracker geometry.

  tws.AddBranch(mitName_,&phits_);
  OS()->add<PixelHitArr>(phits_,mitName_);
}

//--------------------------------------------------------------------------------------------------
void FillerPixelHits::FillDataBlock(const edm::Event      &event, 
                                    const edm::EventSetup &setup)
{
  // Fill pixel hits from edm collection into our collection.

  phits_->Delete();

  // initialize handle and get product
  Handle<SiPixelRecHitCollection> hRecHits;
  GetProduct(edmName_, hRecHits, event);  

  const SiPixelRecHitCollection *hits = hRecHits.product();
  if (!hits->size())
    return;

  // get tracker geometry
  edm::ESHandle<TrackerGeometry> trackerHandle;
  setup.get<TrackerDigiGeometryRecord>().get(trackerHandle);
  const TrackerGeometry *tgeo = trackerHandle.product();

  for(SiPixelRecHitCollection::DataContainer::const_iterator hit = hits->data().begin(), 
        end = hits->data().end(); hit != end; ++hit) {

    if (!hit->isValid())
      continue;

    DetId id(hit->geographicalId());

    int type = 0; // PXB: 1,2,3   // PXF: +/- 11,12
    if(id.subdetId() == int(PixelSubdetector::PixelBarrel)) {
      PXBDetId pid(id);
      type = pid.layer();
    } else if (id.subdetId() == int(PixelSubdetector::PixelEndcap)) {
      PXFDetId pid(id);
      type = (10 + pid.disk())*((pid.side()==1)?-1:1); 
    } else {
      continue;
    }

    bool isAnyPixelOnEdge = false;
    const PixelGeomDetUnit *pgdu = 
      static_cast<const PixelGeomDetUnit*>(tgeo->idToDetUnit(id));
    if (1) {
      const RectangularPixelTopology *pixTopo = 
        static_cast<const RectangularPixelTopology*>(&pgdu->specificTopology());
      const vector<SiPixelCluster::Pixel> pixels(hit->cluster()->pixels());
      for(std::vector<SiPixelCluster::Pixel>::const_iterator pixel = pixels.begin(); 
          pixel != pixels.end(); ++pixel) {
        int pixelX = pixel->x;
        int pixelY = pixel->y;
        if(pixTopo->isItEdgePixelInX(pixelX) || pixTopo->isItEdgePixelInY(pixelY)) {
          isAnyPixelOnEdge = true;
          break;
        }
      }
    }

    LocalPoint lpos = LocalPoint(hit->localPosition().x(),
                                 hit->localPosition().y(),
                                 hit->localPosition().z());
    GlobalPoint gpos = pgdu->toGlobal(lpos);
    mithep::PixelHit *newhit = phits_->Allocate();
    new (newhit) mithep::PixelHit(gpos.x(),gpos.y(),gpos.z());
    newhit->SetType(type);
    newhit->SetQuality(hit->rawQualityWord());
    newhit->SetCharge(static_cast<int>(hit->cluster()->charge()));
    newhit->SetSizeX(hit->cluster()->sizeX());
    newhit->SetSizeY(hit->cluster()->sizeY());
    newhit->SetAnyPixelIsOnEdge(isAnyPixelOnEdge);
  }
  phits_->Trim();
}
