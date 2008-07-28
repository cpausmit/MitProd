// $Id: FillerDecayParts.cc,v 1.8 2008/07/14 21:01:00 loizides Exp $

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticleFwd.h"
#include "DataFormats/RecoCandidate/interface/TrackAssociation.h"

#include "MitEdm/AnalysisDataFormats/interface/CollectionsEdm.h"
#include "MitEdm/AnalysisDataFormats/interface/DecayPart.h"
#include "MitProd/TreeFiller/interface/FillerDecayParts.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataTree/interface/DecayPart.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerDecayParts::FillerDecayParts(const ParameterSet &cfg, const char *name, bool active) : 
  BaseFiller(cfg,name,active),
  edmName_  (Conf().getUntrackedParameter<string>("edmName","")),
  mitName_  (Conf().getUntrackedParameter<string>("mitName","")),
  decays_   (new mithep::DecayPartObjArr(250))
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
  Handle<mitedm::BasePartObjArr> hParts;
  GetProduct(edmName_, hParts, evt);  
  const mitedm::BasePartObjArr *iParts = hParts.product();
  
  // -----------------------------------------------------------------------------------------------
  // loop through all decayParts and fill the information
  // -----------------------------------------------------------------------------------------------
  for (UInt_t i=0; i<iParts->Entries(); ++i) {
    mitedm::DecayPart *p = (mitedm::DecayPart*) iParts->At(i);                    // for convenience
    //cout << "MITEDM...\n";p->print();
    DecayPart *d = new DecayPart((const mithep::DecayPart&) *p);
    decays_->Add(d);
    //cout << "MITHEP...\n";d->print();
  }
  decays_->Trim();
}
