// $Id: FillerBeamSpot.cc,v 1.2 2008/10/23 15:43:15 loizides Exp $

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
  beamSpot_(new mithep::BeamSpot)
{
  // Constructor.
}

//--------------------------------------------------------------------------------------------------
FillerBeamSpot::~FillerBeamSpot()
{
  // Destructor.

  delete beamSpot_;
}

//--------------------------------------------------------------------------------------------------
void FillerBeamSpot::BookDataBlock(TreeWriter &tws)
{
  // Add Vertex branch and the VertexMap to tree.

  tws.AddBranch(mitName_.c_str(),&beamSpot_);
  OS()->add<mithep::BeamSpot>(beamSpot_,mitName_.c_str());

}

//--------------------------------------------------------------------------------------------------
void FillerBeamSpot::FillDataBlock(const edm::Event      &event, 
                                   const edm::EventSetup &setup)
{
  // Fill the BeamSpot branch.

  Handle<reco::BeamSpot> hBeamSpotProduct;
  GetProduct(edmName_, hBeamSpotProduct, event);
  const reco::BeamSpot *inBeamSpot = hBeamSpotProduct.product();  

  new (beamSpot_) mithep::BeamSpot(inBeamSpot->x0(),
                                   inBeamSpot->y0(),
                                   inBeamSpot->z0(),
                                   inBeamSpot->x0Error(),
                                   inBeamSpot->y0Error(),
                                   inBeamSpot->z0Error()
                                   );
                                   
  beamSpot_->SetSigmaZ(inBeamSpot->sigmaZ());
  beamSpot_->SetBeamWidth(inBeamSpot->BeamWidth());
  beamSpot_->SetDxDz(inBeamSpot->dxdz());
  beamSpot_->SetDyDz(inBeamSpot->dydz());
  
  beamSpot_->SetSigmaZErr(inBeamSpot->sigmaZ0Error());
  beamSpot_->SetBeamWidthErr(inBeamSpot->BeamWidthError());
  beamSpot_->SetDxDzErr(inBeamSpot->dxdzError());
  beamSpot_->SetDyDzErr(inBeamSpot->dydzError());

}
