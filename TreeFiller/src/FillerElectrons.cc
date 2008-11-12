// $Id: FillerElectrons.cc,v 1.21 2008/11/06 13:09:23 sixie Exp $

#include "MitProd/TreeFiller/interface/FillerElectrons.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h"
#include "DataFormats/EgammaReco/interface/ClusterShape.h"
#include "DataFormats/EgammaReco/interface/BasicClusterShapeAssociation.h"
#include "DataFormats/Common/interface/RefToPtr.h"
#include "RecoEgamma/EgammaIsolationAlgos/interface/ElectronTkIsolation.h"
#include "RecoEgamma/EgammaIsolationAlgos/interface/EgammaEcalIsolation.h"
#include "RecoEgamma/EgammaIsolationAlgos/interface/EgammaTowerIsolation.h"
#include "MitEdm/Producers/interface/RefToBaseToPtr.h"
#include "MitAna/DataTree/interface/Track.h"
#include "MitAna/DataTree/interface/Names.h"
#include "RecoEcal/EgammaCoreTools/interface/EcalClusterLazyTools.h"
#include "AnalysisDataFormats/Egamma/interface/ElectronID.h"
#include "AnalysisDataFormats/Egamma/interface/ElectronIDAssociation.h"

#include "DataFormats/Common/interface/ValueMap.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerElectrons::FillerElectrons(const edm::ParameterSet &cfg, bool active) :
  BaseFiller(cfg,"Electrons",active),
  edmName_(Conf().getUntrackedParameter<string>("edmName","pixelMatchGsfElectrons")),
  mitName_(Conf().getUntrackedParameter<string>("mitName",Names::gkElectronBrn)),
  gsfTrackMapName_(Conf().getUntrackedParameter<string>("gsfTrackMapName","")),
  trackerTrackMapName_(Conf().getUntrackedParameter<string>("trackerTrackMapName","")),
  barrelEcalRecHitName_(Conf().getUntrackedParameter<string>("barrelEcalRecHitName","")),
  endcapEcalRecHitName_(Conf().getUntrackedParameter<string>("endcapEcalRecHitName","")),
  barrelSuperClusterName_(Conf().getUntrackedParameter<string>("barrelSuperClusterName", "")),
  endcapSuperClusterName_(Conf().getUntrackedParameter<string>("endcapSuperClusterName", "")),
  barrelBasicClusterName_(Conf().getUntrackedParameter<string>("barrelBasicClusterName", "")),
  endcapBasicClusterName_(Conf().getUntrackedParameter<string>("endcapBasicClusterName", "")),
  barrelSuperClusterMapName_(Conf().getUntrackedParameter<string>("barrelSuperClusterMapName","")),
  endcapSuperClusterMapName_(Conf().getUntrackedParameter<string>("endcapSuperClusterMapName","")),
  eIDCutBasedTightName_(Conf().getUntrackedParameter<string>("eIDCutBasedTightName","eidTight")),
  eIDCutBasedLooseName_(Conf().getUntrackedParameter<string>("eIDCutBasedLooseName","eidLoose")),  
  eIDLikelihoodName_(Conf().getUntrackedParameter<string>("eIDLikelihood","eidLikelihood")),     
  eIDNeuralNetName_(Conf().getUntrackedParameter<string>("eIDNeuralNet","eidNeuralNet")),
  isoTrackColName_(Conf().getUntrackedParameter<string>
                   ("IsolationTrackCollectionName","generalTracks")),
  isoCaloTowerColName_(Conf().getUntrackedParameter<string>
                       ("IsolationCaloTowerCollectionName","towerMaker")),
  ecalJurassicIsoName_(Conf().getUntrackedParameter<string>
                       ("EcalJurassicIsolationName","eleIsoFromDepsEcalFromHits")),
  hcalJurassicIsoName_(Conf().getUntrackedParameter<string>
                       ("HcalJurassicIsolationName","eleIsoFromDepsHcalFromHits")),
  trackerIsoName_(Conf().getUntrackedParameter<string>
		  ("TrackerIsolationName","eleIsoFromDepsTk")),
  gsfTrackAssocName_(Conf().getUntrackedParameter<string>("gsfTrackAssocName","")),
  electrons_(new mithep::ElectronArr(16)),
  gsfTrackMap_(0),
  trackerTrackMap_(0),
  barrelSuperClusterMap_(0),
  endcapSuperClusterMap_(0)
{
  // Constructor.
}

//--------------------------------------------------------------------------------------------------
FillerElectrons::~FillerElectrons()
{
  // Destructor.

  delete electrons_;
}

//--------------------------------------------------------------------------------------------------
void FillerElectrons::BookDataBlock(TreeWriter &tws)
{
  // Add electron branch to our tree and get our maps.

  tws.AddBranch(mitName_.c_str(),&electrons_);

  if (!gsfTrackMapName_.empty()) 
    gsfTrackMap_ = OS()->get<TrackMap>(gsfTrackMapName_.c_str());
  if (!trackerTrackMapName_.empty()) 
    trackerTrackMap_ = OS()->get<TrackMap>(trackerTrackMapName_.c_str());  
  if (!barrelSuperClusterMapName_.empty())
    barrelSuperClusterMap_ = OS()->get<SuperClusterMap>(barrelSuperClusterMapName_.c_str());
  if (!endcapSuperClusterMapName_.empty())
    endcapSuperClusterMap_ = OS()->get<SuperClusterMap>(endcapSuperClusterMapName_.c_str());
}

//--------------------------------------------------------------------------------------------------
void FillerElectrons::FillDataBlock(const edm::Event &event, const edm::EventSetup &setup)			 
{
  // Fill electrons from edm collection into our collection.

  electrons_->Reset();
  
  Handle<reco::GsfElectronCollection> hElectronProduct;
  GetProduct(edmName_, hElectronProduct, event);
  
  //Handles to get the electron ID information
  Handle<edm::ValueMap<float> > eidLooseMap;
  GetProduct(eIDCutBasedLooseName_, eidLooseMap, event);
  Handle<edm::ValueMap<float> > eidTightMap;
  GetProduct(eIDCutBasedTightName_, eidTightMap, event);
  Handle<edm::ValueMap<float> > eidLikelihoodMap;
  GetProduct(eIDLikelihoodName_, eidLikelihoodMap, event);

  //get gsf track association map if needed
  mitedm::TrackAssociation gsfAssociation;
  if (trackerTrackMap_ && !gsfTrackAssocName_.empty()) {
    Handle<mitedm::TrackAssociation> gsfAssociationProduct;
    GetProduct(gsfTrackAssocName_, gsfAssociationProduct, event);  
    gsfAssociation = *(gsfAssociationProduct.product());
  }
  
  
  const reco::GsfElectronCollection inElectrons = *(hElectronProduct.product());
  //loop over electrons

  for (reco::GsfElectronCollection::const_iterator iM = inElectrons.begin(); 
       iM != inElectrons.end(); ++iM) {

    //the index and Ref are needed for the eID association Map
    unsigned int iElectron = iM - inElectrons.begin();
    reco::GsfElectronRef eRef(hElectronProduct, iElectron);

    mithep::Electron *outElectron = electrons_->AddNew();
         
    outElectron->SetESuperClusterOverP( iM->eSuperClusterOverP() ) ;
    outElectron->SetESeedClusterOverPout( iM->eSeedClusterOverPout() ) ;
    outElectron->SetPIn( iM->trackMomentumAtVtx().R() ) ;
    outElectron->SetPOut( iM->trackMomentumOut().R() );
    outElectron->SetDeltaEtaSuperClusterTrackAtVtx( iM->deltaEtaSuperClusterTrackAtVtx() ) ;
    outElectron->SetDeltaEtaSeedClusterTrackAtCalo( iM->deltaEtaSeedClusterTrackAtCalo() ) ;
    outElectron->SetDeltaPhiSuperClusterTrackAtVtx( iM->deltaPhiSuperClusterTrackAtVtx() ) ;
    outElectron->SetDeltaPhiSeedClusterTrackAtCalo( iM->deltaPhiSeedClusterTrackAtCalo() ) ;
    outElectron->SetHadronicOverEm( iM->hadronicOverEm() ) ;
    outElectron->SetIsEnergyScaleCorrected( iM->isEnergyScaleCorrected() ) ;
    outElectron->SetIsMomentumCorrected( iM->isMomentumCorrected() ) ;
    outElectron->SetNumberOfClusters( iM->numberOfClusters() ) ;
    outElectron->SetClassification( iM->classification() ) ;

    //shower shape variables   
    EcalClusterLazyTools lazyTools(event, setup, edm::InputTag(barrelEcalRecHitName_), 
                                   edm::InputTag(endcapEcalRecHitName_));
    outElectron->SetE33(lazyTools.e3x3(*(iM->superCluster()->seed())));
    outElectron->SetE55(lazyTools.e5x5(*(iM->superCluster()->seed())));
    std::vector<float> vCov = lazyTools.covariances(*(iM->superCluster()->seed()));
    outElectron->SetCovEtaEta(vCov[0]);
    outElectron->SetCovEtaPhi(vCov[1]);
    outElectron->SetCovPhiPhi(vCov[2]);    
    std::vector<float> vCov2 = lazyTools.localCovariances(*(iM->superCluster()->seed()));
    outElectron->SetCoviEtaiEta(vCov2[0]);

    //compute isolations
    //get the barrel BasicClusters 
    edm::Handle<reco::BasicClusterCollection> barrelBasicClusterHandle;    
    GetProduct(barrelBasicClusterName_, barrelBasicClusterHandle, event);
    const reco::BasicClusterCollection* barrelBasicClusters = barrelBasicClusterHandle.product();

    //get the  endcap BasicClusters
    edm::Handle<reco::BasicClusterCollection> endcapBasicClusterHandle;
    GetProduct(endcapBasicClusterName_, endcapBasicClusterHandle, event);
    const reco::BasicClusterCollection* endcapBasicClusters = endcapBasicClusterHandle.product();
    
    //get the barrel SuperClusters
    edm::Handle<reco::SuperClusterCollection> barrelSuperClusterHandle;
    GetProduct(barrelSuperClusterName_, barrelSuperClusterHandle, event);
    const reco::SuperClusterCollection* barrelSuperClusters = barrelSuperClusterHandle.product();  

    //Get the endcap SuperClusters
    edm::Handle<reco::SuperClusterCollection> endcapSuperClusterHandle;
    GetProduct(endcapSuperClusterName_, endcapSuperClusterHandle, event);
    const reco::SuperClusterCollection* endcapSuperClusters = endcapSuperClusterHandle.product();

    //find out whether this electron super cluster is in the barrel or endcap
    bool isBarrel=false ;    
    if(barrelSuperClusterMap_->HasMit(iM->superCluster()))
      isBarrel = true;
   
    //compute ECAL isolation
    double extRadius = 0.3;
    double etLow = 0.0;
    EgammaEcalIsolation *myEcalIsolation = 0;
    if (!isBarrel) {
      myEcalIsolation = 
        new EgammaEcalIsolation(extRadius,etLow,endcapBasicClusters,endcapSuperClusters);
    } else { 
      myEcalIsolation = 
        new EgammaEcalIsolation(extRadius,etLow,barrelBasicClusters,barrelSuperClusters);
    }

    double ecalIsoValue = myEcalIsolation->getEcalEtSum(&(*iM));
    
    //compute CaloTower isolation
    edm::Handle<CaloTowerCollection> caloTowers;
    GetProduct(isoCaloTowerColName_, caloTowers, event);
    extRadius = 0.3;
    etLow = 0.0;
    double intRadius = 0.02;
    int hcalDepth = -1;  //-1 means we take all depths.
    EgammaTowerIsolation *myTowerIsolation = 
      new EgammaTowerIsolation (extRadius, intRadius, etLow, hcalDepth, caloTowers.product());
    double towerIsoValue = myTowerIsolation->getTowerEtSum(&(*iM));
    outElectron->SetCaloTowerIsolation( towerIsoValue );
  
    //fill the isolation values
    outElectron->SetCaloIsolation( ecalIsoValue ) ;

    //get and fill Track isolation        
    Handle<edm::ValueMap<double> > eleIsoFromDepsTkValueMap;
    GetProduct(trackerIsoName_, eleIsoFromDepsTkValueMap, event);
    outElectron->SetTrackIsolation((*eleIsoFromDepsTkValueMap)[eRef]); 
   
    //get and fill Jurassic isolation values
    Handle<edm::ValueMap<double> > eleIsoFromDepsEcalFromHitsValueMap;
    GetProduct(ecalJurassicIsoName_, eleIsoFromDepsEcalFromHitsValueMap, event);
    Handle<edm::ValueMap<double> > eleIsoFromDepsHcalFromHitsValueMap;
    GetProduct(hcalJurassicIsoName_, eleIsoFromDepsHcalFromHitsValueMap, event);

    outElectron->SetEcalJurassicIsolation((*eleIsoFromDepsEcalFromHitsValueMap)[eRef]);    
    outElectron->SetHcalJurassicIsolation((*eleIsoFromDepsHcalFromHitsValueMap)[eRef]);

    //make proper links to Tracks and Super Clusters
    if (gsfTrackMap_ && iM->gsfTrack().isNonnull()) 
      outElectron->SetGsfTrk(gsfTrackMap_->GetMit(refToPtr(iM->gsfTrack())));
    //make tracker track links, relinking from gsf track associations if configured and 
    //link is otherwise absent
    if (trackerTrackMap_) {
      if (iM->track().isNonnull()) 
        outElectron->SetTrackerTrk(trackerTrackMap_->GetMit(refToPtr(iM->track())));
      else if (!gsfTrackAssocName_.empty() && iM->gsfTrack().isNonnull()) {
        reco::TrackBaseRef gsfRef(iM->gsfTrack());
        std::vector<std::pair<reco::TrackBaseRef, double> > matchedTracks;
        try {
          matchedTracks = gsfAssociation[gsfRef];
        }
        catch (edm::Exception &ex) {
        }
        //take the best match, but only if more than 50% of the hits came from the original
        //gsf track
        reco::TrackBaseRef trackerRef;
        double rMax = 0.0;
        for (uint imatch=0; imatch<matchedTracks.size(); ++imatch) {
          std::pair<reco::TrackBaseRef, double> &match = matchedTracks.at(imatch);
          double r = match.second;
          if ( r>rMax && r>0.5 ) {
            rMax = r;
            trackerRef = match.first;
          }
        }
        if (trackerRef.isNonnull())
          outElectron->SetTrackerTrk(trackerTrackMap_->GetMit(mitedm::refToBaseToPtr(trackerRef)));
      }
    }
    if (barrelSuperClusterMap_ && endcapSuperClusterMap_ && iM->superCluster().isNonnull())
      if(isBarrel) {
        outElectron->SetSuperCluster(barrelSuperClusterMap_->GetMit(iM->superCluster()));        
      } else {
        outElectron->SetSuperCluster(endcapSuperClusterMap_->GetMit(iM->superCluster()));
      }

    //fill Electron ID information
    outElectron->SetPassLooseID((*eidLooseMap)[eRef]);
    outElectron->SetPassTightID((*eidTightMap)[eRef]);
    outElectron->SetIDLikelihood((*eidLikelihoodMap)[eRef]);
  }  
  electrons_->Trim();
}
