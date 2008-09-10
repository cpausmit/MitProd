// $Id: FillerDecayParts.cc,v 1.4 2008/07/31 12:34:04 loizides Exp $

#include "MitAna/DataTree/interface/DecayParticle.h"
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

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerDecayParts::FillerDecayParts(const ParameterSet &cfg, const char *name, bool active) :
  BaseFiller(cfg,name,active),
  edmName_(Conf().getUntrackedParameter<string>("edmName","")),
  mitName_(Conf().getUntrackedParameter<string>("mitName","")),
  basePartMapName_(Conf().getUntrackedParameter<string>("basePartMap","")),
  basePartMap_(0),
  decays_(new mithep::DecayParticleArr(250))
{
  // Constructor.
}

//--------------------------------------------------------------------------------------------------
FillerDecayParts::~FillerDecayParts()
{
  // Destructor.

  delete decays_;
}

//--------------------------------------------------------------------------------------------------
void FillerDecayParts::BookDataBlock(TreeWriter &tws)
{
  // Add tracks branch to tree and get our map.

  tws.AddBranch(mitName_.c_str(),&decays_);

  if (!basePartMapName_.empty()) 
    basePartMap_ = OS()->get<BasePartMap>(basePartMapName_.c_str());
}

//--------------------------------------------------------------------------------------------------
void FillerDecayParts::FillDataBlock(const edm::Event      &evt, 
				     const edm::EventSetup &setup)
{
  // Fill our EDM DecayPart collection into the MIT DecayParticle collection.
  cout << "Decay part block" << endl;
  decays_->Reset();

  Handle<mitedm::DecayPartCol> hParts;
  GetProduct(edmName_, hParts, evt);  
  const mitedm::DecayPartCol *iParts = hParts.product();
  
  // loop through all decayParts and fill the information
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
    
    d->SetChi2(p.chi2());
    d->SetNdof(p.ndof());

    //loop through and add daughters
    if (basePartMap_) {
      for (Int_t j=0; j<p.nChild();j++) {
        mitedm::BasePartBaseRef theRef = p.getChildRef(j);
        mithep::PairIntKey theKey(theRef.id().id(),theRef.key());
        mithep::Particle *daughter = basePartMap_->GetMit(theKey);
        d->AddDaughter(daughter);
      }
    }
    //cout << "MITHEP...\n";d->print();
  }
  decays_->Trim();
}
