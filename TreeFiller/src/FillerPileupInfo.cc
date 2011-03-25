// $Id: FillerPileupInfo.cc,v 1.1 2011/02/08 14:54:13 mzanetti Exp $

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

  Handle<std::vector< PileupSummaryInfo > >  hPileupInfoProduct;
  event.getByLabel(edmName_, hPileupInfoProduct);

  std::vector<PileupSummaryInfo>::const_iterator edmPUInfo = hPileupInfoProduct->begin();
  for (; edmPUInfo != hPileupInfoProduct->end(); ++edmPUInfo) {

    mithep::PileupInfo *puInfo = puInfos_->AddNew();

    puInfo->SetBunchCrossing(edmPUInfo->getBunchCrossing());
    puInfo->SetPU_NumInteractions(edmPUInfo->getPU_NumInteractions());
    for(int i=0; i<edmPUInfo->getPU_NumInteractions(); i++){
      puInfo->PushPU_zPositions(Double32_t(edmPUInfo->getPU_zpositions()[i]));
      puInfo->PushPU_sumpT_lowpT(Double32_t(edmPUInfo->getPU_sumpT_lowpT()[i]));   
      puInfo->PushPU_sumpT_highpT(Double32_t(edmPUInfo->getPU_sumpT_highpT()[i]));  
      puInfo->PushPU_ntrks_lowpT(Double32_t(edmPUInfo->getPU_ntrks_lowpT()[i]));   
      puInfo->PushPU_ntrks_highpT(Double32_t(edmPUInfo->getPU_ntrks_highpT()[i]));  
    }
  }

  puInfos_->Trim();
}
