// $Id: FillerEvtSelData.cc,v 1.3 2009/12/09 12:50:38 edwenger Exp $

#include "MitProd/TreeFiller/interface/FillerEvtSelData.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataTree/interface/EvtSelData.h"
#include "MitEdm/DataFormats/interface/EvtSelData.h"
#include "MitProd/ObjectService/interface/ObjectService.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerEvtSelData::FillerEvtSelData(const ParameterSet &cfg, const char *name,  bool active) : 
  BaseFiller(cfg,"EvtSelData",active),
  mitName_(Conf().getUntrackedParameter<string>("mitName",Names::gkEvtSelDataBrn)),
  edmName_(Conf().getUntrackedParameter<string>("edmName","evtSelData")),
  evtSelData_(new EvtSelData())
{
  // Constructor.
}

//--------------------------------------------------------------------------------------------------
FillerEvtSelData::~FillerEvtSelData()
{
  // Destructor.

  delete evtSelData_;
}

//--------------------------------------------------------------------------------------------------
void FillerEvtSelData::BookDataBlock(TreeWriter &tws, const edm::EventSetup &es)
{
  // Book our branch.

  tws.AddBranch(mitName_,&evtSelData_);
  OS()->add<mithep::EvtSelData>(evtSelData_,mitName_);
}

//--------------------------------------------------------------------------------------------------
void FillerEvtSelData::FillDataBlock(const edm::Event &event, 
                                     const edm::EventSetup &setup)
{
  // Fill our data structures.

  Handle<mitedm::EvtSelData> hEvtSelData;
  bool gotIt = GetProductSafe(edmName_, hEvtSelData, event);
  if (gotIt) {
    evtSelData_->Set(hEvtSelData->eHcalNeg(), hEvtSelData->eHcalPos(),
                     hEvtSelData->eHfNeg(), hEvtSelData->eHfPos(),
                     hEvtSelData->eHfNegTime(), hEvtSelData->eHfPosTime(),
                     hEvtSelData->eCastorNeg(), hEvtSelData->eCastorPos(),
                     hEvtSelData->eCastorNegTime(), hEvtSelData->eCastorPosTime(),
                     hEvtSelData->eZdcNeg(), hEvtSelData->eZdcPos(),
                     hEvtSelData->eZdcNegTime(), hEvtSelData->eZdcPosTime(),
                     hEvtSelData->ePxbHits(), hEvtSelData->ePxHits(),
		     hEvtSelData->eClusVtxQual(), hEvtSelData->eClusVtxDiff(),
		     hEvtSelData->eHPTrkFrac());
  } else {
    evtSelData_->Set(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
  }
}
