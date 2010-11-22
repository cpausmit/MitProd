// $Id: FillerDecayParts.cc,v 1.21 2010/06/23 09:02:45 bendavid Exp $

#include "MitProd/TreeFiller/interface/FillerDecayParts.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/TrackReco/interface/HitPattern.h"
#include "DataFormats/TrackingRecHit/interface/TrackingRecHit.h"
#include "DataFormats/RecoCandidate/interface/TrackAssociation.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticleFwd.h"
#include "MitAna/DataTree/interface/StableDataCol.h"
#include "MitAna/DataTree/interface/DecayDataCol.h"
#include "MitAna/DataTree/interface/DecayParticleCol.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitEdm/DataFormats/interface/Collections.h"
#include "MitProd/ObjectService/interface/ObjectService.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerDecayParts::FillerDecayParts(const ParameterSet &cfg, const char *name, bool active) :
  BaseFiller(cfg,name,active),
  edmName_(Conf().getUntrackedParameter<string>("edmName","")),
  mitName_(Conf().getUntrackedParameter<string>("mitName","")),
  stableDataName_(mitName_ + "_StableDatas"),
  decayDataName_(mitName_ + "_DecayDatas"),
  vertexMapName_(Conf().getUntrackedParameter<string>("vertexMap","")),
  basePartMapNames_(Conf().exists("basePartMaps") ? 
                    Conf().getUntrackedParameter<vector<string> >("basePartMaps") : 
                    vector<string>()),
  vertexMap_(0),
  decays_(new mithep::DecayParticleArr(250)),
  stableData_(new mithep::StableDataArr(500)),
  decayData_(new mithep::DecayDataArr(500))
{
  // Constructor.
}

//--------------------------------------------------------------------------------------------------
FillerDecayParts::~FillerDecayParts()
{
  // Destructor.

  delete decays_;
  delete stableData_;
  delete decayData_;
}

//--------------------------------------------------------------------------------------------------
void FillerDecayParts::BookDataBlock(TreeWriter &tws)
{
  // Add our branches to tree and get our map.
  
  tws.AddBranch(mitName_,        &decays_);
  OS()->add<mithep::DecayParticleArr>(decays_,mitName_);
  tws.AddBranch(stableDataName_, &stableData_);
  OS()->add<mithep::StableDataArr>(stableData_,stableDataName_);
  tws.AddBranch(decayDataName_,  &decayData_);
  OS()->add<mithep::DecayDataArr>(decayData_,decayDataName_);

  if (!vertexMapName_.empty()) {
    vertexMap_ = OS()->get<VertexMap>(vertexMapName_);
    if (vertexMap_)
      AddBranchDep(mitName_,vertexMap_->GetBrName());
  }

  for (std::vector<std::string>::const_iterator bmapName = basePartMapNames_.begin();
        bmapName!=basePartMapNames_.end(); ++bmapName) {
    if (!bmapName->empty()) {
      const BasePartMap *map = OS()->get<BasePartMap>(*bmapName);
      if (map) {
        basePartMaps_.push_back(map);
        AddBranchDep(stableDataName_,map->GetBrName());
        AddBranchDep(decayDataName_,map->GetBrName());
      }
    }
  }
}

//--------------------------------------------------------------------------------------------------
void FillerDecayParts::FillDataBlock(const edm::Event      &evt, 
				     const edm::EventSetup &setup)
{
  // Fill the EDM DecayPart collection into the MIT DecayParticle collection.

  decays_->Delete();
  stableData_->Delete();
  decayData_->Delete();

  Handle<mitedm::DecayPartCol> hParts;
  GetProduct(edmName_, hParts, evt);  
  const mitedm::DecayPartCol *iParts = hParts.product();
  
  // loop through all DecayParts and fill the information
  for (UInt_t i=0; i<iParts->size(); ++i) {
    const mitedm::DecayPart &p = iParts->at(i); // for convenience
    mithep::DecayParticle *d = decays_->Allocate();
    new (d) mithep::DecayParticle(p.pid());
    
    d->SetMassError(p.fittedMassError());
    d->SetLxy(p.lxyToPv());
    d->SetLxyError(p.lxyToPvError());
    d->SetDxy(p.dxyToPv());
    d->SetDxyError(p.dxyToPvError());
    d->SetLz(p.lzToPv());
    d->SetLzError(p.lzToPvError());
    d->SetMom(p.fourMomentum());
    d->SetChi2(p.chi2());
    d->SetNdof(p.ndof());
    
    if (p.primaryVertex().isNonnull()) {
      d->SetPriVertex(vertexMap_->GetMit(p.primaryVertex()));
    }

    if(0) {
      printf("In  Particle position: x = %5f, y = %5f, z = %5f\n",p.position().x(),p.position().y(),p.position().z());
      printf("Out Particle position: x = %5f, y = %5f, z = %5f\n",d->Position().x(),d->Position().y(),d->Position().z());
    }      

    //fill shared layer bitmask
    const reco::HitPattern &sharedHitPattern = p.sharedHits();
    BitMask48 sharedLayers;
    UInt_t numShared=0;
    // search for all good crossed layers (with or without hit)
    for (Int_t hi=0; hi<sharedHitPattern.numberOfHits(); hi++) {
      uint32_t hit = sharedHitPattern.getHitPattern(hi);
      if (sharedHitPattern.getHitType(hit)<=1) {
        if (sharedHitPattern.trackerHitFilter(hit)) {
          numShared++;
          sharedLayers.SetBit(hitReader_.Layer(hit));
        }
      }
    }

    d->SetSharedLayers(sharedLayers);  
    
    if (basePartMaps_.size()) {
       // loop through and add stable daughters
      for (Int_t j=0; j<p.nStableChild(); ++j) {
        const mitedm::StableData &stable = p.getStableData(j);
        mitedm::BasePartPtr thePtr = stable.originalPtr();
        mithep::Particle *daughter = getMitParticle(thePtr);
        
        mithep::StableData *outStable = stableData_->Allocate();
        new (outStable) mithep::StableData(daughter,
                                           stable.p3().x(),stable.p3().y(),stable.p3().z());
        
        // fill bad layer mask information using corrected HitPattern
        const ChargedParticle *cDaughter = dynamic_cast<const ChargedParticle*>(daughter);
        if (stable.HitsFilled() && cDaughter) {
          const reco::HitPattern &hitPattern = stable.Hits();
          BitMask48 crossedLayers;
          UInt_t numCrossed=0;
          // search for all good crossed layers (with or without hit)
          for (Int_t hi=0; hi<hitPattern.numberOfHits(); hi++) {
            uint32_t hit = hitPattern.getHitPattern(hi);
            if (hitPattern.getHitType(hit)<=1) {
              if (hitPattern.trackerHitFilter(hit)) {
                numCrossed++;
                crossedLayers.SetBit(hitReader_.Layer(hit));
              }
            }
          }

          BitMask48 badLayers = crossedLayers ^ cDaughter->Trk()->Hits();
          outStable->SetBadLayers(badLayers);
          
          if(verbose_>1) {
            if (outStable->NWrongHits()) {          
              printf("numCrossed:                %i\n",numCrossed);
              printf("Hit Pattern Size:          %i\n",hitPattern.numberOfHits());
              printf("# of hits:                 %i\n",cDaughter->Trk()->NHits());
              printf("# of crossed layers:       %i\n",crossedLayers.NBitsSet());            
              printf("# of wrong/missing layers: %i\n",badLayers.NBitsSet());
              printf("# of missed layers:        %i\n",outStable->NMissedLayers());
              printf("# of wrong layers:         %i\n",outStable->NWrongLayers());
              printf("Hits              : ");
              for (Int_t biti=63; biti >= 0; --biti) {
                printf("%i",cDaughter->Trk()->Hits().TestBit(biti));
              }
              printf("\n");
              printf("Crossed Layers    : ");
              for (Int_t biti=63; biti >= 0; --biti) {
                printf("%i",crossedLayers.TestBit(biti));
              }
              printf("\n");
              printf("WrongOrMissingHits: ");
              for (Int_t biti=63; biti>=0; --biti) {
                printf("%i",badLayers.TestBit(biti));
              }
              printf("\n");    
            }        
          }
        }

        d->AddDaughterData(outStable);
        
      }

      // loop through and add decay daughters
      for (Int_t j=0; j<p.nDecayChild(); ++j) {
        const mitedm::DecayData &decay = p.getDecayData(j);
        mitedm::BasePartPtr thePtr = decay.originalPtr();
        mithep::Particle *daughter = getMitParticle(thePtr);
        
        mithep::DecayData *outDecay = decayData_->Allocate();
        new (outDecay) mithep::DecayData(daughter,decay.p4());
        outDecay->SetMassError(decay.massErr());
        outDecay->SetLxy(decay.lxy());
        outDecay->SetLxyError(decay.lxyErr());
        outDecay->SetLz(decay.lz());
        outDecay->SetLzError(decay.lzErr());
        outDecay->SetDxy(decay.dxy());
        outDecay->SetDxyError(decay.dxyErr());
        
        d->AddDaughterData(outDecay);
      }
    }
  }
  decays_->Trim();
  stableData_->Trim();
  decayData_->Trim();
}

//--------------------------------------------------------------------------------------------------
mithep::Particle *FillerDecayParts::getMitParticle(mitedm::BasePartPtr ptr) const
{
  // Return our particle referenced by the edm pointer.

  mithep::Particle *mitPart = 0;
  for (std::vector<const mithep::BasePartMap*>::const_iterator bmap = basePartMaps_.begin();
        bmap!=basePartMaps_.end(); ++bmap) {
    const mithep::BasePartMap *theMap = *bmap;
    if (theMap->HasMit(ptr)) {
      mitPart = theMap->GetMit(ptr);
      return mitPart;
    }
  }
  
  if (!mitPart)
    throw edm::Exception(edm::errors::Configuration, "FillerDecayParts::FillDataBlock()\n")
    << "Error! MITHEP Object " 
    << "not found in AssociationMaps (" << typeid(*this).name() << ")." << std::endl;
    
  return mitPart;
}
