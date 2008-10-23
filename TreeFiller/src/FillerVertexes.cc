// $Id: FillerVertexes.cc,v 1.1 2008/09/30 13:03:42 bendavid Exp $

#include "MitProd/TreeFiller/interface/FillerVertexes.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/Common/interface/Handle.h"
#include "MitAna/DataTree/interface/Names.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerVertexes::FillerVertexes(const ParameterSet &cfg, const char *name, bool active) : 
  BaseFiller(cfg,name,active),
  edmName_(Conf().getUntrackedParameter<string>("edmName","PrimaryVertexes")),
  mitName_(Conf().getUntrackedParameter<string>("mitName","PrimaryVertexes")),
  vertexMapName_(Conf().getUntrackedParameter<string>("vertexMapName",
                                                      "VertexMap")),
  vertexes_(new mithep::VertexArr(100)),
  vertexMap_(new mithep::VertexMap)
{
  // Constructor.
}

//--------------------------------------------------------------------------------------------------
FillerVertexes::~FillerVertexes()
{
  // Destructor.

  delete vertexes_;
  delete vertexMap_;
}

//--------------------------------------------------------------------------------------------------
void FillerVertexes::BookDataBlock(TreeWriter &tws)
{
  // Add Vertex branch and the VertexMap to tree.

  tws.AddBranch(mitName_.c_str(),&vertexes_);
  OS()->add<VertexMap>(vertexMap_,vertexMapName_.c_str());
  OS()->add<VertexArr>(vertexes_,mitName_.c_str());

}

//--------------------------------------------------------------------------------------------------
void FillerVertexes::FillDataBlock(const edm::Event      &event, 
                                   const edm::EventSetup &setup)
{
  // Fill the Vertex branch.

  vertexes_->Reset();
  vertexMap_->Reset();

  Handle<reco::VertexCollection> hVertexProduct;
  GetProduct(edmName_, hVertexProduct, event);
  vertexMap_->SetEdmProductId(hVertexProduct.id().id());
  const reco::VertexCollection inVertexes = *(hVertexProduct.product());  

  // loop through all vertexes
  for (reco::VertexCollection::const_iterator inV = inVertexes.begin(); 
       inV != inVertexes.end(); ++inV) {

    mithep::Vertex *outVertex = vertexes_->Allocate();
    new (outVertex) mithep::Vertex(inV->x(), inV->y(), inV->z(),
                                   inV->xError(), inV->yError(), inV->zError());
                                   
    outVertex->SetChi2(inV->chi2());
    outVertex->SetNdof(static_cast<Int_t>(inV->ndof()));
    outVertex->SetNTracks(inV->tracksSize());                                

    //add vertex to the map
    mitedm::VertexPtr thePtr(hVertexProduct, inV-inVertexes.begin());
    vertexMap_->Add(thePtr, outVertex);
          
  }

  vertexes_->Trim();
}
