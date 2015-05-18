#include "MitProd/TreeFiller/interface/FillerPileupInfo.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/Common/interface/Handle.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataTree/interface/PileupInfoCol.h"
#include "MitProd/ObjectService/interface/ObjectService.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerPileupInfo::FillerPileupInfo(const ParameterSet &cfg, edm::ConsumesCollector& collector, ObjectService* os, const char *name, bool active) : 
  BaseFiller(cfg,os,name,active),
  edmToken_(GetToken<std::vector<PileupSummaryInfo> >(collector, "edmName","addPileupInfo")),
  edmSingleToken_(GetToken<PileupSummaryInfo>(collector, "edmName","addPileupInfo")),
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

  std::vector<PileupSummaryInfo const*> inInfos;

  Handle<std::vector< PileupSummaryInfo> >  hPileupInfoProduct;
  if(GetProductSafe(edmToken_, hPileupInfoProduct, event)){
    std::vector<PileupSummaryInfo> const& inInfoCollection = *hPileupInfoProduct;
    for (auto&& info : inInfoCollection)
      inInfos.push_back(&info);
  }
  else {
    Handle<PileupSummaryInfo>  hSinglePileupInfoProduct;
    GetProduct(edmSingleToken_, hSinglePileupInfoProduct, event);
    inInfos.push_back(hSinglePileupInfoProduct.product());
  }

  for (auto edmPUInfo : inInfos) {
    //printf("filling puinfo for bx %i with %i interactions\n",edmPUInfo->getBunchCrossing(), edmPUInfo->getPU_NumInteractions());
    //printf("vector sizes: %i, %i, %i, %i, %i\n",int(edmPUInfo->getPU_zpositions().size()),int(edmPUInfo->getPU_sumpT_lowpT().size()),int(edmPUInfo->getPU_sumpT_highpT().size()),int(edmPUInfo->getPU_ntrks_lowpT().size()),int(edmPUInfo->getPU_ntrks_highpT().size()));
    mithep::PileupInfo *puInfo = puInfos_->AddNew();

    puInfo->SetBunchCrossing(edmPUInfo->getBunchCrossing());
    puInfo->SetPU_NumInteractions(edmPUInfo->getPU_NumInteractions());
    puInfo->SetPU_NumMean(edmPUInfo->getTrueNumInteractions());    
    for(int i=0; i<edmPUInfo->getPU_NumInteractions(); i++){
      //printf("filling interaction %i\n",i);
      if (uint(i)<edmPUInfo->getPU_zpositions().size()) puInfo->PushPU_zPositions(Double32_t(edmPUInfo->getPU_zpositions()[i])); else puInfo->PushPU_zPositions(-99);
      if (uint(i)<edmPUInfo->getPU_sumpT_lowpT().size()) puInfo->PushPU_sumpT_lowpT(Double32_t(edmPUInfo->getPU_sumpT_lowpT()[i])); else puInfo->PushPU_sumpT_lowpT(-99);
      if (uint(i)<edmPUInfo->getPU_sumpT_highpT().size()) puInfo->PushPU_sumpT_highpT(Double32_t(edmPUInfo->getPU_sumpT_highpT()[i])); else puInfo->PushPU_sumpT_highpT(-99);
      if (uint(i)<edmPUInfo->getPU_ntrks_lowpT().size()) puInfo->PushPU_ntrks_lowpT(Double32_t(edmPUInfo->getPU_ntrks_lowpT()[i])); else puInfo->PushPU_ntrks_lowpT(-99);
      if (uint(i)<edmPUInfo->getPU_ntrks_highpT().size()) puInfo->PushPU_ntrks_highpT(Double32_t(edmPUInfo->getPU_ntrks_highpT()[i])); else puInfo->PushPU_ntrks_highpT(-99);
    }
  }

  puInfos_->Trim();
}
