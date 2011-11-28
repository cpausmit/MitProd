// $Id: FillerCaloMet.cc,v 1.14 2010/03/18 20:21:00 pharris Exp $

#include "MitProd/TreeFiller/interface/FillerEmbedWeight.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataTree/interface/EmbedWeightCol.h"
#include "MitProd/ObjectService/interface/ObjectService.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerEmbedWeight::FillerEmbedWeight(const ParameterSet &cfg, const char *name, bool active) : 
  BaseFiller(cfg,name,active),
  edmName_(Conf().getUntrackedParameter<string>("edmName","generator_weight")),
  mitName_(Conf().getUntrackedParameter<string>("mitName",Names::gkEmbedWeightBrn)),
  embedWeight_(new mithep::EmbedWeightArr)
{
  // Constructor.
}

//--------------------------------------------------------------------------------------------------
FillerEmbedWeight::~FillerEmbedWeight()
{
  // Destructor.

   delete embedWeight_;
}

//--------------------------------------------------------------------------------------------------
void FillerEmbedWeight::BookDataBlock(TreeWriter &tws)
{
  // Add mets branch to tree.

  tws.AddBranch(mitName_,&embedWeight_);
  OS()->add<mithep::EmbedWeightArr>(embedWeight_,mitName_);
}

//--------------------------------------------------------------------------------------------------
void FillerEmbedWeight::FillDataBlock(const edm::Event      &event, 
				      const edm::EventSetup &setup)  
{
  // Fill missing energy from edm collection into our collection.
  embedWeight_->Delete();
  Handle<double> hEmbedWeight;
  //GetProduct(edmName_, hEmbedWeight, event);
  event.getByLabel(edm::InputTag(edmName_,"weight","EmbeddedRECO"),hEmbedWeight);

  const double inEmbedWeight = *(hEmbedWeight.product());  
  mithep::EmbedWeight *embedWeight = embedWeight_->Allocate();
  new (embedWeight) mithep::EmbedWeight(inEmbedWeight);
  //embedWeight->SetWeight(inEmbedWeight);
  embedWeight_->Trim();
}
