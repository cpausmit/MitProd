#include "MitProd/TreeFiller/interface/FillerFatJets.h"

#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/PatCandidates/interface/Jet.h"

typedef mithep::FatJet::TrackData TrackData;
typedef mithep::FatJet::SVData SVData;

mithep::FillerFatJets::FillerFatJets(edm::ParameterSet const& cfg, edm::ConsumesCollector& collector, mithep::ObjectService* os, char const* name, bool active/* = true*/) :
  FillerPFJets(cfg, collector, os, name, active),
  fR0(cfg.getUntrackedParameter<double>("R0", .8))
{

  fillFromPAT_ = true;
  jets_ = new mithep::FatJetArr(32);
  fSubjetCollectionTags = cfg.getUntrackedParameter<std::vector<edm::InputTag> >("SubJets");
  fSubjetNames         = cfg.getUntrackedParameter<std::vector<std::string> >("SubJetLabels");
  fPVToken = GetToken<reco::VertexCollection>(collector,cfg,"edmPrimaryVertices");
}

mithep::FillerFatJets::~FillerFatJets()
{
  delete[] fBJetTags;
}

void mithep::FillerFatJets::PrepareSpecific(edm::Event const& iEvent, edm::EventSetup const& iSetup) {
  unsigned int nSubjetTypes = fSubjetNames.size();
  fSubjetCollections.resize( nSubjetTypes );
  for ( size_t i = 0; i < nSubjetTypes; ++i )
    iEvent.getByLabel(fSubjetCollectionTags[i], fSubjetCollections[i]);

}

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
mithep::FillerFatJets::FillSpecificSubjet(mithep::XlSubJet& outBaseJet, edm::Ptr<pat::Jet> inJet) {
    double toRaw = inJet->jecFactor("Uncorrected");
    auto&& rawP4 = inJet->p4() * toRaw;
    outBaseJet.SetRawPtEtaPhiM(rawP4.pt(), rawP4.eta(), rawP4.phi(), rawP4.mass());

}

void
mithep::FillerFatJets::fillPATFatJetVariables(mithep::FatJet& outJet, pat::Jet const& inJet)
{
  fillPATJetVariables(outJet,inJet);
  outJet.SetCharge();
  // get the easy user floats out of the way
  TString suffix = TString::Format("%i",int(fR0*10));
  TString njettiness("Njettiness"); TString pruned("Pruned"); TString trimmed("Trimmed");
  outJet.SetTau1(inJet.userFloat(njettiness+suffix+":tau1"));
  outJet.SetTau2(inJet.userFloat(njettiness+suffix+":tau2"));
  outJet.SetTau3(inJet.userFloat(njettiness+suffix+":tau3"));
  outJet.SetTau4(inJet.userFloat(njettiness+suffix+":tau4"));
  outJet.SetQJetVol(inJet.userFloat(TString("Qjets")+suffix+":QjetsVolatility"));
  outJet.SetPrunedP(Vect4M(inJet.userFloat(pruned+suffix+":Pt"),
                            inJet.userFloat(pruned+suffix+":Eta"),
                            inJet.userFloat(pruned+suffix+":Phi"),
                            inJet.userFloat(pruned+suffix+":Mass")));
  outJet.SetTrimmedP(Vect4M(inJet.userFloat(trimmed+suffix+":Pt"),
                            inJet.userFloat(trimmed+suffix+":Eta"),
                            inJet.userFloat(trimmed+suffix+":Phi"),
                            inJet.userFloat(trimmed+suffix+":Mass")));

  // now let's save subjets
  int eSubjetType = 0; // to map to XlSubJet::SubJetType enum
  for (auto & subjetName : fSubjetNames) { // loop over subjet types
    const PatJetPtrCollection & subjets = inJet.subjets(subjetName);
    for (auto & inSubjetPtr : subjets) {
      pat::Jet inSubjet = *inSubjetPtr;
      XlSubJet * outSubjet = new XlSubJet();
      outSubjet->SetRawPtEtaPhiM(inSubjet.p4().pt(), inSubjet.p4().eta(), inSubjet.p4().phi(), inSubjet.p4().mass());
      outSubjet->SetSigmaEta(TMath::Sqrt(inSubjet.etaetaMoment()));
      outSubjet->SetSigmaPhi(TMath::Sqrt(inSubjet.phiphiMoment()));
      outSubjet->SetJetArea(inSubjet.jetArea());
      FillSpecificSubjet(*outSubjet,inSubjetPtr); // this is only a subjet, not a PF jet
      setCorrections(*outSubjet, inSubjet);
      outSubjet->SetSubJetType((XlSubJet::ESubJetType)eSubjetType);
      outJet.AddSubJet(outSubjet);
    }
    ++eSubjetType;
  }

  // now let's tag some bs
  const IPTagInfo * ipTagInfo = inJet->tagInfoCandIP("pfImpactParameter");
  const SVTagInfo * svTagInfo = inJet->tagInfoCandSecondaryVertex("pfInclusiveSecondaryVertexFinder");

  std::vector<fastjet::PseudoJet> currentAxes;
  recalcNsubjettiness(*inJet,*svTagInfo,outJet,currentAxes);

  const Tracks & selectedTracks( ipTagInfo->selectedTracks() );
  reco::TrackKinematics allKinematics;
  unsigned int trackSize = selectedTracks.size();
  for (unsigned int itt=0; itt < trackSize; ++itt) {
    const reco::Track & ptrack = *(reco::btag::toTrack(selectedTracks[itt]));
    const TrackRef ptrackRef = selectedTracks[itt];
    int trackPV;
    float trackPVWeight;
    setTracksPV(ptrackRef,fPVs,trackPV, trackPVWeight);
    if (!trackPV && trackPVWeight > 0)
      allKinematics.add(ptrack,trackPVWeight);
  }
  math::XYZTLorentzVector allSum =  allKinematics.weightedVectorSum() ; //allKinematics.vectorSum()

  std::map<double, unsigned int> VTXmass;
  unsigned int nSelectedSV = 0;
  float maxSVDeltaR2ToJet = R0-0.1+(R0-0.8)*0.1/0.7;  //linear interpolation
  maxSVDeltaR2ToJet = maxSVDeltaR2ToJet*maxSVDeltaR2ToJet;
  edm::RefToBase<reco::Jet> rJet = ipTagInfo->jet();
  math::XYZVector jetDir = rJet->momentum().Unit();
  for (unsigned int vtx = 0; vtx < nSV_tmp; ++vtx)  {
    const Vertex &vertex = svTagInfo->secondaryVertex(vtx);
    float mass = vertex.p4().mass();
    GlobalVector flightDir = svTagInfo->flightDirection(vtx);
    if (reco::deltaR2(flightDir, jetDir)<maxSVDeltaR2ToJet) {
      VTXmass[svtxMass[vtx]]=vtx;
      ++nSelectedSV;
    }
  }

  GlobalVector flightDir0, flightDir1;
  int cont=0;
  reco::Candidate::LorentzVector svP4_0 , svP4_1;
  // fill FatJet::fSVData in order of decreasing secondary vertex mass
  for ( std::map<double, unsigned int>::reverse_iterator iVtx=VTXmass.rbegin(); iVtx!=VTXmass.rend(); ++iVtx) {
      SVData * svData = new SVData;
      unsigned int idx = iVtx->second;
      const Vertex &vertex = svTagInfo->secondaryVertex(idx);

      svData->mass = iVtx->first;                                              //svx kinematics
      svData->pt = vertex.p4().pt();
      svData->eta = vertex.p4().eta();
      svData->phi = vertex.p4().phi();

      Int_t totcharge=0;                                                             //find svx energy fraction and charge
      reco::TrackKinematics vertexKinematics;
      vertexKinematicsAndCharge(vertex, vertexKinematics, totcharge);
      math::XYZTLorentzVector vertexSum = vertexKinematics.weightedVectorSum();
      svData->energyRatio = vertexSum.E()/allSum.E();
      svData->totCharge = totcharge;

      GlobalVector flightDir = svTagInfo->flightDirection(idx);
      svData->dirX = flightDir.x();
      svData->dirY = flightDir.y();
      svData->dirZ = flightDir.z();
      svData->deltaRJet = reco::deltaR(flightDir, jetDir);
      svData->deltaRSumJet = reco::deltaR(vertexSum, jetDir);
      svData->deltaRSumDir = reco::deltaR(vertexSum, flightDir);

      Line::PositionType pos(GlobalPoint(position(vertex).x(),position(vertex).y(),position(vertex).z()));
      Line trackline(pos,flightDir);
      Line::PositionType pos2(GlobalPoint(pv->x(),pv->y(),pv->z()));
      Line::DirectionType dir2(GlobalVector(jetDir.x(),jetDir.y(),jetDir.z()));
      Line jetline(pos2,dir2);
      svData->vtxDistJetAxis = (jetline.distance(trackline)).mag();

      svData->flight = svTagInfo->flightDistance(idx).value();
      svData->flightErr = svTagInfo->flightDistance(idx).error();
      svData->flight2D = svTagInfo->flightDistance(idx, true).value();
      svData->flight2DErr = svTagInfo->flightDistance(idx, true).error();
      svData->nTrk = svTagInfo->secondaryVertex(idx).nTracks();
      outJet.AddSVData(svData);
  }
}

void NeroFatJets::vertexKinematicsAndCharge(const Vertex & vertex, reco::TrackKinematics & vertexKinematics, Int_t & charge)
{
  const std::vector<reco::CandidatePtr> & tracks = vertex.daughterPtrVector();

  for(std::vector<reco::CandidatePtr>::const_iterator track = tracks.begin(); track != tracks.end(); ++track) {
    const reco::Track& mytrack = *(*track)->bestTrack();
    vertexKinematics.add(mytrack, 1.0);
    charge+=mytrack.charge();
  }
}

void NeroFatJets::setTracksPV(const TrackRef & trackRef, const edm::Handle<reco::VertexCollection> & pvHandle, int & iPV, float & PVweight)
{
  iPV = -1;
  PVweight = 0.;
  const reco::PFCandidate * pfcand = dynamic_cast<const reco::PFCandidate *>(trackRef.get());
  setTracksPVBase(pfcand->trackRef(), pvHandle, iPV, PVweight);
}

void NeroFatJets::setTracksPVBase(const reco::TrackRef & trackRef, const edm::Handle<reco::VertexCollection> & pvHandle, int & iPV, float & PVweight)
{
  iPV = -1;
  PVweight = 0.;
  const reco::TrackBaseRef trackBaseRef( trackRef );
  typedef reco::VertexCollection::const_iterator IV;
  typedef reco::Vertex::trackRef_iterator IT;
  for(IV iv=pvHandle->begin(); iv!=pvHandle->end(); ++iv)
  {
    const reco::Vertex & vtx = *iv;
    // loop over tracks in vertices
    for(IT it=vtx.tracks_begin(); it!=vtx.tracks_end(); ++it)
    {
      const reco::TrackBaseRef & baseRef = *it;
      // one of the tracks in the vertex is the same as the track considered in the function
      if( baseRef == trackBaseRef )
      {
        float w = vtx.trackWeight(baseRef);
        // select the vertex for which the track has the highest weight
        if( w > PVweight )
        {
          PVweight = w;
          iPV = ( iv - pvHandle->begin() );
          break;
        }
      }
    }
  }
}


void mithep::FillerFatJets::recalcNsubjettiness(const pat::Jet &jet,
                                                const SVTagInfo &svTagInfo,
                                                mithep::FatJet &outJet,
                                                std::vector<fastjet::PseudoJet> &currentAxes) {
  // recalculate nsubjettiness removing the bjet
  std::vector<fastjet::PseudoJet> fjParticles;
  std::vector<reco::CandidatePtr> svDaughters;

  // loop over IVF vertices and push them in the vector of FastJet constituents and also collect their daughters
  for(size_t i=0; i<svTagInfo.nVertices(); ++i)
  {
    const reco::VertexCompositePtrCandidate & vtx = svTagInfo.secondaryVertex(i);

    fjParticles.push_back( fastjet::PseudoJet( vtx.px(), vtx.py(), vtx.pz(), vtx.energy() ) );

    const std::vector<reco::CandidatePtr> & daughters = vtx.daughterPtrVector();
    svDaughters.insert(svDaughters.end(), daughters.begin(), daughters.end());
  }

  // loop over jet constituents and select those that are not daughters of IVF vertices
  std::vector<reco::CandidatePtr> constituentsOther;
  for(const reco::CandidatePtr & daughter : jet.daughterPtrVector())
  {
    if (std::find(svDaughters.begin(), svDaughters.end(), daughter) == svDaughters.end())
      constituentsOther.push_back( daughter );
  }

  // loop over jet constituents that are not daughters of IVF vertices and push them in the vector of FastJet constituents
  for(const reco::CandidatePtr & constit : constituentsOther)
  {
    if ( constit.isNonnull() && constit.isAvailable() )
      fjParticles.push_back( fastjet::PseudoJet( constit->px(), constit->py(), constit->pz(), constit->energy() ) );
    else
      edm::LogWarning("MissingJetConstituent") << "Jet constituent required for N-subjettiness computation is missing!";
  }

  // re-calculate N-subjettiness
  outJet.SetTau1IVF(njettiness.getTau(1, fjParticles));
  outJet.SetTau2IVF(njettiness.getTau(2, fjParticles));
  outJet.SetTau3IVF(njettiness.getTau(3, fjParticles));
  currentAxes = njettiness.currentAxes();
}

DEFINE_MITHEP_TREEFILLER(FillerFatJets);
