#include "MitProd/TreeFiller/interface/FillerMCRunInfo.h"

#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/Run.h"

#include "MitAna/DataTree/interface/MCRunInfo.h"
#include "MitAna/DataTree/interface/MCRunInfoFull.h"
#include "MitAna/DataTree/interface/Names.h"

#include "MitProd/ObjectService/interface/ObjectService.h"

#include "TPRegexp.h"

#include <iostream>
#include <cstring>

mithep::FillerMCRunInfo::FillerMCRunInfo(edm::ParameterSet const& cfg, edm::ConsumesCollector& collector, mithep::ObjectService* os, char const* name/* = "MCRunInfo"*/, bool active/* = true*/) :
  BaseFiller(cfg, os, name, active),
  genRunInfoToken_(GetToken<GenRunInfoProduct, edm::InRun>(collector, "genRunInfoEdmName")),
  lheRunInfoToken_(GetToken<LHERunInfoProduct, edm::InRun>(collector, "lheRunInfoEdmName")),
  fillFullInfo_(Conf().getUntrackedParameter<bool>("fillFullInfo", false)),
  mcRunInfo_(fillFullInfo_ ? new mithep::MCRunInfoFull : new mithep::MCRunInfo)
{
}

mithep::FillerMCRunInfo::~FillerMCRunInfo()
{
  delete mcRunInfo_;
}

void
mithep::FillerMCRunInfo::BookDataBlock(mithep::TreeWriter& tws)
{
  // Create run info tree and book our branches.

  // add branches to run info tree
  if (fillFullInfo_) {
    tws.AddBranchToTree(mithep::Names::gkRunTreeName, mithep::Names::gkMCRunInfoFullBrn,
                        mithep::MCRunInfo::Class()->GetName(), &mcRunInfo_);

    OS()->add(static_cast<mithep::MCRunInfoFull*>(mcRunInfo_), mithep::Names::gkMCRunInfoFullBrn);
  }
  else {
    tws.AddBranchToTree(mithep::Names::gkRunTreeName, mithep::Names::gkMCRunInfoBrn,
                        mithep::MCRunInfo::Class()->GetName(), &mcRunInfo_);

    OS()->add(mcRunInfo_, mithep::Names::gkMCRunInfoBrn);
  }
}

void
mithep::FillerMCRunInfo::FillPostRunBlock(edm::Run const& run, edm::EventSetup const&)
{
  // GenRunInfoProduct and LHERunInfoProduct have mergeProduct method which forbids them
  // from being fetched in beginRun

  if (fillFullInfo_) {
    edm::Handle<GenRunInfoProduct> hGenRunInfo;
    run.getByToken(genRunInfoToken_, hGenRunInfo);
    if (!hGenRunInfo.product())
      throw edm::Exception(edm::errors::Configuration, name_ + "::FillRunBlock\n")
        << "Cannot get GenRunInfo for " << Name();

    auto& genRunInfo = *hGenRunInfo;
    auto* outInfo = static_cast<mithep::MCRunInfoFull*>(mcRunInfo_);

    outInfo->SetInternalXSec(genRunInfo.internalXSec().value());
    outInfo->SetInternalXSecErr(genRunInfo.internalXSec().error());
    outInfo->SetExternalXSecLO(genRunInfo.externalXSecLO().value());
    outInfo->SetExternalXSecLOErr(genRunInfo.externalXSecLO().error());
    outInfo->SetExternalXSecNLO(genRunInfo.externalXSecNLO().value());
    outInfo->SetExternalXSecNLOErr(genRunInfo.externalXSecNLO().error());
  }

  edm::Handle<LHERunInfoProduct> hLHERunInfo;
  run.getByToken(lheRunInfoToken_, hLHERunInfo);
  if (hLHERunInfo.product()) {
    auto& lheRunInfo = *hLHERunInfo;
    auto& heprup = lheRunInfo.heprup();

    if (fillFullInfo_) {
      auto* outInfo = static_cast<mithep::MCRunInfoFull*>(mcRunInfo_);
      outInfo->SetHasLHEInfo(true);

      outInfo->SetBeamEnergy(heprup.EBMUP.first); // Go rewrite this if you want to use this for Belle..
      outInfo->SetPdfGroup(heprup.PDFGUP.first); // Who uses different PDF for the two protons?
      outInfo->SetPdfId(heprup.PDFSUP.first);
      outInfo->SetWeightMode(heprup.IDWTUP);
    
      outInfo->SetNProcesses(heprup.NPRUP);
      for (int iP = 0; iP != heprup.NPRUP; ++iP) {
        outInfo->SetProcessXSec(iP, heprup.XSECUP.at(iP));
        outInfo->SetProcessXSecError(iP, heprup.XERRUP.at(iP));
        outInfo->SetProcessXSecMax(iP, heprup.XMAXUP.at(iP));
        outInfo->SetProcessId(iP, heprup.LPRUP.at(iP));
      }

      outInfo->SetNHeaderBlocks(0);
    }

    mcRunInfo_->ClearWeightData();

    for (auto&& hItr = lheRunInfo.headers_begin(); hItr != lheRunInfo.headers_end(); ++hItr) {
      auto& lheHdr = *hItr;

      if (lheHdr.tag() == "initrwgt") {
        setWeightGroups(lheHdr.lines());
        continue;
      }

      if (fillFullInfo_) {
        auto* outInfo = static_cast<mithep::MCRunInfoFull*>(mcRunInfo_);
        unsigned iB = outInfo->NHeaderBlocks();
        outInfo->SetNHeaderBlocks(iB + 1);
        outInfo->SetHeaderBlockTag(iB, lheHdr.tag().c_str());
        TString cont;
        for (auto&& line : lheHdr.lines())
          cont += line + "\n";

        outInfo->SetHeaderBlockContent(iB, cont.Data());
      }
    }

    if (fillFullInfo_) {
      auto* outInfo = static_cast<mithep::MCRunInfoFull*>(mcRunInfo_);
      outInfo->ClearComments();
      for (auto&& cItr = lheRunInfo.comments_begin(); cItr != lheRunInfo.comments_end(); ++cItr)
        outInfo->AddCommentLine(*cItr);
    }
  }
  else{
    if (fillFullInfo_) {
      auto* outInfo = static_cast<mithep::MCRunInfoFull*>(mcRunInfo_);
      outInfo->SetHasLHEInfo(false);
    
      outInfo->SetBeamEnergy(0.); // Go rewrite this if you want to use this for Belle..
      outInfo->SetPdfGroup(0); // Who uses different PDF for the two protons?
      outInfo->SetPdfId(0);
      outInfo->SetWeightMode(0);
    
      outInfo->SetNProcesses(0);
      outInfo->SetNHeaderBlocks(0);
      outInfo->ClearComments();
    }
    mcRunInfo_->SetNWeightGroups(0);
    mcRunInfo_->ClearWeightData();
  }
}  

void
mithep::FillerMCRunInfo::setWeightGroups(std::vector<std::string> const& blockLines)
{
  // Probably not the best idea to implement an original xml parser and also in C++..
  
  TPRegexp openTag("<([^ ]+) +((?:[^ =>]+=[^ >]+ *)*)>");
  TPRegexp closeTag("</([^ >]+)>");
  TPRegexp attribute("([^ =]+)=['\"]?([^ '\"]+)['\"]?");

  unsigned currentWG = mcRunInfo_->NWeightGroups();
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
          mcRunInfo_->SetNWeightGroups(currentWG + 1);

          mcRunInfo_->SetWeightGroupCombination(currentWG, attrs["combine"]);
          mcRunInfo_->SetWeightGroupType(currentWG, attrs["type"]);
        }
        else if (tag.back() == "weight") {
          if (currentWG == mcRunInfo_->NWeightGroups())
            throw edm::Exception(edm::errors::Configuration, name_ + "::FillRunBlock\n")
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
          throw edm::Exception(edm::errors::Configuration, name_ + "::FillRunBlock\n")
            << "Unmatched close tag found in LHE header";

        if (thisTag == "weightgroup") {
          ++currentWG;
        }
        else if (thisTag == "weight") {
          if (currentWG == mcRunInfo_->NWeightGroups() || wid.Length() == 0)
            throw edm::Exception(edm::errors::Configuration, name_ + "::FillRunBlock\n")
              << "Unmatched close tag found in LHE header";

          content += line(pos, line.Index(fullMatch.Data(), pos) - pos);
          content = content.Strip(TString::kBoth);

          mcRunInfo_->AddWeightData(currentWG, wid, content);
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
