// $Id: FillerPhotons.cc,v 1.9 2008/09/14 03:24:01 bendavid Exp $

#include "MitProd/TreeFiller/interface/FillerPhotons.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/EgammaCandidates/interface/Photon.h"
#include "DataFormats/EgammaCandidates/interface/PhotonFwd.h"
#include "DataFormats/EgammaCandidates/interface/PhotonIDFwd.h"
#include "DataFormats/EgammaCandidates/interface/PhotonID.h"
#include "DataFormats/EgammaCandidates/interface/PhotonIDAssociation.h"
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
  photonIDName_(Conf().getUntrackedParameter<string>("photonIDName","PhotonIDProd:PhotonAssociatedID")),
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

  //get photon collection
  Handle<reco::PhotonCollection> hPhotonProduct;
  GetProduct(edmName_, hPhotonProduct, event);
  const reco::PhotonCollection inPhotons = *(hPhotonProduct.product());  
  
  //get associated PhotonID objects
  Handle<reco::PhotonIDAssociationCollection> photonIDMapColl;
  GetProduct(photonIDName_, photonIDMapColl, event);
  const reco::PhotonIDAssociationCollection *photonIDMap = photonIDMapColl.product();

  for (reco::PhotonCollection::const_iterator iP = inPhotons.begin(); 
       iP != inPhotons.end(); ++iP) {

    int photonIndex = iP - inPhotons.begin();
    edm::Ref<reco::PhotonCollection> photonref(hPhotonProduct, photonIndex);
    reco::PhotonIDAssociationCollection::const_iterator photonIter = photonIDMap->find(photonref);
    const reco::PhotonIDRef &phID = photonIter->val;
    const reco::PhotonRef &photon = photonIter->key;

    mithep::Photon *outPhoton = photons_->Allocate();
    new (outPhoton) mithep::Photon(photon->px(),photon->py(),photon->pz(),photon->energy());
    outPhoton->SetIsConverted(photon->isConverted());    
    outPhoton->SetR9(phID->r9());
    outPhoton->SetHadOverEm(photon->hadronicOverEm());
    outPhoton->SetHasPixelSeed(photon->hasPixelSeed());
    outPhoton->SetEcalRecHitIso(phID->isolationEcalRecHit());
    outPhoton->SetHcalRecHitIso(phID->isolationHcalRecHit());
    outPhoton->SetSolidConeTrkIso(phID->isolationSolidTrkCone());
    outPhoton->SetHollowConeTrkIso(phID->isolationHollowTrkCone());
    outPhoton->SetSolidConeNTrk(phID->nTrkSolidCone());
    outPhoton->SetHollowConeNTrk(phID->nTrkHollowCone());
    outPhoton->SetIsEBGap(phID->isEBGap());
    outPhoton->SetIsEEGap(phID->isEEGap());
    outPhoton->SetIsEBEEGap(phID->isEBEEGap());
    outPhoton->SetIsLooseEM(phID->isLooseEM());
    outPhoton->SetIsLoosePhoton(phID->isLoosePhoton());
    outPhoton->SetIsTightPhoton(phID->isTightPhoton());   

    //make links to conversions
    if (photon->isConverted() && conversionMap_) {
      std::vector<reco::ConversionRef> conversionRefs = photon->conversions();
      for (std::vector<reco::ConversionRef>::const_iterator conversionRef = 
             conversionRefs.begin(); conversionRef != conversionRefs.end(); ++conversionRef) {
        outPhoton->AddConversion(conversionMap_->GetMit(*conversionRef));
      }
    }
    //make link to supercluster
    if (barrelSuperClusterMap_ && endcapSuperClusterMap_ && photon->superCluster().isNonnull())
      if(barrelSuperClusterMap_->HasMit(photon->superCluster())) {
        outPhoton->SetSuperCluster(barrelSuperClusterMap_->GetMit(photon->superCluster()));        
      } else {
        outPhoton->SetSuperCluster(endcapSuperClusterMap_->GetMit(photon->superCluster()));
      }
  }

  photons_->Trim();
}
