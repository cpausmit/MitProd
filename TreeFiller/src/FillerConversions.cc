// $Id: FillerConversions.cc,v 1.8 2008/07/31 12:34:04 loizides Exp $

#include "MitProd/TreeFiller/interface/FillerConversions.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/Common/interface/Handle.h"
#include "MitAna/DataTree/interface/Names.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerConversions::FillerConversions(const ParameterSet &cfg, bool active) :
  BaseFiller(cfg,"Conversions",active),
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

  tws.AddBranch(mitName_.c_str(),&conversions_);

  if (!convElectronMapName_.empty())
    OS()->add(conversionMap_,conversionMapName_.c_str());
  convElectronMap_ = OS()->get<ConversionElectronMap>(convElectronMapName_.c_str());
}

//--------------------------------------------------------------------------------------------------
void FillerConversions::FillDataBlock(const edm::Event      &event, 
                                      const edm::EventSetup &setup)
{
  // Fill conversions data structure and maps.

  conversions_->Reset();
  conversionMap_->Reset();

  Handle<reco::ConversionCollection> hConversionProduct;
  GetProduct(edmName_, hConversionProduct, event);

  conversionMap_->SetEdmProductId(hConversionProduct.id().id());
        
  const reco::ConversionCollection inConversions = *(hConversionProduct.product());  
  
  for (reco::ConversionCollection::const_iterator inConversion = inConversions.begin(); 
      inConversion != inConversions.end(); ++inConversion) {
        
    mithep::Conversion *outConversion = conversions_->Allocate();
    new (outConversion) mithep::Conversion(inConversion->conversionVertex().x(),
                                           inConversion->conversionVertex().y(),
                                           inConversion->conversionVertex().z(), 
                                           inConversion->conversionVertex().xError(),
                                           inConversion->conversionVertex().yError(),
                                           inConversion->conversionVertex().zError());
        
    outConversion->DecayVertex().SetChi2(inConversion->conversionVertex().chi2());
    outConversion->DecayVertex().SetNdof((Int_t)inConversion->conversionVertex().ndof());
    
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
        outConversion->AddDaughter(convElectronMap_->GetMit(*trackRef));
      }
    }
    
    reco::ConversionRef theRef(hConversionProduct, inConversion-inConversions.begin());
    conversionMap_->Add(theRef, outConversion);
  }

  conversions_->Trim();
}
