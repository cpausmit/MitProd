// $Id: FillerGlobalMuons.cc,v 1.3 2008/06/18 19:17:21 loizides Exp $

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "SimDataFormats/HepMCProduct/interface/HepMCProduct.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "MitAna/DataTree/interface/Muon.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/EgammaCandidates/interface/Photon.h"
#include "DataFormats/EgammaCandidates/interface/PhotonFwd.h"
#include "DataFormats/EgammaCandidates/interface/Conversion.h"
#include "DataFormats/EgammaCandidates/interface/ConversionFwd.h"

#include "MitAna/DataTree/interface/Track.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitProd/TreeFiller/interface/FillerPhotons.h"

using namespace std;
using namespace edm;
using namespace mithep;

//-------------------------------------------------------------------------------------------------
FillerPhotons::FillerPhotons(const edm::ParameterSet &cfg, bool active, const ConversionMap* conversionMap) : 
  BaseFiller(cfg, "Photons", active),
  edmName_(Conf().getUntrackedParameter<string>("edmName","photons")),
  mitName_(Conf().getUntrackedParameter<string>("mitName",Names::gkPhotonBrn)),
  photons_(new mithep::Array<mithep::Photon>), conversionMap_(conversionMap)
{
}

//-------------------------------------------------------------------------------------------------
FillerPhotons::~FillerPhotons()
{
}

//-------------------------------------------------------------------------------------------------
void FillerPhotons::BookDataBlock(TreeWriter &tws)
{
  tws.AddBranch(mitName_.c_str(),&photons_);
}

//-------------------------------------------------------------------------------------------------
void FillerPhotons::FillDataBlock(const edm::Event      &event, 
                                      const edm::EventSetup &setup)
{
  // Fill global muon information. Links to the various standalone components are created in the
  // link resolution.

  photons_->Reset();
  
  //Handle<TrackingParticleCollection> trackingParticleProduct;
  try {
    event.getByLabel(edm::InputTag(edmName_),photonProduct_);
  } catch (cms::Exception& ex) {
    edm::LogError("FillerPhotons") << "Error! Cannot get collection with label " 
                                      << edmName_ << endl;
    throw edm::Exception(edm::errors::Configuration, "FillerPhotons:FillDataBlock()\n")
      << "Error! Cannot get collection with label " << edmName_ << endl;
  }
  
  //const TrackingParticleCollection TrackingParticles = *(trackingParticleProduct.product());  
  const reco::PhotonCollection inPhotons = *(photonProduct_.product());  
  
  for (reco::PhotonCollection::const_iterator iP = inPhotons.begin(); iP != inPhotons.end(); ++iP) {
        // Initialize a new MitDataObject
        mithep::Photon* outPhoton = photons_->Allocate();
        new (outPhoton) mithep::Photon(iP->px(),iP->py(),iP->pz(),iP->energy());
        if ( iP->isConverted() && conversionMap_) {
                std::vector<reco::ConversionRef> conversionRefs = iP->conversions();
                for (std::vector<reco::ConversionRef>::const_iterator conversionRef = conversionRefs.begin(); conversionRef != conversionRefs.end(); ++conversionRef) {
                        outPhoton->AddConversion(conversionMap_->GetMit(*conversionRef));
                }
        }
  }
  photons_->Trim();
}


