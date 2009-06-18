// $Id: FillerMCEventInfo.cc,v 1.6 2009/06/15 15:00:26 loizides Exp $

#include "MitProd/TreeFiller/interface/FillerMCEventInfo.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/HepMCCandidate/interface/PdfInfo.h"
#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataTree/interface/MCEventInfo.h"
#include "MitProd/ObjectService/interface/ObjectService.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerMCEventInfo::FillerMCEventInfo(const ParameterSet &cfg, const char *name,  bool active) : 
  BaseFiller(cfg,"MCEventInfo",active),
  evtName_(Conf().getUntrackedParameter<string>("evtName",Names::gkMCEvtInfoBrn)),
  genHepMCEvName_(Conf().getUntrackedParameter<string>("genHepMCEventEdmName","source")),
  genEvWeightName_(Conf().getUntrackedParameter<string>("genEventWeightEdmName","genEventWeight")),
  genEvScaleName_(Conf().getUntrackedParameter<string>("genEventScaleEdmName","genEventScale")),
  genEvProcIdName_(Conf().getUntrackedParameter<string>("genEventProcIdEdmName","genEventProcID")),
  genPdfInfoName_(Conf().getUntrackedParameter<string>("genPdfInfoEdmName","genEventPdfInfo")),
  eventInfo_(new MCEventInfo())
{
  // Constructor.
}

//--------------------------------------------------------------------------------------------------
FillerMCEventInfo::~FillerMCEventInfo()
{
  // Destructor.

  delete eventInfo_;
}

//--------------------------------------------------------------------------------------------------
void FillerMCEventInfo::BookDataBlock(TreeWriter &tws)
{
  // Create run info tre and book our branches.

  tws.AddBranch(evtName_,&eventInfo_);
  OS()->add<mithep::MCEventInfo>(eventInfo_,evtName_);
}

//--------------------------------------------------------------------------------------------------
void FillerMCEventInfo::FillDataBlock(const edm::Event &event, 
                                      const edm::EventSetup &setup)
{
  // Fill our data structures.

  if (event.isRealData()) {
    PrintErrorAndExit("Expected monte-carlo record, but did not get it. Aborting.");
  }

  Handle<edm::HepMCProduct> hHepMCProduct;
  if (genHepMCEvName_.empty() || !GetProductSafe(genHepMCEvName_, hHepMCProduct, event)) {

    if (!genEvWeightName_.empty()) {
      Handle<double> genEventWeight;
      GetProduct(genEvWeightName_, genEventWeight, event);
      eventInfo_->SetWeight(*genEventWeight);
    }

    if (!genEvScaleName_.empty()) {
      Handle<double> genEventScale;
      GetProduct(genEvScaleName_, genEventScale, event);
      eventInfo_->SetScale(*genEventScale);
    }

    if (!genEvProcIdName_.empty()) {
      Handle<int> genEventProcId;
      GetProduct(genEvProcIdName_, genEventProcId, event);
      eventInfo_->SetProcessId(*genEventProcId);
    }

    if (!genPdfInfoName_.empty()) {
      Handle<reco::PdfInfo> genPdfInfo;
      GetProduct(genPdfInfoName_, genPdfInfo, event);
      eventInfo_->SetId1(genPdfInfo->id1);
      eventInfo_->SetId2(genPdfInfo->id2);
      eventInfo_->SetPdf1(genPdfInfo->pdf1);
      eventInfo_->SetPdf2(genPdfInfo->pdf2);
      eventInfo_->SetScalePdf(genPdfInfo->scalePDF);
      eventInfo_->SetX1(genPdfInfo->pdf1);
      eventInfo_->SetX2(genPdfInfo->pdf2);
    }
  } else {
    const HepMC::GenEvent *genEvt = hHepMCProduct->GetEvent();
    eventInfo_->SetScale(genEvt->event_scale());
    eventInfo_->SetProcessId(genEvt->signal_process_id());
    HepMC::WeightContainer wc = genEvt->weights();
    if (wc.size() > 0)  
      eventInfo_->SetWeight(wc[0]);
    const HepMC::PdfInfo *genPdfInfo = genEvt->pdf_info();
    eventInfo_->SetId1(genPdfInfo->id1());
    eventInfo_->SetId2(genPdfInfo->id2());
    eventInfo_->SetPdf1(genPdfInfo->pdf1());
    eventInfo_->SetPdf2(genPdfInfo->pdf2());
    eventInfo_->SetScalePdf(genPdfInfo->scalePDF());
    eventInfo_->SetX1(genPdfInfo->pdf1());
    eventInfo_->SetX2(genPdfInfo->pdf2());
  }
}
