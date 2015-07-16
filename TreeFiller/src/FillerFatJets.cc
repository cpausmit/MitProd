#include "MitProd/TreeFiller/interface/FillerFatJets.h"

#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/PatCandidates/interface/Jet.h"

mithep::FillerFatJets::FillerFatJets(edm::ParameterSet const& cfg, edm::ConsumesCollector& collector, mithep::ObjectService* os, char const* name, bool active/* = true*/) :
  FillerPFJets(cfg, collector, os, name, active),
  fR0(cfg.getUntrackedParameter<bool>("R0", false)),
{

  fillFromPAT_ = true;
  jets_ = new mithep::FatJetArr(32);
  fSubjetCollectionsTags = iConfig.getUntrackedParameter<std::vector<edm::InputTag> >("SubJets");
  fSubjetNames         = iConfig.getUntrackedParameter<std::vector<std::string> >("SubJetLabels");
  unsigned int nSubjetTypes = fSubjetNames.size();
  fSubjetCollections.resize( nSubjetTypes );
  for ( size_t i = 0; i < nSubjetTypes; ++i )
    iEvent.getByLabel(fSubjetCollectionsTags[i], fSubjetCollections[i]);


}

mithep::FillerFatJets::~FillerFatJets()
{
}

void

void
mithep::FillerFatJets::FillSpecific(mithep::Jet& outBaseJet, reco::JetBaseRef const& inJetRef)
{
  assert(fillFromPAT_);
  auto& outJet = static_cast<mithep::FatJet&>(outBaseJet);
  auto* inJet = dynamic_cast<pat::Jet const*>(inJetRef.get());
  if (!inJet)
    throw edm::Exception(edm::errors::Configuration, "FillerFatJets::FillSpecific)")
      << "fillFromPAT set on non-PAT input";

  // pat::Jet::p4() returns a corrected p4. Uncorrect here
  double toRaw = inJet->jecFactor("Uncorrected");
  auto&& rawP4 = inJet->p4() * toRaw;
  outJet.SetRawPtEtaPhiM(rawP4.pt(), rawP4.eta(), rawP4.phi(), rawP4.mass());

  fillPATFatJetVariables(outJet, *inJet);
}


void
mithep::FillerFatJets::fillPATFatJetVariables(mithep::FatJet& outJet, pat::Jet const& inPFJet)
{
  fillPatJetVariables(outJet,inPFJet);
  outJet.SetCharge();
  // get the easy user floats out of the way
  TString suffix = TString::Format("%i",int(fR0*10));
  TString njettiness("Njettiness"); TString pruned("Pruned"); TString trimmed("Trimmed");
  outJet.SetTau1(inPFJet.userFloat(njettiness+suffix+":tau1"));
  outJet.SetTau2(inPFJet.userFloat(njettiness+suffix+":tau2"));
  outJet.SetTau3(inPFJet.userFloat(njettiness+suffix+":tau3"));
  outJet.SetTau4(inPFJet.userFloat(njettiness+suffix+":tau4"));
  outJet.SetQJetVol(inPFJet.userFloat(TString("Qjets")+suffix+":QjetsVolatility"));
  outJet.SetPrunedP(Vect4M(inPFJet.userFloat(pruned+suffix+":Pt"),
                            inPFJet.userFloat(pruned+suffix+":Eta"),
                            inPFJet.userFloat(pruned+suffix+":Phi"),
                            inPFJet.userFloat(pruned+suffix+":Mass")));
  outJet.SetTrimmedP(Vect4M(inPFJet.userFloat(trimmed+suffix+":Pt"),
                            inPFJet.userFloat(trimmed+suffix+":Eta"),
                            inPFJet.userFloat(trimmed+suffix+":Phi"),
                            inPFJet.userFloat(trimmed+suffix+":Mass")));

  // now let's save subjets
  unsigned int eSubjetType = 0; // to map to XlSubJet::SubJetType enum
  for (auto & subjetName : fSubjetNames) { // loop over subjet types
    const PatJetPtrCollection & subjets = inPFJet.subjets(subjetName);
    for (auto & inSubjet : subjets) {
      XlSubJet * outSubjet = new XlSubJet();
      outSubjet->SetRawPtEtaPhiM(inSubjet.p4().pt(), inSubjet.p4().eta(), inSubjet.p4().phi(), inSubjet.p4().mass());
      outSubjet->SetSigmaEta(TMath::Sqrt(inSubjet.etaetaMoment()));
      outSubjet->SetSigmaPhi(TMath::Sqrt(inSubjet.phiphiMoment()));
      outSubjet->SetJetArea(inSubjet.jetArea());
      FillerPFJets::FillSpecific(*outSubjet,inSubjet); // this is only a PF jet, not a fatjet
      setBJetTags(*outSubjet, baseRef, bJetTags);
      setCorrections(*outSubjet, inSubjet);
      outSubjet->SetSubJetType(eSubjetType);
      outJet.AddSubjet(outSubjet,eSubjetType);
    }
    ++eSubjetType;
  }
  
}


DEFINE_MITHEP_TREEFILLER(FillerFatJets);
