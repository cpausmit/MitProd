// $Id: FillerDecayParts.cc,v 1.6 2008/09/19 11:40:15 bendavid Exp $

#include "MitAna/DataTree/interface/DecayParticle.h"
#include "MitAna/DataTree/interface/DaughterData.h"
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
  decays_(new mithep::DecayParticleArr(250))
{
  // Constructor.
  if (Conf().exists("basePartMaps"))
    basePartMapNames_ = Conf().getUntrackedParameter<vector<string> >("basePartMaps");
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
    if (basePartMaps_.size()) {
      for (Int_t j=0; j<p.nChild();++j) {
        mitedm::BasePartPtr thePtr = p.getChildPtr(j);
        mithep::Particle *daughter = 0;
        for (std::vector<const mithep::BasePartMap*>::const_iterator bmap = basePartMaps_.begin();
              bmap!=basePartMaps_.end(); ++bmap) {
          const mithep::BasePartMap *theMap = *bmap;
          if (theMap->HasMit(thePtr))
            daughter = theMap->GetMit(thePtr);
        }
        
        if (!daughter)
          throw edm::Exception(edm::errors::Configuration, "FillerDecayParts::FillDataBlock()\n")
         << "Error! MITHEP Object " 
         << "not found in AssociationMaps (" << typeid(*this).name() << ")." << std::endl;
              
        if (p.nChild()==p.nChildMom())
          d->AddDaughter(daughter, mithep::DaughterData(p.getChildMom(j)));
        else
          d->AddDaughter(daughter);
      }
    }
    //cout << "MITHEP...\n";d->print();
  }
  decays_->Trim();
}
