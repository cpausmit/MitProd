// $Id: FillerPhotons.cc,v 1.8 2008/09/12 12:54:08 bendavid Exp $

#include "MitProd/TreeFiller/interface/FillerPhotons.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/EgammaCandidates/interface/Photon.h"
#include "DataFormats/EgammaCandidates/interface/PhotonFwd.h"
#include "DataFormats/EgammaCandidates/interface/Conversion.h"
#include "DataFormats/EgammaCandidates/interface/ConversionFwd.h"
#include "MitAna/DataTree/interface/Names.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerPhotons::FillerPhotons(const edm::ParameterSet &cfg, bool active) :
  BaseFiller(cfg,"Photons",active),
  edmName_(Conf().getUntrackedParameter<string>("edmName","photons")),
  mitName_(Conf().getUntrackedParameter<string>("mitName",Names::gkPhotonBrn)),
  conversionMapName_(Conf().getUntrackedParameter<string>("conversionMapName","")),
  barrelSuperClusterMapName_(Conf().getUntrackedParameter<string>("barrelSuperClusterMapName","")),
  endcapSuperClusterMapName_(Conf().getUntrackedParameter<string>("endcapSuperClusterMapName","")),
  photons_(new mithep::PhotonArr(16)), 
  conversionMap_(0),
  barrelSuperClusterMap_(0),
  endcapSuperClusterMap_(0)
{
  // Constructor.
}

//--------------------------------------------------------------------------------------------------
FillerPhotons::~FillerPhotons()
{
  // Destructor.

  delete photons_;
}

//--------------------------------------------------------------------------------------------------
void FillerPhotons::BookDataBlock(TreeWriter &tws)
{
  // Add photon branch to tree and get the map.

  tws.AddBranch(mitName_.c_str(),&photons_);

  if (!conversionMapName_.empty())
    conversionMap_ = OS()->get<ConversionMap>(conversionMapName_.c_str());
  if (!barrelSuperClusterMapName_.empty())
    barrelSuperClusterMap_ = OS()->get<SuperClusterMap>(barrelSuperClusterMapName_.c_str());
  if (!endcapSuperClusterMapName_.empty())
    endcapSuperClusterMap_ = OS()->get<SuperClusterMap>(endcapSuperClusterMapName_.c_str());
}

//--------------------------------------------------------------------------------------------------
void FillerPhotons::FillDataBlock(const edm::Event      &event, 
                                      const edm::EventSetup &setup)
{
  // Fill photon array.

  photons_->Reset();

  Handle<reco::PhotonCollection> hPhotonProduct;
  GetProduct(edmName_, hPhotonProduct, event);

  const reco::PhotonCollection inPhotons = *(hPhotonProduct.product());  
  
  for (reco::PhotonCollection::const_iterator iP = inPhotons.begin(); 
       iP != inPhotons.end(); ++iP) {

    mithep::Photon *outPhoton = photons_->Allocate();
    new (outPhoton) mithep::Photon(iP->px(),iP->py(),iP->pz(),iP->energy());
    outPhoton->SetIsConverted(iP->isConverted());
    //make links to conversions
    if (iP->isConverted() && conversionMap_) {
      std::vector<reco::ConversionRef> conversionRefs = iP->conversions();
      for (std::vector<reco::ConversionRef>::const_iterator conversionRef = 
             conversionRefs.begin(); conversionRef != conversionRefs.end(); ++conversionRef) {
        outPhoton->AddConversion(conversionMap_->GetMit(*conversionRef));
      }
    }
    //make link to supercluster
    if (barrelSuperClusterMap_ && endcapSuperClusterMap_ && iP->superCluster().isNonnull())
      if(barrelSuperClusterMap_->HasMit(iP->superCluster())) {
        outPhoton->SetSuperCluster(barrelSuperClusterMap_->GetMit(iP->superCluster()));        
      } else {
        outPhoton->SetSuperCluster(endcapSuperClusterMap_->GetMit(iP->superCluster()));
      }
  }

  photons_->Trim();
}
