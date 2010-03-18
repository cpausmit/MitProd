// $Id: FillerMCEventInfo.cc,v 1.15 2010/01/04 20:16:39 loizides Exp $

#include "MitProd/TreeFiller/interface/FillerMCEventInfo.h"
#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataTree/interface/MCEventInfo.h"
#include "MitProd/ObjectService/interface/ObjectService.h"
#include "DataFormats/HepMCCandidate/interface/FlavorHistory.h"
#include "DataFormats/HepMCCandidate/interface/FlavorHistoryEvent.h"
#include "PhysicsTools/HepMCCandAlgos/interface/FlavorHistoryProducer.h"

using namespace std;
using namespace edm;
using namespace mithep;

//--------------------------------------------------------------------------------------------------
FillerMCEventInfo::FillerMCEventInfo(const ParameterSet &cfg, const char *name,  bool active) : 
  BaseFiller(cfg,"MCEventInfo",active),
  evtName_(Conf().getUntrackedParameter<string>("evtName",Names::gkMCEvtInfoBrn)),
  genHepMCEvName_(Conf().getUntrackedParameter<string>("genHepMCEventEdmName","generator")),
  genEvtInfoName_(Conf().getUntrackedParameter<string>("genEvtInfoEdmName","generator")),
  flavorHistoryActive_(Conf().getUntrackedParameter<bool>("flavorHistoryActive",false)),
  flavorHistName_(Conf().getUntrackedParameter<string>("flavorHistEdmName","flavorHistoryFilter")),
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
  // Book our branch.

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
  
  Handle<GenEventInfoProduct> hEvtInfo;

  if (genEvtInfoName_.empty() || !GetProductSafe(genEvtInfoName_, hEvtInfo, event)) {

    // fall back to hepmc if requested
    if (!genHepMCEvName_.empty()) {
      Handle<edm::HepMCProduct> hHepMCProduct;
      GetProduct(genHepMCEvName_, hHepMCProduct, event);
      const HepMC::GenEvent *genEvt = hHepMCProduct->GetEvent();
      eventInfo_->SetScale(genEvt->event_scale());
      eventInfo_->SetProcessId(genEvt->signal_process_id());
      HepMC::WeightContainer wc = genEvt->weights();
      Double_t weight = 0;
      for (int i = 0; i< wc.size(); ++i) 
        weight *= wc[i];
      eventInfo_->SetWeight(weight);
      const HepMC::PdfInfo *genPdfInfo = genEvt->pdf_info();
      if (genPdfInfo) {
        eventInfo_->SetId1(genPdfInfo->id1());
        eventInfo_->SetId2(genPdfInfo->id2());
        eventInfo_->SetPdf1(genPdfInfo->pdf1());
        eventInfo_->SetPdf2(genPdfInfo->pdf2());
        eventInfo_->SetScalePdf(genPdfInfo->scalePDF());
        eventInfo_->SetX1(genPdfInfo->x1());
        eventInfo_->SetX2(genPdfInfo->x2());
      }
    }

  } else {

    // use event info product
    eventInfo_->SetScale(hEvtInfo->qScale());
    eventInfo_->SetProcessId(hEvtInfo->signalProcessID());
    eventInfo_->SetWeight(hEvtInfo->weight());
    if (hEvtInfo->hasPDF()) {
      const gen::PdfInfo *pdf = hEvtInfo->pdf();
      eventInfo_->SetId1(pdf->id.first);
      eventInfo_->SetId2(pdf->id.second);
      eventInfo_->SetPdf1(pdf->xPDF.first);
      eventInfo_->SetPdf2(pdf->xPDF.second);
      eventInfo_->SetScalePdf(pdf->scalePDF);
      eventInfo_->SetX1(pdf->x.first);
      eventInfo_->SetX2(pdf->x.second);
    }
  }

  // fill flavor history path if requested
  if (flavorHistoryActive_) {
    Handle<unsigned int> flavorHistoryPath;
    GetProduct(flavorHistName_, flavorHistoryPath, event);
    eventInfo_->SetFlavorHistoryPath(*flavorHistoryPath);
  }
}
