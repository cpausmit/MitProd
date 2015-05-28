#include "MitProd/TreeFiller/interface/FillerGenMet.h"
#include "DataFormats/METReco/interface/GenMET.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "MitAna/DataTree/interface/GenMetCol.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitProd/ObjectService/interface/ObjectService.h"

mithep::FillerGenMet::FillerGenMet(edm::ParameterSet const& cfg, edm::ConsumesCollector& collector, mithep::ObjectService* os, char const* name, bool active/* = true*/) : 
  BaseFiller(cfg,os,name,active),
  edmToken_(GetToken<METView>(collector, "edmName", "genMetTrue")),
  mitName_(Conf().getUntrackedParameter<std::string>("mitName", mithep::Names::gkGenMetBrn)),
  fillFromPAT_(Conf().getUntrackedParameter<bool>("fillFromPAT", false)),
  genMets_(new mithep::GenMetArr)
{
}

mithep::FillerGenMet::~FillerGenMet()
{
  delete genMets_;
}

void
mithep::FillerGenMet::BookDataBlock(mithep::TreeWriter& tws)
{
  tws.AddBranch(mitName_, &genMets_);
  OS()->add(genMets_, mitName_);
}

void
mithep::FillerGenMet::FillDataBlock(edm::Event const& event, 
                                    edm::EventSetup const&)
{
  // Fill missing energy from edm collection into our collection.

  genMets_->Delete();

  edm::Handle<METView> hMetView;
  GetProduct(edmToken_, hMetView, event);
  auto& metView = *hMetView;

  if (metView.size() != 0) {
    if(fillFromPAT_) { 
      pat::MET const* patMet = dynamic_cast<pat::MET const*>(&metView.at(0));
      if (!patMet)
        throw edm::Exception(edm::errors::Configuration, "FillerGenMet::FillDataBlock")
          << "fillFromPAT set for non-PAT MET input";
    }
    else {
      reco::GenMET const* genMet = dynamic_cast<reco::GenMET const*>(&metView.at(0));
      if (!genMet)
        throw edm::Exception(edm::errors::Configuration, "FillerGenMet::FillDataBlock")
          << "Input is not a GenMETCollection";
    }
  }

  for (auto& inMet : metView) {
    reco::GenMET const* inGenMet = 0;

    if (fillFromPAT_)
      inGenMet = static_cast<pat::MET const&>(inMet).genMET();
    else
      inGenMet = static_cast<reco::GenMET const*>(&inMet);

    if (!inGenMet)
      continue;

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
