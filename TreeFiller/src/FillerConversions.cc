#include "MitProd/TreeFiller/interface/FillerConversions.h"
#include "DataFormats/Common/interface/RefToPtr.h"
#include "MitEdm/DataFormats/interface/RefToBaseToPtr.h"
#include "MitAna/DataTree/interface/ConversionCol.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitProd/ObjectService/interface/ObjectService.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerConversions::FillerConversions(const ParameterSet &cfg, edm::ConsumesCollector& collector, ObjectService* os, const char *name, bool active) :
  BaseFiller         (cfg,os,name,active),
  edmToken_          (GetToken<reco::ConversionCollection>(collector, cfg, "edmName")), //conversions
  mitName_           (cfg.getUntrackedParameter<string>("mitName","Conversions")),
  checkTrackRef_     (cfg.getUntrackedParameter<bool>  ("checkTrackRef" ,true  )),
  stablePartMapNames_(cfg.getUntrackedParameter<vector<string> >("stablePartMaps", vector<string>())),
  conversionMapName_ (cfg.getUntrackedParameter<string>("conversionMapName", mitName_ + "MapName")),
  conversions_       (new mithep::ConversionArr(16)),
  conversionMap_     (new mithep::ConversionMap)
{
  // Constructor.
}

//--------------------------------------------------------------------------------------------------
FillerConversions::~FillerConversions()
{
  // Destructor.

  delete conversions_;
  delete conversionMap_;
}

//--------------------------------------------------------------------------------------------------
void FillerConversions::BookDataBlock(TreeWriter &tws)
{
  // Add conversions to tree. Publish and get our objects.

  tws.AddBranch(mitName_,&conversions_);
  OS()->add<mithep::ConversionArr>(conversions_,mitName_);

  if (!conversionMapName_.empty()) {
    conversionMap_->SetBrName(mitName_);
    OS()->add(conversionMap_,conversionMapName_);
  }
}

void FillerConversions::PrepareLinks()
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
void FillerConversions::FillDataBlock(const edm::Event      &event, 
                                      const edm::EventSetup &setup)
{
  // Fill conversions data structure and maps.

  conversions_->Delete();
  conversionMap_->Reset();

  Handle<reco::ConversionCollection> hConversionProduct;
  GetProduct(edmToken_, hConversionProduct, event);

  conversionMap_->SetEdmProductId(hConversionProduct.id().id());
        
  reco::ConversionCollection const& inConversions = *hConversionProduct;
  
  for (reco::ConversionCollection::const_iterator inConversion = inConversions.begin(); 
      inConversion != inConversions.end(); ++inConversion) {
        
    mithep::Vertex vertex(inConversion->conversionVertex().x(),
                          inConversion->conversionVertex().y(),
                          inConversion->conversionVertex().z(), 
                          inConversion->conversionVertex().xError(),
                          inConversion->conversionVertex().yError(),
                          inConversion->conversionVertex().zError());
    vertex.SetChi2(inConversion->conversionVertex().chi2());
    vertex.SetNdof(inConversion->conversionVertex().ndof());
    vertex.SetNTracksFit(inConversion->conversionVertex().tracksSize());
  
    mithep::Conversion *outConversion = conversions_->Allocate();
    new (outConversion) mithep::Conversion(vertex);
    outConversion->SetDCotTheta(inConversion->pairCotThetaSeparation());
    outConversion->SetEOverP(inConversion->EoverP());
    outConversion->SetPairMass(inConversion->pairInvariantMass());
    outConversion->SetPairMomentum(inConversion->pairMomentum().x(),
                                   inConversion->pairMomentum().y(),
                                   inConversion->pairMomentum().z());
        
    if (stablePartMaps_.size()) {
      std::vector<reco::TrackBaseRef> const& trackRefs = inConversion->tracks();
      for (std::vector<reco::TrackBaseRef>::const_iterator trackRef = trackRefs.begin(); 
           trackRef != trackRefs.end(); ++trackRef) {
        if(GetMitParticle(mitedm::refToBaseToPtr(*trackRef))) outConversion->AddDaughter(GetMitParticle(mitedm::refToBaseToPtr(*trackRef)));
      }
    }
    
    reco::ConversionRef theRef(hConversionProduct, inConversion-inConversions.begin());
    conversionMap_->Add(theRef, outConversion);
  }
  conversions_->Trim();
}

//--------------------------------------------------------------------------------------------------
mithep::Particle *FillerConversions::GetMitParticle(edm::Ptr<reco::Track> ptr) const
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
  
  if (!mitPart && checkTrackRef_ )
    throw edm::Exception(edm::errors::Configuration, "FillerConversions::FillDataBlock()\n")
      << "Error! MITHEP Object " 
      << "not found in AssociationMaps (" << typeid(*this).name() << ")." << std::endl;
    
  return mitPart;
}

DEFINE_MITHEP_TREEFILLER(FillerConversions);
