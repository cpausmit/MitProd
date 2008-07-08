// $Id: FillerCaloMet.cc,v 1.1 2008/07/07 16:13:22 loizides Exp $

#include "MitProd/TreeFiller/interface/FillerCaloMet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/METReco/interface/CaloMET.h"
#include "MitAna/DataTree/interface/Names.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerCaloMet::FillerCaloMet(const ParameterSet &cfg, const char *name, bool active) : 
  BaseFiller(cfg, name, active),
  edmName_(Conf().getUntrackedParameter<string>("edmName","")),
  mitName_(Conf().getUntrackedParameter<string>("mitName",Names::gkCaloJetBrn)),
  mets_(new mithep::MetArr)
{
  // Constructor.
}

//--------------------------------------------------------------------------------------------------
FillerCaloMet::~FillerCaloMet()
{
  // Destructor.

  delete mets_;
}

//--------------------------------------------------------------------------------------------------
void FillerCaloMet::BookDataBlock(TreeWriter &tws)
{
  // Add mets branch to tree.

  tws.AddBranch(mitName_.c_str(),&mets_);
}

//--------------------------------------------------------------------------------------------------
void FillerCaloMet::FillDataBlock(const edm::Event      &event, 
                                  const edm::EventSetup &setup)
{
  // Fill missing energy from edm collection into our collection.

  mets_->Reset();

  Handle<reco::CaloMETCollection> hMetProduct;
  GetProduct(edmName_, hMetProduct, event);

  const reco::CaloMETCollection inMets = *(hMetProduct.product());  

  // loop through all mets
  for (reco::CaloMETCollection::const_iterator inMet = inMets.begin(); 
       inMet != inMets.end(); ++inMet) {
    
    mithep::Met *met = mets_->Allocate();
    if (!met) continue;
  }

  mets_->Trim();

}
