// $Id: FillerGenMet.cc,v 1.1 2009/07/14 13:47:16 bendavid Exp $

#include "MitProd/TreeFiller/interface/FillerGenMet.h"
#include "DataFormats/METReco/interface/GenMET.h"
#include "DataFormats/METReco/interface/GenMETCollection.h"
#include "MitAna/DataTree/interface/GenMetCol.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitProd/ObjectService/interface/ObjectService.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerGenMet::FillerGenMet(const ParameterSet &cfg, const char *name, bool active) : 
  BaseFiller(cfg,name,active),
  edmName_(Conf().getUntrackedParameter<string>("edmName","genMetTrue")),
  mitName_(Conf().getUntrackedParameter<string>("mitName",Names::gkGenMetBrn)),
  genMets_(new mithep::GenMetArr)
{
  // Constructor.
}

//--------------------------------------------------------------------------------------------------
FillerGenMet::~FillerGenMet()
{
  // Destructor.

  delete genMets_;
}

//--------------------------------------------------------------------------------------------------
void FillerGenMet::BookDataBlock(TreeWriter &tws, const edm::EventSetup &es)
{
  // Add mets branch to tree.

  tws.AddBranch(mitName_,&genMets_);
  OS()->add<mithep::GenMetArr>(genMets_,mitName_);
}

//--------------------------------------------------------------------------------------------------
void FillerGenMet::FillDataBlock(const edm::Event      &event, 
                                  const edm::EventSetup &setup)
{
  // Fill missing energy from edm collection into our collection.

  genMets_->Delete();

  Handle<reco::GenMETCollection> hGenMetProduct;
  GetProduct(edmName_, hGenMetProduct, event);

  const reco::GenMETCollection inGenMets = *(hGenMetProduct.product());  

  // loop through all mets
  for (reco::GenMETCollection::const_iterator inGenMet = inGenMets.begin(); 
       inGenMet != inGenMets.end(); ++inGenMet) {
    
    mithep::GenMet *genMet = genMets_->Allocate();
    new (genMet) mithep::GenMet(inGenMet->px(), inGenMet->py());
    
    // fill Met base class data 
    genMet->SetSumEt(inGenMet->sumEt());
    genMet->SetElongitudinal(inGenMet->e_longitudinal());
    for(unsigned i=0; i<inGenMet->mEtCorr().size(); i++) {
      genMet->PushCorrectionX(inGenMet->mEtCorr()[i].mex);
      genMet->PushCorrectionY(inGenMet->mEtCorr()[i].mey);
      genMet->PushCorrectionSumEt(inGenMet->mEtCorr()[i].sumet);
    }
    // fill GenMet class data
    genMet->SetEmEnergy(inGenMet->emEnergy());    
    genMet->SetHadEnergy(inGenMet->hadEnergy());
    genMet->SetInvisibleEnergy(inGenMet->invisibleEnergy());
    genMet->SetAuxiliaryEnergy(inGenMet->auxiliaryEnergy());
  }
  genMets_->Trim();
}
