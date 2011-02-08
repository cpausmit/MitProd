// $Id: FillerPileupInfo.cc,v 1.5 2010/03/18 20:21:00 bendavid Exp $

#include "MitProd/TreeFiller/interface/FillerPileupInfo.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/Common/interface/Handle.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataTree/interface/PileupInfoCol.h"
#include "MitProd/ObjectService/interface/ObjectService.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerPileupInfo::FillerPileupInfo(const ParameterSet &cfg, const char *name, bool active) : 
  BaseFiller(cfg,name,active),
  edmName_(Conf().getUntrackedParameter<string>("edmName","addPileupInfo")),
  mitName_(Conf().getUntrackedParameter<string>("mitName",Names::gkPileupInfoBrn)),
  puInfos_(new mithep::PileupInfoArr)
{
  // Constructor.
}

//--------------------------------------------------------------------------------------------------
FillerPileupInfo::~FillerPileupInfo()
{
  // Destructor.

  delete puInfos_;
}

//--------------------------------------------------------------------------------------------------
void FillerPileupInfo::BookDataBlock(TreeWriter &tws)
{
  // Add pileup branch to tree.
  tws.AddBranch(mitName_,&puInfos_);
  OS()->add<mithep::PileupInfoArr>(puInfos_,mitName_);
}

//--------------------------------------------------------------------------------------------------
void FillerPileupInfo::FillDataBlock(const edm::Event      &event, 
				     const edm::EventSetup &setup)
{
  // Fill missing energy from edm collection into our collection.

  puInfos_->Delete();

  Handle<PileupSummaryInfo> hPileupInfoProduct;
  GetProduct(edmName_, hPileupInfoProduct, event);

  mithep::PileupInfo *puInfo = puInfos_->AddNew();

  puInfo->SetPU_NumInteractions(hPileupInfoProduct->getPU_NumInteractions());

  for(int i=0; i<hPileupInfoProduct->getPU_NumInteractions(); i++){
    puInfo->PushPU_zPositions(Double32_t(hPileupInfoProduct->getPU_zpositions()[i]));
    puInfo->PushPU_sumpT_lowpT(Double32_t(hPileupInfoProduct->getPU_sumpT_lowpT()[i]));   
    puInfo->PushPU_sumpT_highpT(Double32_t(hPileupInfoProduct->getPU_sumpT_highpT()[i]));  
    puInfo->PushPU_ntrks_lowpT(Double32_t(hPileupInfoProduct->getPU_ntrks_lowpT()[i]));   
    puInfo->PushPU_ntrks_highpT(Double32_t(hPileupInfoProduct->getPU_ntrks_highpT()[i]));  
  }
  puInfos_->Trim();
}
