// $Id: FillerElectrons.cc,v 1.53 2011/03/22 00:22:51 bendavid Exp $

#include "MitProd/TreeFiller/interface/FillerElectrons.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h"
#include "DataFormats/EgammaReco/interface/ClusterShape.h"
#include "DataFormats/EgammaReco/interface/BasicClusterShapeAssociation.h"
#include "DataFormats/Common/interface/RefToPtr.h"
#include "DataFormats/Common/interface/ValueMap.h"
#include "AnalysisDataFormats/Egamma/interface/ElectronID.h"
#include "AnalysisDataFormats/Egamma/interface/ElectronIDAssociation.h"
#include "RecoEgamma/EgammaIsolationAlgos/interface/ElectronTkIsolation.h"
#include "RecoEgamma/EgammaIsolationAlgos/interface/EgammaEcalIsolation.h"
#include "RecoEgamma/EgammaIsolationAlgos/interface/EgammaTowerIsolation.h"
#include "RecoEcal/EgammaCoreTools/interface/EcalClusterLazyTools.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/TransientTrack/plugins/TransientTrackBuilderESProducer.h"
#include "RecoVertex/GaussianSumVertexFit/interface/GsfVertexTrackCompatibilityEstimator.h"
#include "TrackingTools/IPTools/interface/IPTools.h"
#include "RecoEgamma/EgammaTools/interface/ConversionFinder.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "MagneticField/Engine/interface/MagneticField.h"
#include "MitAna/DataTree/interface/ElectronCol.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataTree/interface/Track.h"
#include "MitEdm/DataFormats/interface/RefToBaseToPtr.h"
#include "MitProd/ObjectService/interface/ObjectService.h"
#include "MitEdm/DataFormats/interface/DecayPart.h"
#include "MitEdm/ConversionRejection/interface/ConversionMatcher.h"
#include "RecoVertex/VertexTools/interface/LinearizedTrackStateFactory.h"
#include "RecoVertex/VertexTools/interface/VertexTrackFactory.h"
#include "RecoVertex/VertexPrimitives/interface/VertexTrack.h"
#include "RecoVertex/VertexPrimitives/interface/CachingVertex.h"
#include "RecoVertex/KalmanVertexFit/interface/KalmanVertexUpdator.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerElectrons::FillerElectrons(const edm::ParameterSet &cfg, const char *name, bool active) :
  BaseFiller(cfg,name,active),
  edmName_(Conf().getUntrackedParameter<string>("edmName","pixelMatchGsfElectrons")),
  expectedHitsName_(Conf().getUntrackedParameter<string>("expectedHitsName","")),  
  mitName_(Conf().getUntrackedParameter<string>("mitName",Names::gkElectronBrn)),
  gsfTrackMapName_(Conf().getUntrackedParameter<string>("gsfTrackMapName","")),
  trackerTrackMapName_(Conf().getUntrackedParameter<string>("trackerTrackMapName","")),
  barrelSuperClusterMapName_(Conf().getUntrackedParameter<string>("barrelSuperClusterMapName","")),
  endcapSuperClusterMapName_(Conf().getUntrackedParameter<string>("endcapSuperClusterMapName","")),
  pfSuperClusterMapName_(Conf().getUntrackedParameter<string>("pfSuperClusterMapName","")),
  eIDCutBasedTightName_(Conf().getUntrackedParameter<string>("eIDCutBasedTightName","eidTight")),
  eIDCutBasedLooseName_(Conf().getUntrackedParameter<string>("eIDCutBasedLooseName","eidLoose")),
  eIDLikelihoodName_(Conf().getUntrackedParameter<string>("eIDLikelihoodName","")),
  pvEdmName_(Conf().getUntrackedParameter<string>("pvEdmName","offlinePrimaryVertices")),
  pvBSEdmName_(Conf().getUntrackedParameter<string>("pvBSEdmName","offlinePrimaryVerticesWithBS")),  
  recomputeConversionInfo_(Conf().getUntrackedParameter<bool>("recomputeConversionInfo",false)),  
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

  tws.AddBranch(mitName_,&electrons_);
  OS()->add<mithep::ElectronArr>(electrons_,mitName_);

  if (!gsfTrackMapName_.empty()) {
    gsfTrackMap_ = OS()->get<TrackMap>(gsfTrackMapName_);
    if (gsfTrackMap_)
      AddBranchDep(mitName_,gsfTrackMap_->GetBrName());
  }
  if (!trackerTrackMapName_.empty()) {
    trackerTrackMap_ = OS()->get<TrackMap>(trackerTrackMapName_);
    if (trackerTrackMap_)
      AddBranchDep(mitName_,trackerTrackMap_->GetBrName());
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
  if (!pfSuperClusterMapName_.empty()) {
    pfSuperClusterMap_ = OS()->get<SuperClusterMap>(pfSuperClusterMapName_);
    if (pfSuperClusterMap_)
      AddBranchDep(mitName_,pfSuperClusterMap_->GetBrName());
  }
}

//--------------------------------------------------------------------------------------------------
void FillerElectrons::FillDataBlock(const edm::Event &event, const edm::EventSetup &setup)	 
{
  // Fill electrons from edm collection into our collection.

  electrons_->Delete();
  
  Handle<reco::GsfElectronCollection> hElectronProduct;
  GetProduct(edmName_, hElectronProduct, event);
  
  // handles to get the electron ID information
  Handle<edm::ValueMap<float> > eidLooseMap;
  GetProduct(eIDCutBasedLooseName_, eidLooseMap, event);
  Handle<edm::ValueMap<float> > eidTightMap;
  GetProduct(eIDCutBasedTightName_, eidTightMap, event);
  edm::Handle<edm::ValueMap<float> > eidLikelihoodMap;
  if (!eIDLikelihoodName_.empty()) {
    GetProduct(eIDLikelihoodName_, eidLikelihoodMap, event);  
  }
  
  edm::Handle<reco::VertexCollection> hVertex;
  event.getByLabel(pvEdmName_, hVertex);
  const reco::VertexCollection *pvCol = hVertex.product();
  
  edm::Handle<reco::VertexCollection> hVertexBS;
  event.getByLabel(pvBSEdmName_, hVertexBS);
  const reco::VertexCollection *pvBSCol = hVertexBS.product();

  edm::Handle<reco::TrackCollection> hGeneralTracks;
  event.getByLabel("generalTracks", hGeneralTracks);
  //const reco::VertexCollection *trackCol = hGeneralTracks.product();
  
  edm::Handle<reco::GsfTrackCollection> hGsfTracks;
  event.getByLabel("electronGsfTracks", hGsfTracks);

  edm::Handle<std::vector<mitedm::DecayPart> > hConversions;
  event.getByLabel("mvfConversionRemoval", hConversions);
  
  mitedm::ConversionMatcher convMatcher;
     
  edm::ESHandle<TransientTrackBuilder> hTransientTrackBuilder;
  setup.get<TransientTrackRecord>().get("TransientTrackBuilder",hTransientTrackBuilder);
  const TransientTrackBuilder *transientTrackBuilder = hTransientTrackBuilder.product();
  
  GsfVertexTrackCompatibilityEstimator gsfEstimator;

  LinearizedTrackStateFactory lTrackFactory;
  VertexTrackFactory<5> vTrackFactory;
  KalmanVertexUpdator<5> updator;

  //Get Magnetic Field from event setup, taking value at (0,0,0)
  edm::ESHandle<MagneticField> magneticField;
  setup.get<IdealMagneticFieldRecord>().get(magneticField);
  const double bfield = magneticField->inTesla(GlobalPoint(0.,0.,0.)).z();

  const reco::GsfElectronCollection inElectrons = *(hElectronProduct.product());
  // loop over electrons
  for (reco::GsfElectronCollection::const_iterator iM = inElectrons.begin(); 
       iM != inElectrons.end(); ++iM) {

    // the index and Ref are needed for the eID association Map
    unsigned int iElectron = iM - inElectrons.begin();
    reco::GsfElectronRef eRef(hElectronProduct, iElectron);

    mithep::Electron *outElectron = electrons_->AddNew();
    
    outElectron->SetPtEtaPhi(iM->pt(),iM->eta(),iM->phi());
         
    outElectron->SetCharge(iM->charge());
    outElectron->SetScPixCharge(iM->scPixCharge());
    
    outElectron->SetESuperClusterOverP(iM->eSuperClusterOverP());
    outElectron->SetESeedClusterOverPout(iM->eSeedClusterOverPout());
    outElectron->SetPIn(iM->trackMomentumAtVtx().R());
    outElectron->SetPOut(iM->trackMomentumOut().R());
    outElectron->SetDeltaEtaSuperClusterTrackAtVtx(iM->deltaEtaSuperClusterTrackAtVtx());
    outElectron->SetDeltaEtaSeedClusterTrackAtCalo(iM->deltaEtaSeedClusterTrackAtCalo());
    outElectron->SetDeltaPhiSuperClusterTrackAtVtx(iM->deltaPhiSuperClusterTrackAtVtx());
    outElectron->SetDeltaPhiSeedClusterTrackAtCalo(iM->deltaPhiSeedClusterTrackAtCalo());
    outElectron->SetIsEnergyScaleCorrected(iM->isEnergyScaleCorrected());
    outElectron->SetIsMomentumCorrected(iM->isMomentumCorrected());
    outElectron->SetNumberOfClusters(iM->basicClustersSize());
    outElectron->SetClassification(iM->classification());
    outElectron->SetFBrem(iM->fbrem());
    
    // pflow electron stuff
    outElectron->SetIsEcalDriven(iM->ecalDrivenSeed());
    outElectron->SetIsTrackerDriven(iM->trackerDrivenSeed());
    outElectron->SetMva(iM->mva());
    
    // shower shape variables   
    outElectron->SetE15(iM->e1x5());
    outElectron->SetE25Max(iM->e2x5Max());
    outElectron->SetE55(iM->e5x5());
    outElectron->SetCovEtaEta(iM->sigmaEtaEta());
    outElectron->SetCoviEtaiEta(iM->sigmaIetaIeta());
    outElectron->SetHadronicOverEm(iM->hcalOverEcal());
    outElectron->SetHcalDepth1OverEcal(iM->hcalDepth1OverEcal());
    outElectron->SetHcalDepth2OverEcal(iM->hcalDepth2OverEcal());
   
    // fill isolation variables for both cone sizes
    outElectron->SetEcalRecHitIsoDr04(iM->dr04EcalRecHitSumEt());
    outElectron->SetHcalDepth1TowerSumEtDr04(iM->dr04HcalDepth1TowerSumEt());
    outElectron->SetHcalDepth2TowerSumEtDr04(iM->dr04HcalDepth2TowerSumEt());
    outElectron->SetTrackIsolationDr04(iM->dr04TkSumPt());
    outElectron->SetEcalRecHitIsoDr03(iM->dr03EcalRecHitSumEt());
    outElectron->SetHcalTowerSumEtDr03(iM->dr03HcalTowerSumEt());
    outElectron->SetHcalDepth1TowerSumEtDr03(iM->dr03HcalDepth1TowerSumEt());
    outElectron->SetHcalDepth2TowerSumEtDr03(iM->dr03HcalDepth2TowerSumEt());
    outElectron->SetTrackIsolationDr03(iM->dr03TkSumPt());
     
    // fiducial flags
    outElectron->SetIsEB(iM->isEB());
    outElectron->SetIsEE(iM->isEE());
    outElectron->SetIsEBEEGap(iM->isEBEEGap());
    outElectron->SetIsEBEtaGap(iM->isEBEtaGap());
    outElectron->SetIsEBPhiGap(iM->isEBPhiGap());
    outElectron->SetIsEEDeeGap(iM->isEEDeeGap());
    outElectron->SetIsEERingGap(iM->isEERingGap());
    
    // gsf-tracker match quality
    outElectron->SetFracSharedHits(iM->shFracInnerHits());

    // make proper links to Tracks and Super Clusters
    if (gsfTrackMap_ && iM->gsfTrack().isNonnull()) {
      outElectron->SetGsfTrk(gsfTrackMap_->GetMit(refToPtr(iM->gsfTrack())));
    }
    // make links to ambigous gsf tracks
    if (gsfTrackMap_) {
      for (reco::GsfTrackRefVector::const_iterator agsfi = iM->ambiguousGsfTracksBegin(); agsfi != iM->ambiguousGsfTracksEnd(); ++agsfi) {
        outElectron->AddAmbiguousGsfTrack(gsfTrackMap_->GetMit(refToPtr(*agsfi)));
      }
    }
    
    // make tracker track links,
    if (trackerTrackMap_ && iM->closestCtfTrackRef().isNonnull()) {
        outElectron->SetTrackerTrk(trackerTrackMap_->GetMit(refToPtr(iM->closestCtfTrackRef())));
    }
    if (barrelSuperClusterMap_ && endcapSuperClusterMap_ && 
        pfSuperClusterMap_ && iM->superCluster().isNonnull()) {
      if(barrelSuperClusterMap_->HasMit(iM->superCluster())) {
        outElectron->SetSuperCluster(barrelSuperClusterMap_->GetMit(iM->superCluster()));        
      }
      else if (endcapSuperClusterMap_->HasMit(iM->superCluster())) {
        outElectron->SetSuperCluster(endcapSuperClusterMap_->GetMit(iM->superCluster()));
      }
      else if (pfSuperClusterMap_->HasMit(iM->superCluster())) {
        outElectron->SetSuperCluster(pfSuperClusterMap_->GetMit(iM->superCluster()));  
      }
      else throw edm::Exception(edm::errors::Configuration, "FillerElectrons:FillDataBlock()\n")
             << "Error! SuperCluster reference in unmapped collection " << edmName_ << endl;
    }

    //compute impact parameter with respect to PV
    if (iM->gsfTrack().isNonnull()) {
      const reco::TransientTrack &tt = transientTrackBuilder->build(iM->gsfTrack()); 

      reco::TransientTrack ttckf;

      reco::Vertex thevtx;// = pvCol->at(0);
      reco::Vertex thevtxbs;// = pvBSCol->at(0);

      double mindzvtx = 9999.;
      for (uint ivtx = 0; ivtx<pvCol->size(); ++ivtx) {
        reco::Vertex avtx = pvCol->at(ivtx);
        double dzvtx = std::abs(iM->gsfTrack()->dz(avtx.position()));
        if (dzvtx<mindzvtx) {
          mindzvtx = dzvtx;
          thevtx = avtx;
        }
      }

      double mindzvtxbs = 9999.;
      for (uint ivtxbs = 0; ivtxbs<pvBSCol->size(); ++ivtxbs) {
        reco::Vertex avtxbs = pvBSCol->at(ivtxbs);
        double dzvtxbs = std::abs(iM->gsfTrack()->dz(avtxbs.position()));
        if (dzvtxbs<mindzvtxbs) {
          mindzvtxbs = dzvtxbs;
          thevtxbs = avtxbs;
        }
      }


      //check if closest ctf track is included in PV and if so, remove it before computing impact parameters and uncertainties
      if (iM->closestCtfTrackRef().isNonnull()) {
        ttckf = transientTrackBuilder->build(iM->closestCtfTrackRef());

        if (0) { //don't refit vertex for now
      
          if (find(thevtx.tracks_begin(), thevtx.tracks_end(), ttckf.trackBaseRef()) != thevtx.tracks_end()) {
            GlobalPoint linP(Basic3DVector<float> (thevtx.position()));
            KalmanVertexUpdator<5>::RefCountedLinearizedTrackState linTrack = lTrackFactory.linearizedTrackState(linP, ttckf);
            GlobalError err(thevtx.covariance());
            VertexState vState(linP, err);
            KalmanVertexUpdator<5>::RefCountedVertexTrack vertexTrack = vTrackFactory.vertexTrack(linTrack, vState);
          
            std::vector<KalmanVertexUpdator<5>::RefCountedVertexTrack> initialTracks(1, vertexTrack);
            CachingVertex<5> cachingVertex(linP, err, initialTracks, thevtx.chi2());
            const CachingVertex<5> &newCachingVertex = updator.remove(cachingVertex,vertexTrack);

            if (newCachingVertex.isValid()) {
              const TransientVertex &tvtx = newCachingVertex;
              thevtx = tvtx;
            }
          }

          if (find(thevtxbs.tracks_begin(), thevtxbs.tracks_end(), ttckf.trackBaseRef()) != thevtxbs.tracks_end()) {
            GlobalPoint linP(Basic3DVector<float> (thevtxbs.position()));
            KalmanVertexUpdator<5>::RefCountedLinearizedTrackState linTrack = lTrackFactory.linearizedTrackState(linP, ttckf);
            GlobalError err(thevtxbs.covariance());
            VertexState vState(linP, err);
            KalmanVertexUpdator<5>::RefCountedVertexTrack vertexTrack = vTrackFactory.vertexTrack(linTrack, vState);
          
            std::vector<KalmanVertexUpdator<5>::RefCountedVertexTrack> initialTracks(1, vertexTrack);
            CachingVertex<5> cachingVertex(linP, err, initialTracks, thevtxbs.chi2());
            const CachingVertex<5> &newCachingVertex = updator.remove(cachingVertex,vertexTrack);

            if (newCachingVertex.isValid()) {
              const TransientVertex &tvtx = newCachingVertex;
              thevtxbs = tvtx;
            }
          }
        }

      }
      
      //preserve sign of transverse impact parameter (cross-product definition from track, not lifetime-signing)
      const double gsfsign   = ( (-iM->gsfTrack()->dxy(thevtx.position()))   >=0 ) ? 1. : -1.;
      const double gsfsignbs = ( (-iM->gsfTrack()->dxy(thevtxbs.position())) >=0 ) ? 1. : -1.;

      const std::pair<bool,Measurement1D> &d0pv =  IPTools::absoluteTransverseImpactParameter(tt,thevtx);
      if (d0pv.first) {
        outElectron->SetD0PV(gsfsign*d0pv.second.value());
        outElectron->SetD0PVErr(d0pv.second.error());
      }
      else {
        outElectron->SetD0PV(-999.0);
      }


      const std::pair<bool,Measurement1D> &ip3dpv =  IPTools::absoluteImpactParameter3D(tt,thevtx);
      if (ip3dpv.first) {
        outElectron->SetIp3dPV(gsfsign*ip3dpv.second.value());
        outElectron->SetIp3dPVErr(ip3dpv.second.error());
      }
      else {
        outElectron->SetIp3dPV(-999.0);
      }

      const std::pair<bool,Measurement1D> &d0pvbs =  IPTools::absoluteTransverseImpactParameter(tt,thevtxbs);
      if (d0pvbs.first) {
        outElectron->SetD0PVBS(gsfsignbs*d0pvbs.second.value());
        outElectron->SetD0PVBSErr(d0pvbs.second.error());
      }
      else {
        outElectron->SetD0PVBS(-999.0);
      }

      const std::pair<bool,Measurement1D> &ip3dpvbs =  IPTools::absoluteImpactParameter3D(tt,thevtxbs);
      if (ip3dpvbs.first) {
        outElectron->SetIp3dPVBS(gsfsignbs*ip3dpvbs.second.value());
        outElectron->SetIp3dPVBSErr(ip3dpvbs.second.error());
      }
      else {
        outElectron->SetIp3dPVBS(-999.0);
      }

      if (iM->closestCtfTrackRef().isNonnull()) {

        const double ckfsign   = ( (-iM->closestCtfTrackRef()->dxy(thevtx.position()))   >=0 ) ? 1. : -1.;
        const double ckfsignbs = ( (-iM->closestCtfTrackRef()->dxy(thevtxbs.position())) >=0 ) ? 1. : -1.;

        const std::pair<bool,Measurement1D> &d0pvckf =  IPTools::absoluteTransverseImpactParameter(ttckf,thevtx);
        if (d0pvckf.first) {
          outElectron->SetD0PVCkf(ckfsign*d0pvckf.second.value());
          outElectron->SetD0PVCkfErr(d0pvckf.second.error());
        }
        else {
          outElectron->SetD0PVCkf(-999.0);
        }
  
  
        const std::pair<bool,Measurement1D> &ip3dpvckf =  IPTools::absoluteImpactParameter3D(ttckf,thevtx);
        if (ip3dpvckf.first) {
          outElectron->SetIp3dPVCkf(ckfsign*ip3dpvckf.second.value());
          outElectron->SetIp3dPVCkfErr(ip3dpvckf.second.error());
        }
        else {
          outElectron->SetIp3dPVCkf(-999.0);
        }
  
        const std::pair<bool,Measurement1D> &d0pvbsckf =  IPTools::absoluteTransverseImpactParameter(ttckf,thevtxbs);
        if (d0pvbsckf.first) {
          outElectron->SetD0PVBSCkf(ckfsignbs*d0pvbsckf.second.value());
          outElectron->SetD0PVBSCkfErr(d0pvbsckf.second.error());
        }
        else {
          outElectron->SetD0PVBSCkf(-999.0);
        }
  
        const std::pair<bool,Measurement1D> &ip3dpvbsckf =  IPTools::absoluteImpactParameter3D(ttckf,thevtxbs);
        if (ip3dpvbsckf.first) {
          outElectron->SetIp3dPVBSCkf(ckfsignbs*ip3dpvbsckf.second.value());
          outElectron->SetIp3dPVBSCkfErr(ip3dpvbsckf.second.error());
        }
        else {
          outElectron->SetIp3dPVBSCkf(-999.0);
        }
      }
      else {
        outElectron->SetD0PVCkf(-999.0);
        outElectron->SetIp3dPVCkf(-999.0);
        outElectron->SetD0PVBSCkf(-999.0);
        outElectron->SetIp3dPVBSCkf(-999.0);
      }


      if (verbose_>1) {
        printf("gsf track      pt = %5f\n",iM->gsfTrack()->pt());
        printf("gsf track mode pt = %5f\n",iM->gsfTrack()->ptMode());
        printf("ttrack         pt = %5f\n",tt.initialFreeState().momentum().perp());
        //printf("ttrackgsf      pt = %5f\n",ttgsf.innermostMeasurementState().globalMomentum().perp());
        printf("ip3dpv reduced chisquared = %5f, probability = %5f\n", ip3dpv.second.value()/ip3dpv.second.error(), TMath::Prob(ip3dpv.second.value()/ip3dpv.second.error(),1));
        //printf("gsf    reduced chisquared = %5f, probability = %5f\n", pvGsfCompat.second/2, TMath::Prob(pvGsfCompat.second,2));
      }

    }


    //fill conversion partner track info
    if (recomputeConversionInfo_) {
      ConversionFinder convFinder;         outElectron->SetConvPartnerDCotTheta(iM->convDcot());
      ConversionInfo convInfo = convFinder.getConversionInfo(*iM, hGeneralTracks, hGsfTracks, bfield);
  
      outElectron->SetConvFlag(convInfo.flag());
      outElectron->SetConvPartnerDCotTheta(convInfo.dcot());
      outElectron->SetConvPartnerDist(convInfo.dist());
      outElectron->SetConvPartnerRadius(convInfo.radiusOfConversion());
      reco::TrackRef ckfconvTrackRef = convInfo.conversionPartnerCtfTk();
      reco::GsfTrackRef gsfconvTrackRef = convInfo.conversionPartnerGsfTk();
  
  
      if ( gsfconvTrackRef.isNonnull() && gsfTrackMap_  ) {
        outElectron->SetConvPartnerTrk(gsfTrackMap_->GetMit(edm::refToPtr(gsfconvTrackRef)));
      }
      else if (ckfconvTrackRef.isNonnull() && trackerTrackMap_) {
        outElectron->SetConvPartnerTrk(trackerTrackMap_->GetMit(edm::refToPtr(ckfconvTrackRef)));
      }
    }
    else {
      outElectron->SetConvFlag(iM->convFlags());
      outElectron->SetConvPartnerDCotTheta(iM->convDcot());
      outElectron->SetConvPartnerDist(iM->convDist());
      outElectron->SetConvPartnerRadius(iM->convRadius());
      reco::TrackBaseRef convTrackRef = iM->convPartner();
      if (convTrackRef.isNonnull()) {
        if ( dynamic_cast<const reco::GsfTrack*>(convTrackRef.get()) && gsfTrackMap_  ) {
          outElectron->SetConvPartnerTrk(gsfTrackMap_->GetMit(mitedm::refToBaseToPtr(convTrackRef)));
        }
        else if (trackerTrackMap_) {
          outElectron->SetConvPartnerTrk(trackerTrackMap_->GetMit(mitedm::refToBaseToPtr(convTrackRef)));
        }
      }
    }



    // fill Electron ID information
    outElectron->SetPassLooseID((*eidLooseMap)[eRef]);
    outElectron->SetPassTightID((*eidTightMap)[eRef]);
    if (!eIDLikelihoodName_.empty()) {
      outElectron->SetIDLikelihood((*eidLikelihoodMap)[eRef]);
    }
    
    // fill corrected expected inner hits
    if(iM->gsfTrack().isNonnull()) {
      outElectron->SetCorrectedNExpectedHitsInner(iM->gsfTrack()->trackerExpectedHitsInner().numberOfHits());
    }
    
    //fill additional conversion flag
    outElectron->SetMatchesVertexConversion(convMatcher.matchesGoodConversion(*iM,hConversions));
    
    if (verbose_>1) {
      double recomass = sqrt(iM->energy()*iM->energy() - iM->p()*iM->p());
      printf(" mithep::Electron,    pt=%5f, eta=%5f, phi=%5f, energy=%5f, p=%5f, mass=%5f\n",
             outElectron->Pt(), outElectron->Eta(), outElectron->Phi(), 
             outElectron->E(), outElectron->P(), outElectron->Mass());
      printf("reco::GsfElectron   , pt=%5f, eta=%5f, phi=%5f, energy=%5f, p=%5f, mass=%5f\n",
             iM->pt(), iM->eta(), iM->phi(), iM->energy(), iM->p(), recomass);  
    }
  }  
  electrons_->Trim();
}
