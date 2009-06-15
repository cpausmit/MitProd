// $Id: FillerConversions.cc,v 1.14 2009/03/18 14:57:58 loizides Exp $

#include "MitProd/TreeFiller/interface/FillerConversions.h"
#include "DataFormats/Common/interface/RefToPtr.h"
#include "MitAna/DataTree/interface/ConversionCol.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitProd/ObjectService/interface/ObjectService.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerConversions::FillerConversions(const ParameterSet &cfg, const char *name, bool active) :
  BaseFiller(cfg,name,active),
  edmName_(Conf().getUntrackedParameter<string>("edmName","conversions")),
  mitName_(Conf().getUntrackedParameter<string>("mitName","Conversions")),
  convElectronMapName_(Conf().getUntrackedParameter<string>("convElectronMapName","")),
  conversionMapName_(Conf().getUntrackedParameter<string>("conversionMapName",
                                                          Form("%sMapName",mitName_.c_str()))),
  convElectronMap_(0),
  conversions_(new mithep::ConversionArr(16)),
  conversionMap_(new mithep::ConversionMap)
{
  // Constructor.
}

//--------------------------------------------------------------------------------------------------
FillerConversions::~FillerConversions()
{
  // Destructor.

  delete conversions_;
  delete conversionMap_;
}

//--------------------------------------------------------------------------------------------------
void FillerConversions::BookDataBlock(TreeWriter &tws)
{
  // Add conversions to tree. Publish and get our objects.

  tws.AddBranch(mitName_,&conversions_);
  OS()->add<mithep::ConversionArr>(conversions_,mitName_);

  if (!convElectronMapName_.empty()) {
    conversionMap_->SetBrName(mitName_);
    OS()->add(conversionMap_,conversionMapName_);
  }

  if (!convElectronMapName_.empty()) {
    convElectronMap_ = OS()->get<ConversionElectronMap>(convElectronMapName_);
    if (convElectronMap_)
      AddBranchDep(mitName_, convElectronMap_->GetBrName());
  }
}

//--------------------------------------------------------------------------------------------------
void FillerConversions::FillDataBlock(const edm::Event      &event, 
                                      const edm::EventSetup &setup)
{
  // Fill conversions data structure and maps.

  conversions_->Delete();
  conversionMap_->Reset();

  Handle<reco::ConversionCollection> hConversionProduct;
  GetProduct(edmName_, hConversionProduct, event);

  conversionMap_->SetEdmProductId(hConversionProduct.id().id());
        
  const reco::ConversionCollection inConversions = *(hConversionProduct.product());  
  
  for (reco::ConversionCollection::const_iterator inConversion = inConversions.begin(); 
      inConversion != inConversions.end(); ++inConversion) {
        
    mithep::Vertex vertex(inConversion->conversionVertex().x(),
                          inConversion->conversionVertex().y(),
                          inConversion->conversionVertex().z(), 
                          inConversion->conversionVertex().xError(),
                          inConversion->conversionVertex().yError(),
                          inConversion->conversionVertex().zError());
    vertex.SetChi2(inConversion->conversionVertex().chi2());
    vertex.SetNdof((Int_t)inConversion->conversionVertex().ndof());
    vertex.SetNTracks(inConversion->conversionVertex().tracksSize());

    mithep::Conversion *outConversion = conversions_->Allocate();
    new (outConversion) mithep::Conversion(vertex);
    outConversion->SetDCotTheta(inConversion->pairCotThetaSeparation());
    outConversion->SetEOverP(inConversion->EoverP());
    outConversion->SetPairMass(inConversion->pairInvariantMass());
    outConversion->SetPairMomentum(inConversion->pairMomentum().x(),
                                   inConversion->pairMomentum().y(),
                                   inConversion->pairMomentum().z());
        
    if (convElectronMap_) {
      std::vector<reco::TrackRef> trackRefs = inConversion->tracks();
      for (std::vector<reco::TrackRef>::const_iterator trackRef = trackRefs.begin(); 
           trackRef != trackRefs.end(); ++trackRef) {
        outConversion->AddDaughter(convElectronMap_->GetMit(refToPtr(*trackRef)));
      }
    }
    
    reco::ConversionRef theRef(hConversionProduct, inConversion-inConversions.begin());
    conversionMap_->Add(theRef, outConversion);
  }

  conversions_->Trim();
}
