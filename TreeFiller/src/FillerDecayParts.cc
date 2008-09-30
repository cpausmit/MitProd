// $Id: FillerDecayParts.cc,v 1.8 2008/09/27 05:50:47 loizides Exp $

#include "MitAna/DataTree/interface/DecayParticle.h"
#include "MitAna/DataTree/interface/DaughterData.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
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
  // Add tracks branch to tree and get our map.
  std::string stableDataName = mitName_ + "_StableDatas";
  std::string decayDataName  = mitName_ + "_DecayDatas";
  
  tws.AddBranch(mitName_.c_str(),&decays_);
  tws.AddBranch(stableDataName.c_str(), &stableData_);
  tws.AddBranch(decayDataName.c_str(), &decayData_);

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

    if (p.primaryVertex().isNonnull())
      d->SetPriVertex(vertexMap_->GetMit(p.primaryVertex()));
    
    if (basePartMaps_.size()) {
       //loop through and add stable daughters
      for (Int_t j=0; j<p.nStableChild(); ++j) {
        const mitedm::StableData &stable = p.getStableData(j);
        mitedm::BasePartPtr thePtr = stable.originalPtr();
        mithep::Particle *daughter = getMitParticle(thePtr);
        
        mithep::StableData *outStable = stableData_->Allocate();
        new (outStable) mithep::StableData(daughter,stable.p3().x(),stable.p3().y(),stable.p3().z());
        
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
/*    printf("mitedm::DecayPart              position=%5f,%5f,%5f\n",p.position().x(),
                                                          p.position().y(),
                                                          p.position().z());
    printf("mithep::DecayParticle          position=%5f,%5f,%5f\n",d->Position().X(),
                                                          d->Position().Y(),
                                                          d->Position().Z());
    printf("mithep::DecayParticle relative position=%5f,%5f,%5f\n",d->RelativePosition().X(),
                                                          d->RelativePosition().Y(),
                                                          d->RelativePosition().Z()); */                                                                                                    
    //cout << "MITHEP...\n";d->print();
  }
  decays_->Trim();
  stableData_->Trim();
  decayData_->Trim();
}

//--------------------------------------------------------------------------------------------------
mithep::Particle *FillerDecayParts::getMitParticle(mitedm::BasePartPtr ptr) const
{
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
