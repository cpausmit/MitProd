// $Id: FillerBeamSpot.cc,v 1.2 2009/03/03 17:05:09 loizides Exp $

#include "MitProd/TreeFiller/interface/FillerBeamSpot.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/Common/interface/Handle.h"
#include "MitAna/DataTree/interface/Names.h"
#include "DataFormats/BeamSpot/interface/BeamSpot.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerBeamSpot::FillerBeamSpot(const ParameterSet &cfg, const char *name, bool active) : 
  BaseFiller(cfg,name,active),
  edmName_(Conf().getUntrackedParameter<string>("edmName","offlineBeamSpot")),
  mitName_(Conf().getUntrackedParameter<string>("mitName","BeamSpot")),
  beamSpots_(new mithep::BeamSpotArr)
{
  // Constructor.
}

//--------------------------------------------------------------------------------------------------
FillerBeamSpot::~FillerBeamSpot()
{
  // Destructor.

  delete beamSpots_;
}

//--------------------------------------------------------------------------------------------------
void FillerBeamSpot::BookDataBlock(TreeWriter &tws)
{
  // Add Vertex branch and the VertexMap to tree.

  tws.AddBranch(mitName_,&beamSpots_);
  OS()->add<mithep::BeamSpotArr>(beamSpots_,mitName_);
}

//--------------------------------------------------------------------------------------------------
void FillerBeamSpot::FillDataBlock(const edm::Event      &event, 
                                   const edm::EventSetup &setup)
{
  // Fill the BeamSpot branch.

  beamSpots_->Delete();

  Handle<reco::BeamSpot> hBeamSpotProduct;
  GetProduct(edmName_, hBeamSpotProduct, event);
  const reco::BeamSpot *inBeamSpot = hBeamSpotProduct.product();  

  mithep::BeamSpot *bs = beamSpots_->AddNew();
  bs->SetErrors(inBeamSpot->x0Error(),inBeamSpot->y0Error(),inBeamSpot->z0Error());
  bs->SetPosition(inBeamSpot->x0(),inBeamSpot->y0(),inBeamSpot->z0());
}
