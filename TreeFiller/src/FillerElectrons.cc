// $Id: FillerElectrons.cc,v 1.10 2008/07/31 12:34:04 loizides Exp $

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
  superClusterMapName_(Conf().getUntrackedParameter<string>("superClusterMapName", "")),
  electrons_(new mithep::ElectronArr(16)),

  gsfTrackMap_(0),
  trackerTrackMap_(0),
  superClusterMap_(0)
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
  if (!superClusterMapName_.empty())
    superClusterMap_ = OS()->get<SuperClusterMap>(superClusterMapName_.c_str());

}

//--------------------------------------------------------------------------------------------------
void FillerElectrons::FillDataBlock(const edm::Event &event, const edm::EventSetup &setup)			 
{
  // Fill electrons from edm collection into our collection.

  electrons_->Reset();
  
  Handle<reco::GsfElectronCollection> hElectronProduct;
  GetProduct(edmName_, hElectronProduct, event);
  
  const reco::GsfElectronCollection inElectrons = *(hElectronProduct.product());
  
  
  //get association of clusters to their shapes for EcalBarrel
  edm::Handle<reco::BasicClusterShapeAssociationCollection> barrelClShpHandle ;
  try { 
    event.getByLabel ("hybridSuperClusters","hybridShapeAssoc", barrelClShpHandle);
  }
  catch ( cms::Exception& ex ) {
    printf("Can't get ECAL barrel Cluster Shape Collection\n");
  }
  const reco::BasicClusterShapeAssociationCollection& barrelClShpMap = *barrelClShpHandle ;
  
  //get the association of the clusters to their shapes for EcalEndcap
  edm::Handle<reco::BasicClusterShapeAssociationCollection> endcapClShpHandle ;
  try {
    event.getByLabel ("islandBasicClusters","islandEndcapShapeAssoc", endcapClShpHandle);
  }
  catch ( cms::Exception& ex ) {
    printf("Can't get ECAL endcap Cluster Shape Collection\n");
  }
  const reco::BasicClusterShapeAssociationCollection& endcapClShpMap = *endcapClShpHandle ;

  //loop over electrons
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
    outElectron->SetSuperClusterPx( iM->px() );
    outElectron->SetSuperClusterPy( iM->py() );
    outElectron->SetSuperClusterPz( iM->pz() );
    outElectron->SetSuperClusterE( iM->energy() );

    //initialize
    outElectron->SetE33( -999.0 ) ;
    outElectron->SetE55( -999.0 ) ;
    outElectron->SetCovEtaEta( -999.0 ) ;
    outElectron->SetCovEtaPhi( -999.0 ) ;
    outElectron->SetCovPhiPhi( -999.0 ) ;
    bool hasBarrel=true ;
    bool hasEndcap=true ;    
    
    reco::SuperClusterRef sclusRef = iM->get<reco::SuperClusterRef> () ;            
    reco::BasicClusterShapeAssociationCollection::const_iterator seedShpItr ;
    //find the cluster shape object associated with the electron supercluster
    seedShpItr = barrelClShpMap.find (sclusRef->seed()) ;
    if ( seedShpItr==barrelClShpMap.end())  {
      hasBarrel=false ;
      seedShpItr=endcapClShpMap.find (sclusRef->seed()) ;
      if ( seedShpItr==endcapClShpMap.end() ) hasEndcap=false;
    }
    if (hasBarrel || hasEndcap) {
      //fill shape variables
      const reco::ClusterShapeRef& sClShape = seedShpItr->val;
      outElectron->SetE33		      (sClShape->e3x3()     );
      outElectron->SetE55		      (sClShape->e5x5()     );
      outElectron->SetCovEtaEta		      (sClShape->covEtaEta());
      outElectron->SetCovEtaPhi		      (sClShape->covEtaPhi());
      outElectron->SetCovPhiPhi		      (sClShape->covPhiPhi());
    }
    
    //Compute Isolations
    //Get the barrel basicClusters 
    edm::Handle<reco::BasicClusterCollection> barrelBasicClusterHandle;
    event.getByLabel("hybridSuperClusters", barrelBasicClusterHandle);     
    const reco::BasicClusterCollection* barrelBasicClusters = barrelBasicClusterHandle.product();

    //Get the  endcap basicClusters
    edm::Handle<reco::BasicClusterCollection> endcapBasicClusterHandle;
    event.getByLabel("islandBasicClusters", "islandEndcapBasicClusters", endcapBasicClusterHandle);
    const reco::BasicClusterCollection* endcapBasicClusters = endcapBasicClusterHandle.product();
    
    //Get the superClusters
    edm::Handle<reco::SuperClusterCollection> superClusterHandle;
    event.getByLabel("hybridSuperClusters", superClusterHandle);
    const reco::SuperClusterCollection* superClusters = superClusterHandle.product();
    
    //compute ECAL isolation
    double extRadius = 0.3;
    double etLow = 0.0;
    EgammaEcalIsolation myEcalIsolation(extRadius,etLow,barrelBasicClusters,superClusters);
    double ecalIsoValue = myEcalIsolation.getEcalEtSum(&(*iM));

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
    if (superClusterMap_ && iM->superCluster().isNonnull())
      outElectron->SetSuperCluster(superClusterMap_->GetMit(iM->superCluster()));
  }
  electrons_->Trim();
}
