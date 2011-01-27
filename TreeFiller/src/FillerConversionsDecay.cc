// $Id: FillerConversionsDecay.cc,v 1.20 2010/03/18 20:21:00 bendavid Exp $

#include "MitProd/TreeFiller/interface/FillerConversionsDecay.h"
#include "DataFormats/Common/interface/RefToPtr.h"
#include "MitEdm/DataFormats/interface/RefToBaseToPtr.h"
#include "MitAna/DataTree/interface/StableDataCol.h"
#include "MitAna/DataTree/interface/DecayDataCol.h"
#include "MitAna/DataTree/interface/DecayParticleCol.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitProd/ObjectService/interface/ObjectService.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerConversionsDecay::FillerConversionsDecay(const ParameterSet &cfg, const char *name, bool active) :
  BaseFiller(cfg,name,active),
  edmName_(Conf().getUntrackedParameter<string>("edmName","conversions")),
  mitName_(Conf().getUntrackedParameter<string>("mitName","Conversions")),
  stableDataName_(mitName_ + "_StableDatas"),  
  stablePartMapNames_(Conf().exists("stablePartMaps") ? 
                    Conf().getUntrackedParameter<vector<string> >("stablePartMaps") : 
                    vector<string>()),
  conversionMapName_(Conf().getUntrackedParameter<string>("conversionMapName",
                                                          Form("%sMapName",mitName_.c_str()))),
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

  if (!convElectronMapName_.empty()) {
    conversionMap_->SetBrName(mitName_);
    OS()->add(conversionMap_,conversionMapName_);
  }

  for (std::vector<std::string>::const_iterator bmapName = stablePartMapNames_.begin();
        bmapName!=stablePartMapNames_.end(); ++bmapName) {
    if (!bmapName->empty()) {
      const TrackPartMap *map = OS()->get<TrackPartMap>(*bmapName);
      if (map) {
        stablePartMaps_.push_back(map);
        AddBranchDep(mitName_,map->GetBrName());
      }
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

  Handle<reco::ConversionCollection> hConversionProduct;
  GetProduct(edmName_, hConversionProduct, event);

  conversionMap_->SetEdmProductId(hConversionProduct.id().id());
        
  const reco::ConversionCollection inConversions = *(hConversionProduct.product());  
  
  for (reco::ConversionCollection::const_iterator inConversion = inConversions.begin(); 
      inConversion != inConversions.end(); ++inConversion) {
    
    mithep::DecayParticle *outConversion = decays_->Allocate();    
    new (outConversion) mithep::DecayParticle(22);

    math::XYZTLorentzVectorD convP4 = inConversion->refittedPair4Momentum();
    FourVector p4Fitted(convP4.px(),convP4.py(),convP4.pz(),convP4.energy());
  
    outConversion->SetMom(p4Fitted);
  
    const reco::Vertex &vtx = inConversion->conversionVertex();
    ThreeVector vtxPos(vtx.x(),vtx.y(),vtx.z());
    
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
        
    const std::vector<reco::TrackBaseRef> &trackRefs = inConversion->tracks();
    const std::vector<reco::Track> &refittedTracks = vtx.refittedTracks();
    const std::vector<uint8_t> &nHitsBeforeVtx = inConversion->nHitsBeforeVtx();
    const std::vector<Measurement1DFloat> &dlClosestHitToVtx = inConversion->dlClosestHitToVtx();
    
    //fill conversion quality mask
    ConversionQuality &outQuality = outConversion->Quality();
    for (uint i=0; i<16; ++i) {
      outQuality.SetQuality(ConversionQuality::EQuality(i),inConversion->quality(reco::Conversion::ConversionQuality(i)));
    }
    
    //fill daughters
    if (stablePartMaps_.size() && trackRefs.size()==2 && refittedTracks.size()==2) {
      for (uint i=0; i<trackRefs.size(); ++i) {
        
        const reco::TrackBaseRef &trackRef = trackRefs.at(i);
        const reco::Track &refittedTrack = refittedTracks.at(i);
        
        const mithep::Particle *daughter = GetMitParticle(mitedm::refToBaseToPtr(trackRef));
        
        mithep::StableData *outStable = stableData_->Allocate();
        new (outStable) mithep::StableData(daughter,
                                           refittedTrack.px(),refittedTrack.py(),refittedTrack.pz());
        
        if (nHitsBeforeVtx.size()>i) {
          outStable->SetNHitsBeforeVtx(nHitsBeforeVtx.at(i));
        }
        if (dlClosestHitToVtx.size()>i) {
          outStable->SetDlClosestHitToVtx(dlClosestHitToVtx.at(i).value());
          outStable->SetDlClosestHitToVtxErr(dlClosestHitToVtx.at(i).error());          
        }
                                                                            
        outConversion->AddDaughterData(outStable);
      }
    }
    
    reco::ConversionRef theRef(hConversionProduct, inConversion-inConversions.begin());
    conversionMap_->Add(theRef, outConversion);
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
  
  if (!mitPart)
    throw edm::Exception(edm::errors::Configuration, "FillerConversionsDecay::FillDataBlock()\n")
    << "Error! MITHEP Object " 
    << "not found in AssociationMaps (" << typeid(*this).name() << ")." << std::endl;
    
  return mitPart;
}