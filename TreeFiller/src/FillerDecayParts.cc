// $Id: FillerDecayParts.cc,v 1.12 2008/10/23 15:43:15 loizides Exp $

#include "MitAna/DataTree/interface/DecayParticle.h"
#include "MitAna/DataTree/interface/DaughterData.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/TrackReco/interface/HitPattern.h"
#include "DataFormats/TrackingRecHit/interface/TrackingRecHit.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticleFwd.h"
#include "DataFormats/RecoCandidate/interface/TrackAssociation.h"
#include "MitEdm/DataFormats/interface/Collections.h"
#include "MitEdm/DataFormats/interface/DecayPart.h"
#include "MitEdm/DataFormats/interface/BasePart.h"
#include "MitEdm/DataFormats/interface/BasePartFwd.h"
#include "MitProd/TreeFiller/interface/FillerDecayParts.h"
#include "MitAna/DataTree/interface/Names.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerDecayParts::FillerDecayParts(const ParameterSet &cfg, const char *name, bool active) :
  BaseFiller(cfg,name,active),
  edmName_(Conf().getUntrackedParameter<string>("edmName","")),
  mitName_(Conf().getUntrackedParameter<string>("mitName","")),
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

  std::string stableDataName = mitName_ + "_StableDatas";
  std::string decayDataName  = mitName_ + "_DecayDatas";
  
  tws.AddBranch(mitName_.c_str(),       &decays_);
  tws.AddBranch(stableDataName.c_str(), &stableData_);
  tws.AddBranch(decayDataName.c_str(),  &decayData_);

  if (!vertexMapName_.empty())
    vertexMap_ = OS()->get<VertexMap>(vertexMapName_.c_str());
  
  for (std::vector<std::string>::const_iterator bmapName = basePartMapNames_.begin();
        bmapName!=basePartMapNames_.end(); ++bmapName) {
    if (!bmapName->empty()) 
      basePartMaps_.push_back(OS()->get<BasePartMap>(bmapName->c_str()));
  }
}

//--------------------------------------------------------------------------------------------------
void FillerDecayParts::FillDataBlock(const edm::Event      &evt, 
				     const edm::EventSetup &setup)
{
  // Fill our EDM DecayPart collection into the MIT DecayParticle collection.

  decays_->Reset();
  stableData_->Reset();
  decayData_->Reset();

  Handle<mitedm::DecayPartCol> hParts;
  GetProduct(edmName_, hParts, evt);  
  const mitedm::DecayPartCol *iParts = hParts.product();
  
  // loop through all decayParts and fill the information
  for (UInt_t i=0; i<iParts->size(); ++i) {
    const mitedm::DecayPart &p =iParts->at(i);                    // for convenience
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
      
    if (basePartMaps_.size()) {
       //loop through and add stable daughters
      for (Int_t j=0; j<p.nStableChild(); ++j) {
        const mitedm::StableData &stable = p.getStableData(j);
        mitedm::BasePartPtr thePtr = stable.originalPtr();
        mithep::Particle *daughter = getMitParticle(thePtr);
        
        mithep::StableData *outStable = stableData_->Allocate();
        new (outStable) mithep::StableData(daughter,stable.p3().x(),stable.p3().y(),stable.p3().z());
        
        //fill bad layer mask information using corrected HitPattern
        const ChargedParticle *cDaughter = dynamic_cast<const ChargedParticle*>(daughter);
        if (cDaughter) {
          const reco::HitPattern &hitPattern = stable.Hits();
          BitMask48 crossedLayers;
          UInt_t numCrossed=0;
          //search for all good crossed layers (with or without hit)
          for (Int_t hi=0; hi<hitPattern.numberOfHits(); hi++) {
            uint32_t hit = hitPattern.getHitPattern(hi);
            if (hitPattern.getHitType(hit)<=1)
              if (hitPattern.trackerHitFilter(hit)) {
                numCrossed++;
                crossedLayers.SetBit(hitReader_.Layer(hit));
              }
          }
          BitMask48 badLayers = crossedLayers ^ cDaughter->Trk()->Hits();
          outStable->SetBadLayers(badLayers);
          
          if(0) {
            if (outStable->NWrongHits()) {          
              printf("numCrossed:                %i\n",numCrossed);
              printf("Hit Pattern Size:          %i\n",hitPattern.numberOfHits());
              printf("# of hits:                 %i\n",cDaughter->Trk()->NHits());
              printf("# of crossed layers:       %i\n",crossedLayers.NBitsSet());            
              printf("# of wrong/missing layers: %i\n",badLayers.NBitsSet());
              printf("# of missed hits:          %i\n",outStable->NMissedHits());
              printf("# of wrong hits:           %i\n",outStable->NWrongHits());
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

      //loop through and add decay daughters
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
    if (theMap->HasMit(ptr))
      mitPart = theMap->GetMit(ptr);
  }
  
  if (!mitPart)
    throw edm::Exception(edm::errors::Configuration, "FillerDecayParts::FillDataBlock()\n")
    << "Error! MITHEP Object " 
    << "not found in AssociationMaps (" << typeid(*this).name() << ")." << std::endl;
    
  return mitPart;
}
