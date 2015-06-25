#include "MitProd/TreeFiller/interface/FillerMCEventInfo.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataTree/interface/MCRunInfo.h"
#include "MitAna/DataTree/interface/MCEventInfo.h"
#include "MitProd/ObjectService/interface/ObjectService.h"

#include "FWCore/Framework/interface/Run.h"
#include "DataFormats/HepMCCandidate/interface/FlavorHistory.h"
#include "DataFormats/HepMCCandidate/interface/FlavorHistoryEvent.h"
#include "PhysicsTools/HepMCCandAlgos/interface/FlavorHistoryProducer.h"

#include "TPRegexp.h"

#include <set>

//--------------------------------------------------------------------------------------------------
mithep::FillerMCEventInfo::FillerMCEventInfo(edm::ParameterSet const& cfg, edm::ConsumesCollector& collector, mithep::ObjectService* os, char const* name/* = "MCEventInfo"*/,  bool active/* = true*/) : 
  BaseFiller(cfg, os, "MCEventInfo", active), 
  evtName_(cfg.getUntrackedParameter<std::string>("evtName", Names::gkMCEvtInfoBrn)), 
  runName_(cfg.getUntrackedParameter<std::string>("runName", Names::gkMCRunInfoBrn)), 
  genHepMCEvToken_(GetToken<edm::HepMCProduct>(collector, cfg, "genHepMCEventEdmName", "generator")), 
  genEvtInfoToken_(GetToken<GenEventInfoProduct>(collector, cfg, "genEvtInfoEdmName", "generator")), 
  lheEventToken_(GetToken<LHEEventProduct>(collector, cfg, "lheEventEdmName", "externalLHEProducer")),
  lheRunInfoToken_(GetToken<LHERunInfoProduct, edm::InRun>(collector, cfg, "lheRunInfoEdmName", "externalLHEProducer")),
  flavorHistToken_(GetToken<unsigned>(collector, cfg, "flavorHistEdmName", "flavorHistoryFilter")), 
  flavorHistoryActive_(cfg.getUntrackedParameter<bool>("flavorHistoryActive", false)), 
  eventInfo_(new mithep::MCEventInfo()), 
  runInfo_(new mithep::MCRunInfo()),
  weightIds_()
{
  // Constructor.
}

mithep::FillerMCEventInfo::~FillerMCEventInfo()
{
  // Destructor.

  delete eventInfo_;
  delete runInfo_;
}

void
mithep::FillerMCEventInfo::BookDataBlock(mithep::TreeWriter& tws)
{
  // Book our branch.

  tws.AddBranch(evtName_, &eventInfo_);
  OS()->add(eventInfo_, evtName_);

  // add branches to run info tree
  tws.AddBranchToTree(mithep::Names::gkRunTreeName, mithep::Names::gkMCRunInfoBrn, 
                      mithep::MCRunInfo::Class()->GetName(), &runInfo_);

  OS()->add(runInfo_, mithep::Names::gkMCRunInfoBrn);
}

void
mithep::FillerMCEventInfo::FillDataBlock(edm::Event const& event, edm::EventSetup const&)
{
  // Fill our data structures.

  if (event.isRealData()) {
    PrintErrorAndExit("Expected monte-carlo record, but did not get it. Aborting.");
  }
  
  edm::Handle<GenEventInfoProduct> hEvtInfo;
  bool pdfFilled = false;

  if (GetProductSafe(genEvtInfoToken_, hEvtInfo, event)) {
    // use event info product

    eventInfo_->SetScale(hEvtInfo->qScale());
    eventInfo_->SetProcessId(hEvtInfo->signalProcessID());
    eventInfo_->SetWeight(hEvtInfo->weight());

    gen::PdfInfo const* pdf = hEvtInfo->pdf();
    if (pdf) {
      eventInfo_->SetId1(pdf->id.first);
      eventInfo_->SetId2(pdf->id.second);
      eventInfo_->SetPdf1(pdf->xPDF.first);
      eventInfo_->SetPdf2(pdf->xPDF.second);
      eventInfo_->SetScalePdf(pdf->scalePDF);
      eventInfo_->SetX1(pdf->x.first);
      eventInfo_->SetX2(pdf->x.second);

      pdfFilled = true;
    }

  }
  else if (!genHepMCEvToken_.isUninitialized()) {
    // fall back to hepmc if requested

    edm::Handle<edm::HepMCProduct> hHepMCProduct;
    GetProduct(genHepMCEvToken_, hHepMCProduct, event);
    const HepMC::GenEvent *genEvt = hHepMCProduct->GetEvent();
    eventInfo_->SetScale(genEvt->event_scale());
    eventInfo_->SetProcessId(genEvt->signal_process_id());
    HepMC::WeightContainer wc = genEvt->weights();
    Double_t weight = 0;
    for (unsigned int i = 0; i< wc.size(); ++i) 
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

      pdfFilled = true;
    }
  }

  edm::Handle<LHEEventProduct> hLHEEvent;
  if (GetProductSafe(lheEventToken_, hLHEEvent, event)) {

    auto& lheEvent = *hLHEEvent;

    eventInfo_->SetHasLHEInfo(true);

    if (!pdfFilled) {
      gen::PdfInfo const* pdf = lheEvent.pdf();
      if (pdf) {
        eventInfo_->SetId1(pdf->id.first);
        eventInfo_->SetId2(pdf->id.second);
        eventInfo_->SetPdf1(pdf->xPDF.first);
        eventInfo_->SetPdf2(pdf->xPDF.second);
        eventInfo_->SetScalePdf(pdf->scalePDF);
        eventInfo_->SetX1(pdf->x.first);
        eventInfo_->SetX2(pdf->x.second);

        pdfFilled = true;
      }
    }

    eventInfo_->ClearScales();
    for (auto&& s : lheEvent.scales())
      eventInfo_->AddScale(s);

    eventInfo_->SetNPartonsLO(lheEvent.npLO());
    eventInfo_->SetNPartonsNLO(lheEvent.npNLO());

    if (eventInfo_->NReweightScaleFactors() == 0) {
      // first time looping over weights; save id -> index

      eventInfo_->SetNReweightScaleFactors(lheEvent.weights().size());
      for (auto& wgt : lheEvent.weights()) {
        unsigned idx = weightIds_.size();
        weightIds_[wgt.id] = idx;

        eventInfo_->SetReweightScaleFactors(idx, wgt.wgt / lheEvent.originalXWGTUP());
      }
    }
    else {
      for (auto& wgt : lheEvent.weights()) {
        auto&& itr = weightIds_.find(wgt.id);
        if (itr == weightIds_.end()) // not a weight ID found in the first event (should not happen)
          continue;

        eventInfo_->SetReweightScaleFactors(itr->second, wgt.wgt / lheEvent.originalXWGTUP());
      }
    }

    auto& hepeup = lheEvent.hepeup();
    eventInfo_->SetNPartons(hepeup.NUP);
    eventInfo_->SetAlphaQED(hepeup.AQEDUP);
    eventInfo_->SetAlphaQCD(hepeup.AQCDUP);
    for (int iP = 0; iP != hepeup.NUP; ++iP) {
      eventInfo_->SetPartonId(iP, hepeup.IDUP[iP]);
      eventInfo_->SetPartonStatus(iP, hepeup.ISTUP[iP]);
      eventInfo_->SetPartonMother1(iP, hepeup.MOTHUP[iP].first);
      eventInfo_->SetPartonMother2(iP, hepeup.MOTHUP[iP].second);
      eventInfo_->SetPartonColor1(iP, hepeup.ICOLUP[iP].first);
      eventInfo_->SetPartonColor2(iP, hepeup.ICOLUP[iP].second);
      auto& pup = hepeup.PUP[iP];
      eventInfo_->SetPartonMom(iP, pup[0], pup[1], pup[2], pup[3]);
      eventInfo_->SetPartonCTau(iP, hepeup.VTIMUP[iP]);
      eventInfo_->SetPartonSpin(iP, hepeup.SPINUP[iP]);
    }

    eventInfo_->ClearLHEComments();
    for (auto cItr = lheEvent.comments_begin(); cItr != lheEvent.comments_end(); ++cItr)
      eventInfo_->AddLHEComment(cItr->c_str());
  }
  else {
    eventInfo_->SetHasLHEInfo(false);

    eventInfo_->ClearScales();
    eventInfo_->SetNPartonsLO(0);
    eventInfo_->SetNPartonsNLO(0);
    eventInfo_->SetNReweightScaleFactors(0);
    eventInfo_->SetNPartons(0);
    eventInfo_->SetAlphaQED(0.);
    eventInfo_->SetAlphaQCD(0.);
    eventInfo_->ClearLHEComments();
  }

  if (!pdfFilled) {
    eventInfo_->SetId1(0);
    eventInfo_->SetId2(0);
    eventInfo_->SetPdf1(0.);
    eventInfo_->SetPdf2(0.);
    eventInfo_->SetScalePdf(0.);
    eventInfo_->SetX1(0.);
    eventInfo_->SetX2(0.);
  }

  // fill flavor history path if requested
  if (flavorHistoryActive_) {
    edm::Handle<unsigned int> flavorHistoryPath;
    GetProduct(flavorHistToken_, flavorHistoryPath, event);
    eventInfo_->SetFlavorHistoryPath(*flavorHistoryPath);
  }
}

void
mithep::FillerMCEventInfo::FillPostRunBlock(edm::Run const& run, edm::EventSetup const&)
{
  // LHERunInfoProduct (and GenRunInfoProduct FWIW) have mergeProduct method which forbids them
  // from being fetched in beginRun

  edm::Handle<LHERunInfoProduct> hLHERunInfo;
  run.getByToken(lheRunInfoToken_, hLHERunInfo);

  if (hLHERunInfo.isValid()) {
    auto& lheRunInfo = *hLHERunInfo;
    auto& heprup = lheRunInfo.heprup();

    runInfo_->SetHasLHEInfo(true);

    runInfo_->SetBeamEnergy(heprup.EBMUP.first); // Go rewrite this if you want to use this for Belle..
    runInfo_->SetPdfGroup(heprup.PDFGUP.first); // Who uses different PDF for the two protons?
    runInfo_->SetPdfId(heprup.PDFSUP.first);
    runInfo_->SetWeightMode(heprup.IDWTUP);
    
    runInfo_->SetNProcesses(heprup.NPRUP);
    for (int iP = 0; iP != heprup.NPRUP; ++iP) {
      runInfo_->SetProcessXSec(iP, heprup.XSECUP.at(iP));
      runInfo_->SetProcessXSecError(iP, heprup.XERRUP.at(iP));
      runInfo_->SetProcessXSecMax(iP, heprup.XMAXUP.at(iP));
      runInfo_->SetProcessId(iP, heprup.LPRUP.at(iP));
    }

    runInfo_->SetNHeaderBlocks(0);

    runInfo_->SetNWeightDefinitions(0);

    // eliminate duplicate tags - at the moment of writing, the only case where the same tag is
    // repeated is for MGRunCard, where cards for all the MC generation runs that went into the
    // AOD file is saved. These cards differ only by the random number seeds.
    std::set<std::string> tags;

    for (auto&& hItr = lheRunInfo.headers_begin(); hItr != lheRunInfo.headers_end(); ++hItr) {
      auto& lheHdr = *hItr;

      if (lheHdr.tag() == "initrwgt") {
        setWeightGroups(lheHdr.lines());
        continue;
      }

      if (tags.find(lheHdr.tag()) != tags.end())
        continue;

      tags.insert(lheHdr.tag());

      unsigned iB = runInfo_->NHeaderBlocks();
      runInfo_->SetNHeaderBlocks(iB + 1);
      runInfo_->SetHeaderBlockTag(iB, lheHdr.tag().c_str());
      TString cont;
      for (auto&& line : lheHdr.lines())
        cont += line + "\n";

      runInfo_->SetHeaderBlockContent(iB, cont.Data());
    }

    runInfo_->ClearLHEComments();
    for (auto&& cItr = lheRunInfo.comments_begin(); cItr != lheRunInfo.comments_end(); ++cItr)
      runInfo_->AddLHECommentLine(cItr->c_str());
  }
  else{
    runInfo_->SetHasLHEInfo(false);
    
    runInfo_->SetBeamEnergy(0.); // Go rewrite this if you want to use this for Belle..
    runInfo_->SetPdfGroup(0); // Who uses different PDF for the two protons?
    runInfo_->SetPdfId(0);
    runInfo_->SetWeightMode(0);
    
    runInfo_->SetNProcesses(0);
    runInfo_->SetNHeaderBlocks(0);
    runInfo_->SetNWeightDefinitions(0);
    runInfo_->ClearLHEComments();
  }
}  

void
mithep::FillerMCEventInfo::setWeightGroups(std::vector<std::string> const& blockLines)
{
  // Probably not the best idea to implement an original xml parser and also in C++..
  
  TPRegexp openTag("<([^ ]+) +((?:[^ =>]+=[^ >]+ *)*)>");
  TPRegexp closeTag("</([^ >]+)>");
  TPRegexp attribute("([^ =]+)=['\"]?([^ '\"]+)['\"]?");

  unsigned currentWG = runInfo_->NWeightGroups();
  TString wid;

  std::vector<TString> tag;
  TString content;

  for (std::string const& l : blockLines) {
    TString line(l);
    line.Strip(TString::kTrailing, '\n');
    line.Strip(TString::kBoth);

    unsigned pos = 0;
    while (true) {
      if (openTag.MatchB(line, "", pos)) {
        TObjArray* matches = openTag.MatchS(line, "", pos);
        TString fullMatch(matches->At(0)->GetName());
        tag.push_back(matches->At(1)->GetName());
        TString attributes(matches->At(2)->GetName());
        delete matches;

        std::map<TString, TString> attrs;

        int attrpos = 0;
        while (attrpos < attributes.Length()) {
          TObjArray* keyvalue = attribute.MatchS(attributes, "", attrpos);
          TString fullattr(keyvalue->At(0)->GetName());
          attrs[keyvalue->At(1)->GetName()] = keyvalue->At(2)->GetName();

          delete keyvalue;

          attrpos += attributes.Index(fullattr, attrpos) + fullattr.Length();
        }

        if (tag.back() == "weightgroup") {
          runInfo_->SetNWeightGroups(currentWG + 1);

          runInfo_->SetWeightGroupCombination(currentWG, attrs["combine"]);
          runInfo_->SetWeightGroupType(currentWG, attrs["type"]);
        }
        else if (tag.back() == "weight") {
          if (currentWG == runInfo_->NWeightGroups())
            throw edm::Exception(edm::errors::Configuration, name_ + "::FillPostRunBlock\n")
              << "<weight> tag found outside of <weightgroup> in LHE header";
          
          wid = attrs["id"];
        }
        
        content = "";
        pos = line.Index(fullMatch, pos) + fullMatch.Length();

        continue;
      }
      else if (closeTag.MatchB(line, "", pos)) {
        TObjArray* matches = closeTag.MatchS(line, "", pos);
        TString fullMatch(matches->At(0)->GetName());
        TString thisTag(matches->At(1)->GetName());
        delete matches;

        if (thisTag != tag.back())
          throw edm::Exception(edm::errors::Configuration, name_ + "::FillPostRunBlock\n")
            << "Unmatched close tag found in LHE header";

        if (thisTag == "weightgroup") {
          ++currentWG;
        }
        else if (thisTag == "weight") {
          if (currentWG == runInfo_->NWeightGroups() || wid.Length() == 0)
            throw edm::Exception(edm::errors::Configuration, name_ + "::FillPostRunBlock\n")
              << "Unmatched close tag found in LHE header";

          auto idItr = weightIds_.find(wid);
          if (idItr != weightIds_.end()) {
            content += line(pos, line.Index(fullMatch.Data(), pos) - pos);
            content = content.Strip(TString::kBoth);

            runInfo_->AddWeightDefinition(wid, content, currentWG, idItr->second);
          }

          wid = "";
        }

        tag.pop_back();

        content = "";
        pos = line.Index(fullMatch.Data(), pos) + fullMatch.Length();

        continue;
      }

      if (tag.size() != 0)
        content += line(pos, line.Length());

      break;
    }
  }
}

DEFINE_MITHEP_TREEFILLER(FillerMCEventInfo);
