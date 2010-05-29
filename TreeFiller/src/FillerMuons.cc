// $Id: FillerMuons.cc,v 1.29 2010/05/28 18:50:16 pharris Exp $

#include "MitProd/TreeFiller/interface/FillerMuons.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonQuality.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/MuonReco/interface/MuonSelectors.h"
#include "DataFormats/Common/interface/RefToPtr.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/TransientTrack/plugins/TransientTrackBuilderESProducer.h"
#include "TrackingTools/IPTools/interface/IPTools.h"
#include "RecoVertex/KalmanVertexFit/interface/KalmanVertexTrackCompatibilityEstimator.h" 
#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataTree/interface/MuonCol.h"
#include "MitAna/DataTree/interface/Track.h"
#include "MitProd/ObjectService/interface/ObjectService.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerMuons::FillerMuons(const edm::ParameterSet &cfg, const char *name, bool active) :
  BaseFiller(cfg,name,active),
  edmName_(Conf().getUntrackedParameter<string>("edmName","muons")),
  mitName_(Conf().getUntrackedParameter<string>("mitName",Names::gkMuonBrn)),
  globalTrackMapName_(Conf().getUntrackedParameter<string>("globalTrackMapName","")),
  staTrackMapName_(Conf().getUntrackedParameter<string>("staTrackMapName","")),
  staVtxTrackMapName_(Conf().getUntrackedParameter<string>("staVtxTrackMapName","")),
  trackerTrackMapName_(Conf().getUntrackedParameter<string>("trackerTrackMapName","")),
  muonMapName_(Conf().getUntrackedParameter<string>("muonMapName","")),
  pvEdmName_(Conf().getUntrackedParameter<string>("pvEdmName","offlinePrimaryVertices")),
  pvBSEdmName_(Conf().getUntrackedParameter<string>("pvEdmName","offlinePrimaryVerticesWithBS")),
  globalTrackMap_(0),
  standaloneTrackMap_(0),
  standaloneVtxTrackMap_(0),
  trackerTrackMap_(0),
  muonMap_(new mithep::MuonMap),
  muons_(new mithep::MuonArr(16))
{
  // Constructor.
}

//--------------------------------------------------------------------------------------------------
FillerMuons::~FillerMuons() 
{
  // Destructor.

  delete muons_;
  delete muonMap_;
}

//--------------------------------------------------------------------------------------------------
void FillerMuons::BookDataBlock(TreeWriter &tws) 
{ 
  // Add muons branch to tree and get pointers to maps.

  tws.AddBranch(mitName_,&muons_);
  OS()->add<mithep::MuonArr>(muons_,mitName_);

  if (!globalTrackMapName_.empty()) {
    globalTrackMap_ = OS()->get<TrackMap>(globalTrackMapName_);
    if (globalTrackMap_)
      AddBranchDep(mitName_,globalTrackMap_->GetBrName());
  }
  if (!staTrackMapName_.empty()) {
    standaloneTrackMap_ = OS()->get<TrackMap>(staTrackMapName_);
    if (standaloneTrackMap_)
      AddBranchDep(mitName_,standaloneTrackMap_->GetBrName());
  }
  if (!staVtxTrackMapName_.empty()) {
    standaloneVtxTrackMap_ = OS()->get<TrackMap>(staVtxTrackMapName_);
    if (standaloneVtxTrackMap_)
      AddBranchDep(mitName_,standaloneVtxTrackMap_->GetBrName());
  }
  if (!trackerTrackMapName_.empty()) {
    trackerTrackMap_ = OS()->get<TrackMap>(trackerTrackMapName_);
    if (trackerTrackMap_)
      AddBranchDep(mitName_,trackerTrackMap_->GetBrName());
  }
  if (!muonMapName_.empty()) {
    muonMap_->SetBrName(mitName_);
    OS()->add<MuonMap>(muonMap_,muonMapName_);
  }
}

//--------------------------------------------------------------------------------------------------
void FillerMuons::FillDataBlock(const edm::Event      &event, 
                                const edm::EventSetup &setup)
{
  // Fill muon information. 

  muons_->Delete();
  muonMap_->Reset();
  
  Handle<reco::MuonCollection> hMuonProduct;
  GetProduct(edmName_, hMuonProduct, event);  
  const reco::MuonCollection inMuons = *(hMuonProduct.product());  

  edm::Handle<reco::VertexCollection> hVertex;
  event.getByLabel(pvEdmName_, hVertex);
  const reco::VertexCollection *pvCol = hVertex.product();

  edm::Handle<reco::VertexCollection> hVertexBS;
  event.getByLabel(pvBSEdmName_, hVertexBS);
  const reco::VertexCollection *pvBSCol = hVertexBS.product();

  edm::ESHandle<TransientTrackBuilder> hTransientTrackBuilder;
  setup.get<TransientTrackRecord>().get("TransientTrackBuilder",hTransientTrackBuilder);
  const TransientTrackBuilder *transientTrackBuilder = hTransientTrackBuilder.product();

  KalmanVertexTrackCompatibilityEstimator<5> kalmanEstimator;

  for (reco::MuonCollection::const_iterator iM = inMuons.begin(); iM != inMuons.end(); ++iM) {  
    mithep::Muon* outMuon = muons_->AddNew();

    outMuon->SetPtEtaPhi(iM->pt(),iM->eta(),iM->phi());
    outMuon->SetCharge(iM->charge());

    outMuon->SetIsoR03SumPt(iM->isolationR03().sumPt);
    outMuon->SetIsoR03EmEt(iM->isolationR03().emEt);
    outMuon->SetIsoR03HadEt(iM->isolationR03().hadEt);
    outMuon->SetIsoR03HoEt(iM->isolationR03().hoEt);
    outMuon->SetIsoR03NTracks(iM->isolationR03().nTracks);
    outMuon->SetIsoR03NJets(iM->isolationR03().nJets);
    outMuon->SetIsoR05SumPt(iM->isolationR05().sumPt);
    outMuon->SetIsoR05EmEt(iM->isolationR05().emEt);
    outMuon->SetIsoR05HadEt(iM->isolationR05().hadEt);
    outMuon->SetIsoR05HoEt(iM->isolationR05().hoEt);
    outMuon->SetIsoR05NTracks(iM->isolationR05().nTracks);
    outMuon->SetIsoR05NJets(iM->isolationR05().nJets);
    outMuon->SetEmEnergy(iM->calEnergy().em);
    outMuon->SetHadEnergy(iM->calEnergy().had);
    outMuon->SetHoEnergy(iM->calEnergy().ho);
    outMuon->SetEmS9Energy(iM->calEnergy().emS9);
    outMuon->SetHadS9Energy(iM->calEnergy().hadS9);
    outMuon->SetHoS9Energy(iM->calEnergy().hoS9);
    outMuon->SetIsGlobalMuon(iM->isGlobalMuon());
    outMuon->SetIsTrackerMuon(iM->isTrackerMuon());
    outMuon->SetIsStandaloneMuon(iM->isStandAloneMuon());
    outMuon->SetIsCaloMuon(iM->isCaloMuon());

    //save results from the muon selector in the MuonQuality bitmask
    outMuon->Quality().SetQuality(MuonQuality::All);
    if (muon::isGoodMuon(*iM,muon::AllGlobalMuons))
      outMuon->Quality().SetQuality(MuonQuality::AllGlobalMuons);
    if (muon::isGoodMuon(*iM,muon::AllStandAloneMuons))
      outMuon->Quality().SetQuality(MuonQuality::AllStandAloneMuons);
    if (muon::isGoodMuon(*iM,muon::AllTrackerMuons))
      outMuon->Quality().SetQuality(MuonQuality::AllTrackerMuons);
    if (muon::isGoodMuon(*iM,muon::TrackerMuonArbitrated))
      outMuon->Quality().SetQuality(MuonQuality::TrackerMuonArbitrated);
    if (muon::isGoodMuon(*iM,muon::AllArbitrated))
      outMuon->Quality().SetQuality(MuonQuality::AllArbitrated);
    if (muon::isGoodMuon(*iM,muon::GlobalMuonPromptTight))
      outMuon->Quality().SetQuality(MuonQuality::GlobalMuonPromptTight);
    if (muon::isGoodMuon(*iM,muon::TMLastStationLoose))
      outMuon->Quality().SetQuality(MuonQuality::TMLastStationLoose);
    if (muon::isGoodMuon(*iM,muon::TMLastStationTight))
      outMuon->Quality().SetQuality(MuonQuality::TMLastStationTight);
    if (muon::isGoodMuon(*iM,muon::TM2DCompatibilityLoose))
      outMuon->Quality().SetQuality(MuonQuality::TM2DCompatibilityLoose);
    if (muon::isGoodMuon(*iM,muon::TM2DCompatibilityTight))
      outMuon->Quality().SetQuality(MuonQuality::TM2DCompatibilityTight);
    if (muon::isGoodMuon(*iM,muon::TMOneStationLoose))
      outMuon->Quality().SetQuality(MuonQuality::TMOneStationLoose);
    if (muon::isGoodMuon(*iM,muon::TMOneStationTight))
      outMuon->Quality().SetQuality(MuonQuality::TMOneStationTight);
    if (muon::isGoodMuon(*iM,muon::TMLastStationOptimizedLowPtLoose))
      outMuon->Quality().SetQuality(MuonQuality::TMLastStationOptimizedLowPtLoose);
    if (muon::isGoodMuon(*iM,muon::TMLastStationOptimizedLowPtTight))
      outMuon->Quality().SetQuality(MuonQuality::TMLastStationOptimizedLowPtTight);
    if (muon::isGoodMuon(*iM,muon::GMTkChiCompatibility))
      outMuon->Quality().SetQuality(MuonQuality::GMTkChiCompatibility);
    if (muon::isGoodMuon(*iM,muon::GMStaChiCompatibility))
      outMuon->Quality().SetQuality(MuonQuality::GMStaChiCompatibility);
    if (muon::isGoodMuon(*iM,muon::GMTkKinkTight))
      outMuon->Quality().SetQuality(MuonQuality::GMTkKinkTight);
    if (muon::isGoodMuon(*iM,muon::TMLastStationAngLoose))
      outMuon->Quality().SetQuality(MuonQuality::TMLastStationAngLoose);
    if (muon::isGoodMuon(*iM,muon::TMLastStationAngTight))
      outMuon->Quality().SetQuality(MuonQuality::TMLastStationAngTight);
    if (muon::isGoodMuon(*iM,muon::TMOneStationAngLoose))
      outMuon->Quality().SetQuality(MuonQuality::TMOneStationAngLoose);
    if (muon::isGoodMuon(*iM,muon::TMOneStationAngTight))
      outMuon->Quality().SetQuality(MuonQuality::TMOneStationAngTight);
    if (muon::isGoodMuon(*iM,muon::TMLastStationOptimizedBarrelLowPtLoose))
      outMuon->Quality().SetQuality(MuonQuality::TMLastStationOptimizedBarrelLowPtLoose);
    if (muon::isGoodMuon(*iM,muon::TMLastStationOptimizedBarrelLowPtTight))
      outMuon->Quality().SetQuality(MuonQuality::TMLastStationOptimizedBarrelLowPtTight);
 

    if (globalTrackMap_ && iM->combinedMuon().isNonnull()) {
      outMuon->SetGlobalTrk(globalTrackMap_->GetMit(refToPtr(iM->combinedMuon())));    
      outMuon->SetNValidHits (iM->globalTrack()->hitPattern().numberOfValidMuonHits());
    }
    if (standaloneTrackMap_ && standaloneVtxTrackMap_ && iM->standAloneMuon().isNonnull()) { 
      Int_t refProductId = iM->standAloneMuon().id().id();
      if ( refProductId == standaloneVtxTrackMap_->GetEdmProductId())
	outMuon->SetStandaloneTrk(standaloneVtxTrackMap_->GetMit(refToPtr(iM->standAloneMuon())));
      else if ( refProductId == standaloneTrackMap_->GetEdmProductId())
	outMuon->SetStandaloneTrk(standaloneTrackMap_->GetMit(refToPtr(iM->standAloneMuon())));
      else throw edm::Exception(edm::errors::Configuration, "FillerMuons:FillDataBlock()\n")
             << "Error! Track reference in unmapped collection " << edmName_ << endl;
    }
    if (trackerTrackMap_ && iM->track().isNonnull()) {
      outMuon->SetTrackerTrk(trackerTrackMap_->GetMit(refToPtr(iM->track())));
    }

    //compute impact parameter with respect to PV
    if (iM->track().isNonnull()) {
      const reco::TransientTrack &tt = transientTrackBuilder->build(iM->track()); 

      const std::pair<bool,Measurement1D> &d0pv =  IPTools::absoluteTransverseImpactParameter(tt,pvCol->at(0));
      if (d0pv.first) {
        outMuon->SetD0PV(d0pv.second.value());
        outMuon->SetD0PVErr(d0pv.second.error());
      }
      else {
        outMuon->SetD0PV(-99.0);
      }

      const std::pair<bool,Measurement1D> &ip3dpv =  IPTools::absoluteImpactParameter3D(tt,pvCol->at(0));
      if (ip3dpv.first) {
        outMuon->SetIp3dPV(ip3dpv.second.value());
        outMuon->SetIp3dPVErr(ip3dpv.second.error());
      }
      else {
        outMuon->SetIp3dPV(-99.0);
      }

      const std::pair<bool,Measurement1D> &d0pvbs =  IPTools::absoluteTransverseImpactParameter(tt,pvBSCol->at(0));
      if (d0pvbs.first) {
        outMuon->SetD0PVBS(d0pvbs.second.value());
        outMuon->SetD0PVBSErr(d0pvbs.second.error());
      }
      else {
        outMuon->SetD0PVBS(-99.0);
      }

      const std::pair<bool,Measurement1D> &ip3dpvbs =  IPTools::absoluteImpactParameter3D(tt,pvBSCol->at(0));
      if (ip3dpvbs.first) {
        outMuon->SetIp3dPVBS(ip3dpvbs.second.value());
        outMuon->SetIp3dPVBSErr(ip3dpvbs.second.error());
      }
      else {
        outMuon->SetIp3dPVBS(-99.0);
      }

      //compute compatibility with PV using taking into account also the case where muon track
      //was included in the vertex fit
      if (iM->track()->extra().isAvailable()) {
  
        const std::pair<bool,double> &pvCompat = kalmanEstimator.estimate(pvCol->at(0),tt);
        if (pvCompat.first) {
          outMuon->SetPVCompatibility(pvCompat.second);
        }
        else {
          outMuon->SetPVCompatibility(-99.0);
        }
  
        const std::pair<bool,double> &pvbsCompat = kalmanEstimator.estimate(pvBSCol->at(0),tt);
        if (pvbsCompat.first) {
          outMuon->SetPVBSCompatibility(pvbsCompat.second);
        }
        else {
          outMuon->SetPVBSCompatibility(-99.0);
        }
      }

    }
    
    outMuon->SetNChambers  (iM->numberOfChambers());
    outMuon->SetStationMask(iM->stationMask(reco::Muon::SegmentAndTrackArbitration));
    for(int i0 = 0; i0 < 4; i0++) {
      // DTs
      outMuon->SetDX(i0,            iM->dX(i0+1,1));
      outMuon->SetDY(i0,            iM->dY(i0+1,1));
      outMuon->SetPullX(i0,         iM->pullX(i0+1,1));
      outMuon->SetPullY(i0,         iM->pullY(i0+1,1));
      outMuon->SetTrackDist(i0,     iM->trackDist(i0+1,1));
      outMuon->SetTrackDistErr(i0,  iM->trackDistErr(i0+1,1));
      outMuon->SetNSegments(i0,     iM->numberOfSegments(i0+1,1));
      // CSCs
      outMuon->SetDX(4+i0,          iM->dX       (i0+1,2));
      outMuon->SetDY(4+i0,          iM->dY       (i0+1,2));
      outMuon->SetPullX(4+i0,       iM->pullX    (i0+1,2));
      outMuon->SetPullY(4+i0,       iM->pullY    (i0+1,2));
      outMuon->SetTrackDist(4+i0,   iM->trackDist(i0+1,2));
      outMuon->SetTrackDistErr(4+i0,iM->trackDistErr(i0+1,2));
      outMuon->SetNSegments(4+i0,   iM->numberOfSegments(i0+1,2));
    }

    // add muon to map
    edm::Ptr<reco::Muon> thePtr(hMuonProduct, iM - inMuons.begin());
    muonMap_->Add(thePtr, outMuon);
    
    if (verbose_>1) {
      if (!outMuon->HasGlobalTrk() && !outMuon->HasStandaloneTrk()) {
        printf("mithep::Muon, pt=%5f, eta=%5f, phi=%5f, mass=%5f\n",outMuon->Pt(),outMuon->Eta(),outMuon->Phi(), outMuon->Mass());
        printf("  reco::Muon, pt=%5f, eta=%5f, phi=%5f, mass=%5f\n",iM->pt(),iM->eta(),iM->phi(),iM->mass());  
      }
    }

  }
  muons_->Trim();
}
