// $Id: FillerPhotons.cc,v 1.14 2009/06/15 15:00:26 loizides Exp $

#include "MitProd/TreeFiller/interface/FillerPhotons.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/EgammaCandidates/interface/Photon.h"
#include "DataFormats/EgammaCandidates/interface/PhotonFwd.h"
#include "DataFormats/EgammaCandidates/interface/Conversion.h"
#include "DataFormats/EgammaCandidates/interface/ConversionFwd.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataTree/interface/PhotonCol.h"
#include "MitProd/ObjectService/interface/ObjectService.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerPhotons::FillerPhotons(const edm::ParameterSet &cfg, const char *name, bool active) :
  BaseFiller(cfg,name,active),
  edmName_(Conf().getUntrackedParameter<string>("edmName","photons")),
  mitName_(Conf().getUntrackedParameter<string>("mitName",Names::gkPhotonBrn)),
  conversionMapName_(Conf().getUntrackedParameter<string>("conversionMapName","")),
  barrelSuperClusterMapName_(Conf().getUntrackedParameter<string>("barrelSuperClusterMapName","")),
  endcapSuperClusterMapName_(Conf().getUntrackedParameter<string>("endcapSuperClusterMapName","")),
  phIDCutBasedTightName_(Conf().getUntrackedParameter<string>("phIDCutBasedTightName","PhotonIDProd:PhotonCutBasedIDTight")),
  phIDCutBasedLooseName_(Conf().getUntrackedParameter<string>("phIDCutBasedLooseName","PhotonIDProd:PhotonCutBasedIDLoose")),  
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

  tws.AddBranch(mitName_,&photons_);
  OS()->add<mithep::PhotonArr>(photons_,mitName_);

  if (!conversionMapName_.empty()) {
    conversionMap_ = OS()->get<ConversionMap>(conversionMapName_);
    if (conversionMap_)
      AddBranchDep(mitName_,conversionMap_->GetBrName());
  }
  if (!barrelSuperClusterMapName_.empty()) {
    barrelSuperClusterMap_ = OS()->get<SuperClusterMap>(barrelSuperClusterMapName_);
    if (barrelSuperClusterMap_)
      AddBranchDep(mitName_,barrelSuperClusterMap_->GetBrName());
  }
  if (!endcapSuperClusterMapName_.empty()) {
    endcapSuperClusterMap_ = OS()->get<SuperClusterMap>(endcapSuperClusterMapName_);
    if (endcapSuperClusterMap_)
      AddBranchDep(mitName_,endcapSuperClusterMap_->GetBrName());
  }
}

//--------------------------------------------------------------------------------------------------
void FillerPhotons::FillDataBlock(const edm::Event      &event, 
                                  const edm::EventSetup &setup)
{
  // Fill photon array.

  photons_->Delete();

  // get photon collection
  Handle<reco::PhotonCollection> hPhotonProduct;
  GetProduct(edmName_, hPhotonProduct, event);
  const reco::PhotonCollection inPhotons = *(hPhotonProduct.product());  
  
  // handles to get the photon ID information
  Handle<edm::ValueMap<bool> > phidLooseMap;
  GetProduct(phIDCutBasedLooseName_, phidLooseMap, event);
  Handle<edm::ValueMap<bool> > phidTightMap;
  GetProduct(phIDCutBasedTightName_, phidTightMap, event);
  
  for (reco::PhotonCollection::const_iterator iP = inPhotons.begin(); 
       iP != inPhotons.end(); ++iP) {

    int photonIndex = iP - inPhotons.begin();
    reco::PhotonRef phRef(hPhotonProduct, photonIndex);

    mithep::Photon *outPhoton = photons_->Allocate();
    new (outPhoton) mithep::Photon(iP->px(),iP->py(),iP->pz(),iP->energy());
    outPhoton->SetIsConverted(iP->hasConversionTracks());
    outPhoton->SetR9(iP->r9());
    outPhoton->SetHadOverEm(iP->hadronicOverEm());
    outPhoton->SetHasPixelSeed(iP->hasPixelSeed());
    outPhoton->SetEcalRecHitIso(iP->ecalRecHitSumEtConeDR04());
    //outPhoton->SetHcalRecHitIso(phID->isolationHcalRecHit());
    outPhoton->SetSolidConeTrkIso(iP->trkSumPtSolidConeDR04());
    outPhoton->SetHollowConeTrkIso(iP->trkSumPtHollowConeDR04());
    outPhoton->SetSolidConeNTrk(iP->nTrkSolidConeDR04());
    outPhoton->SetHollowConeNTrk(iP->nTrkHollowConeDR04());
    //still some missing isolation variables here
    //few missing gap flag variables also
    outPhoton->SetIsEBGap(iP->isEBGap());
    outPhoton->SetIsEEGap(iP->isEEGap());
    outPhoton->SetIsEBEEGap(iP->isEBEEGap());
    //outPhoton->SetIsLooseEM(phID->isLooseEM()); //deprecated?
    //moved to valuemaps - TODO
    outPhoton->SetIsLoosePhoton((*phidLooseMap)[phRef]);
    outPhoton->SetIsTightPhoton((*phidTightMap)[phRef]);   

    // make links to conversions
    if (iP->hasConversionTracks() && conversionMap_) {
      reco::ConversionRefVector conversionRefs = iP->conversions();
      for (reco::ConversionRefVector::const_iterator conversionRef = 
             conversionRefs.begin(); conversionRef != conversionRefs.end(); ++conversionRef) {
        outPhoton->AddConversion(conversionMap_->GetMit(*conversionRef));
      }
    }

    // make link to supercluster
    if (barrelSuperClusterMap_ && endcapSuperClusterMap_ && iP->superCluster().isNonnull())
      if(barrelSuperClusterMap_->HasMit(iP->superCluster())) {
        outPhoton->SetSuperCluster(barrelSuperClusterMap_->GetMit(iP->superCluster()));        
      } else {
        outPhoton->SetSuperCluster(endcapSuperClusterMap_->GetMit(iP->superCluster()));
      }
  }
  photons_->Trim();
}
