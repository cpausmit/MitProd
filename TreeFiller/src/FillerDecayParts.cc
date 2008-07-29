// $Id: FillerDecayParts.cc,v 1.1 2008/07/28 23:13:44 paus Exp $

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticleFwd.h"
#include "DataFormats/RecoCandidate/interface/TrackAssociation.h"

#include "MitEdm/DataFormats/interface/CollectionsEdm.h"
#include "MitEdm/DataFormats/interface/DecayPart.h"
#include "MitEdm/DataFormats/interface/BasePart.h"
#include "MitEdm/DataFormats/interface/BasePartFwd.h"
#include "MitProd/TreeFiller/interface/FillerDecayParts.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataTree/interface/DecayParticle.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerDecayParts::FillerDecayParts(const ParameterSet &cfg, const char *name, bool active,
                                   const BasePartMap *partMap) : 
  BaseFiller(cfg,name,active),
  edmName_  (Conf().getUntrackedParameter<string>("edmName","")),
  mitName_  (Conf().getUntrackedParameter<string>("mitName","")),
  partMap_  (partMap),
  decays_   (new mithep::DecayParticleArr(250))
{
  // Constructor
}

//--------------------------------------------------------------------------------------------------
FillerDecayParts::~FillerDecayParts()
{
  // Destructor
  delete decays_;
}

//--------------------------------------------------------------------------------------------------
void FillerDecayParts::BookDataBlock(TreeWriter &tws)
{
  // Add tracks branch to tree
  tws.AddBranch(mitName_.c_str(),&decays_);
}

//--------------------------------------------------------------------------------------------------
void FillerDecayParts::FillDataBlock(const edm::Event      &evt, 
				     const edm::EventSetup &setup)
{
  // -----------------------------------------------------------------------------------------------
  // fill edm DecayPart collection into the MIT DecayPart collection
  // -----------------------------------------------------------------------------------------------
  decays_->Reset();
  // access the edm DecayPart collection
  Handle<mitedm::DecayPartCol> hParts;
  GetProduct(edmName_, hParts, evt);  
  const mitedm::DecayPartCol *iParts = hParts.product();
  
  // -----------------------------------------------------------------------------------------------
  // loop through all decayParts and fill the information
  // -----------------------------------------------------------------------------------------------
  for (UInt_t i=0; i<iParts->size(); ++i) {
    const mitedm::DecayPart &p =iParts->at(i);                    // for convenience
    //cout << "MITEDM...\n";p->print();
    mithep::DecayParticle *d = decays_->Allocate();
    new (d) mithep::DecayParticle(p.pid(),(mithep::DecayParticle::DecayType)p.decayType());
    
    d->SetFittedMass(p.fittedMass());
    d->SetFittedMassError(p.fittedMassError());
    d->SetLxy(p.lxy());
    d->SetLxyError(p.lxyError());
    d->SetLxyToPv(p.lxyToPv());
    d->SetLxyToPvError(p.lxyToPvError());
    d->SetDxy(p.dxy());
    d->SetDxyError(p.dxyError());
    d->SetDxyToPv(p.dxyToPv());
    d->SetDxyToPvError(p.dxyToPvError());
    d->SetLz(p.lz());
    d->SetLzError(p.lzError());
    d->SetLzToPv(p.lzToPv());
    d->SetLzToPvError(p.lzToPvError());
    d->SetCTau(p.cTau());
    d->SetCTauError(p.cTauError());
    d->SetPt(p.pt());
    d->SetPtError(p.ptError());
    d->SetMom(p.fourMomentum());
    d->SetPosition(p.position());
    d->SetError(p.error());
    d->SetBigError(p.bigError());
    
    //loop through and add daughters
    if (partMap_) {
      for (Int_t j=0; j<p.nChild();j++) {
        mitedm::BasePartBaseRef theRef = p.getChildRef(j);
        mithep::DoubleIntKey theKey(theRef.id().id(),theRef.key());
        mithep::Particle *daughter = partMap_->GetMit(theKey);
        d->AddDaughter(daughter);
      }
    }
    //cout << "MITHEP...\n";d->print();
  }
  decays_->Trim();
}
