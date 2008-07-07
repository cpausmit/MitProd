// $Id: FillerTracks.cc,v 1.4 2008/07/03 07:56:14 loizides Exp $

#include "MitProd/TreeFiller/interface/FillerCaloJets.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/Common/interface/Handle.h"
#include "MitAna/DataTree/interface/Names.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerCaloJets::FillerCaloJets(const ParameterSet &cfg, const char *name, bool active) : 
  BaseFiller(cfg, name, active),
  edmName_(Conf().getUntrackedParameter<string>("edmName","")),
  mitName_(Conf().getUntrackedParameter<string>("mitName",Names::gkCaloJetBrn)),
  jets_(new mithep::JetArr)
{
  // Constructor.
}

//--------------------------------------------------------------------------------------------------
FillerCaloJets::~FillerCaloJets()
{
  // Destructor.

  delete jets_;
}

//--------------------------------------------------------------------------------------------------
void FillerCaloJets::BookDataBlock(TreeWriter &tws)
{
  // Add jets branch to tree.

  tws.AddBranch(mitName_.c_str(),&jets_);
}

//--------------------------------------------------------------------------------------------------
void FillerCaloJets::FillDataBlock(const edm::Event      &event, 
                                   const edm::EventSetup &setup)
{
  // Fill jets from edm collection into our collection.

  jets_->Reset();

  try {
    event.getByLabel(edm::InputTag(edmName_),jetProduct_);
  } catch (cms::Exception &ex) {
    edm::LogError("FillerCaloJets") << "Error! Cannot get collection with label " 
                                    << edmName_ << endl;
    throw edm::Exception(edm::errors::Configuration, "FillerCaloJets:FillDataBlock()\n")
      << "Error! Cannot get collection with label " << edmName_ << endl;
  }

  const reco::CaloJetCollection inJets = *(jetProduct_.product());  

  // loop through all jets
  for (reco::CaloJetCollection::const_iterator inJet = inJets.begin(); 
       inJet != inJets.end(); ++inJet) {
    
    mithep::Jet *jet = jets_->Allocate();
  }

  jets_->Trim();
}
