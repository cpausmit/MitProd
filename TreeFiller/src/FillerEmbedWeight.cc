// $Id: FillerEmbedWeight.cc,v 1.2 2011/11/28 13:03:07 pharris Exp $

#include "MitProd/TreeFiller/interface/FillerEmbedWeight.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataTree/interface/EmbedWeightCol.h"
#include "MitProd/ObjectService/interface/ObjectService.h"
#include "SimDataFormats/GeneratorProducts/interface/GenFilterInfo.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerEmbedWeight::FillerEmbedWeight(const ParameterSet &cfg, const char *name, bool active) : 
  BaseFiller(cfg,name,active),
  edmName_(Conf().getUntrackedParameter<string>("edmName","generator_weight")),
  genInfo_(Conf().getUntrackedParameter<bool>  ("useGenInfo","True")),
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
  
  double inEmbedWeightValue = 0;
  if(!genInfo_) { 
    Handle<double> hEmbedWeight;
    event.getByLabel(edm::InputTag(edmName_,"weight","EmbeddedRECO"),hEmbedWeight);
    inEmbedWeightValue = *(hEmbedWeight.product());  
  } else { 
    edm::Handle<GenFilterInfo> hGenFilterInfo;
    event.getByLabel(edm::InputTag(edmName_, "minVisPtFilter", "EmbeddedRECO"), hGenFilterInfo);
    inEmbedWeightValue = hGenFilterInfo->filterEfficiency();
  }
  const double inEmbedWeight = inEmbedWeightValue;
  mithep::EmbedWeight *embedWeight = embedWeight_->Allocate();
  new (embedWeight) mithep::EmbedWeight(inEmbedWeight);
  //embedWeight->SetWeight(inEmbedWeight);
  embedWeight_->Trim();
}
