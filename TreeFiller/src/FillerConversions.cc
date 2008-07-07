// $Id: FillerConversions.cc,v 1.2 2008/07/03 07:56:14 loizides Exp $

#include "MitProd/TreeFiller/interface/FillerConversions.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/Common/interface/Handle.h"
#include "MitAna/DataTree/interface/Names.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerConversions::FillerConversions(const ParameterSet &cfg, bool active, 
                                     const ConversionElectronMap *conversionElectronMap) : 
  BaseFiller(cfg, "Conversions", active),
  edmName_(Conf().getUntrackedParameter<string>("edmName","conversions")),
  mitName_(Conf().getUntrackedParameter<string>("mitName","Conversions")),
  conversionElectronMap_(conversionElectronMap),
  conversions_(new mithep::ConversionArr),
  conversionMap_(new mithep::ConversionMap)
{
  // Constructor.
}

//--------------------------------------------------------------------------------------------------
FillerConversions::~FillerConversions()
{
  // Destructor.

  delete conversionMap_;
}

//--------------------------------------------------------------------------------------------------
void FillerConversions::BookDataBlock(TreeWriter &tws)
{
  // Add conversions to tree.

  tws.AddBranch(mitName_.c_str(),&conversions_);
}

//--------------------------------------------------------------------------------------------------
void FillerConversions::FillDataBlock(const edm::Event      &event, 
                                      const edm::EventSetup &setup)
{

  conversions_->Reset();
  conversionMap_->Reset();
  
  try {
    event.getByLabel(edm::InputTag(edmName_),conversionProduct_);
  } catch (cms::Exception& ex) {
    edm::LogError("FillerConversions") << "Error! Cannot get collection with label " 
                                       << edmName_ << endl;
    throw edm::Exception(edm::errors::Configuration, "FillerConversions:FillDataBlock()\n")
      << "Error! Cannot get collection with label " << edmName_ << endl;
  }
        
  conversionMap_->SetEdmProductId(conversionProduct_.id().id());
  
  const reco::ConversionCollection inConversions = *(conversionProduct_.product());  
  
  for (reco::ConversionCollection::const_iterator inConversion = inConversions.begin(); 
      inConversion != inConversions.end(); ++inConversion) {
        
        mithep::Conversion* outConversion = conversions_->Allocate();
        new (outConversion) mithep::Conversion(inConversion->conversionVertex().x(),
                                               inConversion->conversionVertex().y(),
                                               inConversion->conversionVertex().z(), 
                                               inConversion->conversionVertex().xError(),
                                               inConversion->conversionVertex().yError(),
                                               inConversion->conversionVertex().zError());
        
        outConversion->GetVertex().SetChi2(inConversion->conversionVertex().chi2());
        outConversion->GetVertex().SetNDof((Int_t)inConversion->conversionVertex().ndof());
        
        outConversion->SetDCotTheta(inConversion->pairCotThetaSeparation());
        outConversion->SetEOverP(inConversion->EoverP());
        outConversion->SetPairMass(inConversion->pairInvariantMass());
        outConversion->SetPairMomentum(inConversion->pairMomentum().x(),
                                       inConversion->pairMomentum().y(),
                                       inConversion->pairMomentum().z());
        
        if (conversionElectronMap_) {
          std::vector<reco::TrackRef> trackRefs = inConversion->tracks();
          for (std::vector<reco::TrackRef>::const_iterator trackRef = trackRefs.begin(); 
               trackRef != trackRefs.end(); ++trackRef) {
            outConversion->AddDaughter(conversionElectronMap_->GetMit(*trackRef));
          }
        }

        reco::ConversionRef theRef(conversionProduct_, inConversion-inConversions.begin());
        conversionMap_->Add(theRef, outConversion);
  }

  conversions_->Trim();
}
