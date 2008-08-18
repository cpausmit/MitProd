// $Id: FillerElectrons.cc,v 1.11 2008/08/08 11:21:08 sixie Exp $

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
#include "RecoEgamma/EgammaIsolationAlgos/interface/ElectronTkIsolation.h"
#include "RecoEgamma/EgammaIsolationAlgos/interface/EgammaEcalIsolation.h"
#include "MitAna/DataTree/interface/Track.h"
#include "MitAna/DataTree/interface/Names.h"
#include "RecoEcal/EgammaCoreTools/interface/EcalClusterLazyTools.h"
#include "AnalysisDataFormats/Egamma/interface/ElectronID.h"
#include "AnalysisDataFormats/Egamma/interface/ElectronIDAssociation.h"

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
  eIDCutBasedName_(Conf().getUntrackedParameter<string>("eIDCutBasedName","eidCutBased")),         
  eIDCutBasedClassesName_(Conf().getUntrackedParameter<string>("eIDCutBasedClassesName",
                                                               "eidCutBasedClasses'")),
  eIDLikelihoodName_(Conf().getUntrackedParameter<string>("eIDLikelihood","eidLikelihood")),     
  eIDNeuralNetName_(Conf().getUntrackedParameter<string>("eIDNeuralNet","eidNeuralNet")),

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
    gsfTrackMap_ = OS()->get<GsfTrackMap>(gsfTrackMapName_.c_str());
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
  
  //Get Official Electron ID Results
  //cout << "try electron ID \n";
  Handle<reco::GsfElectronRefVector> hCutBasedElectronIDProduct;
  GetProduct(eIDCutBasedName_ , hCutBasedElectronIDProduct, event);
  Handle<reco::GsfElectronRefVector> hCutBasedClassesElectronIDProduct;
  GetProduct(eIDCutBasedClassesName_ , hCutBasedClassesElectronIDProduct, event);
  Handle<reco::GsfElectronRefVector> hLikelihoodElectronIDProduct;
  GetProduct(eIDLikelihoodName_ , hLikelihoodElectronIDProduct, event);
  Handle<reco::GsfElectronRefVector> hNeuralNetElectronIDProduct;
  GetProduct(eIDNeuralNetName_ , hNeuralNetElectronIDProduct, event);

  const reco::GsfElectronCollection inElectrons = *(hElectronProduct.product());
  //loop over electrons
  unsigned int i = 0;
  for (reco::GsfElectronCollection::const_iterator iM = inElectrons.begin(); 
       iM != inElectrons.end(); ++iM) {
    mithep::Electron *outElectron = electrons_->AddNew();
         
    outElectron->SetESuperClusterOverP( iM->eSuperClusterOverP() ) ;
    outElectron->SetESeedClusterOverPout( iM->eSeedClusterOverPout() ) ;
    outElectron->SetDeltaEtaSuperClusterTrackAtVtx( iM->deltaEtaSuperClusterTrackAtVtx() ) ;
    outElectron->SetDeltaEtaSeedClusterTrackAtCalo( iM->deltaEtaSeedClusterTrackAtCalo() ) ;
    outElectron->SetDeltaPhiSuperClusterTrackAtVtx( iM->deltaPhiSuperClusterTrackAtVtx() ) ;
    outElectron->SetDeltaPhiSeedClusterTrackAtCalo( iM->deltaPhiSeedClusterTrackAtCalo() ) ;
    outElectron->SetHadronicOverEm( iM->hadronicOverEm() ) ;
    outElectron->SetIsEnergyScaleCorrected( iM->isEnergyScaleCorrected() ) ;
    outElectron->SetIsMomentumCorrected( iM->isMomentumCorrected() ) ;
    outElectron->SetNumberOfClusters( iM->numberOfClusters() ) ;
    outElectron->SetClassification( iM->classification() ) ;

    //initialize
    outElectron->SetE33( -999.0 ) ;
    outElectron->SetE55( -999.0 ) ;
    outElectron->SetCovEtaEta( -999.0 ) ;
    outElectron->SetCovEtaPhi( -999.0 ) ;
    outElectron->SetCovPhiPhi( -999.0 ) ;        
   
    EcalClusterLazyTools lazyTools( event, setup, edm::InputTag(barrelEcalRecHitName_), 
                                    edm::InputTag(endcapEcalRecHitName_) );        

    //Shower shape variables
    outElectron->SetE33	 (lazyTools.e3x3( *(iM->superCluster()->seed()) ));
    outElectron->SetE55	 (lazyTools.e5x5( *(iM->superCluster()->seed()) ));
    std::vector<float> vCov = lazyTools.covariances( *(iM->superCluster()->seed()) );
    outElectron->SetCovEtaEta(vCov[0]);
    outElectron->SetCovEtaPhi(vCov[1]);
    outElectron->SetCovPhiPhi(vCov[2]);    

    //Compute Isolations
    //Get the barrel basicClusters 
    edm::Handle<reco::BasicClusterCollection> barrelBasicClusterHandle;    
    GetProduct(barrelBasicClusterName_, barrelBasicClusterHandle, event);
    const reco::BasicClusterCollection* barrelBasicClusters = barrelBasicClusterHandle.product();

    //Get the  endcap basicClusters
    edm::Handle<reco::BasicClusterCollection> endcapBasicClusterHandle;
    GetProduct(endcapBasicClusterName_, endcapBasicClusterHandle, event);
    const reco::BasicClusterCollection* endcapBasicClusters = endcapBasicClusterHandle.product();
    
    //Get the barrel superClusters
    edm::Handle<reco::SuperClusterCollection> barrelSuperClusterHandle;
    GetProduct(barrelSuperClusterName_, barrelSuperClusterHandle, event);
    const reco::SuperClusterCollection* barrelSuperClusters = barrelSuperClusterHandle.product();  

    //Get the endcap superClusters
    edm::Handle<reco::SuperClusterCollection> endcapSuperClusterHandle;
    GetProduct(endcapSuperClusterName_, endcapSuperClusterHandle, event);
    const reco::SuperClusterCollection* endcapSuperClusters = endcapSuperClusterHandle.product();

    //find out whether this electron super cluster is in the barrel or endcap
    bool isBarrel=false ;    
    if(barrelSuperClusterMap_->HasMit(iM->superCluster()))
    //if (iM->superCluster()->eta() < 1.49) //If above method doesn't work, just use eta
      isBarrel = true;
   
    //compute ECAL isolation
    double extRadius = 0.3;
    double etLow = 0.0;
    EgammaEcalIsolation *myEcalIsolation = 
      new EgammaEcalIsolation(extRadius,etLow,barrelBasicClusters,barrelSuperClusters);

    if (!isBarrel) {
      delete myEcalIsolation;
      myEcalIsolation = 
        new EgammaEcalIsolation(extRadius,etLow,endcapBasicClusters,endcapSuperClusters);
    }
    double ecalIsoValue = myEcalIsolation->getEcalEtSum(&(*iM));
    
    //Compute Track Isolation        
    edm::Handle<reco::TrackCollection> tracks;
    event.getByLabel("generalTracks",tracks);
    const reco::TrackCollection* trackCollection = tracks.product();
    extRadius = 0.2;
    double intRadius = 0.02;
    double maxVtxDist = 0.1;      
    double ptMin = 1.5;
    ElectronTkIsolation myTkIsolation (extRadius,intRadius,ptMin,maxVtxDist,trackCollection) ;
    double trackIsoValue = myTkIsolation.getPtTracks(&(*iM));

    //Fill the isolation values
    outElectron->SetCaloIsolation( ecalIsoValue ) ;
    outElectron->SetTrackIsolation( trackIsoValue ) ;
   
    //Make proper links to Tracks and Super Clusters
    if (gsfTrackMap_ && iM->gsfTrack().isNonnull()) 
      outElectron->SetGsfTrk(gsfTrackMap_->GetMit(iM->gsfTrack()));
    if (trackerTrackMap_ && iM->track().isNonnull()) 
      outElectron->SetTrackerTrk(trackerTrackMap_->GetMit(iM->track()));
    if (barrelSuperClusterMap_ && endcapSuperClusterMap_ && iM->superCluster().isNonnull())
      if(isBarrel) {
        outElectron->SetSuperCluster(barrelSuperClusterMap_->GetMit(iM->superCluster()));        
      } else {
        outElectron->SetSuperCluster(endcapSuperClusterMap_->GetMit(iM->superCluster()));
      }
  
  }  
  electrons_->Trim();
}