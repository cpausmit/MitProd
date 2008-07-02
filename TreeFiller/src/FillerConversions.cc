// $Id: FillerGlobalMuons.cc,v 1.3 2008/06/18 19:17:21 loizides Exp $

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "SimDataFormats/HepMCProduct/interface/HepMCProduct.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticleFwd.h"
#include "DataFormats/RecoCandidate/interface/TrackAssociation.h"

#include "MitAna/DataTree/interface/Track.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitProd/TreeFiller/interface/FillerConversions.h"

using namespace std;
using namespace edm;
using namespace mithep;

//-------------------------------------------------------------------------------------------------
FillerConversions::FillerConversions(const ParameterSet &cfg, bool active, const ConversionElectronMap* conversionElectronMap) : 
  BaseFiller(cfg, "Conversions", active),
  edmName_(Conf().getUntrackedParameter<string>("edmName","conversions")),
  edmDataName_(Conf().getUntrackedParameter<string>("edmDataName","")),
  mitName_(Conf().getUntrackedParameter<string>("mitName","Conversions")),
  conversionElectronMap_(conversionElectronMap),
  conversions_(new mithep::ConversionArr),
  conversionMap_(new mithep::ConversionMap)
{
}

//-------------------------------------------------------------------------------------------------
FillerConversions::~FillerConversions()
{
delete conversionMap_;
}

//-------------------------------------------------------------------------------------------------
void FillerConversions::BookDataBlock(TreeWriter &tws)
{
  tws.AddBranch(mitName_.c_str(),&conversions_);
}

//-------------------------------------------------------------------------------------------------
void FillerConversions::FillDataBlock(const edm::Event      &event, 
                                      const edm::EventSetup &setup)
{
  // Fill global muon information. Links to the various standalone components are created in the
  // link resolution.

  conversions_->Reset();
  conversionMap_->Reset();
  
  //Handle<TrackingParticleCollection> trackingParticleProduct;
  try {
    event.getByLabel(edm::InputTag(edmName_),conversionProduct_);
  } catch (cms::Exception& ex) {
    edm::LogError("FillerConversions") << "Error! Cannot get collection with label " 
                                      << edmName_ << endl;
    throw edm::Exception(edm::errors::Configuration, "FillerConversions:FillDataBlock()\n")
      << "Error! Cannot get collection with label " << edmName_ << endl;
  }
        
  conversionMap_->SetEdmProductId(conversionProduct_.id().id());
  
    
  //const TrackingParticleCollection TrackingParticles = *(trackingParticleProduct.product());  
  const reco::ConversionCollection inConversions = *(conversionProduct_.product());  
  
  // Loop through all tracks
  for (reco::ConversionCollection::const_iterator inConversion = inConversions.begin(); 
      inConversion != inConversions.end(); ++inConversion) {
        
        mithep::Conversion* outConversion = conversions_->Allocate();
        new (outConversion) mithep::Conversion(inConversion->conversionVertex().x(),inConversion->conversionVertex().y(),inConversion->conversionVertex().z(), inConversion->conversionVertex().xError(),inConversion->conversionVertex().yError(),inConversion->conversionVertex().zError());
        
        
        outConversion->GetVertex().SetChi2(inConversion->conversionVertex().chi2());
        outConversion->GetVertex().SetNDof(inConversion->conversionVertex().ndof());
        
        if (conversionElectronMap_) {
                        std::vector<reco::TrackRef> trackRefs = inConversion->tracks();
                        for (std::vector<reco::TrackRef>::const_iterator trackRef = trackRefs.begin(); trackRef != trackRefs.end(); ++trackRef) {
                                outConversion->AddDaughter(conversionElectronMap_->GetMit(*trackRef));
                        }
        }

        
        reco::ConversionRef theRef(conversionProduct_, inConversion-inConversions.begin());
        conversionMap_->Add(theRef, outConversion);
        
  }
  conversions_->Trim();
}


