// $Id: FillerPixelHits.cc,v 1.1 2009/09/25 08:42:27 loizides Exp $

#include "MitProd/TreeFiller/interface/FillerPixelHits.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "DataFormats/GeometryVector/interface/LocalPoint.h"
#include "DataFormats/SiPixelDetId/interface/PXBDetId.h"
#include "DataFormats/SiPixelDetId/interface/PXFDetId.h"
#include "DataFormats/TrackerRecHit2D/interface/SiPixelRecHitCollection.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "Geometry/CommonDetUnit/interface/GeomDet.h"
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
  phits_(new mithep::PixelHitArr(1000)),
  tgeo_(0)
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

  // get tracker geometry
  edm::ESHandle<TrackerGeometry> trackerHandle;
  es.get<TrackerDigiGeometryRecord>().get(trackerHandle);
  tgeo_ = trackerHandle.product();
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
  for(SiPixelRecHitCollection::DataContainer::const_iterator hit = hits->data().begin(), 
        end = hits->data().end(); hit != end; ++hit) {

    if (!hit->isValid())
      continue;

    DetId id(hit->geographicalId());
    LocalPoint lpos = LocalPoint(hit->localPosition().x(),
                                 hit->localPosition().y(),
                                 hit->localPosition().z());
    GlobalPoint gpos = tgeo_->idToDet(id)->toGlobal(lpos);

    Char_t type = 0;
    if(id.subdetId() == int(PixelSubdetector::PixelBarrel)) {
      PXBDetId pid(id);
      type = pid.layer();
    } else if (id.subdetId() == int(PixelSubdetector::PixelEndcap)) {
      PXFDetId pid(id);
      if (pid.disk()==0)
        type = 11;
      else
        type = 12;
      if (pid.side()!=0)
        type = -type;
    }

    mithep::PixelHit *newhit = phits_->Allocate();
    new (newhit) mithep::PixelHit(gpos.x(),gpos.y(),gpos.z());
    newhit->SetType(type);
    newhit->SetQuality(hit->rawQualityWord());
    newhit->SetCharge(static_cast<int>(hit->cluster()->charge()));
    newhit->SetSize(hit->cluster()->size());
  }
	
  phits_->Trim();
}
