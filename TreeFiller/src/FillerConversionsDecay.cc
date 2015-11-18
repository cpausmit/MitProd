#include "MitProd/TreeFiller/interface/FillerConversionsDecay.h"
#include "DataFormats/Common/interface/RefToPtr.h"
#include "MitEdm/DataFormats/interface/RefToBaseToPtr.h"
#include "MitAna/DataTree/interface/StableDataCol.h"
#include "MitAna/DataTree/interface/DecayDataCol.h"
#include "MitAna/DataTree/interface/DecayParticleCol.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitProd/ObjectService/interface/ObjectService.h"
#include "TrackingTools/IPTools/interface/IPTools.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/TransientTrack/plugins/TransientTrackBuilderESProducer.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateOnSurface.h"
#include "TrackingTools/GeomPropagators/interface/AnalyticalTrajectoryExtrapolatorToLine.h"
#include "TrackingTools/GeomPropagators/interface/AnalyticalImpactPointExtrapolator.h"
#include "DataFormats/GeometryCommonDetAlgo/interface/Measurement1D.h"
#include "TrackingTools/TransientTrack/interface/TransientTrack.h"
#include "RecoVertex/VertexPrimitives/interface/ConvertToFromReco.h"
#include "MagneticField/UniformEngine/src/UniformMagneticField.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerConversionsDecay::FillerConversionsDecay(const ParameterSet &cfg, edm::ConsumesCollector& collector, ObjectService* os, const char *name, bool active) :
  BaseFiller(cfg,os,name,active),
  edmToken_(GetToken<reco::ConversionCollection>(collector, cfg, "edmName")), //conversions
  beamspotToken_(GetToken<reco::BeamSpot>(collector, cfg, "beamspotName")), //offlineBeamSpot
  mitName_(cfg.getUntrackedParameter<string>("mitName","Conversions")),
  checkTrackRef_(cfg.getUntrackedParameter<bool>("checkTrackRef", true)),
  stableDataName_(mitName_ + "_StableDatas"),  
  stablePartMapNames_(cfg.getUntrackedParameter<vector<string> >("stablePartMaps", vector<string>())),
  conversionMapName_(cfg.getUntrackedParameter<string>("conversionMapName", mitName_ + "MapName")),
  decays_(new mithep::DecayParticleArr(250)),
  stableData_(new mithep::StableDataArr(500)),
  conversionMap_(new mithep::ConversionDecayMap)
{
  // Constructor.
}

//--------------------------------------------------------------------------------------------------
FillerConversionsDecay::~FillerConversionsDecay()
{
  // Destructor.

  delete decays_;
  delete stableData_;
  delete conversionMap_;
}

//--------------------------------------------------------------------------------------------------
void FillerConversionsDecay::BookDataBlock(TreeWriter &tws)
{
  // Add conversions to tree. Publish and get our objects.

  tws.AddBranch(mitName_,        &decays_);
  OS()->add<mithep::DecayParticleArr>(decays_,mitName_);
  tws.AddBranch(stableDataName_, &stableData_);
  OS()->add<mithep::StableDataArr>(stableData_,stableDataName_);

  if (!conversionMapName_.empty()) {
    conversionMap_->SetBrName(mitName_);
    OS()->add(conversionMap_,conversionMapName_);
  }
}

void FillerConversionsDecay::PrepareLinks()
{
  for (auto&& bmapName : stablePartMapNames_) {
    if (!bmapName.empty()) {
      const TrackPartMap *map = OS()->get<TrackPartMap>(bmapName);
      stablePartMaps_.push_back(map);
      AddBranchDep(mitName_,map->GetBrName());
    }
  }
}

//--------------------------------------------------------------------------------------------------
void FillerConversionsDecay::FillDataBlock(const edm::Event      &event, 
                                      const edm::EventSetup &setup)
{
  // Fill conversions data structure and maps.

  decays_->Delete();
  stableData_->Delete();
  conversionMap_->Reset();

  // get beamspot
  edm::Handle<reco::BeamSpot> bsHandle;
  GetProduct(beamspotToken_, bsHandle, event);
  const reco::BeamSpot &thebs = *bsHandle.product();

  // transient track producer
  edm::ESHandle<TransientTrackBuilder> hTransientTrackBuilder;
  setup.get<TransientTrackRecord>().get("TransientTrackBuilder",hTransientTrackBuilder);
  const TransientTrackBuilder *transientTrackBuilder = hTransientTrackBuilder.product();

  Handle<reco::ConversionCollection> hConversionProduct;
  GetProduct(edmToken_, hConversionProduct, event);

  conversionMap_->SetEdmProductId(hConversionProduct.id().id());
        
  reco::ConversionCollection const& inConversions = *hConversionProduct;

  for (reco::ConversionCollection::const_iterator inConversion = inConversions.begin(); 
      inConversion != inConversions.end(); ++inConversion) {
    
    mithep::DecayParticle *outConversion = decays_->Allocate();    
    new (outConversion) mithep::DecayParticle(22);

    math::XYZTLorentzVectorD convP4(inConversion->refittedPair4Momentum());
    FourVector p4Fitted(convP4.px(),convP4.py(),convP4.pz(),convP4.energy());
  
    const std::vector<reco::TrackBaseRef> &trackRefs = inConversion->tracks();
    if (trackRefs.size() == 1 && p4Fitted.Pt()<1e-3)
      p4Fitted.SetXYZT(trackRefs.front()->px(),trackRefs.front()->py(),trackRefs.front()->pz(),
		       trackRefs.front()->p());
  
    outConversion->SetMom(p4Fitted);
  
    const reco::Vertex &vtx = inConversion->conversionVertex();
    ThreeVector vtxPos(vtx.x(),vtx.y(),vtx.z());
    GlobalPoint vtxPoint(vtx.x(),vtx.y(),vtx.z());
    
    double dlz = vtxPos.z()*TMath::Abs(p4Fitted.Pz())/p4Fitted.Pz();
    ThreeVector momPerp(p4Fitted.Px(),p4Fitted.Py(),0);
    ThreeVector posPerp(vtxPos.x(),vtxPos.y(),0);
    double dl = momPerp.Dot(posPerp)/momPerp.R();    
    double dxy = momPerp.Cross(vtxPos).Z()/p4Fitted.Pt();
    
    outConversion->SetLz(dlz);
    outConversion->SetLxy(dl);
    outConversion->SetDxy(dxy);
       
    outConversion->SetChi2(vtx.chi2());
    outConversion->SetNdof((Int_t)vtx.ndof());
    
    outConversion->SetNSharedHits(inConversion->nSharedHits());
    
    const std::vector<reco::Track> &refittedTracks = vtx.refittedTracks();
    const std::vector<uint8_t> &nHitsBeforeVtx = inConversion->nHitsBeforeVtx();
    const std::vector<Measurement1DFloat> &dlClosestHitToVtx = inConversion->dlClosestHitToVtx();
    
    // fill conversion quality mask
    ConversionQuality &outQuality = outConversion->Quality();
    for (uint i=0; i<16; ++i)
      outQuality.SetQuality(ConversionQuality::EQuality(i),
			    inConversion->quality(reco::Conversion::ConversionQuality(i)));
    
    // fill daughters
    double zbeamlineerr = -99.;
    if (stablePartMaps_.size()) {
      for (uint i=0; i<trackRefs.size(); ++i) {
	
        const reco::TrackBaseRef &trackRef = trackRefs.at(i);
        const reco::Track *refittedTrack = 0;

        if (refittedTracks.size()>i)
          refittedTrack = &refittedTracks.at(i);
        else 
          refittedTrack = trackRef.get();

        // fill dzError at beamline (take from refitted first track normally)
        if (zbeamlineerr<0.) {
          const reco::TransientTrack &tt = transientTrackBuilder->build(*refittedTrack);
          // null b-field
          const UniformMagneticField nullField(0.0); 
          TransverseImpactPointExtrapolator extrapolator(&nullField);
	
          // const TrajectoryStateOnSurface &bsstate =
	  //   extrapolator.extrapolate(tt.stateOnSurface(vtxPoint),RecoVertex::convertPos(thebs.position()));
          const FreeTrajectoryState &initialfts = tt.initialFreeState();
          const GlobalTrajectoryParameters trackgtp(initialfts.position(),initialfts.momentum(),
						    initialfts.charge(),&nullField);
          const FreeTrajectoryState trackfts(trackgtp,initialfts.cartesianError(),
					     initialfts.curvilinearError());
          const TrajectoryStateOnSurface &bsstate =
	    extrapolator.extrapolate(trackfts, RecoVertex::convertPos(thebs.position()));
          if (bsstate.isValid()) {
            double zbeamline = bsstate.globalPosition().z();
            zbeamlineerr = sqrt((bsstate.cartesianError().matrix())(2,2));
            if (0) {
              math::XYZVector mom(inConversion->refittedPairMomentum());
              double zbeamlineconv = (vtx.z()) -
		((vtx.x()-thebs.position().x())*mom.x()+(vtx.y()-thebs.position().y())*mom.y())/mom.rho()
		* mom.z()/mom.rho();
              double zbeamlinetrk = (refittedTrack->vertex().z()) -
		((refittedTrack->vertex().x()-thebs.position().x())*refittedTrack->momentum().x()+
		 (refittedTrack->vertex().y()-thebs.position().y())*refittedTrack->momentum().y())/
		refittedTrack->momentum().rho() * refittedTrack->momentum().z()/refittedTrack->momentum().rho();

              printf("zbeamlineconv = %5f, zbeamlinetrk = %5f, zbeamline = %5f, zbeamlineerr = %5f\n",
		     zbeamlineconv,zbeamlinetrk,zbeamline,zbeamlineerr);
            }
          }
        }

        const mithep::Particle *daughter = 0; 
	try{
	  daughter = GetMitParticle(mitedm::refToBaseToPtr(trackRef));
	}
	catch(...) { 
	  std::cout << " ======> Broken Track Reference ===> " << std::endl;
	  if (checkTrackRef_)
	    throw edm::Exception(edm::errors::Configuration, "FillerConversionsDecay::FillDataBlock()\n")
	      << "Error! not found in AssociationMaps (" << typeid(*this).name() << ")." << std::endl;
	}
        
        mithep::StableData *outStable = stableData_->Allocate();
        new (outStable) mithep::StableData(daughter,
                                           refittedTrack->px(),refittedTrack->py(),refittedTrack->pz());
        
        if (nHitsBeforeVtx.size()>i)
          outStable->SetNHitsBeforeVtx(nHitsBeforeVtx.at(i));

        if (dlClosestHitToVtx.size()>i) {
          outStable->SetDlClosestHitToVtx(dlClosestHitToVtx.at(i).value());
          outStable->SetDlClosestHitToVtxErr(dlClosestHitToVtx.at(i).error());          
        }
                                                                            
        outConversion->AddDaughterData(outStable);
      }
    }

    outConversion->SetDzBeamlineError(zbeamlineerr);
    
    reco::ConversionRef theRef(hConversionProduct,inConversion-inConversions.begin());
    conversionMap_->Add(theRef,outConversion);
  }
  decays_->Trim();
}

//--------------------------------------------------------------------------------------------------
mithep::Particle *FillerConversionsDecay::GetMitParticle(edm::Ptr<reco::Track> ptr) const
{
  // Return our particle referenced by the edm pointer.
  mithep::Particle *mitPart = 0;
  for (std::vector<const mithep::TrackPartMap*>::const_iterator bmap = stablePartMaps_.begin();
       bmap!=stablePartMaps_.end(); ++bmap) {
    const mithep::TrackPartMap *theMap = *bmap;
    if (theMap->HasMit(ptr)) {
      mitPart = theMap->GetMit(ptr);
      return mitPart;
    }
  }
  if (!mitPart && checkTrackRef_)
    throw edm::Exception(edm::errors::Configuration, "FillerConversionsDecay::FillDataBlock()\n")
    << "Error! MITHEP Object " 
    << "not found in AssociationMaps (" << typeid(*this).name() << ")." << std::endl;

  return mitPart;
}

DEFINE_MITHEP_TREEFILLER(FillerConversionsDecay);
